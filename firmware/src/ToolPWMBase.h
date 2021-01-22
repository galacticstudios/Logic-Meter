/* 
 * File:   ToolPWMBase.h
 * Author: Bob
 *
 * Created on January 5, 2021, 4:00 PM
 */

#ifndef TOOLPWMBASE_H
#define	TOOLPWMBASE_H

#include "Tool.h"
#include "fixed.h"

class ToolPWMBase : public Tool
{
protected:
    ToolPWMBase(const char *title, Pane *pane, const Menu &menu, const Help &help);
    ToolPWMBase(const ToolPWMBase& orig);
    virtual ~ToolPWMBase();
    
    void UpdatePWM();
    
    virtual fixed GetPeriod() const = 0;
    virtual fixed GetDuty() const = 0;
    
private:

};

#endif	/* TOOLPWMBASE_H */

