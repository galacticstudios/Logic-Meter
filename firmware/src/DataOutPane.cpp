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
#include "Utility.h"
#include "Display.h"
#include "DataOutPane.h"

DataOutPane::DataOutPane(bool forceBinary) :
    _terminalPane(forceBinary, DataOutDrawSurface)
{
}

DataOutPane::~DataOutPane() 
{
    laWidget_SetVisible((laWidget *) DataLabel, LA_FALSE);
    laWidget_SetVisible((laWidget *) DataStatusLabel, LA_FALSE);
}

laWidget *DataOutPane::GetWidget() const
{
    return DataOutPanel;
}

void DataOutPane::SetDataName(const char *dataName)
{
    laString str = laString_CreateFromUTF8Buffer(dataName, &MonoFont);
    laLabelWidget_SetText(DataLabel, str);
    laString_Destroy(&str);
    laWidget_SetVisible((laWidget *) DataLabel, LA_TRUE);
}

void DataOutPane::SetStatus(const char *status)
{
    laString str = laString_CreateFromUTF8Buffer(status, &MonoFont);
    laLabelWidget_SetText(DataStatusLabel, str);
    laString_Destroy(&str);
    laWidget_SetVisible((laWidget *) DataStatusLabel, LA_TRUE);
}
