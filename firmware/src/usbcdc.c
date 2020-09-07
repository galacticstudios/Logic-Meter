/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    usbcdc.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "usbcdc.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

static uint8_t CACHE_ALIGN cdcReadBuffer[USBCDC_READ_BUFFER_SIZE];
static uint8_t CACHE_ALIGN cdcWriteBuffer[USBCDC_READ_BUFFER_SIZE];

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the USBCDC_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

USBCDC_DATA usbcdcData;


/*******************************************************
 * USB CDC Device Events - Application Event Handler
 *******************************************************/

static USB_DEVICE_CDC_EVENT_RESPONSE USBCDC_USBDeviceCDCEventHandler
(
    USB_DEVICE_CDC_INDEX index,
    USB_DEVICE_CDC_EVENT event,
    void * pData,
    uintptr_t userData
)
{
    USBCDC_DATA * appDataObject;
    USB_CDC_CONTROL_LINE_STATE * controlLineStateData;
    USB_DEVICE_CDC_EVENT_DATA_READ_COMPLETE * eventDataRead;
    
    appDataObject = (USBCDC_DATA *)userData;

    switch(event)
    {
        case USB_DEVICE_CDC_EVENT_GET_LINE_CODING:

            /* This means the host wants to know the current line
             * coding. This is a control transfer request. Use the
             * USB_DEVICE_ControlSend() function to send the data to
             * host.  */

            USB_DEVICE_ControlSend(appDataObject->deviceHandle,
                    &appDataObject->getLineCodingData, sizeof(USB_CDC_LINE_CODING));

            break;

        case USB_DEVICE_CDC_EVENT_SET_LINE_CODING:

            /* This means the host wants to set the line coding.
             * This is a control transfer request. Use the
             * USB_DEVICE_ControlReceive() function to receive the
             * data from the host */

            USB_DEVICE_ControlReceive(appDataObject->deviceHandle,
                    &appDataObject->setLineCodingData, sizeof(USB_CDC_LINE_CODING));

            break;

        case USB_DEVICE_CDC_EVENT_SET_CONTROL_LINE_STATE:

            /* This means the host is setting the control line state.
             * Read the control line state. We will accept this request
             * for now. */

            controlLineStateData = (USB_CDC_CONTROL_LINE_STATE *)pData;
            appDataObject->controlLineStateData.dtr = controlLineStateData->dtr;
            appDataObject->controlLineStateData.carrier = controlLineStateData->carrier;

            USB_DEVICE_ControlStatus(appDataObject->deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);

            break;

        case USB_DEVICE_CDC_EVENT_SEND_BREAK:

            /* This means that the host is requesting that a break of the
             * specified duration be sent. Read the break duration */

            appDataObject->breakData = ((USB_DEVICE_CDC_EVENT_DATA_SEND_BREAK *)pData)->breakDuration;
            
            /* Complete the control transfer by sending a ZLP  */
            USB_DEVICE_ControlStatus(appDataObject->deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            
            break;

        case USB_DEVICE_CDC_EVENT_READ_COMPLETE:

            /* This means that the host has sent some data*/
            eventDataRead = (USB_DEVICE_CDC_EVENT_DATA_READ_COMPLETE *)pData;
            appDataObject->isReadComplete = true;
            appDataObject->numBytesRead = eventDataRead->length; 
            break;

        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_RECEIVED:

            /* The data stage of the last control transfer is
             * complete. For now we accept all the data */

            USB_DEVICE_ControlStatus(appDataObject->deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;

        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_SENT:

            /* This means the GET LINE CODING function data is valid. We don't
             * do much with this data in this demo. */
            break;

        case USB_DEVICE_CDC_EVENT_WRITE_COMPLETE:

            /* This means that the data write got completed. We can schedule
             * the next read. */

            appDataObject->isWriteComplete = true;
            break;

        default:
            break;
    }

    return USB_DEVICE_CDC_EVENT_RESPONSE_NONE;
}

/***********************************************
 * Application USB Device Layer Event Handler.
 ***********************************************/
static void USBCDC_USBDeviceEventHandler 
(
    USB_DEVICE_EVENT event, 
    void * eventData, 
    uintptr_t context 
)
{
    USB_DEVICE_EVENT_DATA_CONFIGURED *configuredEventData;

    switch(event)
    {
        case USB_DEVICE_EVENT_SOF:

            usbcdcData.sofEventHasOccurred = true;
            
            break;

        case USB_DEVICE_EVENT_RESET:

            /* Update LED to show reset state */
            //LED_Off();

            usbcdcData.isConfigured = false;

            break;

        case USB_DEVICE_EVENT_CONFIGURED:

            /* Check the configuration. We only support configuration 1 */
            configuredEventData = (USB_DEVICE_EVENT_DATA_CONFIGURED*)eventData;
            
            if ( configuredEventData->configurationValue == 1)
            {
                /* Update LED to show configured state */
                //LED_On();
                
                /* Register the CDC Device application event handler here.
                 * Note how the appData object pointer is passed as the
                 * user data */

                USB_DEVICE_CDC_EventHandlerSet(USB_DEVICE_CDC_INDEX_0, USBCDC_USBDeviceCDCEventHandler, (uintptr_t)&usbcdcData);

                /* Mark that the device is now configured */
                usbcdcData.isConfigured = true;
            }
            
            break;

        case USB_DEVICE_EVENT_POWER_DETECTED:

            /* VBUS was detected. We can attach the device */
            USB_DEVICE_Attach(usbcdcData.deviceHandle);
            
            break;

        case USB_DEVICE_EVENT_POWER_REMOVED:

            /* VBUS is not available any more. Detach the device. */
            USB_DEVICE_Detach(usbcdcData.deviceHandle);
            
            //LED_Off();
            
            break;

        case USB_DEVICE_EVENT_SUSPENDED:

            //LED_Off();
            
            break;

        case USB_DEVICE_EVENT_RESUMED:
        case USB_DEVICE_EVENT_ERROR:
        default:
            
            break;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

/*****************************************************
 * This function is called in every step of the
 * application state machine.
 *****************************************************/

static bool USBCDC_StateReset(void)
{
    /* This function returns true if the device
     * was reset  */

    bool retVal;

    if(usbcdcData.isConfigured == false)
    {
        usbcdcData.state = USBCDC_STATE_WAIT_FOR_CONFIGURATION;
        usbcdcData.readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
        usbcdcData.writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
        usbcdcData.isReadComplete = true;
        usbcdcData.isWriteComplete = true;
        retVal = true;
    }
    else
    {
        retVal = false;
    }

    return(retVal);
}
/*******************************************************************************
  Function:
    void USBCDC_Initialize ( void )

  Remarks:
    See prototype in usbcdc.h.
 */

void USBCDC_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    usbcdcData.state = USBCDC_STATE_INIT;
    
    /* Device Layer Handle  */
    usbcdcData.deviceHandle = USB_DEVICE_HANDLE_INVALID ;

    /* Device configured status */
    usbcdcData.isConfigured = false;

    /* Initial get line coding state */
    usbcdcData.getLineCodingData.dwDTERate = 9600;
    usbcdcData.getLineCodingData.bParityType = 0;
    usbcdcData.getLineCodingData.bParityType = 0;
    usbcdcData.getLineCodingData.bDataBits = 8;

    /* Read Transfer Handle */
    usbcdcData.readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;

    /* Write Transfer Handle */
    usbcdcData.writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;

    /* Initialize the read complete flag */
    usbcdcData.isReadComplete = true;

    /*Initialize the write complete flag*/
    usbcdcData.isWriteComplete = true;

    /* Reset other flags */
    usbcdcData.sofEventHasOccurred = false;

    /* Set up the read buffer */
    usbcdcData.cdcReadBuffer = &cdcReadBuffer[0];

    /* Set up the read buffer */
    usbcdcData.cdcWriteBuffer = &cdcWriteBuffer[0];       }


/******************************************************************************
  Function:
    void USBCDC_Tasks ( void )

  Remarks:
    See prototype in usbcdc.h.
 */

void USBCDC_Tasks ( void )
{
    /* Update the application state machine based
     * on the current state */
    int i;
    
    switch(usbcdcData.state)
    {
        case USBCDC_STATE_INIT:

            /* Open the device layer */
            usbcdcData.deviceHandle = USB_DEVICE_Open( USB_DEVICE_INDEX_0, DRV_IO_INTENT_READWRITE );

            if(usbcdcData.deviceHandle != USB_DEVICE_HANDLE_INVALID)
            {
                /* Register a callback with device layer to get event notification (for end point 0) */
                USB_DEVICE_EventHandlerSet(usbcdcData.deviceHandle, USBCDC_USBDeviceEventHandler, 0);

                usbcdcData.state = USBCDC_STATE_WAIT_FOR_CONFIGURATION;
            }
            else
            {
                /* The Device Layer is not ready to be opened. We should try
                 * again later. */
            }

            break;

        case USBCDC_STATE_WAIT_FOR_CONFIGURATION:

            /* Check if the device was configured */
            if(usbcdcData.isConfigured)
            {
                /* If the device is configured then lets start reading */
                usbcdcData.state = USBCDC_STATE_SCHEDULE_READ;
            }
            
            break;

        case USBCDC_STATE_SCHEDULE_READ:

            if(USBCDC_StateReset())
            {
                break;
            }

            /* If a read is complete, then schedule a read
             * else wait for the current read to complete */

            usbcdcData.state = USBCDC_STATE_WAIT_FOR_READ_COMPLETE;
            if(usbcdcData.isReadComplete == true)
            {
                usbcdcData.isReadComplete = false;
                usbcdcData.readTransferHandle =  USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;

                USB_DEVICE_CDC_Read (USB_DEVICE_CDC_INDEX_0,
                        &usbcdcData.readTransferHandle, usbcdcData.cdcReadBuffer,
                        USBCDC_READ_BUFFER_SIZE);
                
                if(usbcdcData.readTransferHandle == USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID)
                {
                    usbcdcData.state = USBCDC_STATE_ERROR;
                    break;
                }
            }

            break;

        case USBCDC_STATE_WAIT_FOR_READ_COMPLETE:

            if(USBCDC_StateReset())
            {
                break;
            }

            /* Check if a character was received the isReadComplete flag gets 
             * updated in the CDC event handler. */

            if(usbcdcData.isReadComplete)
            {
                usbcdcData.state = USBCDC_STATE_SCHEDULE_WRITE;
            }

            break;


        case USBCDC_STATE_SCHEDULE_WRITE:

            if(USBCDC_StateReset())
            {
                break;
            }

            /* Setup the write */

            usbcdcData.writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
            usbcdcData.isWriteComplete = false;
            usbcdcData.state = USBCDC_STATE_WAIT_FOR_WRITE_COMPLETE;

            /* Else echo each received character by adding 1 */
            for(i = 0; i < usbcdcData.numBytesRead; i++)
            {
                if((usbcdcData.cdcReadBuffer[i] != 0x0A) && (usbcdcData.cdcReadBuffer[i] != 0x0D))
                {
                    usbcdcData.cdcWriteBuffer[i] = usbcdcData.cdcReadBuffer[i] + 1;
                }
            }
            USB_DEVICE_CDC_Write(USB_DEVICE_CDC_INDEX_0,
                    &usbcdcData.writeTransferHandle,
                    usbcdcData.cdcWriteBuffer, usbcdcData.numBytesRead,
                    USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE);

            break;

        case USBCDC_STATE_WAIT_FOR_WRITE_COMPLETE:

            if(USBCDC_StateReset())
            {
                break;
            }

            /* Check if a character was sent. The isWriteComplete
             * flag gets updated in the CDC event handler */

            if(usbcdcData.isWriteComplete == true)
            {
                usbcdcData.state = USBCDC_STATE_SCHEDULE_READ;
            }

            break;

        case USBCDC_STATE_ERROR:
        default:
            
            break;
    }
}


/*******************************************************************************
 End of File
 */
