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
 Module for Microchip Graphics Library - Hardware Abstraction Layer

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_ili9488_cmd_defs.h

  Summary:
    Contains ILI9488 command definitions.

  Description:
    Command definitions for ILI9488 commands.
*******************************************************************************/


#ifndef DRV_GFX__ILI9488_CMD_DEFS_H    /* Guard against multiple inclusion */
#define DRV_GFX__ILI9488_CMD_DEFS_H

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

/* ************************************************************************** */
/** ILI9488_CMD_*
  Summary:
    Command defines.
*/
#define ILI9488_CMD_SLEEP_OUT                       0x11
#define ILI9488_CMD_DISPLAY_ON                      0x29
#define ILI9488_CMD_INTERFACE_PIXEL_FORMAT_SET      0x3A
#define ILI9488_CMD_SET_IMAGE_FUNCTION              0xE9
#define ILI9488_CMD_INTERFACE_MODE_CONTROL          0xB0
#define ILI9488_CMD_MEMORY_ACCESS_CONTROL           0x36
#define ILI9488_CMD_COLUMN_ADDRESS_SET              0x2A
#define ILI9488_CMD_PAGE_ADDRESS_SET                0x2B
#define ILI9488_CMD_MEMORY_WRITE                    0x2C
#define ILI9488_CMD_MEMORY_READ                     0x2E

/** ILI9488_COLOR_PIX_FMT_18BPP
  @Summary
    Color pixel format value for 18BPP
*/
#define ILI9488_COLOR_PIX_FMT_16BPP          0x5
#define ILI9488_COLOR_PIX_FMT_18BPP          0x6

/** ILI9488_MADCTL_*
  Summary:
    Memory access control command parameter bit values.
*/
#define ILI9488_MADCTL_RGB_BGR_ORDER_CTRL           (1 << 3)
#define ILI9488_MADCTL_ROW_COLUMN_EXCHANGE          (1 << 5)
#define ILI9488_MADCTL_COL_ADDR_ORDER               (1 << 6)
#define ILI9488_MADCTL_ROW_ADDR_ORDER               (1 << 7)

#ifdef __cplusplus
}
#endif

#endif /* DRV_GFX__ILI9488_CMD_DEFS_H */

/* *****************************************************************************
 End of File
 */
