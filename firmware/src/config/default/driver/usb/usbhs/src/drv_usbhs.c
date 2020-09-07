/*******************************************************************************
  USB Device Driver Core Routines

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usbhs.c

  Summary:
    USB Device Driver Dynamic Implementation of Core routines

  Description:
    The USB device driver provides a simple interface to manage the USB
    modules on Microchip microcontrollers.  This file Implements the core
    interface routines for the USB driver. While building the driver from 
    source, ALWAYS use this file in the build.
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

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************
#include "usb/src/usb_external_dependencies.h"
#include "driver/usb/usbhs/src/drv_usbhs_local.h"

/*********************************************
 * USB Driver object per USB Module instance 
 * present in the microcontroller.
 *********************************************/

DRV_USBHS_OBJ gDrvUSBObj[DRV_USBHS_INSTANCES_NUMBER];

/***********************************
 * Array of USB endpoint objects. 
 * Two objects per endpoint. 
 ***********************************/

DRV_USBHS_DEVICE_ENDPOINT_OBJ gDrvUSBEndpoints [DRV_USBHS_INSTANCES_NUMBER] [DRV_USBHS_ENDPOINTS_NUMBER * 2];

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_USBHS_Initialize
    ( 
        const SYS_MODULE_INDEX drvIndex,
        const SYS_MODULE_INIT * const init    
    )
    
  Summary:
    Initializes the Hi-Speed USB Driver.
	
  Description:
    This function initializes the Hi-Speed USB Driver, making it ready for
    clients to open. The driver initialization does not complete when this
    function returns. The DRV_USBHS_Tasks function must called periodically to
    complete the driver initialization. The DRV_USBHS_Open function will fail if
    the driver was not initialized or if initialization has not completed.

  Remarks:
    See drv_usbhs.h for usage information.
*/

SYS_MODULE_OBJ DRV_USBHS_Initialize 
( 
    const SYS_MODULE_INDEX  drvIndex,
    const SYS_MODULE_INIT * const init
)
{
    DRV_USBHS_OBJ * drvObj = NULL;
    USBHS_MODULE_ID usbID = USBHS_NUMBER_OF_MODULES;
    DRV_USBHS_INIT * usbInit = NULL;
    SYS_MODULE_OBJ returnValue = SYS_MODULE_OBJ_INVALID;

    if ( drvIndex < DRV_USBHS_INSTANCES_NUMBER)
    {
        if ( false == gDrvUSBObj[drvIndex].usbDrvCommonObj.inUse )
        {
            usbInit = (DRV_USBHS_INIT *) init;

            /* User needs to assign the peripheral ID of the USB HS module to
             * the usbID member of the DRV_USBHS_INIT structure. Peripheral ID
             * assigned should be one of the member of USBHS_MODULE_ID
             * enumeration. The following code is to provide backward
             * compatibility with the applications where they have specified
             * usbID as 0. */ 

            if ( 0 == usbInit->usbID )
            {
                /* For the optimized PLIBs, USBHS_ID_X is a pointer to USB
                 * module base address. */
                usbID = USBHS_ID_0; 
            }
            else
            {
                /* No change required */
                usbID = usbInit->usbID;
            }

            drvObj = &gDrvUSBObj[drvIndex];

            if ( (OSAL_RESULT_TRUE == OSAL_MUTEX_Create(&drvObj->usbDrvCommonObj.mutexID)) )
            {
                /* Populate the driver object with the required data */
                drvObj->usbDrvCommonObj.inUse = true;
                drvObj->usbDrvCommonObj.status = SYS_STATUS_BUSY; 
                drvObj->usbDrvCommonObj.usbID = usbID;            
                drvObj->usbDrvCommonObj.operationMode = usbInit->operationMode; 
                drvObj->usbDrvCommonObj.operationSpeed = usbInit->operationSpeed;
                drvObj->usbDrvCommonObj.deviceModeClient = NULL;
                drvObj->usbDrvCommonObj.hostModeClient = NULL;

                /* Assign the endpoint table */
                drvObj->usbDrvCommonObj.endpointTable = &gDrvUSBEndpoints[drvIndex][0];
                drvObj->usbDrvCommonObj.interruptSource = usbInit->interruptSource;

                drvObj->usbDrvCommonObj.interruptSourceUSBDma = usbInit->interruptSourceUSBDma;

                /* Set the starting VBUS level. */
                drvObj->usbDrvCommonObj.vbusLevel = USBHS_VBUS_SESSION_END;
                drvObj->usbDrvCommonObj.sessionInvalidEventSent = false;
                drvObj->usbDrvCommonObj.sessionValidEventSent = false;

                /* Set the state to indicate that the delay will be started */
                drvObj->usbDrvCommonObj.state = DRV_USBHS_TASK_STATE_STARTING_DELAY;

                _DRV_USBHS_HOST_ROOT_HUB_CONFIGURATION

                if (DRV_USBHS_OPMODE_HOST == drvObj->usbDrvCommonObj.operationMode)
                {
                    /* For Host the ID pin needs to be pull down */
                    PLIB_PORTS_ChangeNoticePullDownPerPortEnable( PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_3 );
                }

                returnValue = (SYS_MODULE_OBJ)drvIndex;
            }
            else
            {
                /* Could not create the mutual exclusion */
                SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "\r\nUSBHS Driver: Mutex create failed");
            }
        }
        else
        {
            /* Cannot initialize an object that is already in use. */
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Instance already has been initliazed in DRV_USBHS_Initialize()");
        }
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO,"\r\nUSBHS Driver: Increase the value of DRV_USBHS_INSTANCES_NUMBER in DRV_USBHS_Initialize()");
    }

    /* On success: Valid SYS MODULE Object
     * On failure: SYS_MODULE_OBJ_INVALID */
    return (returnValue);
}

