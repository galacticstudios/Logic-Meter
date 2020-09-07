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


#include "gfx/libaria/inc/libaria_widget_drawsurface.h"

#if LA_DRAWSURFACE_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#define DEFAULT_WIDTH           100
#define DEFAULT_HEIGHT          100

void _laDrawSurfaceWidget_Constructor(laDrawSurfaceWidget* sfc)
{
    _laWidget_Constructor((laWidget*)sfc);
    
    sfc->widget.destructor = (laWidget_Destructor_FnPtr)&_laDrawSurfaceWidget_Destructor;

    sfc->widget.type = LA_WIDGET_DRAWSURFACE;

    // override base class methods
    sfc->widget.paint = (laWidget_Paint_FnPtr)&_laDrawSurfaceWidget_Paint;

    sfc->widget.rect.width = DEFAULT_WIDTH;
    sfc->widget.rect.height = DEFAULT_HEIGHT;

    sfc->widget.borderType = LA_WIDGET_BORDER_NONE;
    sfc->widget.backgroundType = LA_WIDGET_BACKGROUND_FILL;

    sfc->cb = NULL;
}

void _laDrawSurfaceWidget_Destructor(laDrawSurfaceWidget* sfc)
{
    _laWidget_Destructor((laWidget*)sfc);
}

/*void _laDrawSurfaceWidget_Update(laDrawSurfaceWidget* sfc)
{

}*/

laDrawSurfaceWidget* laDrawSurfaceWidget_New()
{
    laDrawSurfaceWidget* sfc = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    sfc = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laDrawSurfaceWidget));

    if(sfc == NULL)
        return NULL;
    
    _laDrawSurfaceWidget_Constructor(sfc);

    return sfc;
}

laDrawSurfaceWidget_DrawCallback laDrawSurfaceWidget_GetDrawCallback(laDrawSurfaceWidget* sfc)
{
    if(sfc == NULL)
        return NULL;

    return sfc->cb;
}

laResult laDrawSurfaceWidget_SetDrawCallback(laDrawSurfaceWidget* sfc,
                                                laDrawSurfaceWidget_DrawCallback cb)
{
    if(sfc == NULL)
        return LA_FAILURE;

    if(sfc->cb == cb)
        return LA_SUCCESS;

    sfc->cb = cb;

    laWidget_Invalidate((laWidget*)sfc);
    
    return LA_SUCCESS;
}

#endif // LA_DRAWSURFACE_WIDGET_ENABLED