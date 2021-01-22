/* 
 * File:   ToolLogicAnalyzer.cpp
 * Author: Bob
 * 
 * Created on November 1, 2020, 10:12 AM
 */

#include <algorithm>
#include "fixed.h"
#include "ToolLogicAnalyzer.h"
#include "Menu.h"
#include "SquareWavePane.h"
#include "Settings.h"
#include "InputCapture.h"
#include "LogicAnalyzerPane.h"

static const Help help("Channel 1", "Channel 3", "Channel 2", 
        "Displays up to three digital signals.");

static uint32_t timerEnableBit = 1 << 15;

static const MenuItem channelMenuItems[5] = {
    MenuItem("CH1", MenuType::NoChange, nullptr, CB(&ToolLogicAnalyzer::ToggleChannel1)), 
    MenuItem("CH2", MenuType::NoChange, nullptr, CB(&ToolLogicAnalyzer::ToggleChannel2)), 
    MenuItem("CH3", MenuType::NoChange, nullptr, CB(&ToolLogicAnalyzer::ToggleChannel3)), 
    MenuItem(), 
    MenuItem("Done", MenuType::ParentMenu)};

static const Menu channelMenu(channelMenuItems);

static const MenuItem modeMenuItems[5] = {
    MenuItem("Auto", MenuType::NoChange, nullptr, CB(&ToolLogicAnalyzer::SetAutoMode)), 
    MenuItem("Normal", MenuType::NoChange, nullptr, CB(&ToolLogicAnalyzer::SetNormalMode)), 
    MenuItem("Single", MenuType::NoChange, nullptr, CB(&ToolLogicAnalyzer::SetSingleMode)), 
    MenuItem(), 
    MenuItem("Done", MenuType::ParentMenu)};

static const Menu modeMenu(modeMenuItems);

static const MenuItem rateMenuItems[5] = {
    MenuItem("10KHz", MenuType::ParentMenu, nullptr, CB(&ToolLogicAnalyzer::Rate10KHz)), 
    MenuItem("100KHz", MenuType::ParentMenu, nullptr, CB(&ToolLogicAnalyzer::Rate100KHz)), 
    MenuItem("1MHz", MenuType::ParentMenu, nullptr, CB(&ToolLogicAnalyzer::Rate1MHz)), 
    MenuItem("10MHz", MenuType::ParentMenu, nullptr, CB(&ToolLogicAnalyzer::Rate10MHz)), 
    MenuItem("Cancel", MenuType::ParentMenu)};

static const Menu rateMenu(rateMenuItems);

static const MenuItem triggerMenuItems[5] = {
    MenuItem("Chan", MenuType::NoChange, nullptr, CB(&ToolLogicAnalyzer::ChangeTriggerChannel)), 
    MenuItem("Edge", MenuType::NoChange, nullptr, CB(&ToolLogicAnalyzer::ChangeTriggerEdge)), 
    MenuItem("Pos", MenuType::NoChange, nullptr, CB(&ToolLogicAnalyzer::ChangeTriggerPosition)), 
    MenuItem(), 
    MenuItem("Done", MenuType::ParentMenu)};

static const Menu triggerMenu(triggerMenuItems);

static const MenuItem menuItems[5] = {
    MenuItem("Chans", MenuType::ChildMenu, &channelMenu), 
    MenuItem("Mode", MenuType::ChildMenu, &modeMenu), 
    MenuItem("Rate", MenuType::ChildMenu, &rateMenu), 
    MenuItem("Trig", MenuType::ChildMenu, &triggerMenu), 
    MenuItem()};

static const Menu menu(menuItems);

#define SAMPLE_BLOCK_SIZE 65536

static union Samples
{
    uint8_t stream[SAMPLE_BLOCK_SIZE * 3];
    uint8_t blocks[3][SAMPLE_BLOCK_SIZE];
}  __attribute__((coherent)) __attribute__((aligned(16))) _samples;
Samples &samples = *(Samples *) KVA0_TO_KVA1(&_samples);
static int nextSampleBlock;

#define PRIMARY_MASK (1 << 0)
#define AUX1_MASK (1 << 1)
#define AUX2_MASK (1 << 4)