// *****************************************************************************
/* Function:
    void DRV_USBHS_Tasks( SYS_MODULE_OBJ object )

  Summary:
    Maintains the driver's state machine when the driver is configured for 
    Polled mode.

  Description:
    Maintains the driver's Polled state machine. This function should be called
    from the SYS_Tasks function.

  Remarks:
    See drv_usbhs.h for usage information.
*/

void DRV_USBHS_Tasks
(
    SYS_MODULE_OBJ object
)
{
    DRV_USBHS_OBJ * hDriver = &gDrvUSBObj[object];
    USBHS_MODULE_ID usbID = hDriver->usbDrvCommonObj.usbID;
    DRV_USBHS_CLIENT_OBJ * deviceModeClient = NULL;
    USBHS_VBUS_LEVEL vbusLevel = USBHS_VBUS_SESSION_END;

    if (hDriver->usbDrvCommonObj.status > SYS_STATUS_UNINITIALIZED)
    {
        /* Check the tasks state and maintain */
        switch(hDriver->usbDrvCommonObj.state)
        {
            case DRV_USBHS_TASK_STATE_STARTING_DELAY:

                /* On PI32MZ DA and EF devices, enable the global USB interrupt
                 * in the USBCRCON register. */
                _DRV_USBHS_CLOCK_CONTROL_GLOBAL_USB_INT_ENABLE(usbID);

                /* Reset the PHY. This is a workaround for an errata */
                PLIB_USBHS_SoftResetEnable(usbID);

                /* Delay has started. Move to the next state */
                hDriver->usbDrvCommonObj.state = DRV_USBHS_TASK_STATE_WAITING_FOR_IS_SOFTRESET_COMPLETE;
                
                break;
				
		    case DRV_USBHS_TASK_STATE_WAITING_FOR_IS_SOFTRESET_COMPLETE :
			
				if ( 0 == USBEOFRSTbits.SOFRST )
				{
					/* Reset completed so Move to the next state */
                    hDriver->usbDrvCommonObj.state = DRV_USBHS_TASK_STATE_MODULE_INIT;
				}
			    break;

            case DRV_USBHS_TASK_STATE_MODULE_INIT:

                /* Setup the USB Module based on the selected mode */

                switch(hDriver->usbDrvCommonObj.operationMode)
                {
					case DRV_USBHS_OPMODE_DUAL_ROLE:
                        /* Device Initialization */
						_DRV_USBHS_DEVICE_INIT(hDriver, object);
                        /* Host Initialization */
                        _DRV_USBHS_HOST_INIT(hDriver, object);

                        /* Note here we do not perform the USB ID related
                         * configuration. This is done at the time of Host
                         * or Device role enabling. */

                    break;

                    case DRV_USBHS_OPMODE_DEVICE:

                    /* Configure the driver object for device mod
                     * operation.  In the PIC32MZ DA devices, the
                     * USBCRCON register needs to configured */

                    _DRV_USBHS_CLOCK_CONTROL_SETUP_DEVICE_MODE(usbID);
                                _DRV_USBHS_DEVICE_INIT(hDriver, object);
                    break;

                    case DRV_USBHS_OPMODE_HOST:

                    /* Configure the driver object for host mode
                     * operation.  In the PICMZ DA devices, the USBCRCON
                     * register needs to configured. */
                    _DRV_USBHS_HOST_INIT(hDriver, object);
                    _DRV_USBHS_CLOCK_CONTROL_SETUP_HOST_MODE(usbID);

                    break;

                    case DRV_USBHS_OPMODE_OTG:
                        break;

                    default:
                        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: What mode are you trying in DRV_USBHS_Tasks() ?");
                    break;
                }

                    /* Clear and enable the interrupts */
                    _DRV_USBHS_InterruptSourceClear(hDriver->usbDrvCommonObj.interruptSource);
                    _DRV_USBHS_InterruptSourceEnable(hDriver->usbDrvCommonObj.interruptSource);

                    _DRV_USBHS_InterruptSourceClear(hDriver->usbDrvCommonObj.interruptSourceUSBDma);
                    _DRV_USBHS_InterruptSourceEnable(hDriver->usbDrvCommonObj.interruptSourceUSBDma);

                    /* Indicate that the object is ready and change the state to running */
                    hDriver->usbDrvCommonObj.status = SYS_STATUS_READY;
                    hDriver->usbDrvCommonObj.state = DRV_USBHS_TASK_STATE_RUNNING;
                

                break;

            case DRV_USBHS_TASK_STATE_RUNNING:
                /* The module is in a running state. In the polling mode the
                 * driver ISR tasks and DMA ISR tasks are called here. We also
                 * check for the VBUS level and generate events if a client
                 * event handler is registered. */

                if ( ((DRV_USBHS_OPMODE_DUAL_ROLE == hDriver->usbDrvCommonObj.operationMode) && 
                            (true == hDriver->usbDrvCommonObj.isDeviceRoleActive)) ||
                        (DRV_USBHS_OPMODE_DEVICE == hDriver->usbDrvCommonObj.operationMode) )
                {
                    /* The module is in device mode. We have a valid client call
                     * back function. Check if VBUS level has changed. This has
                     * to be done in a polling manner because the module does
                     * not generate interrupts for VBUS level changes. */

                    deviceModeClient = hDriver->usbDrvCommonObj.deviceModeClient;
                    vbusLevel = PLIB_USBHS_VBUSLevelGet(hDriver->usbDrvCommonObj.usbID);

                    if ((hDriver->usbDrvCommonObj.vbusLevel != vbusLevel)||(hDriver->usbDrvCommonObj.isProcessingDetach))
                    {
                        /* This means there was a change in the level */
                        if (((vbusLevel==USBHS_VBUS_BELOW_AVALID)&&(hDriver->usbDrvCommonObj.vbusLevel == USBHS_VBUS_SESSION_END))||
                                ((vbusLevel==USBHS_VBUS_BELOW_VBUSVALID)&&(hDriver->usbDrvCommonObj.vbusLevel == USBHS_VBUS_BELOW_AVALID)))
                        {
                            /* Do not do anything here.If the code flow
                             * comes here, it means that VBUS transition
                             * from from USBHS_VBUS_SESSION_END to 
                             * USBHS_VBUS_VALID
                             */
                        }
                        else if ((vbusLevel==USBHS_VBUS_BELOW_VBUSVALID)&&(hDriver->usbDrvCommonObj.vbusLevel == USBHS_VBUS_VALID))
                        {
                                hDriver->usbDrvCommonObj.sessionInvalidEventSent = false;
                                hDriver->usbDrvCommonObj.isProcessingDetach=true;
                        }
                        else if (vbusLevel == USBHS_VBUS_VALID)
                        {
                            /* We have a valid VBUS level. We must let the
                             * device mode client know about this. */
                            if (deviceModeClient != NULL)
                            {
                                if ((NULL != deviceModeClient->pEventCallBack) && (false == hDriver->usbDrvCommonObj.sessionValidEventSent))
                                {
                                    deviceModeClient->pEventCallBack(deviceModeClient->hClientArg, DRV_USBHS_EVENT_DEVICE_SESSION_VALID, NULL);
                                    hDriver->usbDrvCommonObj.sessionValidEventSent = true;
                                }

                                /* We should be ready for send session
                                 * invalid event to the application when
                                 * they happen.*/
                                hDriver->usbDrvCommonObj.sessionInvalidEventSent = false;
                            }
                        }
                        else
                        {
                            /* Any thing other than valid is considered invalid.
                             * This event may occur multiple times, but we send
                             * it only once. */
                            if (!hDriver->usbDrvCommonObj.sessionInvalidEventSent)
                            {   
                                if (deviceModeClient != NULL)
                                {
                                    if (NULL != deviceModeClient->pEventCallBack)
                                    {
                                        deviceModeClient->pEventCallBack(deviceModeClient->hClientArg, DRV_USBHS_EVENT_DEVICE_SESSION_INVALID, NULL);
                                    }

                                    hDriver->usbDrvCommonObj.sessionInvalidEventSent = true;
                                    hDriver->usbDrvCommonObj.sessionValidEventSent = false;
                                    hDriver->usbDrvCommonObj.isProcessingDetach = false;
                                }
                            }
                        }

                        hDriver->usbDrvCommonObj.vbusLevel = vbusLevel;
                    }
                }
                else if ( ((DRV_USBHS_OPMODE_DUAL_ROLE == hDriver->usbDrvCommonObj.operationMode) && 
                            (hDriver->usbDrvCommonObj.isHostRoleActive)) || 
                        (DRV_USBHS_OPMODE_HOST == hDriver->usbDrvCommonObj.operationMode) )
                {
                    _DRV_USBHS_HOST_ATTACH_DETACH_STATE_MACHINE(hDriver);
                    _DRV_USBHS_HOST_RESET_STATE_MACINE(hDriver);
                }
                else
                {
                    /* Do nothing */
                }

                if ( ((DRV_USBHS_OPMODE_DUAL_ROLE == hDriver->usbDrvCommonObj.operationMode) || 
                            (DRV_USBHS_OPMODE_DEVICE == hDriver->usbDrvCommonObj.operationMode))
                        && (true == hDriver->usbDrvCommonObj.isProcessingAttach) )
                {
                    _DRV_USBHS_DEVICE_ATTACH_STATE_MACHINE(hDriver);
                }

                _DRV_USBHS_Tasks_ISR(object);
                _DRV_USBHS_Tasks_ISR_USBDMA(object);
                break;
        }
    }
}

