/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

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

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "console.h"

extern bool ctrlC;
bool breakOnControlC = true;

// *****************************************************************************
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{
   /* Application opens and attaches the device here */
    CONSOLE_STATE_INIT = 0,

    /* Application waits for device configuration*/
    CONSOLE_STATE_WAIT_FOR_CONFIGURATION,

    /* Wait for a character receive */
    CONSOLE_STATE_RUNNING,

    /* Application Error state*/
    CONSOLE_STATE_ERROR

} CONSOLE_STATES;


// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct 
{
    char data[1024];
    size_t head, tail;
} Queue;

static void ResetQueue(Queue *q)
{
    q->head = q->tail = 0;
}

static bool IsEmptyQueue(Queue *q)
{
    return q->head == q->tail;
}

static bool IsFullQueue(Queue *q)
{
    size_t next = q->tail + 1;
    if (next == sizeof(q->data))
        next = 0;
    return q->head == next;
}

static size_t AddToQueue(Queue *q, const char *data, size_t length)
{
    size_t freeBlock;
    size_t copied = 0;
    while (length)
    {
        size_t toCopy = length;
        if (q->tail >= q->head)
            freeBlock = sizeof(q->data) - q->tail;
        else
            freeBlock = q->head - q->tail - 1;
        if (freeBlock < toCopy)
            toCopy = freeBlock;
        // If the queue is full, exit
        if (toCopy == 0)
            break;
        memcpy(q->data + q->tail, data, toCopy);
        q->tail += toCopy;
        if (q->tail == sizeof(q->data))
            q->tail = 0;
        data += toCopy;
        length -= toCopy;
        copied += toCopy;
    }
    return copied;
}

static size_t ReadFromQueue(Queue *q, char *buf, size_t maxLength)
{
    size_t toCopy;
    if (q->tail >= q->head)
        toCopy = q->tail - q->head;
    else
        toCopy = sizeof(q->data) - q->head;
    if (maxLength < toCopy)
        toCopy = maxLength;
    if (toCopy)
    {
        memcpy(buf, q->data + q->head, toCopy);
        q->head += toCopy;
        if (q->head == sizeof(q->data))
            q->head = 0;
    }
    return toCopy;
}

typedef struct
{
   /* Flag indicates that device is configured */
    bool deviceIsConfigured;

    /* Flag indicates that SOF event has occurred */
    bool sofEventHasOccurred;

    /* Flag indicates that read has completed */
    bool readIsComplete;

    /* Flag indicates that write has complete */
    bool writeIsComplete;

    /* Device layer handle returned by device layer open function */
    USB_DEVICE_HANDLE deviceHandle;

    /* Application's current state*/
    CONSOLE_STATES state;

    /* Read Data Buffer */
    char *readBuffer;

    /* Set Line Coding Data */
    USB_CDC_LINE_CODING setLineCodingData;

    /* Get Line Coding Data */
    USB_CDC_LINE_CODING getLineCodingData;

    /* Control Line State */
    USB_CDC_CONTROL_LINE_STATE controlLineStateData;

    /* Break data */
    uint16_t breakData;

    /* Read transfer handle */
    USB_DEVICE_CDC_TRANSFER_HANDLE readTransferHandle;

    /* Write transfer handle */
    USB_DEVICE_CDC_TRANSFER_HANDLE writeTransferHandle;

    Queue readQueue, writeQueue;
    
    OSAL_SEM_DECLARE(semaphore);
    
} CONSOLE_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

#define CONSOLE_READ_BUFFER_SIZE 512
#define CONSOLE_MAKE_BUFFER_DMA_READY __attribute__((coherent, aligned(16)))

/*****************************************
 * Buffers required for reading and sending
 * data over CDC
 *****************************************/
char CONSOLE_MAKE_BUFFER_DMA_READY readBuffer[64];
char CONSOLE_MAKE_BUFFER_DMA_READY writeBuffer[64];

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the CONSOLE_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

CONSOLE_DATA consoleData;


// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/************************************************
 * CDC COM1 Application Event Handler
 ************************************************/