ToolLogicAnalyzer::ToolLogicAnalyzer() :
    Tool("Logic", new LogicAnalyzerPane(), menu, help),
    _acquisitionMode(Auto), _completedSamplingDMAIndex(-1), _wasAcquiring(false),
    // Sample values on the second byte of Port D, which includes all three inputs
    // D9 is Aux2, D10 is Aux1, D11 is Primary
    _samplingDMA1(1, 0, DMASource {(uint8_t *) &PORTD, 1, 1}, DMADestination {samples.blocks[0], SAMPLE_BLOCK_SIZE}, _sampleTimer.TimerIRQ()),
    _samplingDMA2(2, 0, DMASource {(uint8_t *) &PORTD, 1, 1}, DMADestination {samples.blocks[1], SAMPLE_BLOCK_SIZE}, _sampleTimer.TimerIRQ()),
    _samplingDMAs{&_samplingDMA1, &_samplingDMA2},
    _turnOnPostTriggerTimerDMA(3, 0, DMASource {&timerEnableBit, 4, 4}, DMADestination {(void *) &_postTriggerTimer.Regs().TCON.set, 4}, _triggerInputCapture.InputCaptureIRQ()),
    _turnOffSampleTimerDMA(4, 0, DMASource {&timerEnableBit, 4, 4}, DMADestination {(void *) &_sampleTimer.Regs().TCON.clr, 4}, _postTriggerTimer.TimerIRQ())
{
    // There should be at least two
    nextSampleBlock = 2;
    
    // Initialize the timer to the configured freq
    SampleFreq(settings.sampleFreq);
    
    // Set up ping-pong DMA for gathering samples
    _samplingDMA1.SetInterruptPriorities(1, 0);
    _samplingDMA1.RegisterCallback(DMAComplete, this);
    _samplingDMA1.SetDMAInterruptTrigger(DMA::DestinationDone);
    _samplingDMA1.EnableInterrupt();
    _samplingDMA1.SetChaining(DMA::ChainMode::FromLowerPriorityChannel);
    
    // Encode the DMA callback data with our this pointer, and fit the index of 
    // the DMA object into the low bit of the DMA callback data
    _samplingDMA2.SetInterruptPriorities(1, 0);
    _samplingDMA2.RegisterCallback(DMAComplete, (void *) (uint32_t(this) | 1));
    _samplingDMA2.SetDMAInterruptTrigger(DMA::DestinationDone);
    _samplingDMA2.EnableInterrupt();
    _samplingDMA2.SetChaining(DMA::ChainMode::FromHigherPriorityChannel);
    
    _turnOnPostTriggerTimerDMA.SetInterruptPriorities(1, 0);
    _turnOnPostTriggerTimerDMA.RegisterCallback(PostTriggerAcquisitionStarted, this);
    _turnOnPostTriggerTimerDMA.SetDMAInterruptTrigger(DMA::DestinationDone);
    //_turnOnPostTriggerTimerDMA.EnableInterrupt();
    
    _turnOffSampleTimerDMA.SetInterruptPriorities(1, 0);
    _turnOffSampleTimerDMA.RegisterCallback(PostTriggerAcquisitionComplete, this);
    _turnOffSampleTimerDMA.SetDMAInterruptTrigger(DMA::DestinationDone);
    //_turnOffSampleTimerDMA.EnableInterrupt();
}

ToolLogicAnalyzer::~ToolLogicAnalyzer() 
{
}

