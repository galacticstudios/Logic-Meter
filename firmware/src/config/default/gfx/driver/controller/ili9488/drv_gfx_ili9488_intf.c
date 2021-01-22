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
    drv_gfx_ili9488_intf.c

  Summary:
    Implements 4-line SPI interface for ILI9488

  Description:
    Implements 4-line SPI interface for the ILI9488.

    Created with MPLAB Harmony Version 3.0
 *******************************************************************************/
#include "gfx/hal/inc/gfx_common.h"
#include "gfx/hal/inc/gfx_context.h"

#include "drv_gfx_ili9488_cmd_defs.h"
#include "drv_gfx_ili9488_common.h"

#include "gfx/interface/drv_gfx_disp_intf.h"

// BA changed the SPI macros because (1) the SPI peripheral is handling SS
// and (2) the Harmony code was stupidly inefficient
#include "peripheral/gpio/plib_gpio.h"

#define ILI9488_SPI_SS_Assert(intf)
#define ILI9488_SPI_SS_Deassert(intf)
#define ILI9488_SPI_DCX_Command(intf) GFX_DISP_INTF_PIN_RSDC_Clear()
#define ILI9488_SPI_DCX_Data(intf) GFX_DISP_INTF_PIN_RSDC_Set();

/** ILI9488_SPI_PRIV

  Summary:
    Structure contains status and handles for SPI interface.
    
 */
typedef struct 
{
    GFX_Disp_Intf handle;
} ILI9488_SPI_PRIV;

/* ************************************************************************** */

/** 
  Function:
    static GFX_Result ILI9488_Intf_Read(struct ILI9488_DRV *drv, 
                                        uint8_t cmd, 
                                        uint8_t *data,
                                        int bytes)

  Summary:
    Sends read command and returns response from the ILI9488 device.

  Description:
    This function will do a SPI write operation to send the read command 
    to the ILI9488, and then do a SPI read operation to read the response.

  Parameters:
    drv         - ILI9488 driver handle
    cmd         - Read command
    data        - Buffer to store read data
    bytes       - Number of bytes to read
 
  Returns:
    * GFX_SUCCESS       - Operation successful
    * GFX_FAILURE       - Operation failed
 
  Remarks:
    This is a full-blocking read call, and waits for the SPI transaction to 
    complete. 


 */
static GFX_Result ILI9488_Intf_Read(struct ILI9488_DRV *drv,
                                    uint8_t cmd,
                                    uint8_t *data,
                                    int bytes) 
{
    GFX_Result returnValue = GFX_FAILURE;
    GFX_Disp_Intf intf;

    if ((!drv) || (!data) || (bytes <= 0))
        return GFX_FAILURE;

    intf = (GFX_Disp_Intf) drv->port_priv;

    //Assert SS = LOW and D/CX = LOW (command)
    ILI9488_SPI_DCX_Command(intf);
    ILI9488_SPI_SS_Assert(intf);
            
    returnValue = GFX_Disp_Intf_Write(intf, &cmd, 1);
    if (returnValue != GFX_SUCCESS)
        return GFX_FAILURE;
    
    ILI9488_SPI_DCX_Data(intf);
    returnValue = GFX_Disp_Intf_Read(intf, data, bytes);
    if (returnValue != GFX_SUCCESS)
        return GFX_FAILURE;

    ILI9488_SPI_DCX_Command(intf);
    ILI9488_SPI_SS_Deassert(intf);

    return returnValue;
}

/** 
  Function:
    GFX_Result ILI9488_Intf_WriteCmd(struct ILI9488_DRV *drv,
                                     uint8_t cmd,
                                     uint8_t *parms, 
                                     int num_parms)

  Summary:
    Sends write command and parameters to the ILI9488 device.

  Description:
    This function will do a write operation to send the write command 
    and its parameters to the ILI9488.

  Parameters:
    drv         - ILI9488 driver handle
    cmd         - Write command
    parms       - Pointer to array of 8-bit parameters
    bytes       - number of command parameters
 
  Returns:
    * GFX_SUCCESS       - Operation successful
    * GFX_FAILURE       - Operation failed
 
  Remarks:
    In SPI mode, this is a full-blocking call and waits for the SPI transaction
    to complete. 

 */
