/* 
 * File:   ToolPWM.h
 * Author: Bob
 *
 * Created on April 25, 2020, 9:50 AM
 */

#ifndef TOOLSERVO_H
#define	TOOLSERVO_H

#include "Tool.h"
#include "fixed.h"
#include "SquareWavePane.h"

class ToolServo : public Tool 
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
    
private:
    ToolServo(const ToolServo& orig);
    
    SquareWavePane *GetPane() const {return (SquareWavePane *) Tool::GetPane();}
  
    void UpdateServo();
    
    fixed _duty;
};

#endif	/* TOOLSERVO_H */