void ToolLogicAnalyzer::OnIdle()
{
    // If acquisition is running
    if (IsAcquiring())
    {
        _wasAcquiring = true;
    }
    
    // Else (acquisition is stopped)
    else
    {
        // If we just had been acquiring
        if (_wasAcquiring)
        {
            _postTriggerTimer.Disable();
            
            // Which DMA is currently running (but is paused because the timer is disabled)?
            int activeDMA;
            for (activeDMA = 0; activeDMA < countof(_samplingDMAs); ++activeDMA)
            {
                if (_samplingDMAs[activeDMA]->IsBusy())
                    break;
            }
            if (activeDMA < countof(_samplingDMAs))
            {
                TracePoint prevPixels[3];

                // What was the last sample acquired?
                uint8_t *samplePtr = (uint8_t *) PA_TO_KVA1(uint32_t(_samplingDMAs[activeDMA]->GetDestinationAddress())) + _samplingDMAs[activeDMA]->GetDestinationPointer();

                // Convert the samples into pixels for the trace
                volatile uint32_t samplesPerPixel = sizeof(samples) / ((LogicAnalyzerPane *) GetPane())->TracePixelWidth();

                // For each pixel
                for (uint32_t pixel = 0; pixel < sizeof(samples) / samplesPerPixel; ++pixel)
                {
                    uint32_t andedSamples = 0xffffffff, oredSamples = 0;

                    // Combine samples until we reach a 32 bit boundary (we don't have
                    // to worry about running over the end of the sample buffer here)
                    uint32_t combinedSampleCount = 0;
                    while ((uint32_t) samplePtr & 3)
                    {
                        andedSamples &= *samplePtr | 0xffffff00;
                        oredSamples |= *samplePtr;
                        ++samplePtr;
                        ++combinedSampleCount;
                    }

                    // Figure out how many samples we can accumulate before we reach the end of the buffer
                    uint32_t beforeEOB = std::min(uint32_t(samples.stream + sizeof(samples) - samplePtr), samplesPerPixel - combinedSampleCount);
                    // Then, how many samples should we accumulate at the start of the buffer?
                    uint32_t atSOB = samplesPerPixel - combinedSampleCount - beforeEOB;

                    // While there are samples to accumulate before the end of the buffer
                    while (beforeEOB)
                    {
                        // Now combine 4 samples at a time.
                        while (beforeEOB >= 4)
                        {
                            andedSamples &= *(uint32_t *) samplePtr;
                            oredSamples |= *(uint32_t *) samplePtr;
                            samplePtr += 4;
                            beforeEOB -= 4;
                        }

                        // Handle the last few samples
                        while (beforeEOB)
                        {
                            *(uint8_t *) &andedSamples &= *samplePtr;
                            oredSamples |= *samplePtr & 0xff;
                            samplePtr++;
                            beforeEOB--;
                        }

                        // Circle around to the start of the buffer
                        if (atSOB)
                        {
                            beforeEOB = atSOB;
                            samplePtr = (uint8_t *) KVA0_TO_KVA1(samples.stream);
                            atSOB = 0;
                        }
                    }

                    // Combine our 4 bytes into one
                    andedSamples &= andedSamples >> 16;
                    andedSamples &= andedSamples >> 8;
                    oredSamples |= oredSamples >> 16;
                    oredSamples |= oredSamples >> 8;

                    // Figure out what pixel to draw for each channel
                    static const uint32_t masks[] = {PRIMARY_MASK, AUX1_MASK, AUX2_MASK};
                    for (int traceIndex = 0; traceIndex < countof(masks); ++traceIndex)
                    {
                        TracePoint tracePoint, drawTracePoint;
                        // If this channel is enabled
                        if ((settings.enabledChannels & (1 << traceIndex)))
                        {
                            volatile bool high = oredSamples & masks[traceIndex];
                            volatile bool low = (andedSamples & masks[traceIndex]) == 0;
                            volatile bool edge = high & low;
                            tracePoint = drawTracePoint = edge ? TracePoint::Edge : (high ? TracePoint::High : TracePoint::Low);
                            // If there was a previous pixel in the trace
                            if (pixel != 0)
                            {
                                // If it's different from this new pixel, we need to draw an edge
                                if (prevPixels[traceIndex] != TracePoint::Edge && prevPixels[traceIndex] != tracePoint)
                                    drawTracePoint = TracePoint::Edge;
                            }
                            prevPixels[traceIndex] = tracePoint;
                        }

                        // else (channel is not enabled)
                        else
                            drawTracePoint = TracePoint::Blank;

                        // Set the trace pixel
                        ((LogicAnalyzerPane *) GetPane())->SetTracePoint(traceIndex, pixel, drawTracePoint);
                    }
                }

                GetPane()->Update();
            }
            
            // Draw the data on the screen
            _wasAcquiring = false;
        }
        
        // If we should start another acquisition
        if (_acquisitionMode == Auto)
        {
            nextSampleBlock = 0;
            _samplingDMAs[0]->SetDMAInterruptTrigger(DMA::DestinationDone);
            _samplingDMAs[0]->SetDestination(DMADestination {samples.blocks[nextSampleBlock], SAMPLE_BLOCK_SIZE});
            _samplingDMAs[0]->Enable();
            RunAcquisition();
        }
    }
}

