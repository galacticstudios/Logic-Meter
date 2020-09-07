/*******************************************************************************
  USBHS Peripheral Library Template Implementation

  File Name:
    usbhs_EndpointFIFO_Default.h

  Summary:
    USBHS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : EndpointFIFO
    and its Variant : Default
    For following APIs :
        PLIB_USBHS_EndpointFIFOLoad
        PLIB_USBHS_EndpointFIFOUnload
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

#ifndef _USBHS_ENDPOINTFIFO_DEFAULT_H
#define _USBHS_ENDPOINTFIFO_DEFAULT_H

#include "usbhs_registers.h"


//******************************************************************************
/* Function :  USBHS_EndpointFIFOLoad_Default

  Summary:
    Implements Default variant of PLIB_USBHS_EndpointFIFOLoad 

  Description:
    This template implements the Default variant of the
    PLIB_USBHS_EndpointFIFOLoad function.
*/

void PLIB_TEMPLATE USBHS_EndpointFIFOLoad_Default
( 
    USBHS_MODULE_ID index , 
    uint8_t endpoint , 
    void * source , 
    size_t nBytes 
)
{
    /* This function loads the FIFO and then sends the packet */
    
    volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);
    volatile uint8_t * endpointFIFO;
    size_t i;

    /* Obtain pointers to the endpoint specific FIFO and control registers */
    endpointFIFO = (uint8_t *)(&usbhs->FIFO[endpoint]);
    
    /* Load the endpoint FIFO with the user data */
    for(i = 0; i < nBytes; i ++)
    {
        *endpointFIFO = *((uint8_t *)(source) + i);
    }

    /* Set the TXPKTRDY bit. The position of this bit is different for endpoint
     * 0 and other endpoints. */
    if(endpoint == 0)
    {
        usbhs->EPCSR[0].CSR0L_DEVICEbits.TXPKTRDY = 1;
    }
    else
    {
        usbhs->EPCSR[endpoint].TXCSRL_DEVICEbits.TXPKTRDY = 1;
    }
}

//******************************************************************************
/* Function :  USBHS_DeviceEPFIFOLoad_Default

  Summary:
    Implements Default variant of PLIB_USBHS_DeviceEPFIFOLoad 

  Description:
    This template implements the Default variant of the
    PLIB_USBHS_DeviceEPFIFOLoad function.
*/

void PLIB_TEMPLATE USBHS_DeviceEPFIFOLoad_Default
( 
    USBHS_MODULE_ID index , 
    uint8_t endpoint, 
    void * source, 
    size_t nBytes 
)
{
    /* This function loads the FIFO */ 

    volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);
    volatile uint8_t * endpointFIFO;
    size_t i;

    /* Obtain pointers to the endpoint specific FIFO and control registers */
    endpointFIFO = (uint8_t *)(&usbhs->FIFO[endpoint]);
    
    /* Load the endpoint FIFO with the user data */
    for(i = 0; i < nBytes; i ++)
    {
        *endpointFIFO = *((uint8_t *)(source) + i);
    }
}

//******************************************************************************
/* Function :  USBHS_EndpointFIFOUnload_Default

  Summary:
    Implements Default variant of PLIB_USBHS_EndpointFIFOUnload 

  Description:
    This template implements the Default variant of the
    PLIB_USBHS_EndpointFIFOUnload function.
*/

int PLIB_TEMPLATE USBHS_EndpointFIFOUnload_Default
( 
    USBHS_MODULE_ID index, 
    uint8_t endpoint, 
    void * dest
)
{
    /* This function unloads the FIFO and then clears the RX packet ready bit */

    volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);
    volatile uint8_t * fifo, * data;
    volatile unsigned int count;
    size_t i;

    /* Get the pointer to endpoint specific FIFO and control registers. The
     * pointer are obtained by adding an offset to the EP0 FIFO and EP0 Control
     * Register addresses. The offset is based on the input endpoint. */

    fifo = (uint8_t *)(&usbhs->FIFO[endpoint]);
    
    /* This is the size of the data contained in the FIFO */
    count = usbhs->EPCSR[endpoint].RXCOUNTbits.RXCNT;
    data = (uint8_t *) dest;

    /* fifo now has the FIFO address for this endpoint.  Now unload the FIFO
     * into data */

    for(i = 0; i < count; i ++)
    {
        data[i] = *(fifo + (i & 3));
    }

    /* The offset of the RX endpoint control register is different for endpoint
     * 0 and other endpoints. Clear the RXPKDTRY bits after unloading the FIFO */

    if(endpoint == 0)
    {
        usbhs->EPCSR[0].CSR0L_DEVICEbits.RXPKTRDY = 0;
    }
    else
    {
        usbhs->EPCSR[endpoint].RXCSRL_DEVICEbits.RXPKTRDY = 0;
    }
       
    return(count);
}

