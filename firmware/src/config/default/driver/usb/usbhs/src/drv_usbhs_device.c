/*******************************************************************************
  USB Device Driver Implementation of device mode operation routines

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usbhs_device.c

  Summary:
    USB Device Driver Dynamic Implementation of device mode operation routines

  Description:
    The USB device driver provides a simple interface to manage the USB
    modules on Microchip microcontrollers.  This file Implements the
    interface routines for the USB driver when operating in device mode.

    While building the driver from source, ALWAYS use this file in the build if
    device mode operation is required.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
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


#include "driver/usb/usbhs/src/drv_usbhs_local.h"
#include "driver/usb/usbhs/drv_usbhs.h"


/*****************************************************
 * This structure is a pointer to a set of USB Driver
 * Device mode functions. This set is exported to the
 * device layer when the device layer must use the
 * PIC32MZ USB Controller.
 ******************************************************/

DRV_USB_DEVICE_INTERFACE gDrvUSBHSDeviceInterface =
{
    .open = DRV_USBHS_Open,
    .close = DRV_USBHS_Close,
    .eventHandlerSet = DRV_USBHS_ClientEventCallBackSet,
    .deviceAddressSet = DRV_USBHS_DEVICE_AddressSet,
    .deviceCurrentSpeedGet = DRV_USBHS_DEVICE_CurrentSpeedGet,
    .deviceSOFNumberGet = DRV_USBHS_DEVICE_SOFNumberGet,
    .deviceAttach = DRV_USBHS_DEVICE_Attach,
    .deviceDetach = DRV_USBHS_DEVICE_Detach,
    .deviceEndpointEnable = DRV_USBHS_DEVICE_EndpointEnable,
    .deviceEndpointDisable = DRV_USBHS_DEVICE_EndpointDisable,
    .deviceEndpointStall = DRV_USBHS_DEVICE_EndpointStall,
    .deviceEndpointStallClear = DRV_USBHS_DEVICE_EndpointStallClear,
    .deviceEndpointIsEnabled = DRV_USBHS_DEVICE_EndpointIsEnabled,
    .deviceEndpointIsStalled = DRV_USBHS_DEVICE_EndpointIsStalled,
    .deviceIRPSubmit = DRV_USBHS_DEVICE_IRPSubmit,
    .deviceIRPCancelAll = DRV_USBHS_DEVICE_IRPCancelAll,
    .deviceRemoteWakeupStop = DRV_USBHS_DEVICE_RemoteWakeupStop,
    .deviceRemoteWakeupStart = DRV_USBHS_DEVICE_RemoteWakeupStart,
    .deviceTestModeEnter = DRV_USBHS_DEVICE_TestModeEnter
};

// *****************************************************************************
/* Function:
    uint16_t _DRV_USBFS_DEVICE_FIFOAllocate
    (
        DRV_USBHS_OBJ * hDriver,
        uint16_t endpointSize,
    )

  Summary:
    This function locates a free chunk of contiguous memory in the USB module
    FIFO. endpointSize is the adjusted endpoint size, that it should be power of
    2. If the memory could not be allocated, the function returns 0xFFFF;

  Description:
    This function locates a free chunk of contiguous memory in the USB module
    FIFO. endpointSize is the adjusted endpoint size, that it should be power of
    2. If the memory could not be allocated, the function returns 0xFFFF;

  Remarks:
    See drv_usbhs.h for usage information.
*/

uint16_t _DRV_USBHS_DEVICE_FIFOAllocate
(
    DRV_USBHS_OBJ * hDriver, 
    uint16_t endpointSize
)
{
    /* The FIFO size is 9KB. We have an array of bits, where each bit represents
     * the availability of a 8 byte block in the FIFO. The memory is organized
     * in pages of 256 bytes each. Hence there will be 36 pages. Each page will
     * contain 32 units, each unit is 8 bytes of size. Each page is  represented
     * as one entry in the fifoAllocationTable table. */ 

    int returnValue = 0xFFFF;
    int requiredUnits = 0;
    int countedUnits = 0;
    int pageIterator = 0;
    int unitIterator = 0;
    
    int startPage = 0;
    int startUnit = 0;

    uint32_t * fifoAllocationTable = &hDriver->usbDrvCommonObj.fifoAllocationTable[0];

    if(endpointSize != 0)
    {
        /* The fifo size is divisible by 8. We can proceed. Find out how many 8
         * byte units are contained in endpointSize */

        requiredUnits = endpointSize/8;

        /* This loop searched for a contiguous block of units */

        for(pageIterator = 0; pageIterator < DRV_USBHS_FIFO_PAGES; pageIterator ++)
        {
            /* This loop will scan the entire 9KB 8 byte allocation array */
            for(unitIterator = 0; unitIterator < 32; unitIterator++)
            {
                /* This loop will scan for available units within the page. A
                 * unit is available if the unit indicator is 0 */

                if(((fifoAllocationTable[pageIterator] >> unitIterator) & 0x1) == 0)
                {
                    /* This unit is available */ 

                    if(countedUnits == 0)
                    {
                        /* This means this is the first unit we have found. We
                         * should start tracking this. */

                        startPage = pageIterator;
                        startUnit = unitIterator;
                    }

                    /* Start counting the units */
                    countedUnits ++;

                    if(countedUnits == requiredUnits)
                    {
                        /* We have found the number of units that we need */
                        break;
                    }
                }
                else
                {
                    /* The unit is allocated. Reset the counted units. We are
                     * looking for a contiguous chunk. */
                    countedUnits = 0;
                }
            }

            if(countedUnits == requiredUnits)
            {
                /* We have found the number of units that we need */
                break;
            }

        }

        if(countedUnits == requiredUnits)
        {
            /* We have found the number of unitss that we are looking for. This
             * loop marks the units as allocated. The unitIterator must start
             * from startUnit for the first time. */

            unitIterator = startUnit;
            
            for(pageIterator = startPage; pageIterator < DRV_USBHS_FIFO_PAGES; pageIterator ++)
            {
                for(; unitIterator < 32; unitIterator ++)
                {
                    fifoAllocationTable[pageIterator] |= (1 << unitIterator);
                    countedUnits --;

                    if(countedUnits == 0)
                    {
                        /* We have marked all the units */
                        break;
                    }
                }

                if(countedUnits == 0)
                {
                    /* We have marked all the units */
                    break;
                }
                else
                {
                    /* This time the unit iterator must start from 0*/
                    unitIterator = 0;
                }
            }


            /* This is the addressed in units of 8 bytes. So for example if FIFO
             * would have started at after 520 bytes from the start, then start
             * page would be 2 (256 * 2 = 512) and start unit would 1 (8 bytes).
             * The returned address would have to be 520/8. */

            returnValue = ((startPage * 256) + (startUnit * 8)) / 8;
        }
    }

    return(returnValue);
}

// *****************************************************************************
/* Function:
    uint16_t _DRV_USBHS_DEVICE_FIFOFree
    (
        DRV_USBHS_OBJ * hDriver,
        uint16_t fifoAddress,
        uint16_t endpointSize,
    )

  Summary:
    This function frees up the memory allocated to the endpoint. The memory
    start address and the unadjusted endpoint size are the inputs.

  Description:
    This function frees up the memory allocated to the endpoint. The memory
    start address and the unadjusted endpoint size are the inputs.

  Remarks:
    See drv_usbhs.h for usage information.
*/

void _DRV_USBHS_DEVICE_FIFOFree
(
    DRV_USBHS_OBJ * hDriver,
    uint16_t fifoAddress,
    uint16_t endpointSize
)
{
    uint16_t adjustedEndpointSize = 1;
    int startUnit = 0;
    int startPage = 0;
    int totalUnits = 0;
    int pageIterator = 0;
    int unitIterator = 0;

    uint32_t * fifoAllocationTable = &hDriver->usbDrvCommonObj.fifoAllocationTable[0];

    if(endpointSize != 0)
    {
        /* Dividing by 32 will give us the start page */
        startPage = fifoAddress/32;
        startUnit = fifoAddress % 32;

        /* Adjust the endpointSize so that we have a power of 2. This would have
         * been the size that was allocated. */

        while(adjustedEndpointSize < endpointSize)
        {
            adjustedEndpointSize <<= 1;
        }

        /* Calculate the units that were allocated to the endpoint */
        totalUnits = adjustedEndpointSize/8;
        unitIterator = startUnit;

        for (pageIterator = startPage; pageIterator < DRV_USBHS_FIFO_PAGES; pageIterator ++)
        {
            for(;unitIterator < 32; unitIterator ++)
            {
                /* Inverting the bit in this case is actually clearing the bit as
                 * the bit was set in the allocate function. */
                fifoAllocationTable[pageIterator] ^= (1 << unitIterator);
                totalUnits --;
                
                if(totalUnits == 0)
                {
                    break;
                }
            }

            if(totalUnits == 0)
            {
                /* All the units have been accounted for */
                break;
            }
            else
            {
                /* Reset the count for the next page */
                unitIterator = 0;
            }
        }
    }
}

// *****************************************************************************
/* Function:
    void _DRV_USBHS_DEVICE_FIFOFreeAll
    (
        DRV_USBHS_OBJ * hDriver,
    )

  Summary:
    This function frees up the entire FIFO allocation table.

  Description:
    This function frees up the entire FIFO allocation table.

  Remarks:
    See drv_usbhs.h for usage information.
*/

void _DRV_USBHS_DEVICE_FIFOFreeAll
(
    DRV_USBHS_OBJ * hDriver
)
{
    int pageIterator = 0;

    uint32_t * fifoAllocationTable = &hDriver->usbDrvCommonObj.fifoAllocationTable[0];

    for (pageIterator = 0; pageIterator < DRV_USBHS_FIFO_PAGES; pageIterator ++)
    {
            fifoAllocationTable[pageIterator] = 0;
    }
}

uint16_t _DRV_USBHS_ProcessIRPFIFO
(
    DRV_USBHS_OBJ * hDriver,
    DRV_USBHS_DEVICE_ENDPOINT_OBJ * endpointObj,
    uint8_t endpoint,
    uint8_t direction,
    USB_DEVICE_IRP_LOCAL * irp,
    bool * pisDMAUsed,
    bool tryDma
)
{
    /* This function only loads or unloads the FIFO It does not set the Tx
     * Packet Ready or clear' the RX Packet Ready bit. The function returns the
     * amount of data that was written or read from the FIFO */

    uint32_t count = 0;
    unsigned int offset;
    uint8_t dmaChannelGrabbed = 0;
    USBHS_MODULE_ID usbID = USBHS_NUMBER_OF_MODULES;
    uint8_t * data;

    data = (uint8_t *)irp->data;
    usbID = hDriver->usbDrvCommonObj.usbID;

    if(USB_DATA_DIRECTION_DEVICE_TO_HOST == direction)
    {
        /* This means data has to move from device
         * to host. We should write to the FIFO */

        count = (irp->nPendingBytes < endpointObj->maxPacketSize) ? irp->nPendingBytes : endpointObj->maxPacketSize;
        offset = (irp->size - irp->nPendingBytes);

        if(0 == endpoint)
        {
            PLIB_USBHS_DeviceEPFIFOLoad(usbID, endpoint, &data[offset], count);
            *pisDMAUsed = false;
            irp->nPendingBytes -= count;
        }
        else
        {
            /* Non EP0 handling */
            if(true == tryDma)
            {
                /* tryDma flag is true. So we will try to get a free DMA channel
                 * If no channel was found, then we process with manual FIFO
                 * access. */

                dmaChannelGrabbed = _DRV_USBHS_DEVICE_Get_FreeDMAChannel(hDriver, USB_DATA_DIRECTION_DEVICE_TO_HOST, endpoint);
                if((0 == dmaChannelGrabbed))
                {
                    /* NO DMA channel available. So do normal FIFO load */
                    PLIB_USBHS_DeviceEPFIFOLoad(usbID, endpoint, &data[offset], count);
                    *pisDMAUsed = false;
                    irp->nPendingBytes -= count;
                }
                else
                {
                    /* DMA CHANNEL has been grabbed and DMA buffer address is OK
                     * (divisible by 4). So program the DMA registers now to
                     * start DMA operation */

                     hDriver->usbDrvCommonObj.gDrvUSBDMAPool[dmaChannelGrabbed].count = count;
                     irp->nPendingBytes -= count;
                     *pisDMAUsed = true;
                     PLIB_USBHS_DMAOperationEnable(usbID, endpoint, dmaChannelGrabbed,(void *)&data[offset], count, 0);
                }
            }
            else
            {
                /* Do a manual FIFO access as tryDma flag is false */
                PLIB_USBHS_DeviceEPFIFOLoad(usbID, endpoint, &data[offset], count);
                *pisDMAUsed = false;
                irp->nPendingBytes -= count;
            }
        }
    }
    else
    {
        /* This means data is moving from host to device */

        if((0 != endpoint))
        {
            if(true == tryDma)
            {
                dmaChannelGrabbed = _DRV_USBHS_DEVICE_Get_FreeDMAChannel(hDriver, USB_DATA_DIRECTION_HOST_TO_DEVICE, endpoint);

                if((0 == dmaChannelGrabbed))
                {
                    /* NO DMA channel available. So do normal FIFO unload */
                    count = PLIB_USBHS_DeviceEPFIFOUnload(usbID, endpoint, &data[irp->nPendingBytes]);
                    *pisDMAUsed = false;
                    irp->nPendingBytes += count;
                }
                else
                {
                    /* DMA CHANNEL has been grabbed and DMA buffer address is OK
                     * (divisible by 4). So program the DMA registers now to
                     * start DMA operation */

                     /* Load the endpoint in INDEX register to access and obtain
                      * Received data count in bytes */
                     count = (uint32_t) PLIB_USBHS_GetReceiveDataCount(usbID, endpoint);
                     hDriver->usbDrvCommonObj.gDrvUSBDMAPool[dmaChannelGrabbed].count = count;
                     offset = irp->nPendingBytes;
                     irp->nPendingBytes += count;
                     *pisDMAUsed = true;
                     
                     PLIB_USBHS_DMAOperationEnable(usbID, endpoint, dmaChannelGrabbed, (void *)((uint8_t *)(irp->data) + offset),count, 1);
                }
            }
            else
            {
                /* tryDma flag is false. So we perform manual FIFO access */
                count = PLIB_USBHS_DeviceEPFIFOUnload(usbID, endpoint, &data[irp->nPendingBytes]);
                *pisDMAUsed = false;
                irp->nPendingBytes += count;
            }
        }
        else
        {
            /* For endpoint 0 */
            count = PLIB_USBHS_DeviceEPFIFOUnload(usbID, endpoint, &data[irp->nPendingBytes]);
            *pisDMAUsed = false;
            irp->nPendingBytes += count;
        }
    }

    return (count);
}

void _DRV_USBHS_DEVICE_Initialize
(
    DRV_USBHS_OBJ * drvObj,
    SYS_MODULE_INDEX index
)
{
    USBHS_MODULE_ID usbID = USBHS_NUMBER_OF_MODULES;

    usbID = drvObj->usbDrvCommonObj.usbID;

    /* When configured in dual role mode, we will enable requisite interrupts as
     * part of device attach. Device detach will disable device specific
     * interrupts.  */

    if(DRV_USBHS_OPMODE_DUAL_ROLE != drvObj->usbDrvCommonObj.operationMode)
    {
        /* Disable all endpoint interrupts Enable the reset, the SOF, resume and
         * suspend interrupt */
        PLIB_USBHS_InterruptEnableSet(usbID, 0xF, 0x0, 0x0);
    }
    else
    {
        /* Disable all interrupts */
        PLIB_USBHS_InterruptEnableSet(usbID, 0x0, 0x0, 0x0);
    }

    /* Allocate 64 bytes of the FIFO for endpoint 0 */
    _DRV_USBHS_DEVICE_FIFOAllocate(drvObj, 64);
}

// *****************************************************************************
/* Function:
    void DRV_USBHS_DEVICE_AddressSet(DRV_HANDLE handle, uint8_t address);

  Summary:
    This function will set the USB module address that is obtained from the Host.

  Description:
    This function will set the USB module address  that  is  obtained  from  the
    Host in a setup transaction. The address is obtained from  the  SET_ADDRESS
    command issued by the Host. The  primary  (first)  client  of  the  driver
    uses this function to set the module's USB address after decoding the  setup
    transaction from the Host.

  Remarks:
    See drv_usbhs.h for usage information.
*/

void DRV_USBHS_DEVICE_AddressSet
(
    DRV_HANDLE handle,
    uint8_t address
)
{
    DRV_USBHS_OBJ * hDriver =  NULL;
    USBHS_MODULE_ID usbID = USBHS_NUMBER_OF_MODULES;

    if( (DRV_HANDLE_INVALID !=  handle) && (NULL != ((DRV_USBHS_CLIENT_OBJ *)handle)) )
    {
        if(((DRV_USBHS_CLIENT_OBJ *)handle)->inUse)
        {
            hDriver = ((DRV_USBHS_CLIENT_OBJ *)handle)->hDriver;
            usbID = hDriver->usbDrvCommonObj.usbID;

            /* Set the device address */
            PLIB_USBHS_DeviceAddressSet(usbID, address);
        }
        else
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_AddressSet()");
        }
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_AddressSet()");
    }
}