// *****************************************************************************
/* Function:
    void DRV_USBHS_Deinitialize( const SYS_MODULE_OBJ object )

  Summary:
    Dynamic implementation of DRV_USBHS_Deinitialize system interface function.

  Description:
    This is the dynamic implementation of DRV_USBHS_Deinitialize system interface
    function.

  Remarks:
    See drv_usbhs.h for usage information.
*/

void DRV_USBHS_Deinitialize
( 
    const SYS_MODULE_INDEX  object
)
{
    DRV_USBHS_OBJ * drvObj = NULL;
    bool returnValue = false;
    uint8_t counter = 0;

    if (SYS_MODULE_OBJ_INVALID != object)
    {
        if ( object < DRV_USBHS_INSTANCES_NUMBER)
        {
            if (true == gDrvUSBObj[object].usbDrvCommonObj.inUse)
            {
                drvObj = &gDrvUSBObj[object];

                /* Clear and disable the interrupts. Assigning to a value has
                 * been implemented to remove compiler warning in polling mode.*/

                returnValue = _DRV_USBHS_InterruptSourceDisable(drvObj->usbDrvCommonObj.interruptSource);
                returnValue = returnValue;
                _DRV_USBHS_InterruptSourceClear(drvObj->usbDrvCommonObj.interruptSource);

                /* Populate the driver object with the required data */

                drvObj->usbDrvCommonObj.inUse = false;
                drvObj->usbDrvCommonObj.status = SYS_STATUS_UNINITIALIZED; 

                drvObj->usbDrvCommonObj.isDeviceRoleActive = false;
                drvObj->usbDrvCommonObj.isHostRoleActive = false;
                drvObj->usbDrvCommonObj.deviceModeClient = NULL;
                drvObj->usbDrvCommonObj.hostModeClient = NULL;

                for(counter = 0; counter < (DRV_USBHS_CLIENTS_NUMBER); counter++)
                {
                    /* In case of dual role, there would be multiple clients */
                    drvObj->usbDrvClientObj[counter].inUse = false;
                    drvObj->usbDrvClientObj[counter].pEventCallBack = NULL;
                    drvObj->usbDrvClientObj[counter].hDriver = NULL;
                }

                /* Delete the mutex */
                OSAL_MUTEX_Delete(&drvObj->usbDrvCommonObj.mutexID);
            }
            else
            {
                /* Cannot de-initialize an object that is not already in use. */
                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO,"\r\nUSBHS Driver: Driver not initialized in DRV_USBHS_Deinitialize()");
            }
        }
        else
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO,"\r\nUSBHS Driver: Invalid object in DRV_USBHS_Deinitialize()");
        }
    }
    else
    {
        /* Invalid object */
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO,"\r\nUSBHS Driver: Invalid object in DRV_USBHS_Deinitialize()");
    }
} 