void CONSOLE_USBDeviceCDCEventHandler
(
    USB_DEVICE_CDC_INDEX index ,
    USB_DEVICE_CDC_EVENT event ,
    void* pData,
    uintptr_t userData
)
{

    CONSOLE_DATA * consoleDataObject;
    consoleDataObject = (CONSOLE_DATA *) userData;
    USB_CDC_CONTROL_LINE_STATE * controlLineStateData;
    USB_DEVICE_CDC_EVENT_DATA_READ_COMPLETE *data;
    
    switch ( event )
    {
        case USB_DEVICE_CDC_EVENT_GET_LINE_CODING:

            /* This means the host wants to know the current line
             * coding. This is a control transfer request. Use the
             * USB_DEVICE_ControlSend() function to send the data to
             * host.  */

            USB_DEVICE_ControlSend(consoleDataObject->deviceHandle,
                    (uint8_t *)&consoleDataObject->getLineCodingData, sizeof(USB_CDC_LINE_CODING));

            break;

        case USB_DEVICE_CDC_EVENT_SET_LINE_CODING:

            /* This means the host wants to set the line coding.
             * This is a control transfer request. Use the
             * USB_DEVICE_ControlReceive() function to receive the
             * data from the host */

            USB_DEVICE_ControlReceive(consoleDataObject->deviceHandle,
                    (uint8_t *) &consoleDataObject->setLineCodingData, sizeof(USB_CDC_LINE_CODING));

            break;

        case USB_DEVICE_CDC_EVENT_SET_CONTROL_LINE_STATE:

            /* This means the host is setting the control line state.
             * Read the control line state. We will accept this request
             * for now. */

            controlLineStateData = (USB_CDC_CONTROL_LINE_STATE *)pData;
            consoleDataObject->controlLineStateData.dtr = controlLineStateData->dtr;
            consoleDataObject->controlLineStateData.carrier = controlLineStateData->carrier;

            USB_DEVICE_ControlStatus(consoleDataObject->deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);

            break;

        case USB_DEVICE_CDC_EVENT_SEND_BREAK:

            /* This means that the host is requesting that a break of the
             * specified duration be sent. Read the break duration */

            consoleDataObject->breakData = ((USB_DEVICE_CDC_EVENT_DATA_SEND_BREAK *)pData)->breakDuration;
            break;

        case USB_DEVICE_CDC_EVENT_READ_COMPLETE:

            /* This means that the host has sent some data*/
            consoleData.readIsComplete = true;
            if (breakOnControlC && consoleData.readBuffer[0] == '\x03')
            {
                ResetQueue(&consoleDataObject->readQueue);
//                ctrlC = true;
            }
            else
            {
                data = (USB_DEVICE_CDC_EVENT_DATA_READ_COMPLETE *) pData;
                AddToQueue(&consoleDataObject->readQueue, consoleData.readBuffer, data->length);
//                OSAL_SEM_PostISR(consoleDataObject->semaphore);
            }

            break;

        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_RECEIVED:

            /* The data stage of the last control transfer is
             * complete. For now we accept all the data */

            USB_DEVICE_ControlStatus(consoleDataObject->deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;

        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_SENT:

            /* This means the GET LINE CODING function data is valid. We don't
             * do much with this data in this demo. */

            break;

        case USB_DEVICE_CDC_EVENT_WRITE_COMPLETE:

            /* This means that the data write got completed. We can schedule
             * the next read. */

            consoleData.writeIsComplete = true;
//            OSAL_SEM_PostISR(consoleDataObject->semaphore);

            break;

        default:
            break;
    }
}

/*************************************************
 * Application Device Layer Event Handler
 *************************************************/

void CONSOLE_USBDeviceEventCallBack ( USB_DEVICE_EVENT event, void * eventData, uintptr_t context )
{
    uint8_t configurationValue;

    switch ( event )
    {
        case USB_DEVICE_EVENT_RESET:
        case USB_DEVICE_EVENT_DECONFIGURED:

            /* USB device is reset or device is deconfigured.  This means that
             * USB device layer is about to deininitialize all function drivers. */
            consoleData.deviceIsConfigured = false;

            break;

        case USB_DEVICE_EVENT_SOF:

            consoleData.sofEventHasOccurred = true;
            break;

        case USB_DEVICE_EVENT_CONFIGURED:

            /* Check the configuration */
            configurationValue = ((USB_DEVICE_EVENT_DATA_CONFIGURED *)eventData)->configurationValue;;
            if (configurationValue == 1)
            {
                /* The device is in configured state */
                /* Register the CDC Device application event handler here.
                 * Note how the consoleData object pointer is passed as the
                 * user data */

                USB_DEVICE_CDC_EventHandlerSet(USB_DEVICE_CDC_INDEX_0, CONSOLE_USBDeviceCDCEventHandler, (uintptr_t)&consoleData);

                /* Mark that set configuration is complete */
                consoleData.deviceIsConfigured = true;

            }
            break;

        case USB_DEVICE_EVENT_SUSPENDED:
            break;

        case USB_DEVICE_EVENT_POWER_DETECTED:

            /* VBUS was detected. We can attach the device */
            USB_DEVICE_Attach(consoleData.deviceHandle);
            break;

        case USB_DEVICE_EVENT_POWER_REMOVED:

            /* VBUS is not available any more. Detach the device. */
            USB_DEVICE_Detach(consoleData.deviceHandle);
            break;

        /* These events are not used in this demo */
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

/************************************************
 * Application State Reset Function
 ************************************************/

bool CONSOLE_StateReset(void)
{
    /* This function returns true if the device
     * was reset  */

    bool retVal;

    if(!consoleData.deviceIsConfigured)
    {
        consoleData.state = CONSOLE_STATE_WAIT_FOR_CONFIGURATION;
        consoleData.readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
        consoleData.writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
        consoleData.readIsComplete = true;
        consoleData.writeIsComplete = true;
        ResetQueue(&consoleData.readQueue);
        ResetQueue(&consoleData.writeQueue);
        
        retVal = true;
    }
    else
    {
        retVal = false;
    }

    return(retVal);
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void CONSOLE_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void CONSOLE_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    consoleData.state = CONSOLE_STATE_INIT;
    
    /* Device Layer Handle  */
    consoleData.deviceHandle = USB_DEVICE_HANDLE_INVALID;

    /* Initial get line coding state */
    consoleData.getLineCodingData.bDataBits = 8;
    consoleData.getLineCodingData.dwDTERate = 9600;
    consoleData.getLineCodingData.bCharFormat = 0;
    consoleData.getLineCodingData.bParityType = 0;

    /* Read Transfer Handle */
    consoleData.readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;

    /* Write Transfer Handle */
    consoleData.writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;

    /* Initialize the application flags */
    consoleData.readIsComplete     = true;
    consoleData.writeIsComplete    = true;
    consoleData.deviceIsConfigured = false;
    consoleData.sofEventHasOccurred = false;
    ResetQueue(&consoleData.readQueue);
    ResetQueue(&consoleData.writeQueue);

    /* Assign the read buffer */
    consoleData.readBuffer = &readBuffer[0];
    
    OSAL_SEM_Create(&consoleData.semaphore, OSAL_SEM_TYPE_BINARY, 0, 0);
}


/******************************************************************************
  Function:
    void CONSOLE_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void CONSOLE_Tasks (void)
{
    /* Update the application state machine based
     * on the current state */

    switch(consoleData.state)
    {
        case CONSOLE_STATE_INIT:

            /* Open the device layer */
            consoleData.deviceHandle = USB_DEVICE_Open( USB_DEVICE_INDEX_0, DRV_IO_INTENT_READWRITE );

            if(consoleData.deviceHandle != USB_DEVICE_HANDLE_INVALID)
            {
                /* Register a callback with device layer to get event notification (for end point 0) */
                USB_DEVICE_EventHandlerSet(consoleData.deviceHandle, CONSOLE_USBDeviceEventCallBack, 0);

                consoleData.state = CONSOLE_STATE_WAIT_FOR_CONFIGURATION;
            }
            else
            {
                /* The Device Layer is not ready to be opened. We should try
                 * again later. */
            }

            break;

        case CONSOLE_STATE_WAIT_FOR_CONFIGURATION:

            /* Check if the device was configured */
            if(consoleData.deviceIsConfigured)
            {
                /* If the device is configured then lets start reading and writing */
                consoleData.state = CONSOLE_STATE_RUNNING;
            }
            else
//                vTaskDelay(configTICK_RATE_HZ / 20);
            break;

        case CONSOLE_STATE_RUNNING:

//            OSAL_SEM_Pend(consoleData.semaphore, 100);
            
            if(CONSOLE_StateReset())
            {
                break;
            }
           
            /* If a read is complete, then schedule a read */
            if(consoleData.readIsComplete)
            {
                consoleData.readIsComplete = false;
                consoleData.readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
                USB_DEVICE_CDC_Read (USB_DEVICE_CDC_INDEX_0, &consoleData.readTransferHandle, consoleData.readBuffer, CONSOLE_READ_BUFFER_SIZE);
                if(consoleData.readTransferHandle == USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID)
                {
                    consoleData.state = CONSOLE_STATE_ERROR;
                    break;
                }
            }

            /* If a write is complete and there is more data to write */
            if (consoleData.writeIsComplete && !IsEmptyQueue(&consoleData.writeQueue) && consoleData.controlLineStateData.dtr)
            {
                size_t count;
                /* Setup the write */

                consoleData.writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
                consoleData.writeIsComplete = false;

                /* Echo the received character + 1*/
                count = ReadFromQueue(&consoleData.writeQueue, writeBuffer, sizeof(writeBuffer));
                USB_DEVICE_CDC_Write(USB_DEVICE_CDC_INDEX_0, &consoleData.writeTransferHandle,
                    &writeBuffer, count, USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE);
            }
            
            break;

        case CONSOLE_STATE_ERROR:
            break;
            
        default:
            break;
    }
}

int CONSOLE_Read()
{
    int rtn = -1;
    if (!consoleData.controlLineStateData.dtr)
        return 0;
//    SYS_INT_PROCESSOR_STATUS interruptState = SYS_INT_StatusGetAndDisable();
    if (!IsEmptyQueue(&consoleData.readQueue))
    {
        char c = 0;
        ReadFromQueue(&consoleData.readQueue, &c, 1);
        rtn = (unsigned char) c;
    }
//    SYS_INT_StatusRestore(interruptState);
    return rtn;
}

int CONSOLE_Write(char c)
{
    size_t written = 0;
    bool full;
    do
    {
//        SYS_INT_PROCESSOR_STATUS interruptState = SYS_INT_StatusGetAndDisable();
        full = IsFullQueue(&consoleData.writeQueue);
        if (!full)
            written = AddToQueue(&consoleData.writeQueue, &c, 1);
//        SYS_INT_StatusRestore(interruptState);
    } while (full && consoleData.controlLineStateData.dtr);
//    OSAL_SEM_Post(consoleData.semaphore);
    return (int) written;
}

bool CONSOLE_WriteBufferEmpty()
{
    return IsEmptyQueue(&consoleData.writeQueue);
}


/*******************************************************************************
 End of File
 */

