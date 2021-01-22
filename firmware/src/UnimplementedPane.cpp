/* 
 * File:   UnimplementedPane.cpp
 * Author: Bob
 * 
 * Created on January 10, 2021, 11:23 AM
 */

extern "C" 
{
#include "definitions.h"
}
#include "UnimplementedPane.h"

laWidget *UnimplementedPane::GetWidget() const
{
    return UnimplementedPanel;
}