// *****************************************************************************
/* Function:
    SYS_STATUS DRV_USBHS_Status ( SYS_MODULE_OBJ object )

  Summary:
    Provides the current status of the Hi-Speed USB Driver module.

  Description:
    This function provides the current status of the Hi-Speed USB Driver module.

  Remarks:
    See drv_usbhs.h for usage information.
*/

SYS_STATUS DRV_USBHS_Status
(
    SYS_MODULE_OBJ object
)
{
    SYS_STATUS result = SYS_STATUS_UNINITIALIZED;

    if (SYS_MODULE_OBJ_INVALID != object)
    {
        /* Return the status of the driver object */
        result = gDrvUSBObj[object].usbDrvCommonObj.status;
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid System Module Object in DRV_USBHS_Status().");
    }

    return(result);
}

// ****************************************************************************
/* Function:
    DRV_HANDLE DRV_USBHS_Open
    ( 
        const SYS_MODULE_INDEX drvIndex,
        const DRV_IO_INTENT intent
    )
    
  Summary:
    Opens the specified Hi-Speed USB Driver instance and returns a handle to it.
	
  Description:
    This function opens the specified Hi-Speed USB Driver instance and provides a
    handle that must be provided to all other client-level operations to
    identify the caller and the instance of the driver. The intent flag
    should always be
    DRV_IO_INTENT_EXCLUSIVE|DRV_IO_INTENT_READWRITE|DRV_IO_INTENT_NON_BLOCKING.
    Any other setting of the intent flag will return a invalid driver
    handle. A driver instance can only support one client. Trying to open a
    driver that has an existing client will result in an unsuccessful
    function call.

  Remarks:
    See drv_usbhs.h for usage information.
*/

