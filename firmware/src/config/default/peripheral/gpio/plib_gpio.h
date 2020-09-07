/*******************************************************************************
  GPIO PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_gpio.h

  Summary:
    GPIO PLIB Header File

  Description:
    This library provides an interface to control and interact with Parallel
    Input/Output controller (GPIO) module.

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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

#ifndef PLIB_GPIO_H
#define PLIB_GPIO_H

#include <device.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data types and constants
// *****************************************************************************
// *****************************************************************************

/*** Macros for Button2 pin ***/
#define Button2_Set()               (LATESET = (1<<5))
#define Button2_Clear()             (LATECLR = (1<<5))
#define Button2_Toggle()            (LATEINV= (1<<5))
#define Button2_Get()               ((PORTE >> 5) & 0x1)
#define Button2_OutputEnable()      (TRISECLR = (1<<5))
#define Button2_InputEnable()       (TRISESET = (1<<5))
#define Button2_PIN                  GPIO_PIN_RE5
/*** Macros for Button1 pin ***/
#define Button1_Set()               (LATESET = (1<<6))
#define Button1_Clear()             (LATECLR = (1<<6))
#define Button1_Toggle()            (LATEINV= (1<<6))
#define Button1_Get()               ((PORTE >> 6) & 0x1)
#define Button1_OutputEnable()      (TRISECLR = (1<<6))
#define Button1_InputEnable()       (TRISESET = (1<<6))
#define Button1_PIN                  GPIO_PIN_RE6
/*** Macros for GPIO_RB5 pin ***/
#define GPIO_RB5_Set()               (LATBSET = (1<<5))
#define GPIO_RB5_Clear()             (LATBCLR = (1<<5))
#define GPIO_RB5_Toggle()            (LATBINV= (1<<5))
#define GPIO_RB5_Get()               ((PORTB >> 5) & 0x1)
#define GPIO_RB5_OutputEnable()      (TRISBCLR = (1<<5))
#define GPIO_RB5_InputEnable()       (TRISBSET = (1<<5))
#define GPIO_RB5_PIN                  GPIO_PIN_RB5
/*** Macros for GPIO_RB4 pin ***/
#define GPIO_RB4_Set()               (LATBSET = (1<<4))
#define GPIO_RB4_Clear()             (LATBCLR = (1<<4))
#define GPIO_RB4_Toggle()            (LATBINV= (1<<4))
#define GPIO_RB4_Get()               ((PORTB >> 4) & 0x1)
#define GPIO_RB4_OutputEnable()      (TRISBCLR = (1<<4))
#define GPIO_RB4_InputEnable()       (TRISBSET = (1<<4))
#define GPIO_RB4_PIN                  GPIO_PIN_RB4
/*** Macros for GPIO_RB3 pin ***/
#define GPIO_RB3_Set()               (LATBSET = (1<<3))
#define GPIO_RB3_Clear()             (LATBCLR = (1<<3))
#define GPIO_RB3_Toggle()            (LATBINV= (1<<3))
#define GPIO_RB3_Get()               ((PORTB >> 3) & 0x1)
#define GPIO_RB3_OutputEnable()      (TRISBCLR = (1<<3))
#define GPIO_RB3_InputEnable()       (TRISBSET = (1<<3))
#define GPIO_RB3_PIN                  GPIO_PIN_RB3
/*** Macros for GFX_DISP_INTF_PIN_RSDC pin ***/
#define GFX_DISP_INTF_PIN_RSDC_Set()               (LATBSET = (1<<2))
#define GFX_DISP_INTF_PIN_RSDC_Clear()             (LATBCLR = (1<<2))
#define GFX_DISP_INTF_PIN_RSDC_Toggle()            (LATBINV= (1<<2))
#define GFX_DISP_INTF_PIN_RSDC_Get()               ((PORTB >> 2) & 0x1)
#define GFX_DISP_INTF_PIN_RSDC_OutputEnable()      (TRISBCLR = (1<<2))
#define GFX_DISP_INTF_PIN_RSDC_InputEnable()       (TRISBSET = (1<<2))
#define GFX_DISP_INTF_PIN_RSDC_PIN                  GPIO_PIN_RB2
/*** Macros for GPIO_RB1 pin ***/
#define GPIO_RB1_Set()               (LATBSET = (1<<1))
#define GPIO_RB1_Clear()             (LATBCLR = (1<<1))
#define GPIO_RB1_Toggle()            (LATBINV= (1<<1))
#define GPIO_RB1_Get()               ((PORTB >> 1) & 0x1)
#define GPIO_RB1_OutputEnable()      (TRISBCLR = (1<<1))
#define GPIO_RB1_InputEnable()       (TRISBSET = (1<<1))
#define GPIO_RB1_PIN                  GPIO_PIN_RB1
/*** Macros for GPIO_RB0 pin ***/
#define GPIO_RB0_Set()               (LATBSET = (1<<0))
#define GPIO_RB0_Clear()             (LATBCLR = (1<<0))
#define GPIO_RB0_Toggle()            (LATBINV= (1<<0))
#define GPIO_RB0_Get()               ((PORTB >> 0) & 0x1)
#define GPIO_RB0_OutputEnable()      (TRISBCLR = (1<<0))
#define GPIO_RB0_InputEnable()       (TRISBSET = (1<<0))
#define GPIO_RB0_PIN                  GPIO_PIN_RB0
/*** Macros for GFX_DISP_INTF_PIN_RESET pin ***/
#define GFX_DISP_INTF_PIN_RESET_Set()               (LATBSET = (1<<7))
#define GFX_DISP_INTF_PIN_RESET_Clear()             (LATBCLR = (1<<7))
#define GFX_DISP_INTF_PIN_RESET_Toggle()            (LATBINV= (1<<7))
#define GFX_DISP_INTF_PIN_RESET_Get()               ((PORTB >> 7) & 0x1)
#define GFX_DISP_INTF_PIN_RESET_OutputEnable()      (TRISBCLR = (1<<7))
#define GFX_DISP_INTF_PIN_RESET_InputEnable()       (TRISBSET = (1<<7))
#define GFX_DISP_INTF_PIN_RESET_PIN                  GPIO_PIN_RB7
/*** Macros for Selector2 pin ***/
#define Selector2_Set()               (LATBSET = (1<<11))
#define Selector2_Clear()             (LATBCLR = (1<<11))
#define Selector2_Toggle()            (LATBINV= (1<<11))
#define Selector2_Get()               ((PORTB >> 11) & 0x1)
#define Selector2_OutputEnable()      (TRISBCLR = (1<<11))
#define Selector2_InputEnable()       (TRISBSET = (1<<11))
#define Selector2_PIN                  GPIO_PIN_RB11
/*** Macros for Selector3 pin ***/
#define Selector3_Set()               (LATBSET = (1<<12))
#define Selector3_Clear()             (LATBCLR = (1<<12))
#define Selector3_Toggle()            (LATBINV= (1<<12))
#define Selector3_Get()               ((PORTB >> 12) & 0x1)
#define Selector3_OutputEnable()      (TRISBCLR = (1<<12))
#define Selector3_InputEnable()       (TRISBSET = (1<<12))
#define Selector3_PIN                  GPIO_PIN_RB12
/*** Macros for Selector4 pin ***/
#define Selector4_Set()               (LATBSET = (1<<13))
#define Selector4_Clear()             (LATBCLR = (1<<13))
#define Selector4_Toggle()            (LATBINV= (1<<13))
#define Selector4_Get()               ((PORTB >> 13) & 0x1)
#define Selector4_OutputEnable()      (TRISBCLR = (1<<13))
#define Selector4_InputEnable()       (TRISBSET = (1<<13))
#define Selector4_PIN                  GPIO_PIN_RB13
/*** Macros for Selector6 pin ***/
#define Selector6_Set()               (LATBSET = (1<<15))
#define Selector6_Clear()             (LATBCLR = (1<<15))
#define Selector6_Toggle()            (LATBINV= (1<<15))
#define Selector6_Get()               ((PORTB >> 15) & 0x1)
#define Selector6_OutputEnable()      (TRISBCLR = (1<<15))
#define Selector6_InputEnable()       (TRISBSET = (1<<15))
#define Selector6_PIN                  GPIO_PIN_RB15
/*** Macros for GPIO_RC12 pin ***/
#define GPIO_RC12_Set()               (LATCSET = (1<<12))
#define GPIO_RC12_Clear()             (LATCCLR = (1<<12))
#define GPIO_RC12_Toggle()            (LATCINV= (1<<12))
#define GPIO_RC12_Get()               ((PORTC >> 12) & 0x1)
#define GPIO_RC12_OutputEnable()      (TRISCCLR = (1<<12))
#define GPIO_RC12_InputEnable()       (TRISCSET = (1<<12))
#define GPIO_RC12_PIN                  GPIO_PIN_RC12
/*** Macros for GPIO_RC15 pin ***/
#define GPIO_RC15_Set()               (LATCSET = (1<<15))
#define GPIO_RC15_Clear()             (LATCCLR = (1<<15))
#define GPIO_RC15_Toggle()            (LATCINV= (1<<15))
#define GPIO_RC15_Get()               ((PORTC >> 15) & 0x1)
#define GPIO_RC15_OutputEnable()      (TRISCCLR = (1<<15))
#define GPIO_RC15_InputEnable()       (TRISCSET = (1<<15))
#define GPIO_RC15_PIN                  GPIO_PIN_RC15
/*** Macros for GPIO_RF3 pin ***/
#define GPIO_RF3_Set()               (LATFSET = (1<<3))
#define GPIO_RF3_Clear()             (LATFCLR = (1<<3))
#define GPIO_RF3_Toggle()            (LATFINV= (1<<3))
#define GPIO_RF3_Get()               ((PORTF >> 3) & 0x1)
#define GPIO_RF3_OutputEnable()      (TRISFCLR = (1<<3))
#define GPIO_RF3_InputEnable()       (TRISFSET = (1<<3))
#define GPIO_RF3_PIN                  GPIO_PIN_RF3
/*** Macros for PrimaryF4 pin ***/
#define PrimaryF4_Set()               (LATFSET = (1<<4))
#define PrimaryF4_Clear()             (LATFCLR = (1<<4))
#define PrimaryF4_Toggle()            (LATFINV= (1<<4))
#define PrimaryF4_Get()               ((PORTF >> 4) & 0x1)
#define PrimaryF4_OutputEnable()      (TRISFCLR = (1<<4))
#define PrimaryF4_InputEnable()       (TRISFSET = (1<<4))
#define PrimaryF4_PIN                  GPIO_PIN_RF4
/*** Macros for Aux1F5 pin ***/
#define Aux1F5_Set()               (LATFSET = (1<<5))
#define Aux1F5_Clear()             (LATFCLR = (1<<5))
#define Aux1F5_Toggle()            (LATFINV= (1<<5))
#define Aux1F5_Get()               ((PORTF >> 5) & 0x1)
#define Aux1F5_OutputEnable()      (TRISFCLR = (1<<5))
#define Aux1F5_InputEnable()       (TRISFSET = (1<<5))
#define Aux1F5_PIN                  GPIO_PIN_RF5
/*** Macros for Aux1D9 pin ***/
#define Aux1D9_Set()               (LATDSET = (1<<9))
#define Aux1D9_Clear()             (LATDCLR = (1<<9))
#define Aux1D9_Toggle()            (LATDINV= (1<<9))
#define Aux1D9_Get()               ((PORTD >> 9) & 0x1)
#define Aux1D9_OutputEnable()      (TRISDCLR = (1<<9))
#define Aux1D9_InputEnable()       (TRISDSET = (1<<9))
#define Aux1D9_PIN                  GPIO_PIN_RD9
/*** Macros for Aux1D10 pin ***/
#define Aux1D10_Set()               (LATDSET = (1<<10))
#define Aux1D10_Clear()             (LATDCLR = (1<<10))
#define Aux1D10_Toggle()            (LATDINV= (1<<10))
#define Aux1D10_Get()               ((PORTD >> 10) & 0x1)
#define Aux1D10_OutputEnable()      (TRISDCLR = (1<<10))
#define Aux1D10_InputEnable()       (TRISDSET = (1<<10))
#define Aux1D10_PIN                  GPIO_PIN_RD10
/*** Macros for PrimaryD11 pin ***/
#define PrimaryD11_Set()               (LATDSET = (1<<11))
#define PrimaryD11_Clear()             (LATDCLR = (1<<11))
#define PrimaryD11_Toggle()            (LATDINV= (1<<11))
#define PrimaryD11_Get()               ((PORTD >> 11) & 0x1)
#define PrimaryD11_OutputEnable()      (TRISDCLR = (1<<11))
#define PrimaryD11_InputEnable()       (TRISDSET = (1<<11))
#define PrimaryD11_PIN                  GPIO_PIN_RD11
/*** Macros for PrimaryD0 pin ***/
#define PrimaryD0_Set()               (LATDSET = (1<<0))
#define PrimaryD0_Clear()             (LATDCLR = (1<<0))
#define PrimaryD0_Toggle()            (LATDINV= (1<<0))
#define PrimaryD0_Get()               ((PORTD >> 0) & 0x1)
#define PrimaryD0_OutputEnable()      (TRISDCLR = (1<<0))
#define PrimaryD0_InputEnable()       (TRISDSET = (1<<0))
#define PrimaryD0_PIN                  GPIO_PIN_RD0
/*** Macros for Selector5 pin ***/
#define Selector5_Set()               (LATCSET = (1<<13))
#define Selector5_Clear()             (LATCCLR = (1<<13))
#define Selector5_Toggle()            (LATCINV= (1<<13))
#define Selector5_Get()               ((PORTC >> 13) & 0x1)
#define Selector5_OutputEnable()      (TRISCCLR = (1<<13))
#define Selector5_InputEnable()       (TRISCSET = (1<<13))
#define Selector5_PIN                  GPIO_PIN_RC13
/*** Macros for Selector7 pin ***/
#define Selector7_Set()               (LATCSET = (1<<14))
#define Selector7_Clear()             (LATCCLR = (1<<14))
#define Selector7_Toggle()            (LATCINV= (1<<14))
#define Selector7_Get()               ((PORTC >> 14) & 0x1)
#define Selector7_OutputEnable()      (TRISCCLR = (1<<14))
#define Selector7_InputEnable()       (TRISCSET = (1<<14))
#define Selector7_PIN                  GPIO_PIN_RC14
/*** Macros for Aux1D1 pin ***/
#define Aux1D1_Set()               (LATDSET = (1<<1))
#define Aux1D1_Clear()             (LATDCLR = (1<<1))
#define Aux1D1_Toggle()            (LATDINV= (1<<1))
#define Aux1D1_Get()               ((PORTD >> 1) & 0x1)
#define Aux1D1_OutputEnable()      (TRISDCLR = (1<<1))
#define Aux1D1_InputEnable()       (TRISDSET = (1<<1))
#define Aux1D1_PIN                  GPIO_PIN_RD1
/*** Macros for Selector9 pin ***/
#define Selector9_Set()               (LATDSET = (1<<2))
#define Selector9_Clear()             (LATDCLR = (1<<2))
#define Selector9_Toggle()            (LATDINV= (1<<2))
#define Selector9_Get()               ((PORTD >> 2) & 0x1)
#define Selector9_OutputEnable()      (TRISDCLR = (1<<2))
#define Selector9_InputEnable()       (TRISDSET = (1<<2))
#define Selector9_PIN                  GPIO_PIN_RD2
/*** Macros for Selector8 pin ***/
#define Selector8_Set()               (LATDSET = (1<<3))
#define Selector8_Clear()             (LATDCLR = (1<<3))
#define Selector8_Toggle()            (LATDINV= (1<<3))
#define Selector8_Get()               ((PORTD >> 3) & 0x1)
#define Selector8_OutputEnable()      (TRISDCLR = (1<<3))
#define Selector8_InputEnable()       (TRISDSET = (1<<3))
#define Selector8_PIN                  GPIO_PIN_RD3
/*** Macros for Aux2D4 pin ***/
#define Aux2D4_Set()               (LATDSET = (1<<4))
#define Aux2D4_Clear()             (LATDCLR = (1<<4))
#define Aux2D4_Toggle()            (LATDINV= (1<<4))
#define Aux2D4_Get()               ((PORTD >> 4) & 0x1)
#define Aux2D4_OutputEnable()      (TRISDCLR = (1<<4))
#define Aux2D4_InputEnable()       (TRISDSET = (1<<4))
#define Aux2D4_PIN                  GPIO_PIN_RD4
/*** Macros for Aux2D5 pin ***/
#define Aux2D5_Set()               (LATDSET = (1<<5))
#define Aux2D5_Clear()             (LATDCLR = (1<<5))
#define Aux2D5_Toggle()            (LATDINV= (1<<5))
#define Aux2D5_Get()               ((PORTD >> 5) & 0x1)
#define Aux2D5_OutputEnable()      (TRISDCLR = (1<<5))
#define Aux2D5_InputEnable()       (TRISDSET = (1<<5))
#define Aux2D5_PIN                  GPIO_PIN_RD5
/*** Macros for Aux2F0 pin ***/
#define Aux2F0_Set()               (LATFSET = (1<<0))
#define Aux2F0_Clear()             (LATFCLR = (1<<0))
#define Aux2F0_Toggle()            (LATFINV= (1<<0))
#define Aux2F0_Get()               ((PORTF >> 0) & 0x1)
#define Aux2F0_OutputEnable()      (TRISFCLR = (1<<0))
#define Aux2F0_InputEnable()       (TRISFSET = (1<<0))
#define Aux2F0_PIN                  GPIO_PIN_RF0
/*** Macros for Aux2F1 pin ***/
#define Aux2F1_Set()               (LATFSET = (1<<1))
#define Aux2F1_Clear()             (LATFCLR = (1<<1))
#define Aux2F1_Toggle()            (LATFINV= (1<<1))
#define Aux2F1_Get()               ((PORTF >> 1) & 0x1)
#define Aux2F1_OutputEnable()      (TRISFCLR = (1<<1))
#define Aux2F1_InputEnable()       (TRISFSET = (1<<1))
#define Aux2F1_PIN                  GPIO_PIN_RF1
/*** Macros for GPIO_RE0 pin ***/
#define GPIO_RE0_Set()               (LATESET = (1<<0))
#define GPIO_RE0_Clear()             (LATECLR = (1<<0))
#define GPIO_RE0_Toggle()            (LATEINV= (1<<0))
#define GPIO_RE0_Get()               ((PORTE >> 0) & 0x1)
#define GPIO_RE0_OutputEnable()      (TRISECLR = (1<<0))
#define GPIO_RE0_InputEnable()       (TRISESET = (1<<0))
#define GPIO_RE0_PIN                  GPIO_PIN_RE0
/*** Macros for Button3 pin ***/
#define Button3_Set()               (LATESET = (1<<1))
#define Button3_Clear()             (LATECLR = (1<<1))
#define Button3_Toggle()            (LATEINV= (1<<1))
#define Button3_Get()               ((PORTE >> 1) & 0x1)
#define Button3_OutputEnable()      (TRISECLR = (1<<1))
#define Button3_InputEnable()       (TRISESET = (1<<1))
#define Button3_PIN                  GPIO_PIN_RE1
/*** Macros for Button4 pin ***/
#define Button4_Set()               (LATESET = (1<<2))
#define Button4_Clear()             (LATECLR = (1<<2))
#define Button4_Toggle()            (LATEINV= (1<<2))
#define Button4_Get()               ((PORTE >> 2) & 0x1)
#define Button4_OutputEnable()      (TRISECLR = (1<<2))
#define Button4_InputEnable()       (TRISESET = (1<<2))
#define Button4_PIN                  GPIO_PIN_RE2
/*** Macros for Button5 pin ***/
#define Button5_Set()               (LATESET = (1<<3))
#define Button5_Clear()             (LATECLR = (1<<3))
#define Button5_Toggle()            (LATEINV= (1<<3))
#define Button5_Get()               ((PORTE >> 3) & 0x1)
#define Button5_OutputEnable()      (TRISECLR = (1<<3))
#define Button5_InputEnable()       (TRISESET = (1<<3))
#define Button5_PIN                  GPIO_PIN_RE3
/*** Macros for Button6 pin ***/
#define Button6_Set()               (LATESET = (1<<4))
#define Button6_Clear()             (LATECLR = (1<<4))
#define Button6_Toggle()            (LATEINV= (1<<4))
#define Button6_Get()               ((PORTE >> 4) & 0x1)
#define Button6_OutputEnable()      (TRISECLR = (1<<4))
#define Button6_InputEnable()       (TRISESET = (1<<4))
#define Button6_PIN                  GPIO_PIN_RE4