//******************************************************************************
/* Function :  USBHS_DeviceEPFIFOUnload_Default

  Summary:
    Implements Default variant of PLIB_USBHS_DeviceEPFIFOUnload 

  Description:
    This template implements the Default variant of the
    PLIB_USBHS_DeviceEPFIFOUnload function.
*/

int PLIB_TEMPLATE USBHS_DeviceEPFIFOUnload_Default
( 
    USBHS_MODULE_ID index, 
    uint8_t endpoint, 
    void * dest
)
{
    /* This function unloads the FIFO and then clears the RX packet ready bit */

    volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);
    volatile uint8_t * fifo, * data;
    volatile unsigned int count;
    size_t i;

    /* Get the pointer to endpoint specific FIFO and control registers. The
     * pointer are obtained by adding an offset to the EP0 FIFO and EP0 Control
     * Register addresses. The offset is based on the input endpoint. */

    fifo = (uint8_t *)(&usbhs->FIFO[endpoint]);
    
    /* This is the size of the data contained in the FIFO */
    count = usbhs->EPCSR[endpoint].RXCOUNTbits.RXCNT;
    data = (uint8_t *) dest;

    /* fifo now has the FIFO address for this endpoint.  Now unload the FIFO
     * into data */

    for(i = 0; i < count; i ++)
    {
        data[i] = *(fifo + (i & 3));
    }
    
    return(count);
}

//******************************************************************************
/* Function :  USBHS_Endpoint0SetupPacketLoad_Default

  Summary:
    Implements Default variant of PLIB_USBHS_Endpoint0SetupPacketLoad 

  Description:
    This template implements the Default variant of the
    PLIB_USBHS_Endpoint0SetupPacketLoad function.
*/

void PLIB_TEMPLATE USBHS_Endpoint0SetupPacketLoad_Default
( 
    USBHS_MODULE_ID index, 
    void * setupPacket, 
    uint8_t deviceAddress, 
    uint8_t hubAddress, 
    uint8_t hubPortAddress, 
    uint32_t speed 
)
{
    volatile uint8_t * endpointFIFO;
    volatile uint8_t * data;
    unsigned int i;
    volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);
    
    data = (uint8_t *)setupPacket;

    /* Pointer to EP0 FIFO */
    endpointFIFO = (uint8_t *)(&usbhs->FIFO[0]);

    /* Load the endpoint FIFO with the setup packet */
    for(i = 0; i < 8; i++)
    {
        *endpointFIFO = *(data + i);
    }

    /* Set up the target device address, hub address and the hub port address
     * and the target device speed. */
    usbhs->TADDR[0].TXFUNCADDRbits.TXFADDR = deviceAddress;
    usbhs->TADDR[0].TXHUBADDRbits.TXHUBADDR = hubAddress;
    usbhs->TADDR[0].TXHUBPORTbits.TXHUBPRT = hubPortAddress;
    usbhs->EPCSR[0].TYPE0bits.SPEED = speed;
    
    /* Schedule the packet for transfer. The TXPKTRDY and SETUPKT bits should
     * be set together. */
    usbhs->EPCSR[0].CSR0L_HOSTbits.w |= (USBHS_HOST_EP0_TXPKTRDY_SET|USBHS_HOST_EP0_SETUPKT_SET);
}

//******************************************************************************
/* Function :  USBHS_Endpoint0SetupPacketUnload_Default

  Summary:
    Implements Default variant of PLIB_USBHS_Endpoint0SetupPacketUnload 

  Description:
    This template implements the Default variant of the
    PLIB_USBHS_Endpoint0SetupPacketUnload function.
*/

