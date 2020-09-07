/* 
 * File:   LogicMeter.h
 * Author: Bob
 *
 * Created on April 24, 2020, 5:38 PM
 */

#ifndef LOGICMETER_H
#define	LOGICMETER_H

#ifdef __cplusplus
extern "C"
{
#endif
    
void LogicMeterStart();
void LogicMeterTask();
    
#ifdef __cplusplus

class Tool;

class LogicMeter 
{
public:
    LogicMeter();
    ~LogicMeter();
    
    void Task();
    
private:
    LogicMeter(const LogicMeter& orig);

    int ReadSelectorSwitch();
    
    int ReadButtons();
    
    void UserInteraction();
    
    Tool *_currentTool;
    int _currentSelection;
    
    // Keep track of when the user last did something, for dimming the display
    uint32_t _lastUserInteractionTime;
    bool _displayDimmed;
};

#endif

#ifdef __cplusplus
}
#endif

#endif	/* LOGICMETER_H */

