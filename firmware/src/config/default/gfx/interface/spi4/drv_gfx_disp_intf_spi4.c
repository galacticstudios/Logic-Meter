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

/*******************************************************************************
  MPLAB Harmony Generated Driver Implementation File

  File Name:
    drv_gfx_intf_spi4.c

  Summary:
    Implements the display interface driver over SPI

  Description:
    Implements the 4-line SPI interface driver

    Created with MPLAB Harmony Version 3.0
 *******************************************************************************/

#include "driver/spi/drv_spi.h"

#include "gfx/hal/inc/gfx_common.h"
#include "gfx/hal/inc/gfx_context.h"
#include "gfx/interface/drv_gfx_disp_intf.h"

/** SPI_TRANS_STATUS

  Summary:
    Enum type of SPI transaction status

 */
typedef enum 
{
    SPI_TRANS_IDLE,
    SPI_TRANS_CMD_WR_PENDING,
    SPI_TRANS_CMD_RD_PENDING,
    SPI_TRANS_DONE,
    SPI_TRANS_FAIL,
} GFX_DISP_INTF_SPI_TRANS_STATUS;

/** GFX_INTF_SPI

  Summary:
    Structure contains status and handles for SPI interface.
    
 */
typedef struct
{
    /* The GFX HAL context */
    GFX_Context * gfx;
    
    /* SPI Device handle */
    DRV_HANDLE drvSPIHandle;
    
    /* Transfer handle */
    DRV_SPI_TRANSFER_HANDLE drvSPITransferHandle;
    
    /* SPI transaction status */
    volatile GFX_DISP_INTF_SPI_TRANS_STATUS drvSPITransStatus;    
} GFX_DISP_INTF_SPI;

/* ************************************************************************** */

/** 
  Function:
    static void GFX_Disp_Intf_CallBack(DRV_SPI_BUFFER_EVENT event, 
                                     DRV_SPI_BUFFER_HANDLE bufferHandle, 
                                     void * context )

  Summary:
    Callback function called by SPI driver to deliver events.

  Description:

    This callback will set the ILI9488 SPI driver's SPI transaction status 
    based on the event.


  Parameters:
    event           - SPI buffer event
    bufferHandle    - SPI buffer handle
    context         - SPI transaction context

  Returns:
    None

 */
static void GFX_Disp_Intf_CallBack(DRV_SPI_TRANSFER_EVENT event,
                              DRV_SPI_TRANSFER_HANDLE bufferHandle,
                              uintptr_t context)
{
    volatile GFX_DISP_INTF_SPI_TRANS_STATUS *status = 
                    ((GFX_DISP_INTF_SPI_TRANS_STATUS *) context);

   if (!status)
        return;

    switch (event) 
    {
        case DRV_SPI_TRANSFER_EVENT_COMPLETE:
            *status = SPI_TRANS_DONE;
            break;
        case DRV_SPI_TRANSFER_EVENT_ERROR:
            *status = SPI_TRANS_FAIL;
        default:
            break;
    }
}

GFX_Disp_Intf GFX_Disp_Intf_Open(GFX_Context * gfx)
{   
    GFX_DISP_INTF_SPI * intf = NULL;
    
    if (gfx == NULL)
        return 0;
    
    intf = (GFX_DISP_INTF_SPI *) gfx->memory.calloc(1, sizeof (GFX_DISP_INTF_SPI));
    
    if (intf == NULL)
        return 0;
    
    intf->gfx = gfx;
    
    intf->drvSPIHandle = DRV_SPI_Open(0, DRV_IO_INTENT_READWRITE);
    if (DRV_HANDLE_INVALID == intf->drvSPIHandle)
    {
        gfx->memory.free(intf);

        return GFX_FAILURE;
    }

    DRV_SPI_TransferEventHandlerSet(intf->drvSPIHandle,
                                    GFX_Disp_Intf_CallBack,
                                    (uintptr_t)&intf->drvSPITransStatus );

    return (GFX_Disp_Intf) intf;
}

void GFX_Disp_Intf_Close(GFX_Disp_Intf intf)
{
    if (((GFX_DISP_INTF_SPI *) intf) == NULL)
        return;
    
    DRV_SPI_Close(((GFX_DISP_INTF_SPI *) intf)->drvSPIHandle);

    ((GFX_DISP_INTF_SPI *) intf)->gfx->memory.free(((GFX_DISP_INTF_SPI *) intf));
}

GFX_Result GFX_Disp_Intf_PinControl(GFX_Disp_Intf intf, GFX_DISP_INTF_PIN pin, GFX_DISP_INTF_PIN_VALUE value)
{
    GFX_Result res = GFX_FAILURE;
    
    if (((GFX_DISP_INTF_SPI *) intf) == NULL)
        return GFX_FAILURE;
    
    switch(pin)
    {
        case GFX_DISP_INTF_PIN_CS:
#ifdef GFX_DISP_INTF_PIN_CS_Set
            if (value == GFX_DISP_INTF_PIN_CLEAR)
                GFX_DISP_INTF_PIN_CS_Clear();
            else
                GFX_DISP_INTF_PIN_CS_Set();
            
            res = GFX_SUCCESS;
#endif
            break;
        case GFX_DISP_INTF_PIN_WR:
#ifdef GFX_DISP_INTF_PIN_WR_Set
            if (value == GFX_DISP_INTF_PIN_CLEAR)
                GFX_DISP_INTF_PIN_WR_Clear();
            else
                GFX_DISP_INTF_PIN_WR_Set();

            res = GFX_SUCCESS;            
#endif
            break;
        case GFX_DISP_INTF_PIN_RD:
#ifdef GFX_DISP_INTF_PIN_RD_Set
            if (value == GFX_DISP_INTF_PIN_CLEAR)
                GFX_DISP_INTF_PIN_RD_Clear();
            else
                GFX_DISP_INTF_PIN_RD_Set();
            
            res = GFX_SUCCESS;
#endif
            break;
        case GFX_DISP_INTF_PIN_RSDC:
#ifdef GFX_DISP_INTF_PIN_RSDC_Set
            if (value == GFX_DISP_INTF_PIN_CLEAR)
                GFX_DISP_INTF_PIN_RSDC_Clear();
            else
                GFX_DISP_INTF_PIN_RSDC_Set();
            
            res = GFX_SUCCESS;
#endif            
            break;
        default:
            break;
    }
    
    return res;
}