// *****************************************************************************
/* Function:
    USB_SPEED DRV_USBHS_DEVICE_CurrentSpeedGet(DRV_HANDLE handle);

  Summary:
    This function will return the USB speed at which the device is operating.

  Description:
    This function will return the USB speed at which the device is operating. 


  Remarks:
    See drv_usbhs.h for usage information.
*/

USB_SPEED DRV_USBHS_DEVICE_CurrentSpeedGet
(
    DRV_HANDLE handle
)
{
    DRV_USBHS_OBJ * hDriver =  NULL;
    USB_SPEED deviceSpeed = USB_SPEED_ERROR;

    if( (DRV_HANDLE_INVALID !=  handle) && (NULL != ((DRV_USBHS_CLIENT_OBJ *)handle)) )
    {
        if(((DRV_USBHS_CLIENT_OBJ *)handle)->inUse)
        {
            hDriver = ((DRV_USBHS_CLIENT_OBJ *)handle)->hDriver;
            if( (DRV_USBHS_OPMODE_DEVICE == hDriver->usbDrvCommonObj.operationMode) || 
                    ((DRV_USBHS_OPMODE_DUAL_ROLE == hDriver->usbDrvCommonObj.operationMode) && 
                     (true == hDriver->usbDrvCommonObj.isDeviceRoleActive)) )
            {
                /* The current speed in contained in the device speed member of
                 * the driver object */
                deviceSpeed = (hDriver->usbDrvCommonObj.deviceSpeed);
            }
        }
        else
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_CurrentSpeedGet()");
        }
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_CurrentSpeedGet()");
    }
    
    return (deviceSpeed);
}

// *****************************************************************************
/* Function:
      void DRV_USBHS_DEVICE_RemoteWakeupStart(DRV_HANDLE handle)

  Summary:
    Dynamic implementation of DRV_USBHS_DEVICE_RemoteWakeupStart client interface
    function.

  Description:
    This is the dynamic implementation of DRV_USBHS_DEVICE_RemoteWakeupStart client
    interface function for USB device.
    Function checks the input handle validity and on success enables the USB
    device to drive resume signaling. RESUME is enabled by programming LPMRES
    bit in LPM_CNTRL register(self clearing bit).

  Remarks:
    See drv_usbhs.h for usage information.
*/

void DRV_USBHS_DEVICE_RemoteWakeupStart
(
    DRV_HANDLE handle
)
{
    DRV_USBHS_OBJ * hDriver = NULL;
    
    if( (DRV_HANDLE_INVALID !=  handle) && (NULL != ((DRV_USBHS_CLIENT_OBJ *)handle)) )
    {
        if(((DRV_USBHS_CLIENT_OBJ *)handle)->inUse)
        {
            hDriver = ((DRV_USBHS_CLIENT_OBJ *)handle)->hDriver;

            /* Added to avoid build compilation issue */
            hDriver = hDriver;
   
            /* Commented till PLIB implementation is completed */
            //PLIB_USB_ResumeSignalingEnable(hDriver->usbDrvCommonObj.usbID);
        }
        else
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_RemoteWakeupStart()");
        }
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_RemoteWakeupStart()");
    }
}

void DRV_USBHS_DEVICE_RemoteWakeupStop
(
    DRV_HANDLE handle
)
{
    DRV_USBHS_OBJ * hDriver =  NULL;
    
    if( (DRV_HANDLE_INVALID !=  handle) && (NULL != ((DRV_USBHS_CLIENT_OBJ *)handle)) )
    {
        if(((DRV_USBHS_CLIENT_OBJ *)handle)->inUse)
        {
            hDriver = ((DRV_USBHS_CLIENT_OBJ *)handle)->hDriver;

            /* Added to avoid build compilation issue */
            hDriver = hDriver;
            
            /* Commented till PLIB implementation is completed */
            //PLIB_USB_ResumeSignalingDisable(hDriver->usbDrvCommonObj.usbID);
        }
        else
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_RemoteWakeupStop()");
        }
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_RemoteWakeupStop()");
    }
}

// *****************************************************************************
/* Function:
    void DRV_USBHS_DEVICE_Attach(DRV_HANDLE handle);

  Summary:
    This function will enable the attach signaling resistors on the D+ and D-
    lines thus letting the USB Host know that a device has been attached on the
    bus.

  Description:
    This function enables the pull-up resistors on the D+ or D- lines thus
    letting the USB Host know that a device has been attached on the bus . This
    function should be called when the driver client is ready  to  receive
    communication  from  the  Host (typically after all initialization is
    complete). The USB 2.0 specification requires VBUS to be detected before the
    data line pull-ups are enabled. The application must ensure the same.

  Remarks:
    Refer to drv_usbhs.h for usage information.
*/

void DRV_USBHS_DEVICE_Attach
(
    DRV_HANDLE handle
)
{
    DRV_USBHS_OBJ * hDriver =  NULL;

    if( (DRV_HANDLE_INVALID !=  handle) && (NULL != ((DRV_USBHS_CLIENT_OBJ *)handle)) )
    {
        if(((DRV_USBHS_CLIENT_OBJ *)handle)->inUse)
        {
            hDriver = ((DRV_USBHS_CLIENT_OBJ *)handle)->hDriver;
            {
                _DRV_USBHS_CLOCK_CONTROL_SETUP_DEVICE_MODE(hDriver->usbDrvCommonObj.usbID);
                

                /* The role change will require the USB module to be reset. This
                 * reset operation needs to be done in the Task routine as it
                 * will block for 3 millseconds */
                
                hDriver->usbDrvCommonObj.isProcessingAttach = true;
                
                
                if( (DRV_USBHS_OPMODE_DUAL_ROLE == hDriver->usbDrvCommonObj.operationMode) && (false == hDriver->usbDrvCommonObj.isDeviceRoleActive) )
                {
                    hDriver->usbDrvCommonObj.isDeviceRoleActive = true;
                }
            }
        }
        else
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_Attach()");
        }
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_Attach()");
    }
}

void _DRV_USBHS_DEVICE_AttachStateMachine
(
    DRV_USBHS_OBJ * hDriver
)
{    
    /* This function is called in the DRV_USBHS_Tasks() function repeatedly, to
     * check if the module Reset operation that was trigged when a device attach
     * function was called, has completed. */

    if(0 == USBEOFRSTbits.SOFRST  )
    {
        _DRV_USBHS_NonPersistentInterruptSourceClear(hDriver->usbDrvCommonObj.interruptSource);
        _DRV_USBHS_NonPersistentInterruptSourceClear(hDriver->usbDrvCommonObj.interruptSourceUSBDma);

        hDriver->usbDrvCommonObj.isProcessingAttach = false;

        /* Disable all endpoint interrupts Enable the reset, the SOF, resume and
         * suspend interrupt */
        if(DRV_USBHS_OPMODE_DUAL_ROLE == hDriver->usbDrvCommonObj.operationMode)
        {
            PLIB_USBHS_InterruptEnableSet(hDriver->usbDrvCommonObj.usbID, 0xF, 0x0, 0x0);
        }
        
        _DRV_USBHS_PersistentInterruptSourceClear(hDriver->usbDrvCommonObj.interruptSource);
        _DRV_USBHS_PersistentInterruptSourceClear(hDriver->usbDrvCommonObj.interruptSourceUSBDma);
        
        PLIB_USBHS_DeviceAttach(hDriver->usbDrvCommonObj.usbID, hDriver->usbDrvCommonObj.operationSpeed);
    }
} 

// *****************************************************************************
/* Function:
    void DRV_USBHS_DEVICE_Detach(DRV_HANDLE handle);

  Summary:
    This function will disable the attach signaling resistors on the D+ and D-
    lines thus letting the USB Host know that the device has detached from the
    bus.

  Description:
    This function disables the pull-up resistors on the D+ or D- lines. This
    function should be called when the application wants to disconnect the
    device  from  the bus (typically to implement a soft detach or switch  to
    Host  mode operation).  A self-powered device should be detached from the
    bus when the VBUS is not valid.

  Remarks:
    Refer to drv_usbhs.h for usage information.
*/

void DRV_USBHS_DEVICE_Detach
(
    DRV_HANDLE handle
)
{
    DRV_USBHS_OBJ * hDriver =  NULL;
    USBHS_MODULE_ID usbID = USBHS_NUMBER_OF_MODULES;
    uint8_t iEndpoint = 0;
    uint8_t channelCount = 0;
    bool interruptWasEnabled = false;
    bool interruptWasEnabledUSBDMA = false;

    if( (DRV_HANDLE_INVALID !=  handle) && (NULL != ((DRV_USBHS_CLIENT_OBJ *)handle)) )
    {
        if(((DRV_USBHS_CLIENT_OBJ *)handle)->inUse)
        {
            hDriver = ((DRV_USBHS_CLIENT_OBJ *)handle)->hDriver;
            usbID = hDriver->usbDrvCommonObj.usbID;

            if( ((DRV_USBHS_OPMODE_DUAL_ROLE == hDriver->usbDrvCommonObj.operationMode) &&
                        (false == hDriver->usbDrvCommonObj.isDeviceRoleActive)) || 
                    (true == hDriver->usbDrvCommonObj.isProcessingAttach) )
            {
                /* Do not do anything */
                SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "\r\nUSBHS Driver: Inappropriate usage of Device detach in DRV_USBHS_DEVICE_Detach()");
            }
            else
            {
                if(hDriver->usbDrvCommonObj.isInInterruptContext == false)
                {
                    interruptWasEnabled = _DRV_USBHS_InterruptSourceDisable(hDriver->usbDrvCommonObj.interruptSource);
                }

                if(hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA == false)
                {
                    interruptWasEnabledUSBDMA = _DRV_USBHS_InterruptSourceDisable(hDriver->usbDrvCommonObj.interruptSourceUSBDma);
                }

                /* Detach the device */
                PLIB_USBHS_DeviceDetach(usbID);

                _DRV_USBHS_NonPersistentInterruptSourceClear(hDriver->usbDrvCommonObj.interruptSource);
                _DRV_USBHS_NonPersistentInterruptSourceClear(hDriver->usbDrvCommonObj.interruptSourceUSBDma);

                if(DRV_USBHS_OPMODE_DUAL_ROLE == hDriver->usbDrvCommonObj.operationMode)
                {
                    /* Disable all interrupts. When the application changes role
                     * to Host mode, required interrupts will be enabled at that
                     * time */

                    PLIB_USBHS_InterruptEnableSet(usbID, 0x0, 0x0, 0x0);
                }

                PLIB_USBHS_TxInterruptDisable(usbID, USBHS_TXRXINT_ALL);
                PLIB_USBHS_RxInterruptDisable(usbID, USBHS_TXRXINT_ALL);

                for(iEndpoint = 0; iEndpoint < DRV_USBHS_ENDPOINTS_NUMBER; iEndpoint ++)
                {
                    /* Clear up the status of all the endpoints */
                    PLIB_USBHS_TxEPStatusClear(usbID, iEndpoint, USBHS_TXEP_INCOMPTX | USBHS_TXEP_SENTSTALL | USBHS_TXEP_FIFONOTEMPTY | USBHS_TXEP_UNDERRUN);
                    PLIB_USBHS_EndpointTxFIFOFlush(usbID, iEndpoint);

                    PLIB_USBHS_EndpointRxFIFOFlush(usbID, iEndpoint);
                    PLIB_USBHS_RxEPStatusClear(usbID, iEndpoint, USBHS_RXEP_SENTSTALL | USBHS_RXEP_DATAERROR | USBHS_RXEP_OVERRUN | 
                            USBHS_RXEP_FIFOFULL | USBHS_RXEP_PKTRDY);
                }

                /* Read the USB DMA interrupts to clear */
                PLIB_USBHS_DMAInterruptGet(usbID);

                /* Release all the DMA channels and clear up DMA errors if any
                 * */
                for(channelCount = 1; channelCount < 9; channelCount++)
                {
                    if(true == (hDriver->usbDrvCommonObj.gDrvUSBDMAPool[channelCount]).inUse)
                    {
                        /* Found Used DMA Channel - Release this channel */
                        hDriver->usbDrvCommonObj.gDrvUSBDMAPool[channelCount].inUse = false;
                    }
                    /* Clear if any USB DMA error */
                    PLIB_USBHS_DMAErrorGet(usbID, channelCount);
                }

                _DRV_USBHS_PersistentInterruptSourceClear(hDriver->usbDrvCommonObj.interruptSource);
                _DRV_USBHS_PersistentInterruptSourceClear(hDriver->usbDrvCommonObj.interruptSourceUSBDma);

                if(hDriver->usbDrvCommonObj.isInInterruptContext == false)
                {
                    if(interruptWasEnabled)
                    {
                        /* Enable the interrupt only if it was disabled */
                        _DRV_USBHS_InterruptSourceEnable(hDriver->usbDrvCommonObj.interruptSource);
                    }
                }

                if(hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA == false)
                {
                    if(interruptWasEnabledUSBDMA)
                    {
                        /* Enable the interrupt only if it was disabled */
                        _DRV_USBHS_InterruptSourceEnable(hDriver->usbDrvCommonObj.interruptSourceUSBDma);
                    }
                }
            }
        }
        else
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_Detach()");
        }
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_Detach()");
    }
}

// *****************************************************************************
/* Function:
    void _DRV_USBHS_DEVICE_EndpointObjectEnable
    (
        DRV_USBHS_DEVICE_ENDPOINT_OBJ * endpointObject,
        uint16_t endpointSize,
        USB_TRANSFER_TYPE endpointType,
        USB_BUFFER_DATA01 dataToggle
     )

  Summary:
    This helper function configures the endpoint object.

  Description:
    This helper function configures the endpoint object.

  Remarks:
    This is a local function and should not be called directly by the
    application.
*/

void _DRV_USBHS_DEVICE_EndpointObjectEnable
(
    DRV_USBHS_DEVICE_ENDPOINT_OBJ * endpointObject,
    uint16_t endpointSize,
    USB_TRANSFER_TYPE endpointType
   
)
{
    endpointObject->irpQueue        = NULL;
    endpointObject->maxPacketSize   = endpointSize;
    endpointObject->endpointType    = endpointType;
    endpointObject->endpointState  |= DRV_USBHS_DEVICE_ENDPOINT_STATE_ENABLED;
}

// *****************************************************************************
/* Function:
    uint16_t DRV_USBHS_DEVICE_SOFNumberGet(DRV_HANDLE handle);

  Summary:
    This function will return the USB SOF packet number.

  Description:
    This function will return the USB SOF packet number..

  Remarks:
    See drv_usbhs.h for usage information.
*/

uint16_t DRV_USBHS_DEVICE_SOFNumberGet
(
    DRV_HANDLE handle
)
{
    uint16_t sofNumber = 0;

    if( (DRV_HANDLE_INVALID !=  handle) && (NULL != ((DRV_USBHS_CLIENT_OBJ *)handle)) )
    {
        if(((DRV_USBHS_CLIENT_OBJ *)handle)->inUse)
        {

        }
        else
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_SOFNumberGet()");
        }
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_SOFNumberGet()");
    }

    return (sofNumber);
}

// ****************************************************************************
/* Function:
    USB_ERROR DRV_USBHS_DEVICE_EndpointEnable
    (
        DRV_HANDLE handle,
        USB_ENDPOINT endpointAndDirection,
        USB_TRANSFER_TYPE transferType,
        uint16_t endpointSize
    );
    
  Summary:
    This function enables an endpoint for the specified direction and endpoint
    size.
	
  Description:
    This function enables an endpoint for the specified direction and endpoint
    size. The function will enable the endpoint for communication in one
    direction at a time. It must be called twice if the endpoint is required to
    communicate in both the directions, with the exception of control endpoints.
    If the endpoint type is a control endpoint, the endpoint is always
    bidirectional and the function needs to be called only once.  
    
    The size of the endpoint must match the wMaxPacketSize reported in the
    endpoint descriptor for this endpoint. A transfer that is scheduled over
    this endpoint will be scheduled in wMaxPacketSize transactions. The function
    does not check if the endpoint is already in use. It is the client's
    responsibility to make sure that a endpoint is not accidentally reused.

  Remarks:
    See drv_usbhs.h for usage information.
*/