DRV_HANDLE DRV_USBHS_Open
(
    const SYS_MODULE_INDEX iDriver,
    const DRV_IO_INTENT ioIntent 
)
{
    DRV_USBHS_OBJ * drvObj = NULL;
    DRV_HANDLE handle = DRV_HANDLE_INVALID;
    DRV_USBHS_CLIENT_OBJ * clientPool = NULL;
    uint8_t counter = 0;

    /* The iDriver value should be valid. It should be less the number of driver
     * object instances.  */

    if (iDriver < DRV_USBHS_INSTANCES_NUMBER)
    {
        drvObj = &gDrvUSBObj[iDriver];

        if ( SYS_STATUS_READY == drvObj->usbDrvCommonObj.status )
        {
            /* The grabbing of client pool object should be thread safe for all
             * driver operation modes. This is because:
             * 
             * 1. In Dual Role Mode, for a Single USB module running in RTOS
             * environment, Application task and Host task can preempt each
             * other. The preemption might lead to allocation of same client
             * pool object to both Host and Device layer.
             * 
             * 2. For Multi USB modules running in RTOS environment,
             * Host/Application task can preempt another Host/Application task.
             * This issue can happen in all possible driver operation modes */

            if (OSAL_MUTEX_Lock(&(drvObj->usbDrvCommonObj.mutexID), OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
            {
                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Mutex lock failed in DRV_USBHS_Open()");
            }
            else
            {
                /* Allocate a free index in the client pool, if available */
                for(counter = 0; counter < DRV_USBHS_CLIENTS_NUMBER; counter++)
                {
                    clientPool = &(drvObj->usbDrvClientObj[counter]);
                    if ( false == clientPool->inUse )
                    {
                        clientPool->inUse = true;
                        clientPool->hDriver = drvObj;
                        clientPool->pEventCallBack = NULL;
                        handle = ((DRV_HANDLE)clientPool);
                        
                        if(drvObj->usbDrvCommonObj.operationMode == DRV_USBHS_OPMODE_DEVICE)
                        {
                            /* The driver is configured for device mode operation */
                            drvObj->usbDrvCommonObj.deviceModeClient = clientPool;
                            drvObj->usbDrvCommonObj.hostModeClient = NULL;
                        }
                        else if(drvObj->usbDrvCommonObj.operationMode == DRV_USBHS_OPMODE_HOST)
                        {
                            /* The driver is configured for device mode operation */
                            drvObj->usbDrvCommonObj.deviceModeClient = NULL;
                            drvObj->usbDrvCommonObj.hostModeClient = clientPool;
                        }

                        break;
                    }            
                }

                if (DRV_HANDLE_INVALID == handle)
                {
                    SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "\r\nUSBHS Driver: Failed to open driver. No free slots available in pool in DRV_USBHS_Open().");
                }

                if (OSAL_MUTEX_Unlock(&drvObj->usbDrvCommonObj.mutexID) != OSAL_RESULT_TRUE)
                {
                    SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Mutex unlock failed in DRV_USBHS_Open()");
                }
            }
        }
        else
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "\r\nUSBHS Driver: Driver Not yet ready to be opened in DRV_USBHS_Open().");
        }
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "\r\nUSBHS Driver: Bad Driver Index in DRV_USBHS_Open().");
    }

    return (handle);
}