// *****************************************************************************
/* GPIO Port

  Summary:
    Identifies the available GPIO Ports.

  Description:
    This enumeration identifies the available GPIO Ports.

  Remarks:
    The caller should not rely on the specific numbers assigned to any of
    these values as they may change from one processor to the next.

    Not all ports are available on all devices.  Refer to the specific
    device data sheet to determine which ports are supported.
*/

typedef enum
{
    GPIO_PORT_B = 0,
    GPIO_PORT_C = 1,
    GPIO_PORT_D = 2,
    GPIO_PORT_E = 3,
    GPIO_PORT_F = 4,
    GPIO_PORT_G = 5,
} GPIO_PORT;

// *****************************************************************************
/* GPIO Port Pins

  Summary:
    Identifies the available GPIO port pins.

  Description:
    This enumeration identifies the available GPIO port pins.

  Remarks:
    The caller should not rely on the specific numbers assigned to any of
    these values as they may change from one processor to the next.

    Not all pins are available on all devices.  Refer to the specific
    device data sheet to determine which pins are supported.
*/

typedef enum
{
    GPIO_PIN_RB0 = 0,
    GPIO_PIN_RB1 = 1,
    GPIO_PIN_RB2 = 2,
    GPIO_PIN_RB3 = 3,
    GPIO_PIN_RB4 = 4,
    GPIO_PIN_RB5 = 5,
    GPIO_PIN_RB6 = 6,
    GPIO_PIN_RB7 = 7,
    GPIO_PIN_RB8 = 8,
    GPIO_PIN_RB9 = 9,
    GPIO_PIN_RB10 = 10,
    GPIO_PIN_RB11 = 11,
    GPIO_PIN_RB12 = 12,
    GPIO_PIN_RB13 = 13,
    GPIO_PIN_RB14 = 14,
    GPIO_PIN_RB15 = 15,
    GPIO_PIN_RC12 = 28,
    GPIO_PIN_RC13 = 29,
    GPIO_PIN_RC14 = 30,
    GPIO_PIN_RC15 = 31,
    GPIO_PIN_RD0 = 32,
    GPIO_PIN_RD1 = 33,
    GPIO_PIN_RD2 = 34,
    GPIO_PIN_RD3 = 35,
    GPIO_PIN_RD4 = 36,
    GPIO_PIN_RD5 = 37,
    GPIO_PIN_RD9 = 41,
    GPIO_PIN_RD10 = 42,
    GPIO_PIN_RD11 = 43,
    GPIO_PIN_RE0 = 48,
    GPIO_PIN_RE1 = 49,
    GPIO_PIN_RE2 = 50,
    GPIO_PIN_RE3 = 51,
    GPIO_PIN_RE4 = 52,
    GPIO_PIN_RE5 = 53,
    GPIO_PIN_RE6 = 54,
    GPIO_PIN_RE7 = 55,
    GPIO_PIN_RF0 = 64,
    GPIO_PIN_RF1 = 65,
    GPIO_PIN_RF3 = 67,
    GPIO_PIN_RF4 = 68,
    GPIO_PIN_RF5 = 69,
    GPIO_PIN_RG6 = 86,
    GPIO_PIN_RG7 = 87,
    GPIO_PIN_RG8 = 88,
    GPIO_PIN_RG9 = 89,

    /* This element should not be used in any of the GPIO APIs.
       It will be used by other modules or application to denote that none of the GPIO Pin is used */
    GPIO_PIN_NONE = -1

} GPIO_PIN;