USB_ERROR DRV_USBHS_DEVICE_EndpointEnable
(
    DRV_HANDLE handle, 
    USB_ENDPOINT endpointAndDirection, 
    USB_TRANSFER_TYPE endpointType,
    uint16_t endpointSize
)
{
    int direction = 0;
    int fifoSize = 0;
    uint8_t endpoint = 0;
    unsigned int shiftWord = 0;
    bool interruptWasEnabled = false;
    bool interruptWasEnabledUSBDMA = false;
    bool mutexLock = false;

    DRV_USBHS_OBJ * hDriver = NULL;
    USBHS_MODULE_ID usbID = USBHS_NUMBER_OF_MODULES;
    DRV_USBHS_DEVICE_ENDPOINT_OBJ * endpointObject = NULL;
    uint16_t adjustedEndpointSize = 1;
    USB_ERROR returnValue = USB_ERROR_PARAMETER_INVALID;

    /* Enable the endpoint */
    endpoint = endpointAndDirection & 0xF;
    direction = ((endpointAndDirection & 0x80) != 0);
    
    if(endpoint < DRV_USBHS_ENDPOINTS_NUMBER)
    {
        if( (DRV_HANDLE_INVALID !=  handle) && (NULL != ((DRV_USBHS_CLIENT_OBJ *)handle)) )
        {
            if(((DRV_USBHS_CLIENT_OBJ *)handle)->inUse)
            {
                hDriver = ((DRV_USBHS_CLIENT_OBJ *)handle)->hDriver;
                usbID = hDriver->usbDrvCommonObj.usbID;

                /* Get the pointer to the endpoint object */

                endpointObject = (hDriver->usbDrvCommonObj.endpointTable + (2 * endpoint) + 0);

                if(endpoint == 0)
                {
                    /* There are two endpoint objects for a control endpoint */

                    _DRV_USBHS_DEVICE_EndpointObjectEnable(endpointObject, endpointSize, endpointType);
                    endpointObject ++;
                    _DRV_USBHS_DEVICE_EndpointObjectEnable(endpointObject, endpointSize, endpointType);
                    PLIB_USBHS_TxInterruptEnable(usbID, USBHS_TXRXINT_EP0);

                    /* EP0 does not require any configuration. It is ready to
                     * receive packets when the device is attached */

                    returnValue = USB_ERROR_NONE;
                }
                else
                {
                    /* Non EP0 endpoints require configuration. Update the
                     * endpoint database. The direction variable allow to obtain
                     * the pointer to the direction specific endpoint object. */

                    endpointObject += direction;
                    _DRV_USBHS_DEVICE_EndpointObjectEnable(endpointObject, endpointSize, endpointType);

                    if((hDriver->usbDrvCommonObj.isInInterruptContext == false) && (hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA == false))
                    {
                        if(OSAL_MUTEX_Lock(&hDriver->usbDrvCommonObj.mutexID, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
                        {
                            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Mutex lock failed in DRV_USBHS_DEVICE_EndpointEnable()");
                            returnValue = USB_ERROR_OSAL_FUNCTION;
                        }
                        else
                        {
                            mutexLock = true;
                        }
                    }

                    /* The following code is thread safe. This is needed because
                     * the FIFO allocation has to be thread safe. */

                    if(USB_ERROR_OSAL_FUNCTION != returnValue)
                    {
                        if(hDriver->usbDrvCommonObj.isInInterruptContext == false)
                        {
                            interruptWasEnabled = _DRV_USBHS_InterruptSourceDisable(hDriver->usbDrvCommonObj.interruptSource);
                        }

                        if(hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA == false)
                        {
                            interruptWasEnabledUSBDMA = _DRV_USBHS_InterruptSourceDisable(hDriver->usbDrvCommonObj.interruptSourceUSBDma);
                        }

                        /* The following code maps the endpoint size to the
                         * value that should be loaded in the FIFOSZ register.
                         * The amount of FIFO allocated to the endpoint will be
                         * an adjusted to nearest largest power of 2 value. */

                        fifoSize = 0;

                        /* Find upper 2 power number of endpointSize */
                        if (endpointSize)
                        {
                            while(adjustedEndpointSize < endpointSize)
                            {
                                adjustedEndpointSize <<= 1;
                            }
                        }
                        else
                        {
                            adjustedEndpointSize = 0;
                        }

                        shiftWord = adjustedEndpointSize;
                        endpointObject->fifoStartAddress = _DRV_USBHS_DEVICE_FIFOAllocate(hDriver, adjustedEndpointSize);

                        while((shiftWord & 0x1) != 1)
                        {
                            shiftWord = (shiftWord >> 1);
                            fifoSize ++;
                        }

                        fifoSize -= 3;

                        if(direction == USB_DATA_DIRECTION_DEVICE_TO_HOST)
                        {
                            /* Set up the maxpacket size, fifo start address
                             * fifosize and enable the interrupt. CLear the data
                             * toggle. */

                            PLIB_USBHS_DeviceTxEndpointConfigure(usbID, endpoint, endpointSize, endpointObject->fifoStartAddress, fifoSize, endpointType);
                        }
                        else
                        {
                            /* Setup the RX endpoint. Disable NYET handshakes. */
                            PLIB_USBHS_DeviceRxEndpointConfigure(usbID, endpoint, endpointSize, endpointObject->fifoStartAddress, fifoSize, endpointType);
                        }

                        if(hDriver->usbDrvCommonObj.isInInterruptContext == false)
                        {
                            if(interruptWasEnabled)
                            {
                                /* Enable the interrupt only if it was disabled */
                                _DRV_USBHS_InterruptSourceEnable(hDriver->usbDrvCommonObj.interruptSource);
                            }
                        }

                        if(hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA == false)
                        {
                            if(interruptWasEnabledUSBDMA)
                            {
                                /* Enable the interrupt only if it was disabled */
                                _DRV_USBHS_InterruptSourceEnable(hDriver->usbDrvCommonObj.interruptSourceUSBDma);
                            }
                        }

                        if(mutexLock == true)
                        {
                            if(OSAL_MUTEX_Unlock(&hDriver->usbDrvCommonObj.mutexID) != OSAL_RESULT_TRUE)
                            {
                                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Mutex unlock failed in DRV_USBHS_DEVICE_EndpointEnable()");
                            }
                        }
                    }

                    returnValue = USB_ERROR_NONE;
                } 
            } 
            else
            {
                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_EndpointEnable()");
            }
        }
        else
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_EndpointEnableI()");
        }
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO,"\r\nUSBHS Driver: Unsupported endpoint in DRV_USBHS_DEVICE_EndpointEnable()");
        returnValue = USB_ERROR_DEVICE_ENDPOINT_INVALID;
    }

    return (returnValue);
}

// ***************************************************************************
/* Function:
    USB_ERROR DRV_USBHS_DEVICE_EndpointDisable
    (
        DRV_HANDLE handle,
        USB_ENDPOINT endpointAndDirection
    )
    
  Summary:
    This function disables an endpoint.
	
  Description:
    This function disables an endpoint. If the endpoint type is a control
    endpoint type, both directions are disabled. For non-control endpoints, the
    function disables the specified direction only. The direction to be disabled 
    is specified by the Most Significant Bit (MSB) of the endpointAndDirection 
    parameter.

  Remarks:
    See drv_usbhs.h for usage information.
*/

USB_ERROR DRV_USBHS_DEVICE_EndpointDisable
(
    DRV_HANDLE handle,
    USB_ENDPOINT endpointAndDirection
)
{
    /* This routine disables the specified endpoint.  It does not check if there
     * is any ongoing communication on the bus through the endpoint */

    uint8_t endpoint = 0;
    int direction = 0;
    int iEntry = 0;
    DRV_USBHS_OBJ * hDriver = NULL;
    DRV_USBHS_DEVICE_ENDPOINT_OBJ * endpointObject = NULL;
    USBHS_MODULE_ID usbID = USBHS_NUMBER_OF_MODULES;
    bool interruptWasEnabled = false;
    bool interruptWasEnabledUSBDMA = false;
    bool mutexLock = false;
    USB_ERROR returnValue = USB_ERROR_PARAMETER_INVALID;
   
    /* Check if the handle is valid */
    if( (DRV_HANDLE_INVALID !=  handle) && (NULL != ((DRV_USBHS_CLIENT_OBJ *)handle)) )
    {
        if(((DRV_USBHS_CLIENT_OBJ *)handle)->inUse)
        {
            hDriver = ((DRV_USBHS_CLIENT_OBJ *)handle)->hDriver;
            usbID = hDriver->usbDrvCommonObj.usbID;

            /* Get the pointer to associated endpoint object table */
            endpointObject = hDriver->usbDrvCommonObj.endpointTable;
            endpoint = endpointAndDirection & 0xF;
            direction = ((endpointAndDirection & 0x80) != 0);

            if((endpointAndDirection != DRV_USBHS_DEVICE_ENDPOINT_ALL) && (endpoint >= DRV_USBHS_ENDPOINTS_NUMBER))
            {
                /* The endpoint number must be either
                 * _DRV_USBHS_DEVICE_ENDPOINT_ALL or a valid endpoint */

                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO,"\r\nUSBHS Driver: Unsupported endpoint in DRV_USBHS_DEVICE_EndpointDisable()");
                returnValue = USB_ERROR_DEVICE_ENDPOINT_INVALID;
            }
            else
            {
                if((hDriver->usbDrvCommonObj.isInInterruptContext == false) && (hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA == false))
                {
                    if(OSAL_MUTEX_Lock(&hDriver->usbDrvCommonObj.mutexID, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
                    {
                        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Mutex lock failed in DRV_USBHS_DEVICE_EndpointDisable()");
                        returnValue = USB_ERROR_OSAL_FUNCTION;
                    }
                    else
                    {
                        mutexLock = true;
                    }
                }

                if(USB_ERROR_OSAL_FUNCTION != returnValue)
                {
                    if(hDriver->usbDrvCommonObj.isInInterruptContext == false)
                    {
                        interruptWasEnabled = _DRV_USBHS_InterruptSourceDisable(hDriver->usbDrvCommonObj.interruptSource);
                    }

                    if(hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA == false)
                    {
                        interruptWasEnabledUSBDMA = _DRV_USBHS_InterruptSourceDisable(hDriver->usbDrvCommonObj.interruptSourceUSBDma);
                    }

                    /* If the endpointAndDirection is _DRV_USBHS_DEVICE_ENDPOINT_ALL
                     * then this means that the DRV_USBHS_DEVICE_EndpointDisableAll()
                     * function was called */

                    if(endpointAndDirection == DRV_USBHS_DEVICE_ENDPOINT_ALL)
                    {
                        /* Free up the memory allocated to all endpoints and
                         * then reallocate memory for endpoint 0 */
                        _DRV_USBHS_DEVICE_FIFOFreeAll(hDriver);
                        _DRV_USBHS_DEVICE_FIFOAllocate(hDriver, 64);

                        /* Disable all the interrupts */

                        PLIB_USBHS_TxInterruptDisable(usbID, USBHS_TXRXINT_ALL);
                        PLIB_USBHS_RxInterruptDisable(usbID, USBHS_TXRXINT_ALL);

                        for(iEntry = 0; iEntry < DRV_USBHS_ENDPOINTS_NUMBER; iEntry ++)
                        {
                            /* Update the endpoint database */

                            endpointObject->endpointState  &= ~DRV_USBHS_DEVICE_ENDPOINT_STATE_ENABLED;
                            endpointObject ++;
                            endpointObject->endpointState  &= ~DRV_USBHS_DEVICE_ENDPOINT_STATE_ENABLED;
                            endpointObject ++;
                        }
                    } 
                    else
                    {
                        /* Setup the endpointObj to point to the correct
                         * endpoint object Interrupt is still disabled */

                        endpointObject += (endpoint * 2) + direction;

                        if(((endpointObject->endpointState) & DRV_USBHS_DEVICE_ENDPOINT_STATE_ENABLED))
                        {
                            if(endpointObject->endpointType == USB_TRANSFER_TYPE_CONTROL)
                            {
                                /* Disable a control endpoint and update the
                                 * endpoint database. */
                                endpointObject->endpointState  &= ~DRV_USBHS_DEVICE_ENDPOINT_STATE_ENABLED;
                                endpointObject += 1;
                                endpointObject->endpointState  &= ~DRV_USBHS_DEVICE_ENDPOINT_STATE_ENABLED;
                            }
                            else
                            {
                                /* Disable a specific endpoint direction for non
                                 * control endpoints. */

                                endpointObject->endpointState  &= ~DRV_USBHS_DEVICE_ENDPOINT_STATE_ENABLED;

                                /* Deallocate the FIFO memory allocated to this
                                 * endpoint. Note that we are only updating a
                                 * local table. Nothing is done in hardware. */
                                _DRV_USBHS_DEVICE_FIFOFree(hDriver,endpointObject->fifoStartAddress, endpointObject->maxPacketSize);

                                if(direction == USB_DATA_DIRECTION_DEVICE_TO_HOST)
                                {
                                    /* Disable the TX interrupt */
                                    PLIB_USBHS_TxInterruptDisable(usbID, (1 << endpoint));
                                }
                                else
                                {
                                    /* Disable the RX interrupt */
                                    PLIB_USBHS_RxInterruptDisable(usbID, (1 << endpoint));
                                }            
                            }
                        }
                    }

                    /* Release interrupts and mutex */
                    if(hDriver->usbDrvCommonObj.isInInterruptContext == false)
                    {
                        if(interruptWasEnabled)
                        {
                            /* IF the interrupt was enabled when entering the routine
                             * re-enable it now */
                            _DRV_USBHS_InterruptSourceEnable(hDriver->usbDrvCommonObj.interruptSource);
                        }
                    }

                    if(hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA == false)
                    {
                        if(interruptWasEnabledUSBDMA)
                        {
                            /* Enable the interrupt only if it was disabled */
                            _DRV_USBHS_InterruptSourceEnable(hDriver->usbDrvCommonObj.interruptSourceUSBDma);
                        }
                    }

                    if(mutexLock == true)
                    {
                        /* OSAL: Return mutex */
                        if(OSAL_MUTEX_Unlock(&hDriver->usbDrvCommonObj.mutexID) != OSAL_RESULT_TRUE)
                        {
                            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Mutex unlock failed in DRV_USBHS_DEVICE_EndpointDisable()");
                        }
                    }

                    returnValue = USB_ERROR_NONE;
                } 
            } 
        } 
        else
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_EndpointDisable()");
        }
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_EndpointDisable()");
    }
    
    return (returnValue);
}

// *****************************************************************************
/* Function:
    bool DRV_USBHS_DEVICE_EndpointIsEnabled
    (
        DRV_HANDLE handle,
        USB_ENDPOINT endpointAndDirection
    )
    
  Summary:
    This function returns the enable/disable status of the specified endpoint
    and direction.
	
  Description:
    This function returns the enable/disable status of the specified endpoint
    and direction.

  Remarks:
    See drv_usbhs.h for usage information.
*/

bool DRV_USBHS_DEVICE_EndpointIsEnabled
(
    DRV_HANDLE handle,
    USB_ENDPOINT endpointAndDirection
)
{
    DRV_USBHS_OBJ * hDriver =  NULL;
    DRV_USBHS_DEVICE_ENDPOINT_OBJ * endpointObj = NULL;
    bool returnValue = false;
    uint8_t endpoint = endpointAndDirection & 0xF;
    int direction = ((endpointAndDirection & 0x80) != 0);

    if( (DRV_HANDLE_INVALID !=  handle) && (NULL != ((DRV_USBHS_CLIENT_OBJ *)handle)) )
    {
        if(((DRV_USBHS_CLIENT_OBJ *)handle)->inUse)
        {
            hDriver = ((DRV_USBHS_CLIENT_OBJ *)handle)->hDriver;
            endpointObj = hDriver->usbDrvCommonObj.endpointTable + (2 * endpoint) + direction;
            
            if((endpointObj->endpointState & DRV_USBHS_DEVICE_ENDPOINT_STATE_ENABLED) != 0)
            {
                returnValue = true;
            }
        }
        else
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_EndpointIsEnabled()");
        }
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_EndpointIsEnabled()");
    }
    
    /* Return the state of the endpoint */
    return (returnValue);
    
}

// *****************************************************************************
/* Function:
    bool DRV_USBHS_DEVICE_EndpointIsStalled
    (
        DRV_HANDLE handle, 
        USB_ENDPOINT endpointAndDirection
    ) 
  
  Summary:
    This function returns the stall status of the specified endpoint and
    direction.

  Description:
    This function returns the stall status of the specified endpoint and
    direction.

  Remarks:
    See drv_usbhs.h for usage information.
*/

bool DRV_USBHS_DEVICE_EndpointIsStalled
(
    DRV_HANDLE handle,
    USB_ENDPOINT endpointAndDirection
)
{
    DRV_USBHS_OBJ * hDriver =  NULL;
    DRV_USBHS_DEVICE_ENDPOINT_OBJ * endpointObj = NULL;
    bool returnValue = false;
    uint8_t endpoint = endpointAndDirection & 0xF;
    int direction = ((endpointAndDirection & 0x80) != 0);

    if( (DRV_HANDLE_INVALID !=  handle) && (NULL != ((DRV_USBHS_CLIENT_OBJ *)handle)) )
    {
        if(((DRV_USBHS_CLIENT_OBJ *)handle)->inUse)
        {
            hDriver = ((DRV_USBHS_CLIENT_OBJ *)handle)->hDriver;
            endpointObj = hDriver->usbDrvCommonObj.endpointTable + (2 * endpoint) + direction;
            
            if((endpointObj->endpointState & DRV_USBHS_DEVICE_ENDPOINT_STATE_STALLED) != 0)
            {
                returnValue = true;
            }
        }
        else
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_EndpointIsStalled()");
        }
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_EndpointIsStalled()");
    }
    
    /* Return the state of the endpoint */
    return (returnValue);
}

