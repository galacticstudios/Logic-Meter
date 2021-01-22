/* 
 * File:   ToolLogicAnalyzer.h
 * Author: Bob
 *
 * Created on November 1, 2020, 10:12 AM
 */

#ifndef TOOLLOGICANALYZER_H
#define	TOOLLOGICANALYZER_H

#include <stdint.h>
#include "DMA.h"
#include "TimerB.h"
#include "InputCapture.h"
#include "Tool.h"

class ToolLogicAnalyzer : public Tool
{
public:
    static Tool *Factory() {return new ToolLogicAnalyzer;}

    ToolLogicAnalyzer();
    virtual ~ToolLogicAnalyzer();
    
    virtual void OnIdle();
    
    virtual void Update();
    
    void ToggleChannel1() {ToggleChannel(1);}
    void ToggleChannel2() {ToggleChannel(2);}
    void ToggleChannel3() {ToggleChannel(3);}

    void SetAutoMode() {}
    void SetNormalMode() {}
    void SetSingleMode() {}
    
    void Rate10KHz() {SampleFreq(10000);}
    void Rate100KHz() {SampleFreq(100000);}
    void Rate1MHz() {SampleFreq(1000000);}
    void Rate10MHz() {SampleFreq(10000000);}
    
    void ChangeTriggerChannel();
    void ChangeTriggerEdge();
    void ChangeTriggerPosition();
    
private:
    ToolLogicAnalyzer(const ToolLogicAnalyzer& orig);
    
    void RunAcquisition();
    
    void DMAComplete(uint32_t dmaIndex);
    static void DMAComplete(void *pthis) 
    {
        uint32_t ithis = uint32_t(pthis);
        // The pthis argument is the this pointer for the ToolLogicAnalyzer.
        // The low bit is 0 or 1 indicating a callback from the first or second
        // DMA object
        ((ToolLogicAnalyzer *) (ithis & ~1))->DMAComplete(ithis & 1);
    }
    
    void ToggleChannel(int ch);
    void SampleFreq(uint32_t freq);
    
    enum {Auto, Single} _acquisitionMode;
    
    bool IsAcquiring() {return _sampleTimer.Regs().TCON.bits.ON;}
    bool _wasAcquiring;
    
    TimerB<6> _sampleTimer;
    DMA _samplingDMA1, _samplingDMA2;
    DMA *_samplingDMAs[2];
    int32_t _completedSamplingDMAIndex;
    
    // A timer for the post-trigger acquisition time
    TimerB32<8> _postTriggerTimer;
    
    // Capture the edge trigger
    InputCapture<4> _triggerInputCapture;
    // DMA to turn on a timer when the input capture happens
    DMA _turnOnPostTriggerTimerDMA;
    void PostTriggerAcquisitionStarted(); 
    static void PostTriggerAcquisitionStarted(void *pthis) 
    {
        ((ToolLogicAnalyzer *) (pthis))->PostTriggerAcquisitionStarted();
    }
    // DMA to turn off acquisition when the acquisition timer finishes
    DMA _turnOffSampleTimerDMA;
    void PostTriggerAcquisitionComplete(); 
    static void PostTriggerAcquisitionComplete(void *pthis) 
    {
        ((ToolLogicAnalyzer *) (pthis))->PostTriggerAcquisitionComplete();
    }
    
    void InputCaptured();
    static void InputCaptured(void *context) 
    {
        ToolLogicAnalyzer *la = (ToolLogicAnalyzer *) context;
        la->InputCaptured();
    }

};

#endif	/* TOOLLOGICANALYZER_H */