void PLIB_TEMPLATE USBHS_Endpoint0SetupPacketUnload_Default
( 
    USBHS_MODULE_ID index, 
    void * dest 
)
{
    /* Unloads a setup packet from endpoint 0 fifo. Get the endpoint 0 FIFO and
     * control register addresses. */
    volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);
    uint8_t * data = (uint8_t *)dest;

    /* Read the 8 byte packet as 2 4-byte words */
    *((uint32_t *)(data)) = usbhs->FIFO[0];
    *((uint32_t *)(data + 4)) = usbhs->FIFO[0];

    /* Let the hardware know that the packet has been unloaded */
    usbhs->EPCSR[0].CSR0L_DEVICEbits.SVCRPR = 1;
}

//******************************************************************************
/* Function :  USBHS_Endpoint0FIFOFlush_Default

  Summary:
    Implements Default variant of PLIB_USBHS_Endpoint0FIFOFlush 

  Description:
    This template implements the Default variant of the
    PLIB_USBHS_Endpoint0FIFOFlush function.
*/

void PLIB_TEMPLATE USBHS_Endpoint0FIFOFlush_Default( USBHS_MODULE_ID index )
{
    /* This function flushes the EP0 FIFO. The FIFO can be flushed only if it
     * contains data. Hence the check for TXPKTRDY or RXPKTRDY */

    volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);
 
    /* Check if transmit packet ready or if the receive packet ready is set. If
     * so then clear the flush */
    
    if((usbhs->EPCSR[0].CSR0L_DEVICEbits.TXPKTRDY) || (usbhs->EPCSR[0].CSR0L_DEVICEbits.RXPKTRDY))
    {
        usbhs->EPCSR[0].CSR0H_DEVICEbits.FLSHFIFO = 1;
    }
}

//******************************************************************************
/* Function :  USBHS_EndpointTxFIFOFlush_Default

  Summary:
    Implements Default variant of PLIB_USBHS_EndpointTxFIFOFlush 

  Description:
    This template implements the Default variant of the
    PLIB_USBHS_EndpointTxFIFOFlush function.
*/

void PLIB_TEMPLATE USBHS_EndpointTxFIFOFlush_Default
( 
    USBHS_MODULE_ID index, 
    uint8_t endpoint 
)
{
    /* This function will clear the FIFO for a non-zero endpoint */
   
    volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);
    if(usbhs->EPCSR[endpoint].TXCSRL_DEVICEbits.TXPKTRDY)
    {
        usbhs->EPCSR[endpoint].TXCSRL_DEVICEbits.FLUSH = 1;
    }
}

//******************************************************************************
/* Function :  USBHS_EndpointRxFIFOFlush_Default

  Summary:
    Implements Default variant of PLIB_USBHS_EndpointRxFIFOFlush 

  Description:
    This template implements the Default variant of the
    PLIB_USBHS_EndpointRxFIFOFlush function.
*/

void PLIB_TEMPLATE USBHS_EndpointRxFIFOFlush_Default
( 
    USBHS_MODULE_ID index, 
    uint8_t endpoint 
)
{
    /* Get the pointer to the endpoint Receive Control and Status register */
    volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);
    
    if(usbhs->EPCSR[endpoint].RXCSRL_DEVICEbits.RXPKTRDY == 1)
    {
        usbhs->EPCSR[endpoint].RXCSRL_DEVICEbits.FLUSH = 1;
    }
}

//******************************************************************************
/* Function :  USBHS_ExistsEndpointFIFO_Default

  Summary:
    Implements Default variant of PLIB_USBHS_ExistsEndpointFIFO

  Description:
    This template implements the Default variant of the
    PLIB_USBHS_ExistsEndpointFIFO function.
*/

#define PLIB_USBHS_ExistsEndpointFIFO PLIB_USBHS_ExistsEndpointFIFO
bool PLIB_TEMPLATE USBHS_ExistsEndpointFIFO_Default( USBHS_MODULE_ID index )
{
    return true;
}


#endif /*_USBHS_ENDPOINTFIFO_DEFAULT_H*/

/******************************************************************************
 End of File
*/

