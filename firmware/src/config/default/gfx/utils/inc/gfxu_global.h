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
 Module for Microchip Graphics Library - Graphics Utilities Library

  Company:
    Microchip Technology Inc.

  File Name:
    gfxu_global.h

  Summary:
    Global defines for graphics utility library.

  Description:
    Type definitions.
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef GFX_UTILS_GLOBAL_H
#define GFX_UTILS_GLOBAL_H
//DOM-IGNORE-END

#include "gfx/hal/gfx.h"

#define GFXU_ASSET_LOCATION_INTERNAL 0

typedef struct GFXU_AssetHeader_t GFXU_AssetHeader; 

typedef struct GFXU_ExternalAssetReader_t GFXU_ExternalAssetReader;


// *****************************************************************************
/* typedef:
    GFXU_MediaOpenRequest_FnPtr

  Summary:
    A callback that indicates that a media decoder wishes to read from an
    external media source and that the application should prepare that source.
    
    The argument is the asset that needs to be read. 
    
    If the result is false then the decoder will abort.
*/
typedef GFX_Result (*GFXU_MediaOpenRequest_FnPtr)(GFXU_AssetHeader* ast);

// *****************************************************************************
/* typedef:
    GFXU_MediaReadRequestCallback_FnPtr

  Summary:
    A callback that signifies that a media read request has been fulfilled.
    Often signals a state machine to continue processing a decode
    operation.
    
    The argument is the reader that requested the memory.
*/
typedef void (*GFXU_MediaReadRequestCallback_FnPtr)(GFXU_ExternalAssetReader*);

// *****************************************************************************
/* typedef:
    GFXU_MemoryReadRequest_FnPtr

  Summary:
    A memory read request function is a request that is issued by anything that
    needs external support to access data that is stored in some external
    location.  For instance, an image may be stored in an SPI memory chip.
    A JPEG decoder has no knowledge of what SPI is.  This read request is issued
    to the application so that the application can then assume responsibility for
    communication with the peripherial to retrieve the data.  If the reader pointer
    or the callback pointer are null then the read request must be serviced 
    immediately.  In other words, the requesting decoder does not support
    non-blocking streaming.
    
    From the reader the handler can determine which asset is being decoded and
    which media it resides in.
*/                                                 
typedef GFX_Result (*GFXU_MediaReadRequest_FnPtr)(GFXU_ExternalAssetReader* reader, // asset reader
                                                  GFXU_AssetHeader* asset, // the asset being read
                                                  void*,     // address
                                                  uint32_t,  // dest size
                                                  uint8_t*,  // dest buffer
                                                  GFXU_MediaReadRequestCallback_FnPtr); // callback
          
// *****************************************************************************
/* typedef:
    GFXU_MediaCloseRequest_FnPtr

  Summary:
    A callback that indicates that a media decoder is finished with a given
    media location and that the application can close if it.
    
    The argument is the asset that was being read. 
*/
typedef void (*GFXU_MediaCloseRequest_FnPtr)(GFXU_AssetHeader* ast);


// *****************************************************************************
/* typedef:
    GFXU_ExternalAssetReaderRun_FnPtr

  Summary:
    This function pointer represents a function that maintains the state of
    an external reader state machine.
    
    The argument is the state machine to process.
*/ 
typedef GFX_Result (*GFXU_ExternalAssetReaderRun_FnPtr)(GFXU_ExternalAssetReader*);

// *****************************************************************************
/* enum:
    GFXU_AssetType_t

  Summary:
    Enumerates known asset types.
*/ 
typedef enum GFXU_AssetType_t
{
    GFXU_ASSET_TYPE_IMAGE = 0,
    GFXU_ASSET_TYPE_PALETTE,
    GFXU_ASSET_TYPE_FONT,
    GFXU_ASSET_TYPE_BINARY,
    GFXU_ASSET_TYPE_STRINGTABLE
} GFXU_AssetType;

// *****************************************************************************
/* Structure:
    GFXU_AssetHeader_t

  Summary:
    Defines a common header for all assets supported by the generic decoder 
    interface.
    
  Description:
    type - GFXU_AssetType - indicates the type of the asset
    dataLocation - indicates the location of the asset data.  0 is always
                   internal flash.  any other number must be understood
                   by the application
    dataAddress - the address at which the data resides.  may be a local pointer
                  or a location in some external storage location not in the
                  local memory map.
    dataSize - the size of the asset data in bytes
*/
typedef struct GFXU_AssetHeader_t
{
    uint32_t type;
    uint32_t dataLocation;
    void* dataAddress;
    uint32_t dataSize;
} GFXU_AssetHeader; 

// *****************************************************************************
/* Structure:
    GFXU_MemoryIntf_t

  Summary:
    Defines a memory interface for all memory operations.  Essentially wraps a
    GFX_MemoryIntf with the notable addition lf a GFXU_MemoryReadRequest_FnPtr.
    
    The 'read' function pointer will be invoked when a decoder attempts to
    access an asset that has a location that is greater than zero.  The
    application must then look at the asset's location id and determine which
    peripheral to communicate with.
    
  Description:
    heap - function pointer for memory operations
    read - function pointer to use for memory read requests
*/
typedef struct GFXU_MemoryIntf_t
{
    GFX_MemoryIntf heap;
    GFXU_MediaOpenRequest_FnPtr open;
    GFXU_MediaReadRequest_FnPtr read;
    GFXU_MediaCloseRequest_FnPtr close;
} GFXU_MemoryIntf;

// *****************************************************************************
/* enum:
    GFXU_ExternalAssetReaderStatus_t

  Summary:
    Enumerates external reader state machine states.
    
    Invalid - state machine hasn't been initialized
    Read - state machine is ready to begin processing but hasn't been run yet
    Waiting - state machine is waiting for a memory operation to complete
    Drawing - state machine is waiting for a drawing operation to complete
    Finished - state machine has finished drawing the entire asset
    Aborted - state machine encountered an error and has aborted
*/ 
typedef enum GFXU_ExternalAssetReaderStatus_t
{
    GFXU_READER_STATUS_INVALID = 0,
    GFXU_READER_STATUS_READY,
    GFXU_READER_STATUS_WAITING,
    GFXU_READER_STATUS_DRAWING,
    GFXU_READER_STATUS_FINISHED,
    GFXU_READER_STATUS_ABORTED
} GFXU_ExternalAssetReaderStatus;

// *****************************************************************************
/* Structure:
    GFXU_MemoryIntf_t

  Summary:
    Defines the base description of an external asset reader.  Specific decoder
    implementations will build on this foundation for each decoder type.
    
  Description:
    status - the overall status of the decoder state machine
    state - mostly for decoder internal use
    result - can be used for an overall result of the state of an operation
    memIntf - the memory interface the decoder is using for memory operations
    run - the run pointer that must be called periodically to allow the state
          machine to process to completion.
    
*/
typedef struct GFXU_ExternalAssetReader_t
{
    GFXU_ExternalAssetReaderStatus status;
    uint32_t state;
    int32_t result;
    
    GFXU_MemoryIntf* memIntf;
    GFXU_ExternalAssetReaderRun_FnPtr run;
    
    void* userData;
} GFXU_ExternalAssetReader;

#endif /* GFX_UTILS_GLOBAL_H */