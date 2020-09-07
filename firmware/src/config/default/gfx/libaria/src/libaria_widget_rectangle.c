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


#include "gfx/libaria/inc/libaria_widget_rectangle.h"

#if LA_RECTANGLE_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#define DEFAULT_WIDTH           100
#define DEFAULT_HEIGHT          100

#define DEFAULT_THICKNESS       2

static void _laRectangleWidget_Destructor(laRectangleWidget* rect);
void _laRectangleWidget_Paint(laRectangleWidget* rect);

static void _laRectangleWidget_Constructor(laRectangleWidget* rect)
{
    _laWidget_Constructor((laWidget*)rect);
    
    rect->widget.destructor = (laWidget_Destructor_FnPtr)&_laRectangleWidget_Destructor;

    rect->widget.type = LA_WIDGET_RECTANGLE;

    // override base class methods
    rect->widget.paint = (laWidget_Paint_FnPtr)&_laRectangleWidget_Paint;

    rect->widget.rect.width = DEFAULT_WIDTH;
    rect->widget.rect.height = DEFAULT_HEIGHT;

    rect->widget.borderType = LA_WIDGET_BORDER_NONE;
    rect->widget.backgroundType = LA_WIDGET_BACKGROUND_FILL;

    rect->thickness = DEFAULT_THICKNESS;
}

static void _laRectangleWidget_Destructor(laRectangleWidget* rect)
{
    _laWidget_Destructor((laWidget*)rect);
}

laRectangleWidget* laRectangleWidget_New()
{
    laRectangleWidget* rect = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    rect = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laRectangleWidget));

    if(rect == NULL)
        return NULL;
    
    _laRectangleWidget_Constructor(rect);

    return rect;
}

int32_t laRectangleWidget_GetThickness(laRectangleWidget* rect)
{
    if(rect == NULL)
        return 0;
        
    return rect->thickness;
}

laResult laRectangleWidget_SetThickness(laRectangleWidget* rect, int32_t thk)
{
    if(rect == NULL || thk < 0)
        return LA_FAILURE;
        
    if(rect->thickness == thk)
        return LA_SUCCESS;
        
    _laRectangleWidget_InvalidateRect(rect);
        
    rect->thickness = thk;
    
    _laRectangleWidget_InvalidateRect(rect);
            
    return LA_SUCCESS;
}

#endif // LA_RECTANGLE_WIDGET_ENABLED