void ToolLogicAnalyzer::Update()
{
}
    
void ToolLogicAnalyzer::RunAcquisition()
{   
    _postTriggerTimer.Disable();
    _postTriggerTimer.Reset();
    _triggerInputCapture.Disable();
    _turnOffSampleTimerDMA.Disable();
    memset(&_samples, 0, sizeof(_samples));
    
    // If we're triggering off any channel
    if (settings.triggerChannel)
    {
        // Here's how triggering works. 
        // First, keep in mind that sampling is done by DMA, which is driven by the Timer in _sampleTimer.
        // For triggering, we set up Input Capture (IC) on the trigger channel's input pin.
        // Note that we do not start a timer for the IC. We don't care about the time;
        // we just want the edge interrupt.
        // When the IC triggers, it starts _turnOnPostTriggerTimerDMA of one word, writing to _postTriggerTimer's
        // control register and starting the timer. The timer is set to run a given 
        // amount of time, enough to accumulate the samples following the trigger event.
        // Then, _postTriggerTimer's interrupt sets off _turnOffSampleTimerDMA that writes to _sampleTimer's control
        // register, turning off the timer and stopping acquisition of any more samples.
        
        // Start acquisition
        _sampleTimer.Enable();

        // Set up the _postTriggerTimer for the amount of time we should accumulate samples
        // after the trigger
        uint32_t samplesAfterTrigger = (100 - settings.triggerPosition) * countof(_samples.stream) / 100;
        fixed secondsAfterTrigger = fixed(samplesAfterTrigger) / settings.sampleFreq;
        _postTriggerTimer.InitializeDuration(secondsAfterTrigger);
        
        switch (settings.triggerChannel)
        {
            case 1 : IC4R = RPD11; break;
            case 2 : IC4R = RPF5; break;
            case 3 : IC4R = RPF0; break;
            default : __builtin_software_breakpoint(); break;
        }
        auto edge = _triggerInputCapture.EveryEdge;
        switch (settings.triggerEdge)
        {
            case TriggerEdge::Rising : edge = _triggerInputCapture.EveryRising; break;
            case TriggerEdge::Falling : edge = _triggerInputCapture.EveryFalling; break;
            case TriggerEdge::Either : edge = _triggerInputCapture.EveryEdge; break;
        }
        
        _triggerInputCapture.RegisterCallback(&ToolLogicAnalyzer::InputCaptured, this);
        _triggerInputCapture.SetInterruptPriorities();

        _triggerInputCapture.DisableInterrupt();
        _triggerInputCapture.Initialize(edge, _triggerInputCapture.Timer32Bit);
        _triggerInputCapture.Enable();
        while (_triggerInputCapture.DataReady())
            _triggerInputCapture.ReadData();
        _triggerInputCapture.ClearInterrupt();
        _triggerInputCapture.EnableInterrupt();
        
        // _sampleTimer is what drives the DMA channels that acquire the samples.
        // _postTriggerTimer is started when the edge trigger happens, and expires
        // when we should stop acquiring samples.
        // Set up a DMA channel that turns off _sampleTimer when the _postTriggerTimer expires.
        _turnOffSampleTimerDMA.Enable();
        
        // Set up a DMA channel that turns _postTriggerTimer on when InputCapture _triggerInputCapture fires
        _turnOnPostTriggerTimerDMA.Enable();
    }
    
    // Else (we're not triggering off anything
    else
    {
        // This is a simplified version of what we did with triggering.
        // First, keep in mind that sampling is done by DMA, which is driven by the Timer in _sampleTimer.
        // We start the _postTriggerTimer. The timer is set to run a given 
        // amount of time, enough to accumulate the all the samples for the buffer.
        // Then, _postTriggerTimer's interrupt sets off _turnOffSampleTimerDMA that writes to _sampleTimer's control
        // register, turning off the timer and stopping acquisition of any more samples.

        // Set up the _postTriggerTimer for the amount of time we should accumulate samples
        // after the trigger
        fixed duration = fixed((uint32_t) countof(_samples.stream)) / fixed(settings.sampleFreq);
        _postTriggerTimer.InitializeDuration(duration);
               
        // _sampleTimer is what drives the DMA channels that acquire the samples.
        // _postTriggerTimer is started here in the code, and expires
        // when we should stop acquiring samples.
        // Set up a DMA channel that turns off _sampleTimer when the _postTriggerTimer expires.
        _turnOffSampleTimerDMA.Enable();
        
        // Start acquisition
        _sampleTimer.Enable();
        
        // Start _postTriggerTimer
        _postTriggerTimer.Enable();
    }
}