GFX_Result ILI9488_Intf_WriteCmd(struct ILI9488_DRV *drv,
                                 uint8_t cmd,
                                 uint8_t *parms,
                                 int num_parms) 
{
    GFX_Result returnValue = GFX_FAILURE;
    GFX_Disp_Intf intf;

    if (!drv)
        return GFX_FAILURE;

    intf = (GFX_Disp_Intf) drv->port_priv;

    ILI9488_SPI_SS_Assert(intf);
    
    returnValue = GFX_Disp_Intf_WriteCommandParm(intf, cmd, parms, num_parms);
    
    ILI9488_SPI_SS_Deassert(intf);

    return returnValue;
}

// BA moved ILI9488_Intf_WritePixels to drv_gfx_disp_intf.cpp and rewrote
// it for speed

/** 
  Function:
    GFX_Result ILI9488_Intf_ReadPixels(struct ILI9488_DRV *drv,
                                      uint32_t x,
                                      uint32_t y,
                                      uint16_t *value,
                                      unsigned int num_pixels)

  Summary:
    Read pixel data from specified position in ILI9488 GRAM.

  Description:
    This function will first write the start column, page information, then
    read the pixel data from the ILI9488 GRAM.

  Parameters:
    drv             - ILI9488 driver handle
    x               - Column position
    y               - Page position
    value           - Value to store the read pixel color (8-bit/pixel RGB)
    num_pixels      - Number of pixels to read
 
  Returns:
    * GFX_SUCCESS       - Operation successful
    * GFX_FAILURE       - Operation failed

  Remarks
    For SPI mode, this function performs multiple full-blocking write/read calls
    to the SPI port and won't return until the SPI transaction completes.

 */
GFX_Result ILI9488_Intf_ReadPixels(struct ILI9488_DRV *drv,
                                  uint32_t x,
                                  uint32_t y,
                                  uint8_t *value,
                                  unsigned int num_pixels)
{
    GFX_Result returnValue = GFX_FAILURE;
    uint8_t cmd = ILI9488_CMD_MEMORY_READ;
    uint8_t dummy;
    uint8_t buf[4];
    GFX_Disp_Intf intf;

    if (!drv)
        return GFX_FAILURE;

    intf = (GFX_Disp_Intf) drv->port_priv;
    
    ILI9488_SPI_SS_Assert(intf);

    //Set column
    buf[0] = ((x & 0xff00) >> 8);
    buf[1] = (x & 0xff);
    buf[2] = (((drv->gfx->display_info->rect.width - 1) & 0xff00) >> 8);
    buf[3] = ((drv->gfx->display_info->rect.width - 1) & 0xff);
    returnValue = GFX_Disp_Intf_WriteCommandParm(intf,
                                       ILI9488_CMD_COLUMN_ADDRESS_SET,
                                       buf,
                                       4);
    if (GFX_SUCCESS != returnValue)
        return GFX_FAILURE;

    //Set page
    buf[0] = ((y & 0xff00) >> 8);
    buf[1] = (y & 0xff);
    buf[2] = (((drv->gfx->display_info->rect.height - 1) & 0xff00) >> 8);
    buf[3] = ((drv->gfx->display_info->rect.height - 1) & 0xff);
    returnValue = GFX_Disp_Intf_WriteCommandParm(intf,
                                       ILI9488_CMD_PAGE_ADDRESS_SET,
                                       buf,
                                       4);
    if (GFX_SUCCESS != returnValue)
    {
        ILI9488_SPI_SS_Deassert(intf);
        return GFX_FAILURE;
    }

    //Assert SS = LOW and D/CX = LOW (command)
    ILI9488_SPI_DCX_Command(intf);
    ILI9488_SPI_SS_Assert(intf);

    returnValue = GFX_Disp_Intf_Write(intf, &cmd, 1);
    if (GFX_SUCCESS != returnValue)
    {
        ILI9488_SPI_SS_Deassert(intf);        
        return GFX_FAILURE;
    }

    ILI9488_SPI_DCX_Data(intf);

    // Read the dummy byte
    returnValue = GFX_Disp_Intf_Read(intf, &dummy, 1);
    if (GFX_SUCCESS != returnValue)
        return GFX_FAILURE;

// BA changed the number of bytes per pixel because MHC was generating the code wrong
    returnValue = GFX_Disp_Intf_Read(intf, value, num_pixels * 2);
    if (GFX_SUCCESS != returnValue)
        return GFX_FAILURE;

    ILI9488_SPI_SS_Deassert(intf);

    return returnValue;
}

