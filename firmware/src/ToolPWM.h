/* 
 * File:   ToolPWM.h
 * Author: Bob
 *
 * Created on April 25, 2020, 9:50 AM
 */

#ifndef TOOLPWM_H
#define	TOOLPWM_H

#include "Tool.h"
#include "fixed.h"
#include "SquareWavePane.h"

class ToolPWM : public Tool 
{
public:
    static Tool *Factory() {return new ToolPWM;}
    
    ToolPWM();
    virtual ~ToolPWM();
     
    void OnFreq();
    void OnPeriod();
    void OnDuty();
    void OnPulseWidth();
    void Up();
    void Down();
    void Left();
    void Right();
    void Done();
    
private:
    ToolPWM(const ToolPWM& orig);
    
    SquareWavePane *GetPane() const {return (SquareWavePane *) Tool::GetPane();}
  
    void UpdatePWM();
    
    fixed _hertz, _duty;
};

#endif	/* TOOLPWM_H */