void GPIO_Initialize(void);

// *****************************************************************************
// *****************************************************************************
// Section: GPIO Functions which operates on multiple pins of a port
// *****************************************************************************
// *****************************************************************************

uint32_t GPIO_PortRead(GPIO_PORT port);

void GPIO_PortWrite(GPIO_PORT port, uint32_t mask, uint32_t value);

uint32_t GPIO_PortLatchRead ( GPIO_PORT port );

void GPIO_PortSet(GPIO_PORT port, uint32_t mask);

void GPIO_PortClear(GPIO_PORT port, uint32_t mask);

void GPIO_PortToggle(GPIO_PORT port, uint32_t mask);

void GPIO_PortInputEnable(GPIO_PORT port, uint32_t mask);

void GPIO_PortOutputEnable(GPIO_PORT port, uint32_t mask);

// *****************************************************************************
// *****************************************************************************
// Section: GPIO Functions which operates on one pin at a time
// *****************************************************************************
// *****************************************************************************

static inline void GPIO_PinWrite(GPIO_PIN pin, bool value)
{
    GPIO_PortWrite((GPIO_PORT) (pin>>4), (uint32_t)(0x1) << (pin & 0xF), (uint32_t)(value) << (pin & 0xF));
}

static inline bool GPIO_PinRead(GPIO_PIN pin)
{
    return (bool)(((GPIO_PortRead((GPIO_PORT) (pin>>4))) >> (pin & 0xF)) & 0x1);
}

static inline bool GPIO_PinLatchRead(GPIO_PIN pin)
{
    return (bool)((GPIO_PortLatchRead((GPIO_PORT) (pin>>4)) >> (pin & 0xF)) & 0x1);
}

static inline void GPIO_PinToggle(GPIO_PIN pin)
{
    GPIO_PortToggle((GPIO_PORT) (pin>>4), 0x1 << (pin & 0xF));
}

static inline void GPIO_PinSet(GPIO_PIN pin)
{
    GPIO_PortSet((GPIO_PORT) (pin>>4), 0x1 << (pin & 0xF));
}

static inline void GPIO_PinClear(GPIO_PIN pin)
{
    GPIO_PortClear((GPIO_PORT) (pin>>4), 0x1 << (pin & 0xF));
}

static inline void GPIO_PinInputEnable(GPIO_PIN pin)
{
    GPIO_PortInputEnable((GPIO_PORT) (pin>>4), 0x1 << (pin & 0xF));
}

static inline void GPIO_PinOutputEnable(GPIO_PIN pin)
{
    GPIO_PortOutputEnable((GPIO_PORT) (pin>>4), 0x1 << (pin & 0xF));
}


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END
#endif // PLIB_GPIO_H
