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


#include "gfx/libaria/inc/libaria_widget_gradient.h"

#if LA_GRADIENT_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#define DEFAULT_WIDTH           100
#define DEFAULT_HEIGHT          100

void _laGradientWidget_Constructor(laGradientWidget* grad)
{
    _laWidget_Constructor((laWidget*)grad);
    
    grad->widget.destructor = (laWidget_Destructor_FnPtr)&_laGradientWidget_Destructor;

    grad->widget.type = LA_WIDGET_GRADIENT;

    // override base class methods
    grad->widget.paint = (laWidget_Paint_FnPtr)&_laGradientWidget_Paint;

    grad->widget.rect.width = DEFAULT_WIDTH;
    grad->widget.rect.height = DEFAULT_HEIGHT;

    grad->widget.borderType = LA_WIDGET_BORDER_NONE;
    grad->widget.backgroundType = LA_WIDGET_BACKGROUND_FILL;

    grad->dir = LA_GRADIENT_DIRECTION_RIGHT;
}

void _laGradientWidget_Destructor(laGradientWidget* grad)
{
    _laWidget_Destructor((laWidget*)grad);
}

laGradientWidget* laGradientWidget_New()
{
    laGradientWidget* grad = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    grad = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laGradientWidget));

    if(grad == NULL)
        return NULL;
    
    _laGradientWidget_Constructor(grad);

    return grad;
}

laGradientWidgetDirection laGradientWidget_GetDirection(laGradientWidget* grad)
{
    if(grad == NULL)
        return 0;
        
    return grad->dir;
}

laResult laGradientWidget_SetDirection(laGradientWidget* grad,
                                       laGradientWidgetDirection dir)
{
    if(grad == NULL)
        return LA_FAILURE;
        
    if(grad->dir == dir)
        return LA_SUCCESS;
        
    grad->dir = dir;
    
    laWidget_Invalidate((laWidget*)grad);
        
    return LA_SUCCESS;
}

#endif // LA_GRADIENT_WIDGET_ENABLED