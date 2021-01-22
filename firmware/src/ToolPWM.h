/* 
 * File:   ToolPWM.h
 * Author: Bob
 *
 * Created on April 25, 2020, 9:50 AM
 */

#ifndef TOOLPWM_H
#define	TOOLPWM_H

#include "ToolPWMBase.h"
#include "fixed.h"
#include "SquareWavePane.h"

class ToolPWM : public ToolPWMBase
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
    
protected:  
    fixed GetPeriod() const;
    fixed GetDuty() const {return _duty;}
    
private:
    ToolPWM(const ToolPWM& orig);
    
    SquareWavePane *GetPane() const {return (SquareWavePane *) Tool::GetPane();}
    
    fixed _hertz, _duty;
};

#endif	/* TOOLPWM_H */

