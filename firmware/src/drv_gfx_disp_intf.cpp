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

extern "C"
{
#include "definitions.h"
}
#include "gfx/hal/inc/gfx_common.h"
#include "gfx/hal/inc/gfx_context.h"
#include "gfx/interface/drv_gfx_disp_intf.h"
#include "gfx/driver/controller/ili9488/drv_gfx_ili9488_cmd_defs.h"
#include "gfx/driver/controller/ili9488/drv_gfx_ili9488_common.h"
#include "Utility.h"
#include "DMA.h"
#include "TimerB.h"

static bool lcdUpdateBusy = false;
extern "C" bool IsLCDUpdateBusy()
{
    return lcdUpdateBusy;
}

//#define PMPLCD
#define SPILCD

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
    
    /* SPI transaction status */
    volatile GFX_DISP_INTF_SPI_TRANS_STATUS drvSPITransStatus;    
} GFX_DISP_INTF_SPI;


#ifdef SPILCD
#ifdef PMPLCD
#error Both SPILCD and PMPLCD are defined. Only one HW interface should be defined
#endif

#include "SPI.h"

static SPI<2> *spi;

struct DMACommand
{
    void *address;
    size_t size;
};

CQueue<DMACommand, 10> dmaCommands;

#elif defined(PMPLCD)

#include "definitions.h"

#else
#error No HW interface selected for LCD
#endif

/* ************************************************************************** */

extern "C"
GFX_Disp_Intf GFX_Disp_Intf_Open(GFX_Context * gfx)
{   
    GFX_DISP_INTF_SPI * intf = NULL;
    
    if (gfx == NULL)
        return 0;
    
    intf = (GFX_DISP_INTF_SPI *) gfx->memory.calloc(1, sizeof (GFX_DISP_INTF_SPI));
    
    if (intf == NULL)
        return 0;
    
    intf->gfx = gfx;
    
#ifdef SPILCD
    
    uint32_t rdata;
    
    spi = new SPI<2>;
    spi->Initialize(true, 0, false);
    spi->SetMode(0, 0);
    spi->UseSPISelect(true);
    spi->SetTransmitInterruptTrigger(spi->TransmitInterruptTrigger::BufferNotFull);
    spi->Enable();
    
#else // PMPLCD

    // Don't need to do any more; Harmony already called PMP_Initialize()
    
#endif
    
    return (GFX_Disp_Intf) intf;
}

extern "C"
void GFX_Disp_Intf_Close(GFX_Disp_Intf intf)
{
    ((GFX_DISP_INTF_SPI *) intf)->gfx->memory.free(((GFX_DISP_INTF_SPI *) intf));
    delete spi;
    spi = NULL;
}

static void StartDMACommand();

static void DMAComplete(void *)
{
    // Discard the just-completed command
    dmaCommands.read(nullptr);
    // Start the next command
    StartDMACommand();
}

void StartDMACommand()
{
    static TimerB<5> tmr;
    static bool firstTime = true;
    if (firstTime)
    {
        firstTime = false;
        tmr.Initialize(4000000);
        tmr.Enable();
    }
    static DMA *dma0 = nullptr;
    delete dma0;
    dma0 = nullptr;
    
    if (!dmaCommands.empty())
    {
        lcdUpdateBusy = true;
        DMACommand cmd;
        dmaCommands.peek(&cmd);

        dma0 = new DMA(0, 0, DMASource {cmd.address, 1, cmd.size}, spi->DMADestination(), tmr.TimerIRQ());
        dma0->SetInterruptPriorities(1, 0);
        dma0->RegisterCallback(DMAComplete, nullptr);
        // DMA works, but it goes too fast for the LCD (ILI9341 based). It wants more
        // time between bytes. So we trigger the DMA off a timer instead of the
        // SPI buffer state
        dma0->SetDMAInterruptTrigger(DMA::SourceDone);
        dma0->EnableInterrupt();
        dma0->Enable();
    }
    
    else
        lcdUpdateBusy = false;
}