// ***********************************************************************************
/* Function:
    USB_ERROR DRV_USBHS_DEVICE_IRPSubmit
    (
        DRV_HANDLE client,
        USB_ENDPOINT endpointAndDirection,
        USB_DEVICE_IRP * irp
    );
    
  Summary:
    This function submits an I/O Request Packet (IRP) for processing to the
    Hi-Speed USB Driver.
	
  Description:
    This function submits an I/O Request Packet (IRP) for processing to the USB
    Driver. The IRP allows a client to send and receive data from the USB Host.
    The data will be sent or received through the specified endpoint. The direction
    of the data transfer is indicated by the direction flag in the
    endpointAndDirection parameter. Submitting an IRP arms the endpoint to
    either send data to or receive data from the Host.  If an IRP is already
    being processed on the endpoint, the subsequent IRP submit operation
    will be queued. The contents of the IRP (including the application buffers)
    should not be changed until the IRP has been processed.
    
    Particular attention should be paid to the size parameter of IRP. The
    following should be noted:
    
      * The size parameter while sending data to the Host can be less than,
        greater than, equal to, or be an exact multiple of the maximum packet size
        for the endpoint. The maximum packet size for the endpoint determines
        the number of transactions required to process the IRP.
      * If the size parameter, while sending data to the Host is less than the
        maximum packet size, the transfer will complete in one transaction.
      * If the size parameter, while sending data to the Host is greater
        than the maximum packet size, the IRP will be processed in multiple
        transactions.
      * If the size parameter, while sending data to the Host is equal to or
        an exact multiple of the maximum packet size, the client can optionally
        ask the driver to send a Zero Length Packet(ZLP) by specifying the
        USB_DEVICE_IRP_FLAG_DATA_COMPLETE flag as the flag parameter.
      * The size parameter, while receiving data from the Host must be an
        exact multiple of the maximum packet size of the endpoint. If this is
        not the case, the driver will return a USB_ERROR_IRP_SIZE_INVALID
        result. If while processing the IRP, the driver receives less than
        maximum packet size or a ZLP from the Host, the driver considers the
        IRP as processed. The size parameter at this point contains the actual
        amount of data received from the Host. The IRP status is returned as
        USB_DEVICE_IRP_STATUS_COMPLETED_SHORT.
      * If a ZLP needs to be sent to Host, the IRP size should be specified
        as 0 and the flag parameter should be set as
        USB_DEVICE_IRP_FLAG_DATA_COMPLETE.
      * If the IRP size is an exact multiple of the endpoint size, the client
        can request the driver to not send a ZLP by setting the flag parameter
        to USB_DEVICE_IRP_FLAG_DATA_PENDING. This flag indicates that there is
        more data pending in this transfer.
      * Specifying a size less than the endpoint size along with the
        USB_DEVICE_IRP_FLAG_DATA_PENDING flag will cause the driver to return a
        USB_ERROR_IRP_SIZE_INVALID.
      * If the size is greater than but not a multiple of the endpoint size, and
        the flag is specified as USB_DEVICE_IRP_FLAG_DATA_PENDING, the driver
        will send multiple of endpoint size number of bytes. For example, if the
        IRP size is 130 and the endpoint size if 64, the number of bytes sent
        will 128.

  Remarks:
    See drv_usbhs.h for usage information.
*/

USB_ERROR DRV_USBHS_DEVICE_IRPSubmit
(
    DRV_HANDLE handle,
    USB_ENDPOINT endpointAndDirection, 
    USB_DEVICE_IRP * inputIRP
)
{
    uint8_t endpoint;
    int direction;
    USBHS_MODULE_ID usbID = USBHS_NUMBER_OF_MODULES;
    DRV_USBHS_OBJ * hDriver =  NULL;
    USB_DEVICE_IRP_LOCAL * irp = (USB_DEVICE_IRP_LOCAL *)inputIRP;
    DRV_USBHS_DEVICE_ENDPOINT_OBJ * endpointObj = NULL;
    bool interruptWasEnabled = false;
    bool interruptWasEnabledUSBDMA = false;
    bool mutexLock = false;
    unsigned int count;
    uint8_t ep0Status;
    uint8_t rxEPStatus;
    bool isDmaUsed = false;
    bool tryDma = true;
    int remainder;
    USB_ERROR returnValue = USB_ERROR_NONE;

    /* Check for a valid client */
    if( (DRV_HANDLE_INVALID ==  handle) || (NULL == ((DRV_USBHS_CLIENT_OBJ *)handle)) )
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_IRPSubmit()");
        returnValue = USB_ERROR_PARAMETER_INVALID;
    }
    else if(irp->status > USB_DEVICE_IRP_STATUS_SETUP)
    {
        /* This means that the IRP is in use */
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Device IRP is already in use in DRV_USBHS_DEVICE_IRPSubmit()");
        returnValue = USB_ERROR_DEVICE_IRP_IN_USE;
    }
    else
    {
        /* Check for a valid endpoint */
        endpoint = endpointAndDirection & 0xF;
        direction = ((endpointAndDirection & 0x80) != 0);

        if(endpoint >= DRV_USBHS_ENDPOINTS_NUMBER)
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Endpoint is not provisioned for in DRV_USBHS_DEVICE_IRPSubmit()");
            returnValue = USB_ERROR_DEVICE_ENDPOINT_INVALID;
        }
        else if(false == ((DRV_USBHS_CLIENT_OBJ *)handle)->inUse)
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_IRPSubmit()");
            returnValue = USB_ERROR_PARAMETER_INVALID;
        }
        else
        {
            /* Get the driver object, the module ID and the endpoint and direction
             * specific BDT entry and the endpoint object. */

            hDriver = ((DRV_USBHS_CLIENT_OBJ *)handle)->hDriver;
            usbID = hDriver->usbDrvCommonObj.usbID;
            endpointObj = hDriver->usbDrvCommonObj.endpointTable + (2 * endpoint) + direction;

            if(!(endpointObj->endpointState & DRV_USBHS_DEVICE_ENDPOINT_STATE_ENABLED))
            {
                /* This means the endpoint is not enabled */
                returnValue = USB_ERROR_ENDPOINT_NOT_CONFIGURED;
            }
            else
            {
                /* Check the size of the IRP. If the endpoint receives data from the
                 * host, then IRP size must be multiple of maxPacketSize. If the send
                 * ZLP flag is set, then size must be multiple of endpoint size. */

                remainder = irp->size % endpointObj->maxPacketSize;

                if(remainder == 0)
                {
                    /* The IRP size is either 0 or a exact multiple of maxPacketSize */

                    if(USB_DATA_DIRECTION_DEVICE_TO_HOST == direction)
                    {
                        if(((irp->flags & USB_DEVICE_IRP_FLAG_DATA_COMPLETE) == USB_DEVICE_IRP_FLAG_DATA_COMPLETE) && (irp->size != 0))
                        {
                            /* This means a ZLP should be sent after the data is sent */
                            irp->flags |= USB_DEVICE_IRP_FLAG_SEND_ZLP;
                        }
                    }
                }
                else
                {
                    /* Not exact multiple of maxPacketSize */
                    if(USB_DATA_DIRECTION_HOST_TO_DEVICE == direction)
                    {
                        /* For receive IRP it needs to exact multiple of maxPacketSize.
                         * Hence this is an error condition. */
                        returnValue =  USB_ERROR_PARAMETER_INVALID;
                    }
                }

                if(returnValue == USB_ERROR_NONE)
                {
                    /* Now we check if the interrupt context is active. If so
                     * the we dont need to get a mutex or disable interrupts.
                     * If this were being done in non interrupt context, we, then
                     * we would disable the interrupt. In which case we would
                     * get the mutex and then disable the interrupt */

                    if((hDriver->usbDrvCommonObj.isInInterruptContext == false) && (hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA == false))
                    {
                        if(OSAL_MUTEX_Lock(&hDriver->usbDrvCommonObj.mutexID, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
                        {
                            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Mutex lock failed in DRV_USBHS_DEVICE_IRPSubmit()");
                            returnValue = USB_ERROR_OSAL_FUNCTION;
                        }
                        else
                        {
                            mutexLock = true;
                        }
                    }

                    if(returnValue != USB_ERROR_OSAL_FUNCTION)
                    {
                        if(hDriver->usbDrvCommonObj.isInInterruptContext == false)
                        {
                            interruptWasEnabled = _DRV_USBHS_InterruptSourceDisable(hDriver->usbDrvCommonObj.interruptSource);
                        }

                        if(hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA == false)
                        {
                            interruptWasEnabledUSBDMA = _DRV_USBHS_InterruptSourceDisable(hDriver->usbDrvCommonObj.interruptSourceUSBDma);
                        }

                        irp->next = NULL;

                        /* If the data is moving from device to host then pending bytes is data
                         * remaining to be sent to the host. If the data is moving from host to
                         * device, nPendingBytes tracks the amount of data received so far */

                        if(USB_DATA_DIRECTION_DEVICE_TO_HOST == direction)
                        {
                            irp->nPendingBytes = irp->size;
                        }
                        else
                        {
                            irp->nPendingBytes = 0;
                        }

                        /* Mark the IRP status as pending */
                        irp->status = USB_DEVICE_IRP_STATUS_PENDING;

                        /* Get the last object in the endpoint object IRP Queue */
                        if(endpointObj->irpQueue == NULL)
                        {
                            /* Queue is empty */
                            endpointObj->irpQueue = irp;
                            irp->previous = NULL;

                            irp->status = USB_DEVICE_IRP_STATUS_IN_PROGRESS;

                            if(endpoint == 0)
                            {
                                /* Endpoint zero IRPs require special handling */

                                if(direction == USB_DATA_DIRECTION_DEVICE_TO_HOST)
                                {
                                    /* Because we allow the client to provide three/two
                                     * different IRPs for the three/two stages of the control
                                     * transfer. we need to check what stage we are in when we
                                     * get an IRP. The endpoint0State is manipulated by both the
                                     * main thread and the interrupt thread. */

                                    /* Check if the endpoint is waiting for a IRP */
                                    if(hDriver->usbDrvDeviceObj.endpoint0State == DRV_USBHS_DEVICE_EP0_STATE_WAITING_FOR_TX_DATA_IRP_FROM_CLIENT)
                                    {
                                        /* In this state, the USB module is waiting for transmit
                                         * data from the client.  This would be a Control Read
                                         * transfer. And client has provided it */

                                        count = _DRV_USBHS_ProcessIRPFIFO(hDriver, endpointObj, (uint8_t)0, (uint8_t)USB_DATA_DIRECTION_DEVICE_TO_HOST, 
                                                irp, &isDmaUsed, tryDma);
                                        hDriver->usbDrvDeviceObj.endpoint0State = DRV_USBHS_DEVICE_EP0_STATE_TX_DATA_STAGE_IN_PROGRESS;

                                        if(count < endpointObj->maxPacketSize)
                                        {
                                            /* This means that this is the last packet in the
                                             * control read transfer. Set the transmit ready and
                                             * the data end bit */
                                            PLIB_USBHS_EP0TxPktRdyDataEnd(usbID);
                                        }
                                        else
                                        {
                                            /* This is a multi data stage transfer. Dont set the
                                             * data end bit. Only set the transmit ready bit. */
                                            PLIB_USBHS_EP0TxPktRdy(usbID);
                                        }
                                    }
                                    else if(hDriver->usbDrvDeviceObj.endpoint0State == DRV_USBHS_DEVICE_EP0_STATE_WAITING_FOR_TX_STATUS_IRP_FROM_CLIENT)
                                    {
                                        /* In this state, the module is waiting for the client
                                         * to complete the handshake stage of the control write
                                         * transfer. The client needs to let the module know
                                         * that it has received the data. */

                                        hDriver->usbDrvDeviceObj.endpoint0State = DRV_USBHS_DEVICE_EP0_STATE_WAITING_FOR_TX_STATUS_STAGE_FROM_HOST;
                                        PLIB_USBHS_EP0RxPktRdyServicedDataEnd(usbID);
                                    }
                                }
                                else
                                {
                                    /* Receive direction handling here */

                                    if(hDriver->usbDrvDeviceObj.endpoint0State == DRV_USBHS_DEVICE_EP0_STATE_WAITING_FOR_RX_DATA_IRP_FROM_CLIENT)
                                    {
                                        ep0Status = PLIB_USBHS_EP0StatusGet(usbID);

                                        /* Check if data is already present in the FIFO */
                                        if(ep0Status & USBHS_EP0_RXPKTRDY)
                                        {
                                            /* This means we already have data in the FIFO We
                                             * will need to check if the IRP is completed */

                                            count = _DRV_USBHS_ProcessIRPFIFO(hDriver, endpointObj, 0, USB_DATA_DIRECTION_HOST_TO_DEVICE, irp, &isDmaUsed, tryDma);

                                            /* Check if the irp is completed */
                                            if((count < endpointObj->maxPacketSize) || (irp->nPendingBytes >= irp->size))
                                            {
                                                /* This means we received a short packet or the
                                                 * amount of data received is the amount that
                                                 * was expected */

                                                if(irp->size < irp->nPendingBytes)
                                                {
                                                    irp->status = USB_DEVICE_IRP_STATUS_COMPLETED_SHORT;
                                                }
                                                else
                                                {
                                                    irp->status = USB_DEVICE_IRP_STATUS_COMPLETED;
                                                }

                                                irp->size = irp->nPendingBytes;

                                                endpointObj->irpQueue = irp->next;
                                                if(irp->callback)
                                                {
                                                    irp->callback((USB_DEVICE_IRP *)irp);
                                                }

                                                /* We can now move to the handshake stage. Check
                                                 * if a TX IRP is available */

                                                if((endpointObj + 1)->irpQueue != NULL)
                                                {
                                                    hDriver->usbDrvDeviceObj.endpoint0State = DRV_USBHS_DEVICE_EP0_STATE_WAITING_FOR_TX_STATUS_STAGE_FROM_HOST;

                                                    /* Data stage is complete. Clear the Rx Pkt
                                                     * Ready and set the data end bit. */
                                                    PLIB_USBHS_EP0RxPktRdyServicedDataEnd(usbID);
                                                }
                                                else
                                                {
                                                    hDriver->usbDrvDeviceObj.endpoint0State = DRV_USBHS_DEVICE_EP0_STATE_WAITING_FOR_TX_STATUS_IRP_FROM_CLIENT;
                                                }
                                            }
                                            else
                                            {
                                                /* We are expecting more data. Let the module
                                                 * know that we have completed reading the data
                                                 * */
                                                PLIB_USBHS_EP0RxPktRdyServiced(usbID);
                                            }
                                        }
                                        else
                                        {
                                            /* There is no data in the FIFO yet */
                                            hDriver->usbDrvDeviceObj.endpoint0State = DRV_USBHS_DEVICE_EP0_STATE_WAITING_FOR_RX_DATA_STAGE_FROM_HOST;
                                        }
                                    }
                                    else if(hDriver->usbDrvDeviceObj.endpoint0State == DRV_USBHS_DEVICE_EP0_STATE_WAITING_FOR_RX_STATUS_IRP_FROM_CLIENT)
                                    {
                                        /* This IRP will receive the status stage ZLP for a
                                         * control read transfer. The ZLP in this case comes
                                         * from the host. */

                                        hDriver->usbDrvDeviceObj.endpoint0State = DRV_USBHS_DEVICE_EP0_STATE_WAITING_FOR_RX_STATUS_STAGE_FROM_HOST;
                                        PLIB_USBHS_EP0DataEndSet(usbID);
                                    }
                                }
                            }
                            else
                            {
                                /* Non zero endpoint irp */

                                if(direction == USB_DATA_DIRECTION_DEVICE_TO_HOST)
                                {
                                    /* Data is moving from device to host. So this a TX IRP. We
                                     * dont have to check for the TXxPktRdy bit because the fact
                                     * that the queue is empty means there is nothing in the
                                     * queue */

                                    _DRV_USBHS_ProcessIRPFIFO(hDriver, endpointObj, endpoint, direction, irp, &isDmaUsed, tryDma);

                                    if(isDmaUsed == false)
                                    {
                                        /* Set the TX packet Ready bit */

                                        PLIB_USBHS_DeviceTxEndpointPacketReady(usbID, endpoint);
                                    }

                                    /* The rest of the IRP processing takes place in ISR */
                                }
                                else
                                {
                                    /* We have an IRP to process. Lets check if the RxPktRdy bit
                                     * is already set on the endpoint. If so we can start
                                     * unloading the fifo */

                                    rxEPStatus = PLIB_USBHS_RxEPStatusGet(usbID, endpoint);

                                    if(rxEPStatus & USBHS_RXEP_PKTRDY)
                                    {
                                        /* This means the FIFO already contains data */
                                        tryDma = false;
                                        count = _DRV_USBHS_ProcessIRPFIFO(hDriver, endpointObj, endpoint, direction, irp, &isDmaUsed, tryDma);

                                        if(isDmaUsed == false)
                                        {
                                            /* Clear the Rx Packet Ready Bit */
                                            PLIB_USBHS_RxEPStatusClear(usbID, endpoint, USBHS_RXEP_PKTRDY);

                                            /* Check if the irp is completed */

                                            if((count < endpointObj->maxPacketSize) || (irp->nPendingBytes >= irp->size))
                                            {
                                                /* This means we received a short packet or the
                                                 * amount of data received is the amount that
                                                 * was expected */

                                                if(irp->size < irp->nPendingBytes)
                                                {
                                                    irp->status = USB_DEVICE_IRP_STATUS_COMPLETED_SHORT;
                                                }
                                                else
                                                {
                                                    irp->status = USB_DEVICE_IRP_STATUS_COMPLETED;
                                                }

                                                irp->size = irp->nPendingBytes;

                                                irp->nPendingBytes = 0;

                                                endpointObj->irpQueue = irp->next;
                                                if(irp->callback)
                                                {
                                                    irp->callback((USB_DEVICE_IRP *)irp);
                                                }
                                            }
                                        } 
                                    }
                                }
                            }
                        }
                        else
                        {
                            /* This means we should surf the linked list to get to the last
                             * entry . */
                            USB_DEVICE_IRP_LOCAL * iterator;
                            iterator = endpointObj->irpQueue;
                            while(iterator->next != NULL)
                            {
                                iterator = iterator->next;
                            }
                            iterator->next = irp;
                            irp->previous = iterator;
                            irp->status = USB_DEVICE_IRP_STATUS_PENDING;
                        }

                        if(hDriver->usbDrvCommonObj.isInInterruptContext == false)
                        {
                            if(interruptWasEnabled)
                            {
                                /* Enable the interrupt only if it was disabled */
                                _DRV_USBHS_InterruptSourceEnable(hDriver->usbDrvCommonObj.interruptSource);
                            }
                        }

                        if(hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA == false)
                        {
                            if(interruptWasEnabledUSBDMA)
                            {
                                /* Enable the interrupt only if it was disabled */
                                _DRV_USBHS_InterruptSourceEnable(hDriver->usbDrvCommonObj.interruptSourceUSBDma);
                            }
                        }

                        if(mutexLock == true)
                        {
                            if(OSAL_MUTEX_Unlock(&hDriver->usbDrvCommonObj.mutexID) != OSAL_RESULT_TRUE)
                            {
                                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Mutex unlock failed in DRV_USBHS_DEVICE_IRPSubmit()");
                            }
                        }
                    }
                }
            }
        }
    }

    return (returnValue);
}

