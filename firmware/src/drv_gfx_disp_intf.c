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

#include "definitions.h"
#include "gfx/hal/inc/gfx_common.h"
#include "gfx/hal/inc/gfx_context.h"
#include "gfx/interface/drv_gfx_disp_intf.h"
#include "gfx/driver/controller/ili9488/drv_gfx_ili9488_cmd_defs.h"

//#define PMPLCD
#define SPILCD
//#define INVERT_DISPLAY

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

#define SPI3_CON_MSTEN                      (1 << _SPI3CON_MSTEN_POSITION)
#define SPI3_CON_CKP                        (0 << _SPI3CON_CKP_POSITION)
#define SPI3_CON_CKE                        (1 << _SPI3CON_CKE_POSITION)
#define SPI3_CON_MODE_32_MODE_16            (0 << _SPI3CON_MODE16_POSITION)
#define SPI3_CON_ENHBUF                     (1 << _SPI3CON_ENHBUF_POSITION)
#define SPI3_CON_MCLKSEL                    (0 << _SPI3CON_MCLKSEL_POSITION)
#define SPI3_CON_MSSEN                      (1 << _SPI3CON_MSSEN_POSITION)

#elif defined(PMPLCD)

#include "definitions.h"

#else
#error No HW interface selected for LCD
#endif

/* ************************************************************************** */

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
    
    // Enable the SPI3 module
    PMD5bits.SPI3MD = 0;

    /* Disable SPI3 Interrupts */
    IEC6CLR = 0x4000000;
    IEC6CLR = 0x8000000;
    IEC6CLR = 0x10000000;

    /* STOP and Reset the SPI */
    SPI3CON = 0;

    /* Clear the Receiver buffer */
    rdata = SPI3BUF;
    rdata = rdata;

    /* Clear SPI3 Interrupt flags */
    IFS6CLR = 0x4000000;
    IFS6CLR = 0x8000000;
    IFS6CLR = 0x10000000;

    /* BAUD Rate register Setup */
    SPI3BRG = 0;

    /* Clear the Overflow */
    SPI3STATCLR = _SPI3STAT_SPIROV_MASK;

    /*
    MSTEN = 1
    CKP = 0
    CKE = 1
    MODE<32,16> = 0
    ENHBUF = 1
    MSSEN = 1
    MCLKSEL = 0
    */
    SPI3CONSET = (SPI3_CON_MSSEN | SPI3_CON_MCLKSEL | SPI3_CON_ENHBUF | SPI3_CON_MODE_32_MODE_16 | SPI3_CON_CKE | SPI3_CON_CKP | SPI3_CON_MSTEN);

    /* Enable transmit interrupt when transmit buffer is completely empty (STXISEL = '01') */
    /* Enable receive interrupt when the receive buffer is not empty (SRXISEL = '01') */
    SPI3CONSET = 0x00000005;

    /* Enable SPI3 */
    SPI3CONSET = _SPI3CON_ON_MASK;
    
#else // PMPLCD

    // Don't need to do any more; Harmony already called PMP_Initialize()
    
#endif
    
    return (GFX_Disp_Intf) intf;
}

void GFX_Disp_Intf_Close(GFX_Disp_Intf intf)
{
    ((GFX_DISP_INTF_SPI *) intf)->gfx->memory.free(((GFX_DISP_INTF_SPI *) intf));
}

GFX_Result GFX_Disp_Intf_WriteCommand(GFX_Disp_Intf intf, uint8_t cmd)
{
    GFX_DISP_INTF_PIN_RSDC_Clear();
    
    return GFX_Disp_Intf_Write(intf, &cmd, 1);
}

GFX_Result GFX_Disp_Intf_WriteData(GFX_Disp_Intf intf, uint8_t * data, int bytes)
{
    GFX_DISP_INTF_PIN_RSDC_Set();
    
    return GFX_Disp_Intf_Write(intf, data, bytes);
}

GFX_Result GFX_Disp_Intf_WriteCommandParm(GFX_Disp_Intf intf, uint8_t cmd, uint8_t * parm, int num_parms)
{
    GFX_DISP_INTF_PIN_RSDC_Clear();

#ifdef SPILCD    
    while (SPI3STATbits.SPITBF || SPI3STATbits.SPIBUSY) {}
    SPI3BUF = cmd;
    
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
        while (!SPI3STATbits.SPITBE || SPI3STATbits.SPIBUSY) {} 
        
        GFX_DISP_INTF_PIN_RSDC_Set();
        
        while (num_parms--)
        {
            while (!SPI3STATbits.SPITBE || SPI3STATbits.SPIBUSY) {}
            // Need a really short delay here. Not sure why - otherwise the LCD doesn't read
            // all our commands properly. Slowing down the SPI data rate doesn't fix it.
            // Assigning a volatile variable here doesn't fix it. Calling a zero us delay does
            // (so it's just the overhead of the delay function)
            CORETIMER_DelayUs(0);
#ifdef INVERT_DISPLAY
            if (cmd == ILI9488_CMD_MEMORY_WRITE)
                SPI3BUF = ~*parm++;
            else
#endif
                SPI3BUF = *parm++;
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

GFX_Result GFX_Disp_Intf_Write(GFX_Disp_Intf intf, uint8_t * data, int bytes)
{
    GFX_DISP_INTF_SPI * spiIntf = (GFX_DISP_INTF_SPI *) intf;

#ifdef PMPLCD
    LCDnCS_Clear();
#endif
    
    while (bytes--)
    {
#ifdef SPILCD
        while (SPI3STATbits.SPITBF || SPI3STATbits.SPIBUSY || !SPI3STATbits.SRMT) {}
        SPI3BUF = *data++;
        
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


GFX_Result GFX_Disp_Intf_Read(GFX_Disp_Intf intf, uint8_t * data, int bytes)
{
    //Not supported
    return GFX_FAILURE;
}

/* *****************************************************************************
 End of File
 */
