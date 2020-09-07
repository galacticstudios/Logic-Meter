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


#include "gfx/libaria/inc/libaria_widget_arc.h"

#if LA_ARC_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#define DEFAULT_WIDTH           101
#define DEFAULT_HEIGHT          101

#define DEFAULT_ORIGIN_X        50
#define DEFAULT_ORIGIN_Y        50

#define DEFAULT_RADIUS          50
#define DEFAULT_START_ANGLE     0
#define DEFAULT_CENTER_ANGLE    135
#define DEFAULT_THICKNESS       20

void _laArcWidget_Constructor(laArcWidget* arc)
{
    _laWidget_Constructor((laWidget*)arc);

    arc->widget.type = LA_WIDGET_ARC;
    
    arc->widget.destructor = (laWidget_Destructor_FnPtr)&_laArcWidget_Destructor;

    // override base class methods
    arc->widget.paint = (laWidget_Paint_FnPtr)&_laArcWidget_Paint;

    arc->widget.rect.width = DEFAULT_WIDTH;
    arc->widget.rect.height = DEFAULT_HEIGHT;

    arc->widget.borderType = LA_WIDGET_BORDER_NONE;
    arc->widget.backgroundType = LA_WIDGET_BACKGROUND_NONE;

    arc->radius = DEFAULT_RADIUS;
    arc->startAngle = DEFAULT_START_ANGLE;
    arc->centerAngle = DEFAULT_CENTER_ANGLE;
    arc->thickness = DEFAULT_THICKNESS;
    arc->endAngle = arc->startAngle +  arc->centerAngle;
    
    arc->roundEdge = LA_FALSE;
}

void _laArcWidget_Destructor(laArcWidget* arc)
{
    _laWidget_Destructor((laWidget*)arc);
}

laArcWidget* laArcWidget_New()
{
    laArcWidget* arc = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    arc = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laArcWidget));

    if(arc == NULL)
        return NULL;
    
    _laArcWidget_Constructor(arc);

    return arc;
}

uint32_t laArcWidget_GetRadius(laArcWidget* arc)
{
    if(arc == NULL)
        return 0;
        
    return arc->radius;
}

laResult laArcWidget_SetRadius(laArcWidget* arc, uint32_t rad)
{
    if(arc == NULL)
        return LA_FAILURE;
        
    if(arc->radius == rad)
        return LA_SUCCESS;
        
    arc->radius = rad;
    
    laWidget_Invalidate((laWidget*)arc);
        
    return LA_SUCCESS;
}

int32_t laArcWidget_GetStartAngle(laArcWidget* arc)
{
    if(arc == NULL)
        return 0;
        
    return arc->startAngle;
    
    return LA_SUCCESS;
}

laResult laArcWidget_SetStartAngle(laArcWidget* arc, int32_t angle)
{
    if(arc == NULL)
        return LA_FAILURE;
        
    if(arc->startAngle == angle)
        return LA_SUCCESS;
        
    arc->startAngle = angle;
    
    laWidget_Invalidate((laWidget*)arc);
        
    return LA_SUCCESS;
}

uint32_t laArcWidget_GetThickness(laArcWidget* arc)
{
    if(arc == NULL)
        return 0;
        
    return arc->thickness;
}

laResult laArcWidget_SetThickness(laArcWidget* arc, uint32_t thickness)
{
    if(arc == NULL)
        return LA_FAILURE;

    if (thickness > arc->radius)
        return LA_FAILURE;

    if(arc->thickness == thickness)
        return LA_SUCCESS;
        
    arc->thickness = thickness;
    
    laWidget_Invalidate((laWidget*)arc);
        
    return LA_SUCCESS;
}

int32_t laArcWidget_GetCenterAngle(laArcWidget* arc)
{
    if(arc == NULL)
        return 0;
        
    return arc->centerAngle;
}

laResult laArcWidget_SetCenterAngle(laArcWidget* arc, int32_t angle)
{
    if(arc == NULL)
        return LA_FAILURE;

    if(arc->centerAngle == angle)
        return LA_SUCCESS;
        
    arc->centerAngle = angle;
    
    laWidget_Invalidate((laWidget*)arc);
        
    return LA_SUCCESS;
}

laBool laArcWidget_GetRoundEdge(laArcWidget* arc)
{
    if(arc == NULL)
        return LA_FALSE;

    return arc->roundEdge;
}

laResult laArcWidget_SetRoundEdge(laArcWidget* arc, laBool round)
{
    if(arc == NULL)
        return LA_FAILURE;

    arc->roundEdge = round;
    
    return LA_SUCCESS;
}

#endif // LA_ARC_WIDGET_ENABLED