// *****************************************************************************
/* Function:
    void DRV_USBHS_Close( DRV_HANDLE handle )

  Summary:
    Closes an opened-instance of the Hi-Speed USB Driver.

  Description:
    This function closes an opened-instance of the Hi-Speed USB Driver,
    invalidating the handle.

  Remarks:
    See drv_usbhs.h for usage information.
*/

void DRV_USBHS_Close
(
    DRV_HANDLE client
)
{
    DRV_USBHS_CLIENT_OBJ * clientPool = NULL;

    if ( (DRV_HANDLE_INVALID !=  client) && (NULL != ((DRV_USBHS_CLIENT_OBJ *)client)) )
    {
        if ( ((DRV_USBHS_CLIENT_OBJ *)client)->inUse )
        {
            clientPool = (DRV_USBHS_CLIENT_OBJ *)client;
            clientPool->inUse = false;
            clientPool->pEventCallBack = NULL;
            clientPool->hDriver = NULL;
        }
        else
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_Close()");
        }
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_Close()");
    }
}

// *****************************************************************************
/* Function:
    void DRV_USBHS_Tasks_ISR( SYS_MODULE_OBJ object )

  Summary:
    Maintains the driver's Interrupt state machine and implements its ISR.

  Description:
    This function is used to maintain the driver's internal Interrupt state
    machine and implement its ISR for interrupt-driven implementations.

  Remarks:
    See drv_usbhs.h for usage information.
*/

