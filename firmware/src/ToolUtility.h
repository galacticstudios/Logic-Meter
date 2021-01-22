/* 
 * File:   ToolUtility.h
 * Author: Bob
 *
 * Created on September 23, 2020, 1:22 PM
 */

#ifndef TOOLUTILITY_H
#define	TOOLUTILITY_H

#include "Tool.h"

class ToolUtility : public Tool
{
public:
    static Tool *Factory() {return new ToolUtility;}

    ToolUtility();
    virtual ~ToolUtility();
    
    void DumpDisk();
    
    virtual void OnIdle();

private:
    ToolUtility(const ToolUtility& orig);
    
    int _diskOffset;
    int _diskSize;
};

#endif	/* TOOLUTILITY_H */

