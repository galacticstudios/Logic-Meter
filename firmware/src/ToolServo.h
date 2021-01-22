/* 
 * File:   ToolPWM.h
 * Author: Bob
 *
 * Created on April 25, 2020, 9:50 AM
 */

#ifndef TOOLSERVO_H
#define	TOOLSERVO_H

#include "ToolPWMBase.h"
#include "fixed.h"
#include "SquareWavePane.h"

class ToolServo : public ToolPWMBase 
{
public:
    static Tool *Factory() {return new ToolServo;}
    
    ToolServo();
    virtual ~ToolServo();
     
    void OnPulseWidth();
    void Up();
    void Down();
    void Left();
    void Right();
    void Done();
    
protected:
  fixed GetPeriod()const;
  fixed GetDuty() const {return _duty;}
    
private:
    ToolServo(const ToolServo& orig);
    
    SquareWavePane *GetPane() const {return (SquareWavePane *) Tool::GetPane();}
    
    fixed _duty;
};

#endif	/* TOOLSERVO_H */