void DRV_USBHS_Tasks_ISR
(
    SYS_MODULE_OBJ object
)
{
    DRV_USBHS_OBJ * hDriver = NULL;

    hDriver = &gDrvUSBObj[object];
    hDriver->usbDrvCommonObj.isInInterruptContext = true;
    
    switch(hDriver->usbDrvCommonObj.operationMode)
    {
        case DRV_USBHS_OPMODE_DEVICE:

            /* Driver is in Device mode */
            _DRV_USBHS_DEVICE_TASKS_ISR(hDriver);
            break;
        
        case DRV_USBHS_OPMODE_HOST:

            /* Driver is in Host Mode */
            _DRV_USBHS_HOST_TASKS_ISR(hDriver);
            break;
        
        case DRV_USBHS_OPMODE_DUAL_ROLE:

            /* Driver is in Dual Role mode. Check in which mode the driver is
             * presently operating */
            if ( (true == hDriver->usbDrvCommonObj.isDeviceRoleActive) && (true == hDriver->usbDrvCommonObj.isHostRoleActive) )
            {
                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Host and Device cannot be active simultaneously in DRV_USBHS_Tasks_ISR()");
            }
            else if ( true == hDriver->usbDrvCommonObj.isDeviceRoleActive )
            {
                _DRV_USBHS_DEVICE_TASKS_ISR(hDriver);
            }
            else if ( true == hDriver->usbDrvCommonObj.isHostRoleActive )
            {
                _DRV_USBHS_HOST_TASKS_ISR(hDriver);
            }
            
            break;    
        
        case DRV_USBHS_OPMODE_OTG:
            break;
        
        default:
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: What mode are you trying in DRV_USBHS_Tasks_ISR() ?");
            break;
    }
 
    /* Clear the interrupt */
    _DRV_USBHS_PersistentInterruptSourceClear(hDriver->usbDrvCommonObj.interruptSource);
    hDriver->usbDrvCommonObj.isInInterruptContext = false;
}

// *****************************************************************************
/* Function:
    void DRV_USBHS_Tasks_ISR_USBDMA( SYS_MODULE_OBJ object )

  Summary:
    Maintains the driver's DMA Transfer state machine and implements its ISR.

  Description:
    This function is used to maintain the driver's internal DMA Transfer state
    machine and implement its ISR for interrupt-driven implementations.

  Remarks:
    See drv_usbhs.h for usage information.
*/

void DRV_USBHS_Tasks_ISR_USBDMA
(
    SYS_MODULE_OBJ object
)
{
    DRV_USBHS_OBJ * hDriver =  NULL;

    hDriver = &gDrvUSBObj[object];
    hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA = true;
    
    switch (hDriver->usbDrvCommonObj.operationMode)
    {
        case DRV_USBHS_OPMODE_DEVICE:

            /* Driver is in Device Mode */
            _DRV_USBHS_DEVICE_TASKS_ISR_USBDMA(hDriver);
            break;
        
        case DRV_USBHS_OPMODE_HOST:

            /* Driver is in Host Mode */
            _DRV_USBHS_HOST_TASKS_ISR_USBDMA(hDriver);
            break;
        
        case DRV_USBHS_OPMODE_DUAL_ROLE:

            /* Driver is in Dual Role mode. Check whether we are in Device or
             * Host mode. */
            if ( (true == hDriver->usbDrvCommonObj.isDeviceRoleActive) && (true == hDriver->usbDrvCommonObj.isHostRoleActive) )
            {
                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Host and Device cannot be active simultaneously in DRV_USBHS_Tasks_ISR_USBDMA()");
            }
            if ( true == hDriver->usbDrvCommonObj.isDeviceRoleActive )
            {
                /* Run the Device DMA tasks */
                _DRV_USBHS_DEVICE_TASKS_ISR_USBDMA(hDriver);
            }
            else if ( true == hDriver->usbDrvCommonObj.isHostRoleActive )
            {
                /* Run the Host DMA Tasks */
                _DRV_USBHS_HOST_TASKS_ISR_USBDMA(hDriver);
            }
            
            break;
        
        case DRV_USBHS_OPMODE_OTG:
            break;
        
        default:
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: What mode are you trying in DRV_USBHS_Tasks_ISR_USBDMA() ?");
            break;
    }
        
    /* Clear the interrupt */
    hDriver->usbDrvCommonObj.isInInterruptContextUSBDMA = false;
    _DRV_USBHS_InterruptSourceClear(hDriver->usbDrvCommonObj.interruptSourceUSBDma);
}

