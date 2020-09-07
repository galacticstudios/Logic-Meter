/*******************************************************************************
  USBHS Peripheral Library Template Implementation

  File Name:
    usbhs_EndpointFIFO_Unsupported.h

  Summary:
    USBHS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : EndpointFIFO
    and its Variant : Unsupported
    For following APIs :
        PLIB_USBHS_EndpointFIFOLoad
        PLIB_USBHS_EndpointFIFOUnload
        PLIB_USBHS_Endpoint0FIFOFlush
        PLIB_USBHS_Endpoint0SetupPacketLoad
        PLIB_USBHS_ExistsEndpointFIFO

*******************************************************************************/

//DOM-IGNORE-BEGIN
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

//DOM-IGNORE-END

#ifndef _USBHS_ENDPOINTFIFO_UNSUPPORTED_H
#define _USBHS_ENDPOINTFIFO_UNSUPPORTED_H

//******************************************************************************
/* Function :  USBHS_EndpointFIFOLoad_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_EndpointFIFOLoad 

  Description:
    This template implements the Unsupported variant of the
    PLIB_USBHS_EndpointFIFOLoad function.
*/

PLIB_TEMPLATE void USBHS_EndpointFIFOLoad_Unsupported
( 
    USBHS_MODULE_ID index, 
    uint8_t endpoint, 
    void * source, 
    size_t nBytes 
)
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_EndpointFIFOLoad");
}

//******************************************************************************
/* Function :  USBHS_EndpointFIFOUnload_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_EndpointFIFOUnload 

  Description:
    This template implements the Unsupported variant of the
    PLIB_USBHS_EndpointFIFOUnload function.
*/

PLIB_TEMPLATE int USBHS_EndpointFIFOUnload_Unsupported
( 
    USBHS_MODULE_ID index, 
    uint8_t endpoint, 
    void * dest 
)
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_EndpointFIFOUnload");

    return 0;
}

//******************************************************************************
/* Function :  USBHS_Endpoint0FIFOFlush_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_Endpoint0FIFOFlush 

  Description:
    This template implements the Unsupported variant of the
    PLIB_USBHS_Endpoint0FIFOFlush function.
*/

PLIB_TEMPLATE void USBHS_Endpoint0FIFOFlush_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_Endpoint0FIFOFlush");
}

//******************************************************************************
/* Function :  USBHS_Endpoint0SetupPacketLoad_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_Endpoint0SetupPacketLoad 

  Description:
    This template implements the Unsupported variant of the
    PLIB_USBHS_Endpoint0SetupPacketLoad function.
*/

PLIB_TEMPLATE void USBHS_Endpoint0SetupPacketLoad_Unsupported
( 
    USBHS_MODULE_ID index, 
    void * setupPacket, 
    uint8_t deviceAddress, 
    uint8_t hubAddress, 
    uint8_t hubPortAddress, 
    USB_SPEED speed
)
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_Endpoint0SetupPacketLoad");
}

//******************************************************************************
/* Function :  USBHS_ExistsEndpointFIFO_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_ExistsEndpointFIFO

  Description:
    This template implements the Unsupported variant of the
    PLIB_USBHS_ExistsEndpointFIFO function.
*/

PLIB_TEMPLATE bool USBHS_ExistsEndpointFIFO_Unsupported( USBHS_MODULE_ID index )
{
    return false;
}


#endif /*_USBHS_ENDPOINTFIFO_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

