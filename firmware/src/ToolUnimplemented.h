/* 
 * File:   ToolUnimplemented.h
 * Author: Bob
 *
 * Created on January 10, 2021, 11:00 AM
 */

#ifndef TOOLUNIMPLEMENTED_H
#define	TOOLUNIMPLEMENTED_H

#include "Tool.h"

class ToolUnimplemented : public Tool
{
public:
    static Tool *Factory() {return new ToolUnimplemented;}

    virtual ~ToolUnimplemented();
    
    virtual void OnIdle() {}
    virtual void Update() {}
 
protected:
    ToolUnimplemented();

private:
    ToolUnimplemented(const ToolUnimplemented& orig);
};

#endif	/* TOOLUNIMPLEMENTED_H */