// *****************************************************************************
/* Function:
    void DRV_USBHS_ClientEventCallBackSet
    ( 
        DRV_HANDLE handle,
        uintptr_t hReferenceData,
        DRV_USBHS_EVENT_CALLBACK myEventCallBack 
    );

  Summary:
    This function sets up the event callback function that is invoked by the USB
    controller driver to notify the client of USB bus events.
	
  Description:
    This function sets up the event callback function that is invoked by the USB
    controller driver to notify the client of USB bus events. The callback is
    disabled by either not calling this function after the DRV_USBHS_Open
    function has been called or by setting the myEventCallBack argument as NULL.
    When the callback function is called, the hReferenceData argument is
    returned.

  Remarks:
    See drv_usbhs.h for usage information.
*/

void DRV_USBHS_ClientEventCallBackSet
( 
    DRV_HANDLE client,
    uintptr_t hReferenceData,
    DRV_USB_EVENT_CALLBACK eventCallBack 
)
{
    DRV_USBHS_OBJ * hDriver = NULL;
    OSAL_CRITSECT_DATA_TYPE osalCritSectData = 0;

    if ( (DRV_HANDLE_INVALID !=  client) && (NULL != ((DRV_USBHS_CLIENT_OBJ *)client)) )
    {
        if ( ((DRV_USBHS_CLIENT_OBJ *)client)->inUse )
        {
            /* Assign event call back and reference data */
            ((DRV_USBHS_CLIENT_OBJ *)client)->hClientArg = hReferenceData;
            ((DRV_USBHS_CLIENT_OBJ *)client)->pEventCallBack = eventCallBack;

            /* Check if the VBUS is already valid. The current VBUS voltage is
             * tracked in the vbusLevel member of common driver object and is
             * updated in the driver polled tasks routine. */
            hDriver = ((DRV_USBHS_CLIENT_OBJ *)client)->hDriver;
            
            if( (DRV_USBHS_OPMODE_DUAL_ROLE == hDriver->usbDrvCommonObj.operationMode) )
            {
                hDriver->usbDrvCommonObj.deviceModeClient = ((DRV_USBHS_CLIENT_OBJ *)client);
                hDriver->usbDrvCommonObj.isDeviceRoleActive = true;
            }
                      
            if((hDriver->usbDrvCommonObj.vbusLevel >= USBHS_VBUS_BELOW_VBUSVALID) && ((DRV_USBHS_CLIENT_OBJ *)(client) == hDriver->usbDrvCommonObj.deviceModeClient))
            {
                /* This means the VBUS level is valid and the event handler is
                 * being set by the device mode client. We should send the
                 * event to the device mode client. The sessionValidEventSent
                 * flag is updated inside a critical section. This is done to
                 * avoid a situation where the driver polling tasks, which would
                 * be running in another thread, would also update the
                 * sessionValidEventSent flag. */
                
                osalCritSectData = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
                hDriver->usbDrvCommonObj.sessionValidEventSent = true;
                OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, osalCritSectData);

                hDriver->usbDrvCommonObj.deviceModeClient->pEventCallBack(hReferenceData, DRV_USBHS_EVENT_DEVICE_SESSION_VALID, NULL);
                hDriver->usbDrvCommonObj.sessionInvalidEventSent = false;
                hDriver->usbDrvCommonObj.sessionValidEventSent = true;
            }
        }
        else
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_ClientEventCallBackSet()");
        }
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBHS Driver: Invalid client in DRV_USBHS_ClientEventCallBackSet()");
    }
}

// *****************************************************************************
/* Function:
    void DRV_USBHS_InterruptHandler(void)

  Summary:
    USBHS Interrupt Handler 
	
  Description:
    This is USBHS Interrupt Handler 

  Remarks:
	None 
*/

void DRV_USBHS_InterruptHandler(void)
{
	DRV_USBHS_Tasks_ISR(sysObj.drvUSBHSObject); 
}

// *****************************************************************************
/* Function:
    void DRV_USBHS_DMAInterruptHandler(void)

  Summary:
    USBHS DAM Interrupt Handler 
	
  Description:
    This is USBHS DMA Interrupt Handler 

  Remarks:
	None 
*/
void DRV_USBHS_DMAInterruptHandler(void)
{
	DRV_USBHS_Tasks_ISR_USBDMA(sysObj.drvUSBHSObject); 
}