// DMAComplete runs when when a DMA channel that's acquiring samples fills up its buffer
void ToolLogicAnalyzer::DMAComplete(uint32_t dmaIndex)
{
    // Figure out the next available sample buffer
    _completedSamplingDMAIndex = int32_t(dmaIndex);
    if (++nextSampleBlock >= countof(samples.blocks))
        nextSampleBlock = 0;

    // The DMA channel that just completed, chained to another DMA channel that is
    // now filling up another buffer. While that runs, we reconfigure the completed
    // channel to fill up yet a third buffer.
    // Set up the DMA channel for the next time it runs
    _samplingDMAs[dmaIndex]->SetDMAInterruptTrigger(DMA::DestinationDone);
    _samplingDMAs[dmaIndex]->SetDestination(DMADestination {samples.blocks[nextSampleBlock], SAMPLE_BLOCK_SIZE});
}

// Enable or disable a channel in response to a user's button press
void ToolLogicAnalyzer::ToggleChannel(int ch)
{
    int mask = 1 << (ch - 1);
    settings.enabledChannels ^= mask;
    SettingsModified();
    if ((settings.enabledChannels & mask) == 0)
        ((LogicAnalyzerPane *) GetPane())->SetTracePoint(ch - 1, 0, TracePoint::Blank);
    Update();
}

void ToolLogicAnalyzer::SampleFreq(uint32_t freq)
{
    if (freq != settings.sampleFreq)
    {
        settings.sampleFreq = freq;
        SettingsModified();
    }
    _sampleTimer.Disable();
    _sampleTimer.Initialize(settings.sampleFreq);
    
    char buf[20];
    if (freq >= 1000000)
        sprintf(buf, "%dMHz", freq / 1000000);
    else
        sprintf(buf, "%dKHz", freq / 1000);
    SetStatusText(buf);
    
    Update();
}

void ToolLogicAnalyzer::ChangeTriggerChannel()
{
    // Look at the next possible channel
    uint8_t newTriggerChannel = settings.triggerChannel + 1;
    while (true)
    {
        // If we've run out of channels, reset to 0 (no trigger channel)
        if (newTriggerChannel > LA_CHANNEL_COUNT)
        {
            // Turn off the trigger channel
            newTriggerChannel = 0;
            break;
        }
        // Otherwise (we haven't run out of possible trigger channels)
        // if the newTriggerChannel is enabled
        if (settings.enabledChannels & (1 << (newTriggerChannel - 1)))
            // We've found our new trigger
            break;
        // Otherwise, look at the next possible channel
        ++newTriggerChannel;
    }
    settings.triggerChannel = newTriggerChannel;
    SettingsModified();
    ((LogicAnalyzerPane *) GetPane())->SetTrigger(settings.triggerChannel, settings.triggerEdge);
}

void ToolLogicAnalyzer::ChangeTriggerEdge()
{
    switch (settings.triggerEdge)
    {
        case TriggerEdge::Rising : settings.triggerEdge = TriggerEdge::Falling; break;
        case TriggerEdge::Falling : settings.triggerEdge = TriggerEdge::Either; break;
        case TriggerEdge::Either : settings.triggerEdge = TriggerEdge::Rising; break;
    }
    SettingsModified();
    ((LogicAnalyzerPane *) GetPane())->SetTrigger(settings.triggerChannel, settings.triggerEdge);
}

void ToolLogicAnalyzer::ChangeTriggerPosition()
{
    ((LogicAnalyzerPane *) GetPane())->SetTrigger(settings.triggerChannel, settings.triggerEdge);
}

void ToolLogicAnalyzer::PostTriggerAcquisitionStarted()
{
    _turnOnPostTriggerTimerDMA.ClearDMAInterruptFlags();
}

void ToolLogicAnalyzer::PostTriggerAcquisitionComplete()
{
    _turnOffSampleTimerDMA.ClearDMAInterruptFlags();
}

void ToolLogicAnalyzer::InputCaptured()
{
    _triggerInputCapture.ReadData();
}