extern "C"
GFX_Result GFX_Disp_Intf_WriteCommandParm(GFX_Disp_Intf intf, uint8_t cmd, uint8_t * parm, int num_parms)
{  
    // Wait for all pending DMAs to complete
    while (!dmaCommands.empty())
    {
    }

    while (!spi->TXEmpty()) {}
    
    GFX_DISP_INTF_PIN_RSDC_Clear();

#ifdef SPILCD    
    spi->TXData(cmd);
    
#else // PMPLCD
    while (PMP_PortIsBusy()) {}
    LCDnCS_Clear();
    PMP_MasterSend(cmd);
#endif
    
    if (num_parms > 0 && parm != NULL)
    {
#ifdef SPILCD    
        // Wait until the whole command byte has been transferred before
        // changing the D/C signal
        while (!spi->TXEmpty()) {} 
        
        GFX_DISP_INTF_PIN_RSDC_Set();

        // If we're not writing an image, just output the (small number of) parameters
        // ****** For some reason, the image data gets corrupted when we're DMAing it at the
        // same time the Logic Analyzer is capturing samples via DMA. So we've stopped using
        // DMA image updates until we can figure this out
        if (true) //cmd != ILI9488_CMD_MEMORY_WRITE)
        {
            while (num_parms--)
            {
                while (!spi->TXEmpty()) {}
                spi->TXData(*parm++);
            }
        }
        
        // Else (outputting and image) use DMA to send the image data
        else
        {
            // Set up a list of DMA transfers to make
            while (num_parms)
            {
                size_t transferSize = num_parms > 65536 ? 65536 : num_parms;
                dmaCommands.write((DMACommand) {parm, transferSize});
                num_parms -= transferSize;
                parm += transferSize;
            }
            
            // Start the first transfer. When it's done, we'll get an interrupt.
            // The DMAComplete() function will handle it.
            if (!dmaCommands.empty())
            {
                StartDMACommand();
            }
        }
        
#else // PMPLCD
        while (num_parms--)
        {
            while (PMP_PortIsBusy()) {}
            PMP_MasterSend(*parm++);
        }
#endif
    }
    
#ifdef PMPLCD
    LCDnCS_Set();
#endif
    
    GFX_DISP_INTF_SPI * spiIntf = (GFX_DISP_INTF_SPI *) intf;
    spiIntf->drvSPITransStatus = SPI_TRANS_DONE;

    return GFX_SUCCESS;
}

extern "C"
GFX_Result GFX_Disp_Intf_Write(GFX_Disp_Intf intf, uint8_t * data, int bytes)
{
    GFX_DISP_INTF_SPI * spiIntf = (GFX_DISP_INTF_SPI *) intf;

#ifdef PMPLCD
    LCDnCS_Clear();
#endif
    
    while (bytes--)
    {
#ifdef SPILCD
        while (!spi->TXReady()) {}
        spi->TXData(*data++);
        
#else // PMPLCD
        while (PMP_PortIsBusy()) {}
        PMP_MasterSend(*data++);
#endif
    }
    
#ifdef PMPLCD
    LCDnCS_Set();
#endif    
    
    spiIntf->drvSPITransStatus = SPI_TRANS_DONE;
    
    return GFX_SUCCESS;
}


extern "C"
GFX_Result GFX_Disp_Intf_Read(GFX_Disp_Intf intf, uint8_t * data, int bytes)
{
    //Not supported
    return GFX_FAILURE;
}


/** 
  Function:
    GFX_Result ILI9488_Intf_WritePixels(struct ILI9488_DRV *drv,
                                              uint32_t start_x,
                                              uint32_t start_y,
                                              uint8_t *data,
                                              unsigned int num_pixels)

  Summary:
    Writes pixel data to ILI9488 GRAM from specified position.

  Description:
    This function will fist write the start column, page information, then 
    write the pixel data to the ILI9488 GRAM.

  Parameters:
    drv             - ILI9488 driver handle
    start_x         - Start column position
    start_y         - Start page position
    data            - Array of 8-bit pixel data (8-bit/pixel RGB)
    num_pixels      - Number of pixels
 
  Returns:
    * GFX_SUCCESS       - Operation successful
    * GFX_FAILURE       - Operation failed

  Remarks:
    In SPI mode, this function performs multiple full-blocking write calls to
    the SPI port and won't return until the SPI transaction completes.

 */
#define IMAGE_WIDTH 320
#define IMAGE_HEIGHT 240

GFX_Result ILI9488_Intf_WritePixels(struct ILI9488_DRV *drv,
                                   uint32_t start_x,
                                   uint32_t start_y,
                                   uint8_t *data,
                                   unsigned int num_pixels)
{
    GFX_Result returnValue = GFX_FAILURE;
    uint8_t buf[4];
    GFX_Disp_Intf intf;
    
    if (!drv)
        return GFX_FAILURE;
    
    intf = (GFX_Disp_Intf) drv->port_priv;

    //Set column
    buf[0] = (start_x >> 8);
    buf[1] = (start_x & 0xff);
    buf[2] = (((drv->gfx->display_info->rect.width - 1) & 0xff00) >> 8);
    buf[3] = ((drv->gfx->display_info->rect.width - 1) & 0xff);
    returnValue = GFX_Disp_Intf_WriteCommandParm(intf,
                                            ILI9488_CMD_COLUMN_ADDRESS_SET,
                                            buf,
                                            4);
    if (GFX_SUCCESS != returnValue)
    {
        return GFX_FAILURE;
    }

    //Set page
    buf[0] = (start_y >> 8);
    buf[1] = (start_y & 0xff);
    buf[2] = (((drv->gfx->display_info->rect.height - 1) & 0xff00) >> 8);
    buf[3] = ((drv->gfx->display_info->rect.height - 1) & 0xff);
    returnValue = GFX_Disp_Intf_WriteCommandParm(intf,
                                            ILI9488_CMD_PAGE_ADDRESS_SET,
                                            buf,
                                            4);
    if (GFX_SUCCESS != returnValue)
    {
        return GFX_FAILURE;
    }

    returnValue = GFX_Disp_Intf_WriteCommandParm(intf,
                                            ILI9488_CMD_MEMORY_WRITE,
                                            data,
                                            num_pixels * 2);
    

    return returnValue;
}