// *****************************************************************************
/* Function:
    void _DRV_USBHS_DEVICE_IRPQueueFlush
    (
       DRV_USBHS_DEVICE_ENDPOINT_OBJ * endpointObject,
       USB_DEVICE_IRP_STATUS status

    )

  Summary:
    Dynamic implementation of _DRV_USBHS_DEVICE_IRPQueueFlush function.

  Description:
    This is the dynamic implementation of _DRV_USBHS_DEVICE_IRPQueueFlush
    function for USB device.
    Function scans for all the IRPs on the endpoint queue and cancels them all.

  Remarks:
    This is a local function and should not be called directly by the
    application.
*/

void _DRV_USBHS_DEVICE_IRPQueueFlush
(
    DRV_USBHS_DEVICE_ENDPOINT_OBJ * endpointObject,
    USB_DEVICE_IRP_STATUS status

)
{
    USB_DEVICE_IRP_LOCAL * iterator;
    
    /* Check if any IRPs are assigned on this endpoint and 
     * abort them */

    if(endpointObject->irpQueue != NULL)
    {
        /* Cancel the IRP and deallocate driver IRP 
         * objects */

        iterator = endpointObject->irpQueue;

        while(iterator != NULL)
        {
            iterator->status = status;
            if(iterator->callback != NULL)
            {
                iterator->callback((USB_DEVICE_IRP *)iterator);
            }
            iterator = iterator->next;
        }
    }
    
    /* Set the head pointer to NULL */
    endpointObject->irpQueue = NULL;
}

// *****************************************************************************
/* Function:
      USB_ERROR DRV_USBHS_DEVICE_IRPCancelAll
      (
         DRV_HANDLE client,
         USB_ENDPOINT endpointAndDirection
      )

  Summary:
    Dynamic implementation of DRV_USBHS_DEVICE_IRPCancelAll client interface
    function.

  Description:
    This is the dynamic implementation of DRV_USBHS_DEVICE_IRPCancelAll client
    interface function for USB device.
    Function checks the validity of the input arguments and on success cancels
    all the IRPs on the specific endpoint object queue.

  Remarks:
    See drv_usbhs.h for usage information.
*/

USB_ERROR DRV_USBHS_DEVICE_IRPCancelAll
(
    DRV_HANDLE handle,
    USB_ENDPOINT endpointAndDirection
)
{
    int direction = 0;
    uint8_t endpoint = 0;
    DRV_USBHS_OBJ * hDriver = NULL;
    USBHS_MODULE_ID   usbID = USBHS_NUMBER_OF_MODULES;
    DRV_USBHS_DEVICE_ENDPOINT_OBJ * endpointObject = NULL;
    bool interruptWasEnabled = false;
    bool interruptWasEnabledUSBDMA = false;
    bool mutexLock = false;
    USB_ERROR returnValue = USB_ERROR_PARAMETER_INVALID;

    endpoint = endpointAndDirection & 0xF;
    direction = ((endpointAndDirection & 0x80) != 0);

    if(endpoint < DRV_USBHS_ENDPOINTS_NUMBER)
    {
        if( (DRV_HANDLE_INVALID !=  handle) && (NULL != ((DRV_USBHS_CLIENT_OBJ *)handle)) )
        {
            if(((DRV_USBHS_CLIENT_OBJ *)handle)->inUse)
            {
                hDriver = ((DRV_USBHS_CLIENT_OBJ *)handle)->hDriver;
                usbID = hDriver->usbDrvCommonObj.usbID;

                /* Get the endpoint object */
                endpointObject = hDriver->usbDrvCommonObj.endpointTable + (2 * endpoint) + direction;

                if((hDriver->usbDrvCommonObj.isInInterruptContext == false) && (hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA == false))
                {
                    if(OSAL_MUTEX_Lock(&hDriver->usbDrvCommonObj.mutexID, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
                    {
                        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Mutex lock failed in DRV_USBHS_DEVICE_IRPCancelAll()");
                        returnValue = USB_ERROR_OSAL_FUNCTION;
                    }
                    else
                    {
                        mutexLock = true;
                    }
                }

                if(USB_ERROR_OSAL_FUNCTION != returnValue)
                {
                    if(hDriver->usbDrvCommonObj.isInInterruptContext == false)
                    {
                        interruptWasEnabled = _DRV_USBHS_InterruptSourceDisable(hDriver->usbDrvCommonObj.interruptSource);
                    }

                    if(hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA == false)
                    {
                        interruptWasEnabledUSBDMA = _DRV_USBHS_InterruptSourceDisable(hDriver->usbDrvCommonObj.interruptSourceUSBDma);
                    }

                    /* Flush the endpoint */
                    _DRV_USBHS_DEVICE_IRPQueueFlush(endpointObject, USB_DEVICE_IRP_STATUS_ABORTED);

                    /* We need to flush the TXFIFO if any of them are loaded.
                     * Endpoint zero does not have a direction.  For other
                     * endpoints we check if the direction is transmit */

                    if((endpoint == 0) || ((endpoint != 0) && (direction == USB_DATA_DIRECTION_DEVICE_TO_HOST)))
                    {
                        if(endpoint == 0)
                        {
                        }  
                        else
                        {
                            PLIB_USBHS_EndpointTxFIFOFlush(usbID, endpoint);
                        }
                    }

                    if(hDriver->usbDrvCommonObj.isInInterruptContext == false)
                    {
                        if(interruptWasEnabled)
                        {
                            /* Enable the interrupt only if it was disabled */
                            _DRV_USBHS_InterruptSourceEnable(hDriver->usbDrvCommonObj.interruptSource);
                        }
                    }

                    if(hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA == false)
                    {
                        if(interruptWasEnabledUSBDMA)
                        {
                            /* Enable the interrupt only if it was disabled */
                            _DRV_USBHS_InterruptSourceEnable(hDriver->usbDrvCommonObj.interruptSourceUSBDma);
                        }
                    }

                    if(mutexLock == true)
                    {
                        if(OSAL_MUTEX_Unlock(&hDriver->usbDrvCommonObj.mutexID) != OSAL_RESULT_TRUE)
                        {
                            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Mutex unlock failed in DRV_USBHS_DEVICE_IRPCancelAll()");
                        }
                    }

                    returnValue = USB_ERROR_NONE;
                }
            }
            else
            {
                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_IRPCancelAll()");
            }
        }
        else
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_IRPCancelAll()");
        }
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO,"\r\nUSBHS Driver: Unsupported endpoint in DRV_USBHS_DEVICE_IRPCancelAll()");
        returnValue = USB_ERROR_DEVICE_ENDPOINT_INVALID;
    }
    
    return (returnValue);
}

// ****************************************************************************
/* Function:
    void DRV_USBHS_HOST_IRPCancel(USB_HOST_IRP * inputIRP);
    
  Summary:
    Cancels the specified IRP.
	
  Description:
    This function attempts to cancel the specified IRP. If the IRP is queued and
    its processing has not started, it will be cancelled successfully. If the
    IRP in progress, the ongoing transaction will be allowed to complete. 

  Remarks:
    See drv_usbhs.h for usage information.
*/

