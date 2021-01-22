/* 
 * File:   UtilityPane.cpp
 * Author: Bob
 * 
 * Created on September 23, 2020, 1:29 PM
 */

extern "C" 
{
#include "definitions.h"
}
#include "UtilityPane.h"

UtilityPane::UtilityPane() 
{
}

UtilityPane::~UtilityPane() 
{
}

laWidget *UtilityPane::GetWidget() const
{
    return UtilityPanel;
}