GFX_Result GFX_Disp_Intf_WriteCommand(GFX_Disp_Intf intf, uint8_t cmd)
{
    GFX_Disp_Intf_PinControl(intf, GFX_DISP_INTF_PIN_RSDC, GFX_DISP_INTF_PIN_CLEAR);
    
    return GFX_Disp_Intf_Write(intf, &cmd, 1);
}

GFX_Result GFX_Disp_Intf_WriteData(GFX_Disp_Intf intf, uint8_t * data, int bytes)
{
    GFX_Disp_Intf_PinControl(intf, GFX_DISP_INTF_PIN_RSDC, GFX_DISP_INTF_PIN_SET);
    
    return GFX_Disp_Intf_Write(intf, data, bytes);
}

GFX_Result GFX_Disp_Intf_ReadData(GFX_Disp_Intf intf, uint8_t * data, int bytes)
{
    GFX_Disp_Intf_PinControl(intf, GFX_DISP_INTF_PIN_RSDC, GFX_DISP_INTF_PIN_SET);
    
    return GFX_Disp_Intf_Read(intf, data, bytes);
}

GFX_Result GFX_Disp_Intf_ReadCommandData(GFX_Disp_Intf intf, uint8_t cmd, uint8_t * data, int num_data)
{
    GFX_Result retval;

    retval = GFX_Disp_Intf_WriteCommand(intf, cmd);
    if (retval != GFX_SUCCESS)
        return GFX_FAILURE;

    GFX_Disp_Intf_PinControl(intf, GFX_DISP_INTF_PIN_RSDC, GFX_DISP_INTF_PIN_SET);
    
    return GFX_Disp_Intf_Read(intf, data, num_data);
}

GFX_Result GFX_Disp_Intf_WriteCommandParm(GFX_Disp_Intf intf, uint8_t cmd, uint8_t * parm, int num_parms)
{
    GFX_Result retval;
    
    GFX_Disp_Intf_PinControl(intf, GFX_DISP_INTF_PIN_RSDC, GFX_DISP_INTF_PIN_CLEAR);
    
    retval = GFX_Disp_Intf_Write(intf, &cmd, 1);
    if (retval != GFX_SUCCESS)
        return GFX_FAILURE;
    
    if (num_parms > 0 && parm != NULL)
    {
        GFX_Disp_Intf_PinControl(intf, GFX_DISP_INTF_PIN_RSDC, GFX_DISP_INTF_PIN_SET);
         retval = GFX_Disp_Intf_Write(intf, parm, num_parms);
    }

    return retval;
}

GFX_Result GFX_Disp_Intf_Write(GFX_Disp_Intf intf, uint8_t * data, int bytes)
{
    GFX_DISP_INTF_SPI * spiIntf = (GFX_DISP_INTF_SPI *) intf;
    
    if (spiIntf == NULL ||
        bytes == 0 ||
        data == NULL)
        return GFX_FAILURE;
    
    spiIntf->drvSPITransStatus = SPI_TRANS_CMD_WR_PENDING;
    DRV_SPI_WriteTransferAdd(spiIntf->drvSPIHandle,
                            (void *) data,
                            (size_t) bytes,
                            (void *) &spiIntf->drvSPITransferHandle);
    if (DRV_SPI_TRANSFER_HANDLE_INVALID == spiIntf->drvSPITransferHandle)
    {
        return GFX_FAILURE;
    }
    
    while (SPI_TRANS_CMD_WR_PENDING == spiIntf->drvSPITransStatus);
    
    return GFX_SUCCESS;
}


GFX_Result GFX_Disp_Intf_Read(GFX_Disp_Intf intf, uint8_t * data, int bytes)
{
    GFX_DISP_INTF_SPI * spiIntf = (GFX_DISP_INTF_SPI *) intf;
    
    if (spiIntf == NULL ||
        bytes == 0 ||
        data == NULL)
        return GFX_FAILURE;
    
    // Read the valid pixels
    spiIntf->drvSPITransStatus = SPI_TRANS_CMD_RD_PENDING;
    DRV_SPI_ReadTransferAdd(spiIntf->drvSPIHandle,
                            (void *) data,
                            bytes,
                            (void *) &spiIntf->drvSPITransferHandle);
    if (DRV_SPI_TRANSFER_HANDLE_INVALID == spiIntf->drvSPITransferHandle)
        return GFX_FAILURE;

    //Wait for the callback (full block/no timeout)
    while (SPI_TRANS_CMD_RD_PENDING == spiIntf->drvSPITransStatus);
    
    return GFX_SUCCESS;
}

GFX_Result GFX_Disp_Intf_WriteData16(GFX_Disp_Intf intf, uint16_t * data, int num)
{
    //Not supported
    return GFX_FAILURE;
}

GFX_Result GFX_Disp_Intf_ReadData16(GFX_Disp_Intf intf, uint16_t * data, int num)
{
    //Not supported
    return GFX_FAILURE;
}

/* *****************************************************************************
 End of File
 */