USB_ERROR DRV_USBHS_DEVICE_IRPCancel
(
    DRV_HANDLE client, 
    USB_DEVICE_IRP * irp
)
{
    USB_ERROR returnValue = USB_ERROR_PARAMETER_INVALID;
    DRV_USBHS_OBJ * hDriver = NULL;
    USB_DEVICE_IRP_LOCAL * irpToCancel = (USB_DEVICE_IRP_LOCAL *)irp;
    bool interruptWasEnabled = false;
    bool interruptWasEnabledUSBDMA = false;
    bool mutexLock = false;
    
    /* Check if the handle is valid */
    if( (DRV_HANDLE_INVALID !=  client) && (NULL != ((DRV_USBHS_CLIENT_OBJ *)client)) )
    {
        if(NULL != irpToCancel)
        {
            if(((DRV_USBHS_CLIENT_OBJ *)client)->inUse)
            {
                hDriver = ((DRV_USBHS_CLIENT_OBJ *)client)->hDriver;

                if((hDriver->usbDrvCommonObj.isInInterruptContext == false) && (hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA == false))
                {
                    if(OSAL_MUTEX_Lock(&hDriver->usbDrvCommonObj.mutexID, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
                    {
                        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Mutex lock failed in DRV_USBHS_DEVICE_IRPCancel()");
                        returnValue = USB_ERROR_OSAL_FUNCTION;
                    }
                    else
                    {
                        mutexLock = true;
                        if(hDriver->usbDrvCommonObj.isInInterruptContext == false)
                        {
                            interruptWasEnabled = _DRV_USBHS_InterruptSourceDisable(hDriver->usbDrvCommonObj.interruptSource);
                        }

                        if(hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA == false)
                        {
                            interruptWasEnabledUSBDMA = _DRV_USBHS_InterruptSourceDisable(hDriver->usbDrvCommonObj.interruptSourceUSBDma);
                        }
                    }
                }

                if(USB_ERROR_OSAL_FUNCTION != returnValue)
                {
                    if(irpToCancel->status <= USB_DEVICE_IRP_STATUS_COMPLETED_SHORT)
                    {
                        /* This IRP has either completed or has been aborted.*/
                    }
                    else
                    {
                        /* The code will come here both when the IRP is NOT the
                         * 1st in queue as well as when it is at the HEAD. We
                         * will change the IRP status for either scenario but
                         * will give the callback only if it is NOT at the HEAD
                         * of the queue.
                         * 
                         * What it means for HEAD IRP case is it will be caught
                         * in USB ISR and will be further processed in ISR. This
                         * is done to make sure that the user cannot release the
                         * IRP buffer before ABORT callback*/

                        /* Mark the IRP status as aborted */
                        irpToCancel->status = USB_DEVICE_IRP_STATUS_ABORTED;

                        /* No data for this IRP was sent or received */
                        irpToCancel->size = 0;

                        if(irpToCancel->previous != NULL)
                        {
                            /* This means this is not the HEAD IRP in the IRP
                             * queue.  Can be removed from the endpoint object
                             * queue safely.*/
                            irpToCancel->previous->next = irpToCancel->next;

                            if(irpToCancel->next != NULL)
                            {
                                /* If this is not the last IRP in the queue then
                                 * update the previous link connection for the
                                 * next IRP */
                                irpToCancel->next->previous = irpToCancel->previous;
                            }

                            irpToCancel->previous = NULL;
                            irpToCancel->next = NULL;

                            if(irpToCancel->callback != NULL)
                            {
                                irpToCancel->callback((USB_DEVICE_IRP *)irpToCancel);
                            }
                        }

                        returnValue = USB_ERROR_NONE;
                    }
                }

                if(USB_ERROR_NONE == returnValue)
                {
                    if(hDriver->usbDrvCommonObj.isInInterruptContext == false)
                    {
                        if(interruptWasEnabled)
                        {
                            /* Enable the interrupt only if it was disabled */
                            _DRV_USBHS_InterruptSourceEnable(hDriver->usbDrvCommonObj.interruptSource);
                        }
                    }

                    if(hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA == false)
                    {
                        if(interruptWasEnabledUSBDMA)
                        {
                            /* Enable the interrupt only if it was disabled */
                            _DRV_USBHS_InterruptSourceEnable(hDriver->usbDrvCommonObj.interruptSourceUSBDma);
                        }
                    }

                    if(mutexLock == true)
                    {
                        /* OSAL: Return mutex */
                        if(OSAL_MUTEX_Unlock(&hDriver->usbDrvCommonObj.mutexID) != OSAL_RESULT_TRUE)
                        {
                            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Mutex unlock failed in DRV_USBHS_DEVICE_IRPCancel()");
                        }
                    }
                }
            }
            else
            {
                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_IRPCancel()");
            }
        }
        else
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: IRP is invalid in DRV_USBHS_DEVICE_IRPCancel()");
        }
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_IRPCancel()");
    }

    return (returnValue);
}

// *****************************************************************************
/* Function:
      USB_ERROR DRV_USBHS_DEVICE_EndpointStall
      (
        DRV_HANDLE client,
        USB_ENDPOINT endpointAndDirection
      )

  Summary:
    Dynamic implementation of DRV_USBHS_DEVICE_EndpointStall client interface
    function.

  Description:
    This is the dynamic implementation of DRV_USBHS_DEVICE_EndpointStall client
    interface function for USB device.
    Function sets the STALL state of the specified endpoint.

  Remarks:
    See drv_usbhs.h for usage information.
*/

USB_ERROR DRV_USBHS_DEVICE_EndpointStall
(
    DRV_HANDLE handle,
    USB_ENDPOINT endpointAndDirection
)
{
    int direction = 0;
    bool interruptWasEnabled = false;
    bool interruptWasEnabledUSBDMA = false;
    bool mutexLock = false;
    uint8_t endpoint = 0;
    DRV_USBHS_OBJ * hDriver = NULL;
    USBHS_MODULE_ID usbID = USBHS_NUMBER_OF_MODULES;
    DRV_USBHS_DEVICE_ENDPOINT_OBJ * endpointObject = NULL;
    USB_ERROR returnValue = USB_ERROR_PARAMETER_INVALID;

    endpoint = endpointAndDirection & 0xF;
    direction = ((endpointAndDirection & 0x80) != 0); 

    if(endpoint < DRV_USBHS_ENDPOINTS_NUMBER)
    {
        if( (DRV_HANDLE_INVALID !=  handle) && (NULL != ((DRV_USBHS_CLIENT_OBJ *)handle)) )
        {
            if(((DRV_USBHS_CLIENT_OBJ *)handle)->inUse)
            {
                hDriver = ((DRV_USBHS_CLIENT_OBJ *)handle)->hDriver;
                usbID = hDriver->usbDrvCommonObj.usbID;

                /* Get the endpoint object */
                endpointObject = hDriver->usbDrvCommonObj.endpointTable + (2 * endpoint);

                if((hDriver->usbDrvCommonObj.isInInterruptContext == false) && (hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA == false))
                {
                    if(OSAL_MUTEX_Lock(&hDriver->usbDrvCommonObj.mutexID, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
                    {
                        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Mutex lock failed in DRV_USBHS_DEVICE_EndpointStall()");
                        returnValue = USB_ERROR_OSAL_FUNCTION;
                    }
                    else
                    {
                        mutexLock = true;
                    }
                }

                if(USB_ERROR_OSAL_FUNCTION != returnValue)
                {
                    if(hDriver->usbDrvCommonObj.isInInterruptContext == false)
                    {
                        interruptWasEnabled = _DRV_USBHS_InterruptSourceDisable(hDriver->usbDrvCommonObj.interruptSource);
                    }

                    if(hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA == false)
                    {
                        interruptWasEnabledUSBDMA = _DRV_USBHS_InterruptSourceDisable(hDriver->usbDrvCommonObj.interruptSourceUSBDma);
                    }

                    if(endpoint == 0)
                    {
                        PLIB_USBHS_EP0StallEnable(usbID);

                        /* While stalling endpoint 0 we stall both directions */
                        endpointObject->endpointState |= DRV_USBHS_DEVICE_ENDPOINT_STATE_STALLED;
                        _DRV_USBHS_DEVICE_IRPQueueFlush(endpointObject, USB_DEVICE_IRP_STATUS_ABORTED_ENDPOINT_HALT);
                        endpointObject ++;
                        endpointObject->endpointState |= DRV_USBHS_DEVICE_ENDPOINT_STATE_STALLED;
                        _DRV_USBHS_DEVICE_IRPQueueFlush(endpointObject, USB_DEVICE_IRP_STATUS_ABORTED_ENDPOINT_HALT);
                    }
                    else
                    {
                        /* Stalling a non zero endpoint */
                        if(direction == USB_DATA_DIRECTION_DEVICE_TO_HOST)
                        {
                            /* Stall the TX direction */
                            PLIB_USBHS_DeviceTxEndpointStallEnable(usbID, endpoint);
                        }
                        else
                        {
                            /* Stall the RX direction */
                            PLIB_USBHS_DeviceRxEndpointStallEnable(usbID, endpoint);
                        }

                        endpointObject += direction;
                        endpointObject->endpointState |= DRV_USBHS_DEVICE_ENDPOINT_STATE_STALLED;
                        _DRV_USBHS_DEVICE_IRPQueueFlush(endpointObject, USB_DEVICE_IRP_STATUS_ABORTED_ENDPOINT_HALT);
                    }

                    if(hDriver->usbDrvCommonObj.isInInterruptContext == false)
                    {
                        if(interruptWasEnabled)
                        {
                            /* Enable the interrupt only if it was disabled */
                            _DRV_USBHS_InterruptSourceEnable(hDriver->usbDrvCommonObj.interruptSource);
                        }
                    }

                    if(hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA == false)
                    {
                        if(interruptWasEnabledUSBDMA)
                        {
                            /* Enable the interrupt only if it was disabled */
                            _DRV_USBHS_InterruptSourceEnable(hDriver->usbDrvCommonObj.interruptSourceUSBDma);
                        }
                    }

                    if(mutexLock == true)
                    {
                        if(OSAL_MUTEX_Unlock(&hDriver->usbDrvCommonObj.mutexID) != OSAL_RESULT_TRUE)
                        {
                            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Mutex unlock failed in DRV_USBHS_DEVICE_EndpointStall()");
                        }
                    }

                    returnValue = USB_ERROR_NONE;
                }
            }
            else
            {
                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_EndpointStall()");
            }
        }
        else
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_EndpointStall()");
        }
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO,"\r\nUSBHS Driver: Unsupported endpoint in DRV_USBHS_DEVICE_EndpointStall()");
        returnValue = USB_ERROR_DEVICE_ENDPOINT_INVALID;
    }

    return (returnValue);

}/* End of DRV_USBHS_DEVICE_EndpointStall() */

// *****************************************************************************
/* Function:
      USB_ERROR DRV_USBHS_DEVICE_EndpointStallClear
      (
        DRV_HANDLE client,
        USB_ENDPOINT endpointAndDirection
      )

  Summary:
    Dynamic implementation of DRV_USBHS_DEVICE_EndpointStallClear client interface
    function.

  Description:
    This is the dynamic implementation of DRV_USBHS_DEVICE_EndpointStallClear
    client interface function for USB device.  Function clears the STALL state
    of the specified endpoint and resets the data toggle value.

  Remarks:
    See drv_usbhs.h for usage information.
*/

USB_ERROR DRV_USBHS_DEVICE_EndpointStallClear
(
    DRV_HANDLE handle,
    USB_ENDPOINT endpointAndDirection
)
{
    int direction = 0;
    bool interruptWasEnabled = false;
    bool interruptWasEnabledUSBDMA = false;
    bool mutexLock = false;
    uint8_t endpoint = 0;
    DRV_USBHS_OBJ  * hDriver = NULL;
    USBHS_MODULE_ID   usbID = USBHS_NUMBER_OF_MODULES;
    DRV_USBHS_DEVICE_ENDPOINT_OBJ * endpointObject = NULL;
    USB_ERROR returnValue = USB_ERROR_PARAMETER_INVALID;

    endpoint = endpointAndDirection & 0xF;
    direction = ((endpointAndDirection & 0x80) != 0);

    if(endpoint < DRV_USBHS_ENDPOINTS_NUMBER)
    {
        if( (DRV_HANDLE_INVALID !=  handle) && (NULL != ((DRV_USBHS_CLIENT_OBJ *)handle)) )
        {
            if(((DRV_USBHS_CLIENT_OBJ *)handle)->inUse)
            {
                hDriver = ((DRV_USBHS_CLIENT_OBJ *)handle)->hDriver;
                usbID = hDriver->usbDrvCommonObj.usbID;

                if(endpoint == 0)
                {
                    /* For endpoint zero, the STALL is automatically cleared
                     * when the STALL is sent to the host */
                    returnValue = USB_ERROR_NONE;
                }
                else
                {
                    if((hDriver->usbDrvCommonObj.isInInterruptContext == false) && (hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA == false))
                    {
                        if(OSAL_MUTEX_Lock(&hDriver->usbDrvCommonObj.mutexID, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
                        {
                            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Mutex lock failed in DRV_USBHS_DEVICE_EndpointStallClear()");
                            returnValue = USB_ERROR_OSAL_FUNCTION;
                        }
                        else
                        {
                            mutexLock = true;
                        }
                    }

                    if(USB_ERROR_OSAL_FUNCTION != returnValue)
                    {
                        if(hDriver->usbDrvCommonObj.isInInterruptContext == false)
                        {
                            interruptWasEnabled = _DRV_USBHS_InterruptSourceDisable(hDriver->usbDrvCommonObj.interruptSource);
                        }

                        if(hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA == false)
                        {
                            interruptWasEnabledUSBDMA = _DRV_USBHS_InterruptSourceDisable(hDriver->usbDrvCommonObj.interruptSourceUSBDma);
                        }

                        if(direction == USB_DATA_DIRECTION_DEVICE_TO_HOST)
                        {
                            /* Clear the stall on the TX direction and the stall
                             * sent bit */

                            PLIB_USBHS_DeviceTxEndpointStallDisable(usbID, endpoint);
                        }
                        else
                        {
                            /* Clear the stall on the RX direction and the stall
                             * sent bit */

                            PLIB_USBHS_DeviceRxEndpointStallDisable(usbID, endpoint);
                        }

                        /* Get the endpoint object */
                        endpointObject = hDriver->usbDrvCommonObj.endpointTable + (2 * endpoint) + direction;
                        endpointObject->endpointState &= ~DRV_USBHS_DEVICE_ENDPOINT_STATE_STALLED; 
                        _DRV_USBHS_DEVICE_IRPQueueFlush(endpointObject, USB_DEVICE_IRP_STATUS_TERMINATED_BY_HOST);

                        if(hDriver->usbDrvCommonObj.isInInterruptContext == false)
                        {
                            if(interruptWasEnabled)
                            {
                                /* Enable the interrupt only if it was disabled */
                                _DRV_USBHS_InterruptSourceEnable(hDriver->usbDrvCommonObj.interruptSource);
                            }
                        }

                        if(hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA == false)
                        {
                            if(interruptWasEnabledUSBDMA)
                            {
                                /* Enable the interrupt only if it was disabled */
                                _DRV_USBHS_InterruptSourceEnable(hDriver->usbDrvCommonObj.interruptSourceUSBDma);
                            }
                        }

                        if(mutexLock == true)
                        {
                            if(OSAL_MUTEX_Unlock(&hDriver->usbDrvCommonObj.mutexID) != OSAL_RESULT_TRUE)
                            {
                                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Mutex unlock failed in DRV_USBHS_DEVICE_EndpointStallClear()");
                            }
                        }

                        returnValue = USB_ERROR_NONE;
                    }
                }
            }
            else
            {
                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_EndpointStallClear()");
            }
        }
        else
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_EndpointStallClear()");
        }
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO,"\r\nUSBHS Driver: Unsupported endpoint in DRV_USBHS_DEVICE_EndpointStallClear()");
        returnValue = USB_ERROR_DEVICE_ENDPOINT_INVALID;
    }
    
    return (returnValue);
}

// *****************************************************************************
/* Function:
      void _DRV_USBHS_DEVICE_Tasks_ISR(DRV_USBHS_OBJ * hDriver)

  Summary:
    Dynamic implementation of _DRV_USBHS_DEVICE_Tasks_ISR ISR handler function.

  Description:
    This is the dynamic implementation of _DRV_USBHS_DEVICE_Tasks_ISR ISR handler
    function for USB device.
    Function will get called automatically due to USB interrupts in interrupt mode.
    In polling mode this function will be routinely called from USB driver
    DRV_USBHS_Tasks() function.
    This function performs necessary action based on the interrupt and clears the
    interrupt after that. The USB device layer callback is called with the
    interrupt event details, if callback function is registered.

  Remarks:
    This is a local function and should not be called directly by the
    application.
*/

void _DRV_USBHS_DEVICE_Tasks_ISR
(
    DRV_USBHS_OBJ * hDriver
)
{
    uint8_t usbInterrupts = 0;
    uint16_t endpointMask = 0;
    uint8_t  iEndpoint = 0;
    uint8_t  ep0Status = 0;
    uint8_t  txEPStatus = 0;
    uint8_t  rxEPStatus = 0;
    uint8_t endpointTXInterrupts = 0;
    uint8_t endpointRXInterrupts = 0;
    uint16_t endpoint0DataStageSize = 0;
    DRV_USBHS_DEVICE_ENDPOINT_OBJ * endpointObjReceive = NULL;
    DRV_USBHS_DEVICE_ENDPOINT_OBJ * endpointObjTransmit = NULL;
    USBHS_MODULE_ID usbID = USBHS_NUMBER_OF_MODULES;
    USB_DEVICE_IRP_LOCAL * irp = NULL;
    unsigned int endpoint0DataStageDirection = 0;
    DRV_USBHS_CLIENT_OBJ * deviceModeClient = NULL;

    unsigned int count = 0;
    uint8_t * data = NULL;
    bool isDmaUsed = false;
    bool tryDma = true;

    _DRV_USBHS_NonPersistentInterruptSourceClear(hDriver->usbDrvCommonObj.interruptSource);

    usbID = hDriver->usbDrvCommonObj.usbID;

    /* Reading the interrupt flags will cause the flags to get cleared */

    usbInterrupts = PLIB_USBHS_GenInterruptFlagsGet(usbID);

    /* The fact the we have entered this function could mean two things, the
     * driver was either configured for device mode operation and is operating
     * in device mode or is configured for dual role operation and is presently
     * operatinf in device mode. In either case, deviceModeClient will contain
     * the client that is operating the driver in device mode. */

    deviceModeClient = hDriver->usbDrvCommonObj.deviceModeClient;

    if(usbInterrupts & USBHS_GENINT_RESET)
    {
        /* This means that RESET signaling was detected on the bus. This
         * means the packet that we should get on the bus for EP0 should be
         * a setup packet */

        hDriver->usbDrvDeviceObj.endpoint0State = DRV_USBHS_DEVICE_EP0_STATE_EXPECTING_SETUP_FROM_HOST;

        /* Default speed is full speed */
        hDriver->usbDrvCommonObj.deviceSpeed = USB_SPEED_FULL;

        if(hDriver->usbDrvCommonObj.operationSpeed == USB_SPEED_HIGH)
        {
            /* If high speed operation was specified and we were able to
             * connect at high speed then update the speed.  */
            if(PLIB_USBHS_HighSpeedIsConnected(usbID))
            {
                /* This means high speed was successful*/
                hDriver->usbDrvCommonObj.deviceSpeed = USB_SPEED_HIGH;
            }
        }

        if(NULL != deviceModeClient->pEventCallBack)
        {
            deviceModeClient->pEventCallBack(deviceModeClient->hClientArg, DRV_USBHS_EVENT_RESET_DETECT,  NULL);
        }
    }

    if(usbInterrupts & USBHS_GENINT_SUSPEND)
    {
        /* This means that the bus was SUSPENDED. */
        if(NULL != deviceModeClient->pEventCallBack)
        {
            deviceModeClient->pEventCallBack(deviceModeClient->hClientArg, DRV_USBHS_EVENT_IDLE_DETECT,  NULL);
        }
    }

    if(usbInterrupts & USBHS_GENINT_RESUME)
    {
        /* This means that RESUME was detected on the bus. */
        if(NULL != deviceModeClient->pEventCallBack)
        {
            deviceModeClient->pEventCallBack(deviceModeClient->hClientArg, DRV_USBHS_EVENT_RESUME_DETECT,  NULL);
        }
    }        

    if(usbInterrupts & USBHS_GENINT_SOF)
    {
        /* This means that there was a SOF. */
        if(NULL != deviceModeClient->pEventCallBack)
        {
            deviceModeClient->pEventCallBack(deviceModeClient->hClientArg, DRV_USBHS_EVENT_SOF_DETECT,  NULL);
        }
    }        

    /* Read the endpoint interrupts */

    endpointTXInterrupts = PLIB_USBHS_TxInterruptFlagsGet(usbID);
    endpointRXInterrupts = PLIB_USBHS_RxInterruptFlagsGet(usbID);

    /* Setup the endpoint objects */

    endpointObjReceive = hDriver->usbDrvCommonObj.endpointTable; 
    endpointObjTransmit = hDriver->usbDrvCommonObj.endpointTable + 1;

    if(endpointTXInterrupts & USBHS_TXRXINT_EP0)
    {
        /* This means this is EP0 interrupt */
        PLIB_USBHS_LoadEPInIndex(usbID, 0);
        ep0Status = PLIB_USBHS_EP0StatusGet(usbID);

        if(ep0Status & USBHS_EP0_SENTSTALL)
        {
            /* This means a stall was sent. Reset the endpoint 0 state.
             * Clear the sent stall bit */
            hDriver->usbDrvDeviceObj.endpoint0State = DRV_USBHS_DEVICE_EP0_STATE_EXPECTING_SETUP_FROM_HOST;
            PLIB_USBHS_EP0SentStallClear(usbID);
            endpointObjReceive->endpointState &= ~DRV_USBHS_DEVICE_ENDPOINT_STATE_STALLED;
            endpointObjTransmit->endpointState &= ~DRV_USBHS_DEVICE_ENDPOINT_STATE_STALLED;
        }

        if(ep0Status & USBHS_EP0_SETUPEND)
        {
            /* This means the current control transfer end prematurely. We dont
             * need to end any transfers. The device layer will manage the
             * premature transfer end. We clear the SetupEnd bit and reset the
             * driver control transfer state machine to waiting for next setup
             * packet from host. */
            PLIB_USBHS_EP0SetupEndServiced(usbID);
            hDriver->usbDrvDeviceObj.endpoint0State = DRV_USBHS_DEVICE_EP0_STATE_EXPECTING_SETUP_FROM_HOST;
        }

        /* We have to check for these conditions before we check for the
         * endpoint 0 receive packet because these conditions could
         * potentially clear up the IRP queue. These conditions do not have
         * any explicit status bit associated with them, but the module will
         * generate an interrupt anyways*/

        if(ep0Status & USBHS_EP0_RXPKTRDY)
        {
            /* This means we have received data on the endpoint. The data may
             * have been received when endpoint was expecting to transmit data
             * to the host. In such a case, this indicates that the control
             * transfer has been aborted. We must reset the endpoint state in
             * such a case. */

            if((hDriver->usbDrvDeviceObj.endpoint0State == DRV_USBHS_DEVICE_EP0_STATE_WAITING_FOR_TX_DATA_IRP_FROM_CLIENT)
                    ||(hDriver->usbDrvDeviceObj.endpoint0State == DRV_USBHS_DEVICE_EP0_STATE_TX_DATA_STAGE_IN_PROGRESS)
                    ||(hDriver->usbDrvDeviceObj.endpoint0State == DRV_USBHS_DEVICE_EP0_STATE_WAITING_FOR_TX_STATUS_STAGE_FROM_HOST)
                    ||(hDriver->usbDrvDeviceObj.endpoint0State == DRV_USBHS_DEVICE_EP0_STATE_WAITING_FOR_TX_STATUS_IRP_FROM_CLIENT))
            {
                /* The endpoint was in some type of a transmit state. This must
                 * be reset to receive. */

                hDriver->usbDrvDeviceObj.endpoint0State = DRV_USBHS_DEVICE_EP0_STATE_EXPECTING_SETUP_FROM_HOST;
            }

            if (hDriver->usbDrvDeviceObj.endpoint0State == DRV_USBHS_DEVICE_EP0_STATE_EXPECTING_SETUP_FROM_HOST)
            {
                /* This means we were expecting a SETUP packet and we got one.
                 * Get the endpoint 0 RX endpoint object and get the IRP at top
                 * of the queue */

                irp = endpointObjReceive->irpQueue;

                /* If we have received a setup packet, this means that if a
                 * stall was scheduled, then it has been sent. So we can clear
                 * the stall status of the endpoint object */
                endpointObjReceive->endpointState &= ~DRV_USBHS_DEVICE_ENDPOINT_STATE_STALLED;
                endpointObjTransmit->endpointState &= ~DRV_USBHS_DEVICE_ENDPOINT_STATE_STALLED;

                if(irp == NULL)
                {
                    /* This means the device layer did not schedule a IRP to
                     * receive a SETUP packet! */

                    SYS_DEBUG_MESSAGE(SYS_ERROR_INFO,"\r\nUSBHS Driver: No IRP to receive setup packet in _DRV_USBHS_DEVICE_Tasks_ISR()!");
                }
                else
                {
                    /* We have an IRP for the setup packet. Transfer the data
                     * from the FIFO to the IRP */

                    data = (uint8_t *)irp->data;
                    *((unsigned int *)(data)) = *((unsigned int *)PLIB_USBHS_GetEP0FIFOAddress(usbID));
                    *((unsigned int *)(data + 4)) = *((unsigned int *)PLIB_USBHS_GetEP0FIFOAddress(usbID));

                    /* Remember the size and direction of the data stage */
                    endpoint0DataStageSize = *((unsigned short int *)(data + 6));
                    endpoint0DataStageDirection = ((data[0] & 0x80) != 0);

                    /* Indicate that this is a setup IRP */
                    irp->status = USB_DEVICE_IRP_STATUS_SETUP;
                    irp->size = 8;

                    /* Update the endpoint IRP queue. If irp->next is NULL, then
                     * this means at this point we dont have anything in the
                     * queue */

                    endpointObjReceive->irpQueue = irp->next;
                    if(irp->callback != NULL)
                    {
                        irp->callback((USB_DEVICE_IRP *)irp);
                    }

                    /* Check the size and the direction of the control transfer
                     * */

                    if(endpoint0DataStageSize == 0)
                    {
                        if(!(endpointObjReceive->endpointState & DRV_USBHS_DEVICE_ENDPOINT_STATE_STALLED))
                        {
                            if(endpoint0DataStageDirection == USB_DATA_DIRECTION_HOST_TO_DEVICE)
                            {
                                /* This means data was moving from host to
                                 * device. The status stage ZLP will come from
                                 * device. Service the TX endpoint on device */

                                if(endpointObjTransmit->irpQueue != NULL)
                                {
                                    /* That means we have a IRP to be processed */
                                    endpointObjTransmit->irpQueue->status = USB_DEVICE_IRP_STATUS_IN_PROGRESS;
                                    hDriver->usbDrvDeviceObj.endpoint0State = DRV_USBHS_DEVICE_EP0_STATE_WAITING_FOR_TX_STATUS_STAGE_FROM_HOST;

                                    /* Set the data end bit and the serviced packet
                                     * ready bit */
                                    *(PLIB_USBHS_GetEP0CSRAddress(usbID)) = 0x48;
                                }
                                else
                                {
                                    /* If here means we have to wait for an IRP
                                     * submit to complete this transfer. The
                                     * RxPktRdyServiced and the data end bit is
                                     * set in the IRP submit function */
                                    hDriver->usbDrvDeviceObj.endpoint0State = DRV_USBHS_DEVICE_EP0_STATE_WAITING_FOR_TX_STATUS_IRP_FROM_CLIENT;
                                }
                            }
                            else if(endpoint0DataStageDirection == USB_DATA_DIRECTION_DEVICE_TO_HOST)
                            {
                                /* This means that control transfer data stage
                                 * must move from device to host. We should
                                 * allow the device to send back a ZLP data
                                 * stage. Clear the RxPacketRdy bit */

                                *(PLIB_USBHS_GetEP0CSRAddress(usbID)) = 0x40;
                                if(endpointObjTransmit->irpQueue != NULL)
                                {
                                    /* This means we have a TX IRP to process */

                                    hDriver->usbDrvDeviceObj.endpoint0State = DRV_USBHS_DEVICE_EP0_STATE_TX_DATA_STAGE_IN_PROGRESS;
                                    endpointObjTransmit->irpQueue->status = USB_DEVICE_IRP_STATUS_IN_PROGRESS;

                                    /* We completely ignore the data size and
                                     * set the nPendingBytes to zero. Note that
                                     * nothing is writtent to the FIFO. */

                                    endpointObjTransmit->irpQueue->nPendingBytes = 0;
                                    *(PLIB_USBHS_GetEP0CSRAddress(usbID)) = 0xA;
                                }
                                else
                                {
                                    /* This means we dont have a IRP ready. The
                                     * device must submit a zero data stage IRP. */
                                    hDriver->usbDrvDeviceObj.endpoint0State = DRV_USBHS_DEVICE_EP0_STATE_WAITING_FOR_TX_DATA_IRP_FROM_CLIENT;
                                }
                            }
                        }
                    }
                    else
                    {
                        if(!(endpointObjReceive->endpointState & DRV_USBHS_DEVICE_ENDPOINT_STATE_STALLED))
                        {
                            /* This means that there is a data stage */
                            if(endpoint0DataStageDirection == USB_DATA_DIRECTION_DEVICE_TO_HOST)
                            {
                                /* Clear the Rx packet ready bit */
                                *(PLIB_USBHS_GetEP0CSRAddress(usbID)) = 0x40;

                                /* This means the data stage is moving from
                                 * device to host which means a transmit
                                 * endpoint is used */

                                if(endpointObjTransmit->irpQueue != NULL)
                                {
                                    /* This means we have a TX IRP to process */

                                    hDriver->usbDrvDeviceObj.endpoint0State = DRV_USBHS_DEVICE_EP0_STATE_TX_DATA_STAGE_IN_PROGRESS;
                                    endpointObjTransmit->irpQueue->status = USB_DEVICE_IRP_STATUS_IN_PROGRESS;

                                    count = _DRV_USBHS_ProcessIRPFIFO(hDriver,endpointObjTransmit, 0, USB_DATA_DIRECTION_DEVICE_TO_HOST, 
                                            endpointObjTransmit->irpQueue, &isDmaUsed, tryDma);

                                    if(count < endpointObjTransmit->maxPacketSize)
                                    {
                                        /* This means that this is the last
                                         * packet in the control read transfer.
                                         * Set the transmit ready and the data
                                         * end bit */
                                        *(PLIB_USBHS_GetEP0CSRAddress(usbID)) = 0xA;
                                    }
                                    else
                                    {
                                        /* This is a multi data stage transfer.
                                         * Don't set the data end bit. Only set
                                         * the transmit ready bit. */
                                        *(PLIB_USBHS_GetEP0CSRAddress(usbID)) = 0x2;
                                    }
                                }
                                else
                                {
                                    /* This means we dont have a IRP ready */
                                    hDriver->usbDrvDeviceObj.endpoint0State = DRV_USBHS_DEVICE_EP0_STATE_WAITING_FOR_TX_DATA_IRP_FROM_CLIENT;
                                }
                            }
                            else
                            {
                                /* This means the data stage is moving from host
                                 * to device which means a receive endpoint is
                                 * used */

                                *(PLIB_USBHS_GetEP0CSRAddress(usbID)) = 0x40;
                                if(endpointObjReceive->irpQueue != NULL)
                                {
                                    /* This means we have a RX IRP to process */

                                    hDriver->usbDrvDeviceObj.endpoint0State = DRV_USBHS_DEVICE_EP0_STATE_WAITING_FOR_RX_DATA_STAGE_FROM_HOST;
                                    endpointObjReceive->irpQueue->status = USB_DEVICE_IRP_STATUS_IN_PROGRESS;
                                }
                                else
                                {
                                    /* This means we dont have a IRP ready */
                                    hDriver->usbDrvDeviceObj.endpoint0State = DRV_USBHS_DEVICE_EP0_STATE_WAITING_FOR_RX_DATA_IRP_FROM_CLIENT;
                                }
                            }
                        }
                    } 
                } 
            } 
            else if(hDriver->usbDrvDeviceObj.endpoint0State == DRV_USBHS_DEVICE_EP0_STATE_WAITING_FOR_RX_DATA_STAGE_FROM_HOST)
            {
                /* This means were expecting some data from the host and have
                 * received it */

                irp = endpointObjReceive->irpQueue;
                if(irp != NULL)
                {
                    /* This means we have a irp to process */
                    count = _DRV_USBHS_ProcessIRPFIFO(hDriver, endpointObjReceive, 0, USB_DATA_DIRECTION_HOST_TO_DEVICE, irp, &isDmaUsed, tryDma);

                    /* Clear the Rx Packet Ready Bit */
                    *(PLIB_USBHS_GetEP0CSRAddress(usbID)) = 0x40;

                    /* Check if the irp is completed */
                    if((count < endpointObjReceive->maxPacketSize) || (irp->nPendingBytes >= irp->size))
                    {
                        /* This means we received a short packet or the amount
                         * of data received is the amount that was expected */

                        if(irp->size < irp->nPendingBytes)
                        {
                            irp->status = USB_DEVICE_IRP_STATUS_COMPLETED_SHORT;
                        }
                        else
                        {
                            irp->status = USB_DEVICE_IRP_STATUS_COMPLETED;
                        }

                        irp->size = irp->nPendingBytes;
                        endpointObjReceive->irpQueue = irp->next;

                        if(irp->callback)
                        {
                            irp->callback((USB_DEVICE_IRP *)irp);
                        }

                        /* We can now move to the handshake stage. Check if a TX
                         * IRP is available */

                        if(endpointObjTransmit->irpQueue != NULL)
                        {
                            hDriver->usbDrvDeviceObj.endpoint0State = DRV_USBHS_DEVICE_EP0_STATE_WAITING_FOR_TX_STATUS_STAGE_FROM_HOST;

                            /* Set the data end bit */
                            *(PLIB_USBHS_GetEP0CSRAddress(usbID)) = 0x8;
                        }
                        else
                        {
                            hDriver->usbDrvDeviceObj.endpoint0State = DRV_USBHS_DEVICE_EP0_STATE_WAITING_FOR_TX_STATUS_IRP_FROM_CLIENT;
                        }
                    }
                }
            }
        }
        else
        {
            /* We reach here if there was no setup end, no stall handling and no
             * receive interrupt. */

            if(hDriver->usbDrvDeviceObj.endpoint0State == DRV_USBHS_DEVICE_EP0_STATE_TX_DATA_STAGE_IN_PROGRESS)
            {
                irp = endpointObjTransmit->irpQueue;

                if(irp != NULL)
                {
                    /* Check if the Transmit IRP is done */
                    if(irp->nPendingBytes == 0)
                    {
                        if(irp->flags & USB_DEVICE_IRP_FLAG_DATA_PENDING)
                        {
                            /* This is for a case where the device layer wants
                             * to submit multiple IRPs for the data stage. We
                             * should send a IRP callback but should not move to
                             * the status stage of the control transfer. The
                             * size of the IRP must be endpoint size and the
                             * device layer should have another data stage IRP
                             * to send. */
 
                            endpointObjTransmit->irpQueue = irp->next;
                            irp->status = USB_DEVICE_IRP_STATUS_COMPLETED;

                            if(irp->callback != NULL)
                            {
                                irp->callback((USB_DEVICE_IRP *)irp);
                            }

                            /* Now check if the next IRP for the data stage is
                             * available */
                            if(endpointObjTransmit->irpQueue != NULL)
                            {
                                /* Indeed there is an IRP to be sent */
                                irp = endpointObjTransmit->irpQueue;
                                count =  _DRV_USBHS_ProcessIRPFIFO(hDriver,endpointObjTransmit, 0, USB_DATA_DIRECTION_DEVICE_TO_HOST, irp, &isDmaUsed, tryDma);

                                if(count < endpointObjTransmit->maxPacketSize)
                                {
                                    /* This means that this is the last packet
                                     * in the control read transfer. Set the
                                     * transmit ready and the data end bit */

                                    *(PLIB_USBHS_GetEP0CSRAddress(usbID)) = 0xA;

                                }
                                else
                                {
                                    /* This is a multi data stage transfer. Dont
                                     * set the data end bit. Only set the
                                     * transmit ready bit. */

                                    *(PLIB_USBHS_GetEP0CSRAddress(usbID)) = 0x2;
                                }
                            }
                            else
                            {

                             /* If there was no IRP submitted, the we must
                              * return the endpoint state to waiting for TX Data
                              * stage from client. */

                                hDriver->usbDrvDeviceObj.endpoint0State = DRV_USBHS_DEVICE_EP0_STATE_WAITING_FOR_TX_DATA_IRP_FROM_CLIENT;
                            }
                        }
                        else if((irp->flags & USB_DEVICE_IRP_FLAG_SEND_ZLP) != 0)
                        {
                            /* This means we have to send a ZLP. Clear the flag
                             * */

                            irp->flags &= ~USB_DEVICE_IRP_FLAG_SEND_ZLP;

                            /* Only set the transmit ready bit. */
                            PLIB_USBHS_EP0TxPktRdy(usbID);
                        }
                        else
                        {
                            /* This means we got done with the IRP */
                            endpointObjTransmit->irpQueue = irp->next;
                            irp->status = USB_DEVICE_IRP_STATUS_COMPLETED;

                            if(irp->callback != NULL)
                            {
                                irp->callback((USB_DEVICE_IRP *)irp);
                            }

                            /* We should check if the status stage IRP is available */

                            if(endpointObjReceive->irpQueue != NULL)
                            {
                                /* This is the status IRP */

                                irp = endpointObjReceive->irpQueue;
                                irp->status = USB_DEVICE_IRP_STATUS_COMPLETED;
                                irp->size = 0;
                                endpointObjReceive->irpQueue = irp->next;

                                if(irp->callback != NULL)
                                {
                                    irp->callback((USB_DEVICE_IRP *)irp);
                                }
                            }
                            else
                            {
                                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Was expecting status IRP in _DRV_USBHS_DEVICE_Tasks_ISR()");
                            }

                            /* We will just move on to the next control transfer */
                            hDriver->usbDrvDeviceObj.endpoint0State = DRV_USBHS_DEVICE_EP0_STATE_EXPECTING_SETUP_FROM_HOST;
                        }
                    }
                    else
                    {
                        /* There is more data in the IRP.  Continue to process
                         * it */

                        count =  _DRV_USBHS_ProcessIRPFIFO(hDriver,endpointObjTransmit, 0, USB_DATA_DIRECTION_DEVICE_TO_HOST, irp, &isDmaUsed, tryDma);

                        if(count < endpointObjTransmit->maxPacketSize)
                        {
                            /* This means that this is the last packet in the
                             * control read transfer. Set the transmit ready and
                             * the data end bit */
                            *(PLIB_USBHS_GetEP0CSRAddress(usbID)) = 0xA;

                        }
                        else
                        {
                            /* This is a multi data stage transfer. Dont set the
                             * data end bit. Only set the transmit ready bit. */
                            *(PLIB_USBHS_GetEP0CSRAddress(usbID)) = 0x2;
                        }
                    }
                }
            } 
            else if(hDriver->usbDrvDeviceObj.endpoint0State == DRV_USBHS_DEVICE_EP0_STATE_WAITING_FOR_TX_STATUS_STAGE_FROM_HOST)
            {
                /* This means the TX status stage has completed */

                irp = endpointObjTransmit->irpQueue;
                if(irp != NULL)
                {
                    irp->size = 0;
                    irp->status = USB_DEVICE_IRP_STATUS_COMPLETED;
                    endpointObjTransmit->irpQueue = irp->next;
                    if(irp->callback != NULL)
                    {
                        irp->callback((USB_DEVICE_IRP *)irp);
                    }

                    hDriver->usbDrvDeviceObj.endpoint0State = DRV_USBHS_DEVICE_EP0_STATE_EXPECTING_SETUP_FROM_HOST;
                }
            }
        }
    }

    /* Here we check if any other endpoint interrupts
     * have occurred. Start the endpoint mask with
     * endpoint 1 as the endpoint 0 is already serviced
     * */

    endpointMask = 0x2;
    for(iEndpoint = 1; iEndpoint < DRV_USBHS_ENDPOINTS_NUMBER; iEndpoint ++)
    {
        endpointObjReceive = hDriver->usbDrvCommonObj.endpointTable + (2 * iEndpoint);
        endpointObjTransmit = hDriver->usbDrvCommonObj.endpointTable + (2 * iEndpoint) + 1;

        if(endpointTXInterrupts & endpointMask)
        {
            /* Check if the endpoint is enabled */

            if(endpointObjTransmit->endpointState & DRV_USBHS_DEVICE_ENDPOINT_STATE_ENABLED)
            {
                /* This means the endpoint is enabled. We got a TX interrupt 
                 * because a IRP was scheduled */

                txEPStatus = PLIB_USBHS_TxEPStatusGet(usbID, iEndpoint);

                if(txEPStatus & USBHS_TXEP_SENTSTALL)
                {
                    /* This means a stall was sent. Clear this 
                     * the sent stall bit
                     */

                    PLIB_USBHS_TxEPStatusClear(usbID, iEndpoint, USBHS_TXEP_SENTSTALL);
                }

                if(endpointObjTransmit->irpQueue != NULL)
                {
                    /* We have a valid IRP at the head
                     * of the queue */

                    irp = endpointObjTransmit->irpQueue;
                    if(irp->status == USB_DEVICE_IRP_STATUS_ABORTED)
                    {
                        endpointObjTransmit->irpQueue = irp->next;
                        if(irp->callback != NULL)
                        {
                            /* Invoke the IRP */
                            irp->callback((USB_DEVICE_IRP *) irp);
                        }
                    }
                    else if(irp->nPendingBytes == 0)
                    {
                        /* We don't have any more bytes to send in this 
                         * IRP. Check if the ZLP flag is set else end 
                         * the IRP */

                        if((irp->flags & USB_DEVICE_IRP_FLAG_SEND_ZLP) != 0)
                        {
                            /* This means we have to send a ZLP. Clear the
                             * flag */

                            irp->flags &= ~USB_DEVICE_IRP_FLAG_SEND_ZLP;
                            PLIB_USBHS_TxEPStatusClear(usbID, iEndpoint, USBHS_TXEP_UNDERRUN);
                            PLIB_USBHS_DeviceTxEndpointPacketReady(usbID, iEndpoint);
                        }
                        else
                        {
                            /* This means this IRP is processed */
                            irp->status = USB_DEVICE_IRP_STATUS_COMPLETED;
                            endpointObjTransmit->irpQueue = irp->next;
                            if(irp->callback != NULL)
                            {
                                /* Invoke the IRP */
                                irp->callback((USB_DEVICE_IRP *) irp);
                            }

                            if(endpointObjTransmit->irpQueue != NULL)
                            {
                                /* This means we have one more IRP in the
                                 * queue */

                                _DRV_USBHS_ProcessIRPFIFO(hDriver, endpointObjTransmit, iEndpoint, USB_DATA_DIRECTION_DEVICE_TO_HOST,
                                        endpointObjTransmit->irpQueue, &isDmaUsed, tryDma);

                                if(isDmaUsed == false)
                                {
                                    /* Set the TX packet Ready bit. Clear the
                                     * FIFO underrun bit if it is set. */
                                    PLIB_USBHS_TxEPStatusClear(usbID, iEndpoint, USBHS_TXEP_UNDERRUN);
                                    PLIB_USBHS_DeviceTxEndpointPacketReady(usbID, iEndpoint);
                                }
                            }
                        }
                    }
                    else
                    {
                        /* This means are more pending bytes in this
                         * IRP */

                        _DRV_USBHS_ProcessIRPFIFO(hDriver, endpointObjTransmit, iEndpoint, USB_DATA_DIRECTION_DEVICE_TO_HOST, irp, &isDmaUsed, tryDma);

                        if(isDmaUsed == false)
                        {
                            /* Set the TX packet Ready bit. Clear the FIFO
                             * underrun bit if it is set. */
                            PLIB_USBHS_TxEPStatusClear(usbID, iEndpoint, USBHS_TXEP_UNDERRUN);
                            PLIB_USBHS_DeviceTxEndpointPacketReady(usbID, iEndpoint);
                        }

                    }

                }
            }
        }

        if(endpointRXInterrupts & endpointMask)
        {
            /* Check if the endpoint is enabled */

            if(endpointObjReceive->endpointState & DRV_USBHS_DEVICE_ENDPOINT_STATE_ENABLED)
            {
                /* This means the endpoint is enabled */

                rxEPStatus = PLIB_USBHS_RxEPStatusGet(usbID, iEndpoint);
                if(rxEPStatus & USBHS_RXEP_SENTSTALL)
                {
                    /* This means a stall was sent. Clear this the sent stall
                     * bit */

                    PLIB_USBHS_RxEPStatusClear(usbID, iEndpoint, USBHS_RXEP_SENTSTALL);
                }

                if(endpointObjReceive->irpQueue != NULL)
                {
                    /* This means we have a IRP to process */

                    if(rxEPStatus & USBHS_RXEP_PKTRDY)
                    {
                        /* This means the FIFO already contains data */

                        irp = endpointObjReceive->irpQueue;

                        if(irp->status == USB_DEVICE_IRP_STATUS_ABORTED)
                        {
                            /* Move the IRP queue HEAD */
                            endpointObjReceive->irpQueue = irp->next;

                            /* Flush the data from FIFO */
                            PLIB_USBHS_EndpointRxFIFOFlush(usbID, iEndpoint);

                            /* Clear the Rx Packet Ready Bit */
                            PLIB_USBHS_RxEPStatusClear(usbID, iEndpoint, USBHS_RXEP_PKTRDY);

                            /* Callback with ABORT status */
                            if(irp->callback)
                            {
                                irp->callback((USB_DEVICE_IRP *)irp);
                            }
                        }

                        else
                        {
                            /* This means there is data in the FIFO and this
                             * data is presently not being unloaded. So we can
                             * go ahead and try to unload the FIFO.*/

                            count = _DRV_USBHS_ProcessIRPFIFO(hDriver, endpointObjReceive, iEndpoint, USB_DATA_DIRECTION_HOST_TO_DEVICE, irp, &isDmaUsed, tryDma);

                            if(isDmaUsed == false)
                            {
                                /* Clear the Rx Packet Ready Bit */

                                PLIB_USBHS_RxEPStatusClear(usbID, iEndpoint, USBHS_RXEP_PKTRDY);

                                /* Check if the irp is completed */

                                if((count < endpointObjReceive->maxPacketSize) || (irp->nPendingBytes >= irp->size))
                                {
                                    /* This means we received a short packet or
                                     * the amount of data received is the amount
                                     * that was expected */

                                    if(irp->size > irp->nPendingBytes)
                                    {
                                        /* This means that we received less data
                                         * than what was expected. */
                                        irp->status = USB_DEVICE_IRP_STATUS_COMPLETED_SHORT;
                                    }
                                    else
                                    {
                                        irp->status = USB_DEVICE_IRP_STATUS_COMPLETED;
                                    }

                                    irp->size = irp->nPendingBytes;
                                    irp->nPendingBytes = 0;
                                    endpointObjReceive->irpQueue = irp->next;

                                    if(irp->callback)
                                    {
                                        irp->callback((USB_DEVICE_IRP *)irp);
                                    }
                                }
                            }   
                        }
                    }
                }
            } 
        }

        endpointMask = endpointMask << 1;
    }
}

// *****************************************************************************
/* Function:
    void _DRV_USBHS_DEVICE_Get_FreeDMAChannel
    (
        DRV_USBHS_OBJ * hDriver,
        bool endpointDir,
        uint8_t iEndpoint
    )

  Summary:
    Get a free DMA channel and use it, if available.

  Description:
    Get a free DMA channel and use it, if available.

  Remarks:
    This is a local function and should not be called directly by the
    application.
*/

uint8_t _DRV_USBHS_DEVICE_Get_FreeDMAChannel
(
    DRV_USBHS_OBJ * hDriver,
    bool endpointDir,
    uint8_t iEndpoint
)
{
    uint8_t dmaChannel = 0;
    uint8_t channelCount = 0;
    
    for(channelCount = 1; channelCount < 9; channelCount++)
    {
        if((hDriver->usbDrvCommonObj.gDrvUSBDMAPool[channelCount]).inUse == false)
        {
            /* Found Not used DMA Channel - Use this channel */
            dmaChannel = channelCount;
            hDriver->usbDrvCommonObj.gDrvUSBDMAPool[dmaChannel].inUse = true;
            hDriver->usbDrvCommonObj.gDrvUSBDMAPool[dmaChannel].endpointDir = endpointDir;
            hDriver->usbDrvCommonObj.gDrvUSBDMAPool[dmaChannel].iEndpoint = iEndpoint;
            break;
        }
     }

     return dmaChannel;
}

// ****************************************************************************
/* Function:
    USB_ERROR DRV_USBHS_DEVICE_TestModeEnter
    (
        DRV_HANDLE handle, 
        USB_TEST_MODE_SELECTORS testMode
    );
  
  Summary:
    This function enables the specified USB 2.0 Test Mode.
	
  Description:
    This function causes the device to enter the specified USB 2.0 defined test
    mode. It is called in response to Set Feature command from the host. The
    wValue field of this command specifies the Test Mode to enter. The USB
    module will perform the action identified by the testMode parameter.

  Remarks:
    See drv_usb.h for usage information.
*/

USB_ERROR DRV_USBHS_DEVICE_TestModeEnter
(
    DRV_HANDLE handle,
    USB_TEST_MODE_SELECTORS testMode
)
{
    DRV_USBHS_OBJ * hDriver = NULL;
    USBHS_MODULE_ID usbID = USBHS_NUMBER_OF_MODULES;
    USB_ERROR returnValue = USB_ERROR_PARAMETER_INVALID;
    uint8_t testModeData[53] = 
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
        0xAA, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
        0xEE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xBF, 0xDF,
        0xEF, 0xF7, 0xFB, 0xFD, 0xFC, 0x7E, 0xBF, 0xDF,
        0xEF, 0xF7, 0xFB, 0xFD, 0x7E
    };

    if( (DRV_HANDLE_INVALID !=  handle) && (NULL != ((DRV_USBHS_CLIENT_OBJ *)handle)) )
    {
        if(((DRV_USBHS_CLIENT_OBJ *)handle)->inUse)
        {
            hDriver = ((DRV_USBHS_CLIENT_OBJ *)handle)->hDriver;
            usbID = hDriver->usbDrvCommonObj.usbID;
            
            /* Load the test data to FIFO if test mode matches */
            if( USB_TEST_MODE_SELCTOR_TEST_PACKET == testMode )
            {
                PLIB_USBHS_DeviceEPFIFOLoad(usbID, 0, &testModeData[0], 53);
            }
            /* Enter into Test Mode */
            if( (false == PLIB_USBHS_TestModeEnter(usbID, (uint8_t)testMode)) )
            {
                /* Failure */
                returnValue = USB_ERROR_PARAMETER_INVALID;
            }
            else
            {
                /* Send the test data if test mode matches */
                if( USB_TEST_MODE_SELCTOR_TEST_PACKET == testMode )
                {
                    PLIB_USBHS_EP0TxPktRdy(usbID);
                }
                /* Success */
                returnValue = USB_ERROR_NONE;
            }
        }
        else
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_TestModeEnter()");
        }
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_TestModeEnter()");
    }
    
    return (returnValue);
}