/** 
   Function:
    GFX_Result ILI9488_Intf_ReadCmd(struct ILI9488_DRV *drv, 
                                          uint8_t cmd, 
                                          uint8_t *data,
                                          int bytes);

  Summary:
    Sends read command and reads response from ILI9488.

  Description:
    This function will fist write the the read command and then read back the 
    response from the ILI9488 GRAM.

  Parameters:
    drv             - ILI9488 driver handle
    cmd             - Read command
    data            - Buffer to store the read data to
    bytes           - Number of bytes to read
 
  Returns:
    * GFX_SUCCESS       Operation successful
    * GFX_FAILURE       Operation failed
 
  Remarks:
    This function only supports 8-, 24- or 32-bit reads.
    In SPI mode, this function performs multiple full-blocking write/read calls 
    to the SPI port and won't return until the SPI transaction completes.

 */
GFX_Result ILI9488_Intf_ReadCmd(struct ILI9488_DRV *drv,
                               uint8_t cmd,
                               uint8_t *data,
                               int bytes)
{
    GFX_Result returnValue = GFX_FAILURE;
    uint8_t buff[5];

    //API supports only 8-, 24-, or 32-bit reads
    if ((!drv) || (!data) ||
        ((bytes != 1) && (bytes != 3) && (bytes != 4)))
        return GFX_FAILURE;

    returnValue = ILI9488_Intf_Read(drv, cmd, buff, bytes + 1);
    if (returnValue == GFX_SUCCESS)
    {
        switch (bytes)
        {
            case 1:
                data[0] = buff[0];
                break;
                // For 3 or 4-byte reads, ili9488 requires an extra dummy clock 
                // before read data becomes available so shift left 1 bit, and 
                // use MSB of next bytes as LSB
            case 4:
                data[3] = (buff[3] << 1) | (buff[4] >> 7);
                // no break, fall through
            case 3:
                data[2] = (buff[2] << 1) | (buff[3] >> 7);
                data[1] = (buff[1] << 1) | (buff[2] >> 7);
                data[0] = (buff[0] << 1) | (buff[1] >> 7);
                break;
            default:
                break;
        }
    }

    return returnValue;
}

// *****************************************************************************

/** 
  Function:
    GFX_Result ILI9488_Intf_Open(ILI9488_DRV *drv)

  Summary:
    Opens the specified port to the ILI9488 device.

  Description:
    In SPI mode, this function will open the SPI port, allocate the port-specific
    data structures and set the port operation handler functions. When done 
    using the port, ILI9488_Intf_Close must be called to free up the data 
    structures and close the port.

  Parameters:
    drv         - ILI9488 driver handle

  Returns:
    * GFX_SUCCESS       - Operation successful
    * GFX_FAILURE       - Operation failed

 */
GFX_Result ILI9488_Intf_Open(ILI9488_DRV *drv)
{
    if (!drv)
        return GFX_FAILURE;

    drv->port_priv = (void *) GFX_Disp_Intf_Open(drv->gfx);

    return GFX_SUCCESS;
}

/** 
  Function:
    void ILI9488_Intf_Close(ILI9488_DRV *drv)

  Summary:
    Closes the HW interface to the ILI9488 device.

  Description:
    This function will close the specified interface, free the port-specific
    data structures and unset the port operation handler functions.

  Parameters:
    drv         - ILI9488 driver handle
 
  Returns:
    None.

 */
void ILI9488_Intf_Close(ILI9488_DRV *drv) 
{
    if (!drv)
        return;

    GFX_Disp_Intf_Close((GFX_Disp_Intf) drv->port_priv);

    drv->port_priv = NULL;
}
/* *****************************************************************************
 End of File
 */
