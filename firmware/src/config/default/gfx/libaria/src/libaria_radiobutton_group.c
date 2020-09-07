// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
// DOM-IGNORE-END


#include "gfx/libaria/inc/libaria_radiobutton_group.h"

#if LA_RADIOBUTTON_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_widget_radiobutton.h"

laResult laRadioButtonGroup_Create(laRadioButtonGroup** grp)
{
    laRadioButtonGroup* btnGrp;
    
    if(grp == NULL)
        return LA_FAILURE;
        
    *grp = NULL;
    
    btnGrp = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laRadioButtonGroup));

    if(btnGrp == NULL)
        return LA_FAILURE;

    btnGrp->selected = NULL;
    btnGrp->initialized = LA_FALSE;

    if(laArray_Create(&btnGrp->buttonList) == LA_FAILURE)
        return LA_FAILURE;

    btnGrp->initialized = LA_TRUE;
    
    *grp = btnGrp;

    return LA_SUCCESS;
}

void laRadioButtonGroup_Destroy(laRadioButtonGroup* grp)
{
    if(grp == NULL)
        return;

    laArray_Destroy(&grp->buttonList);
    
    laContext_GetActive()->memIntf.heap.free(grp);
}

laResult laRadioButtonGroup_AddButton(laRadioButtonGroup* grp,
                                      laRadioButtonWidget* btn)
{
    if(grp == NULL || grp->initialized != LA_TRUE)
        return LA_FAILURE;

    if(laArray_Find(&grp->buttonList, btn) != -1)
        return LA_FAILURE;

    if(grp->buttonList.size > 0)
        _laRadioButtonWidget_Deselect(btn);
    else
        grp->selected = btn;

    laArray_PushBack(&grp->buttonList, btn);
    
    btn->group = grp;

    return LA_SUCCESS;
}

laResult laRadioButtonGroup_RemoveButton(laRadioButtonGroup* grp,
                                         laRadioButtonWidget* btn)
{
    if(grp == NULL || grp->initialized != LA_TRUE)
        return LA_FAILURE;

    if(laArray_Find(&grp->buttonList, btn) == -1)
        return LA_FAILURE;

    laArray_Remove(&grp->buttonList, btn);

    btn->group = NULL;
    
    laRadioButtonWidget_SetSelected(btn);
    
    if(grp->buttonList.size == 0)
        laRadioButtonGroup_Destroy(grp);

    return LA_SUCCESS;
}

laResult laRadioButtonGroup_SelectButton(laRadioButtonGroup* grp,
                                         laRadioButtonWidget* btn)
{
    if(grp == NULL || grp->initialized != LA_TRUE || grp->selected == btn)
        return LA_FAILURE;

    if(laArray_Find(&grp->buttonList, btn) == -1)
        return LA_FAILURE;

    _laRadioButtonWidget_Deselect(grp->selected);
    _laRadioButtonWidget_Select(btn);

    grp->selected = btn;

    return LA_SUCCESS;
}

#endif // LA_RADIOBUTTON_WIDGET_ENABLED