// ****************************************************************************
/* Function:
    USB_ERROR DRV_USBHS_DEVICE_TestModeExit
    (
        DRV_HANDLE handle, 
        USB_TEST_MODE_SELECTORS testMode
    );
  
  Summary:
    This function disables the specified USB 2.0 Test Mode.
	
  Description:
    This function causes the device to stop the specified USB 2.0 defined test
    mode. This function can be called after calling the
    DRV_USBHS_DEVICE_TestModeEnter function to stop the test mode execution. 

  Remarks:
    See drv_usb.h for usage information.
*/

USB_ERROR DRV_USBHS_DEVICE_TestModeExit
(
    DRV_HANDLE handle,
    USB_TEST_MODE_SELECTORS testMode
)
{
    DRV_USBHS_OBJ * hDriver = NULL;
    USBHS_MODULE_ID usbID = USBHS_NUMBER_OF_MODULES;
    USB_ERROR returnValue = USB_ERROR_PARAMETER_INVALID;

    if( (DRV_HANDLE_INVALID !=  handle) && (NULL != ((DRV_USBHS_CLIENT_OBJ *)handle)) )
    {
        if(((DRV_USBHS_CLIENT_OBJ *)handle)->inUse)
        {
            hDriver = ((DRV_USBHS_CLIENT_OBJ *)handle)->hDriver;
            usbID = hDriver->usbDrvCommonObj.usbID;
            
            if( (true == PLIB_USBHS_TestModeExit(usbID, (uint8_t)testMode)) )
            {
                returnValue = USB_ERROR_NONE;
            }
        }
        else
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_TestModeExit()");
        }
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_DEVICE_TestModeExit()");
    }
    
    return (returnValue);
}

void _DRV_USBHS_DEVICE_Tasks_ISR_USBDMA
(
    DRV_USBHS_OBJ * hDriver
)
{
    uint8_t usbDMAInterrupts = 0;
    uint8_t  iEndpoint = 0;
    uint8_t usbDMAChannel = 0;
    USBHS_MODULE_ID usbID = USBHS_NUMBER_OF_MODULES;
    bool usbDMAError = false;
    bool endpointDir = false;
    DRV_USBHS_DEVICE_ENDPOINT_OBJ * endpointObjReceive = NULL;
    DRV_USBHS_DEVICE_ENDPOINT_OBJ * endpointObjTransmit = NULL;
    USB_DEVICE_IRP_LOCAL * irp = NULL;

    usbID = hDriver->usbDrvCommonObj.usbID;

    /* Reading the interrupt flags will cause the flags to get cleared */
    usbDMAInterrupts = PLIB_USBHS_DMAInterruptGet(usbID);

    for(usbDMAChannel = 1; usbDMAChannel < 9; usbDMAChannel++)
    {
        if((usbDMAInterrupts >> (usbDMAChannel - 1)) & 0x01)
        {
            /* Check if there is DMA error */
            usbDMAError = PLIB_USBHS_DMAErrorGet(usbID, usbDMAChannel);
            
            if(usbDMAError == true)
            {
                continue;
            }

            /* DMA interrupt set for this channel.  Map the EP it is assigned
             * for. */
            iEndpoint = hDriver->usbDrvCommonObj.gDrvUSBDMAPool[usbDMAChannel].iEndpoint;
            endpointDir = hDriver->usbDrvCommonObj.gDrvUSBDMAPool[usbDMAChannel].endpointDir;

            endpointObjReceive = hDriver->usbDrvCommonObj.endpointTable + (2 * iEndpoint);
            endpointObjTransmit = hDriver->usbDrvCommonObj.endpointTable + (2 * iEndpoint) + 1;

            /* Check the EP direction */
            if(endpointDir == USB_DATA_DIRECTION_HOST_TO_DEVICE)
            {
                /* This is RX Endpoint */
                irp = endpointObjReceive->irpQueue;
            }
            else
            {
                /* This is TX Endpoint */
                irp = endpointObjTransmit->irpQueue;

                if(irp->status == USB_DEVICE_IRP_STATUS_ABORTED)
                {
                    PLIB_USBHS_TxEPStatusClear(usbID, iEndpoint, USBHS_TXEP_UNDERRUN);
                    PLIB_USBHS_EndpointTxFIFOFlush(usbID, iEndpoint);
                }
                else
                {
                    /* Set the TX packet Ready bit. Clear the
                     * FIFO underrun bit if it is set. */
                    PLIB_USBHS_TxEPStatusClear(usbID, iEndpoint, USBHS_TXEP_UNDERRUN);
                    PLIB_USBHS_DeviceTxEndpointPacketReady(usbID, iEndpoint);
                }

                hDriver->usbDrvCommonObj.gDrvUSBDMAPool[usbDMAChannel].inUse = false;
            }

            /* Check if the irp is completed */

            if(endpointDir == USB_DATA_DIRECTION_DEVICE_TO_HOST)
            {
                /* TX PATH */
                /* No need to do anything */
            }

            else
            {
                /* The IRP can be NULL if the endpoint was disabled. The
                 * endpoint can get disabled asynchronously if the alternate
                 * settings has changed or when ever the end point disable
                 * function is called. At the point, there could still be a IRP
                 * that the USB module has already started processing.  */

                if(irp != NULL && (irp->status != USB_DEVICE_IRP_STATUS_ABORTED))
                {
                    /* RX PATH */
                    if(((hDriver->usbDrvCommonObj.gDrvUSBDMAPool[usbDMAChannel].count) < (endpointObjReceive->maxPacketSize)) || (irp->nPendingBytes >= irp->size))
                    {
                        /* This means we received a short packet or the amount
                         * of data received is the amount that was expected */
                        if(irp->size > irp->nPendingBytes)
                        {
                            /* This means that we received less data than what
                             * was expected. */
                            irp->status = USB_DEVICE_IRP_STATUS_COMPLETED_SHORT;
                        }
                        else
                        {
                            irp->status = USB_DEVICE_IRP_STATUS_COMPLETED;
                        }

                        irp->size = irp->nPendingBytes;
                        irp->nPendingBytes = 0;
                        endpointObjReceive->irpQueue = irp->next;

                        /* Return DMA channel back to the pool.*/
                        hDriver->usbDrvCommonObj.gDrvUSBDMAPool[usbDMAChannel].inUse = false;

                        /* Clear RXPktRDY bit in the corresponding register.
                         * Doing this will ensure that if IRP submit is called
                         * in the IRP callback, it will NOT receive a false RX
                         * ready status */
                        PLIB_USBHS_RxEPStatusClear(usbID, iEndpoint, USBHS_RXEP_PKTRDY);

                        if(irp->callback)
                        {
                            irp->callback((USB_DEVICE_IRP *)irp);
                        }
                    }
                    else
                    {
                        /* This means we have received max packet size of data
                         * but the transfer is not yet complete. So we clear the
                         * RX ready flag and release the DMA channel to receive
                         * the next packet */
                        hDriver->usbDrvCommonObj.gDrvUSBDMAPool[usbDMAChannel].inUse = false;

                        /* Clear RXPktRDY bit in the corresponding register */
                        PLIB_USBHS_RxEPStatusClear(usbID, iEndpoint, USBHS_RXEP_PKTRDY);
                    }
                }

                if(irp!= NULL && (irp->status == USB_DEVICE_IRP_STATUS_ABORTED))
                {
                    /* Release the DMA channel */
                    hDriver->usbDrvCommonObj.gDrvUSBDMAPool[usbDMAChannel].inUse = false;

                    /* Move IRP queue HEAD */
                    endpointObjReceive->irpQueue = irp->next;
                    
                    /* Clear RXPktRDY bit in the corresponding register */
                    PLIB_USBHS_RxEPStatusClear(usbID, iEndpoint, USBHS_RXEP_PKTRDY);
                    
                    /* ABORT callback */
                    if(irp->callback)
                    {
                        irp->callback((USB_DEVICE_IRP *)irp);
                    }
                }
            }
        }
    }
}

