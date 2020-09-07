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


#ifndef GFXU_IMAGE_JPG_COMMON_H
#define GFXU_IMAGE_JPG_COMMON_H

//#include "gfx/hal/inc/gfx_context.h"
#include "gfx/utils/inc/gfxu_image.h"
//#include "gfx/utils/inc/gfxu_palette.h"

#define JPEG_SendError(x)  pJpegDecoder->bError = x; return(x); /* Point to proper error handler routine */

#define JPEG_SAMPLE_1x1 0
#define JPEG_SAMPLE_1x2 1
#define JPEG_SAMPLE_2x1 2
#define JPEG_SAMPLE_2x2 3

#define MAX_CHANNELS      3 /* This supports only Grayscale and YCbCr images - DONT CHANGE THIS */
#define MAX_BLOCKS        6 /* To decode one logical block, we have to decode 1 to 6 blocks depending on channels and subsampling - DONT REDUCE THIS */
#define MAX_HUFF_TABLES   2 /* Each causes 2 tables -> One for AC and another for DC - DONT REDUCE THIS */
#define MAX_DATA_BUF_LEN  512 /* Increase if you have more data memory */

#define EXIF_2_2_ORIENTATION_OFFSET 0x2E

/* Error list */
enum Errors
{
    IMAGE_FILE_NOT_AVAILABLE
};

/* JPEG Markers list */
enum Markers
{
    SOF0 = 0xC0,
    DHT = 0xC4,
    SOI = 0xD8,
    EOI = 0xD9,
    SOS = 0xDA,
    DQT = 0xDB,
    DRI = 0xDD,
    APP0 = 0xE0,
    APP1 = 0xE1, // EXIF
    COM = 0xFE,
    /* The below markers doesn't have parameters */
    TEM = 0x01,
    RST0 = 0xD0,
    RST1 = 0xD1,
    RST2 = 0xD2,
    RST3 = 0xD3,
    RST4 = 0xD4,
    RST5 = 0xD5,
    RST6 = 0xD6,
    RST7 = 0xD7
};

typedef struct _JPEGDECODER JPEGDECODER;

typedef uint32_t (*JPEG_Read_FnPtr)(void*, uint32_t, uint32_t, JPEGDECODER*);

/*************************/
/**** DATA STRUCTURES ****/
/*************************/
typedef struct _JPEGDECODER
{
        GFXU_ImageAsset *pImageFile;           /* Image file pointer */
        GFXU_MemoryIntf* memIntf;              /* memory interface */
        GFX_PixelBuffer pixelBlockBuffer;      /* Pointer to the pixel buffer for the current block to blit */
        JPEG_Read_FnPtr readPtr;               /* function to read memory */
        
        uint32_t fileIndex;                    /* pointer to current location in image */

/*********** From APP0 ***********/
        uint8_t blJFIF;                            /* JFIF marker found flag */
        uint8_t bMajorRev;                         /* Should be 1 */
        uint8_t bMinorRev;                         /* Should be 0-2 but is not a show stopper */
        /*------- The x/y densities and thumbnail data are ignored --------*/

        /*********** From APP1 EXIF ***********/
        uint8_t bOrientation;
        
        /*********** From SOF0 ***********/
        uint8_t bDataBits;                         /* Data precision, can be 8(, 12 or 16) */
        uint16_t wWidth;                            /* Width in pixels */
        uint16_t wHeight;                           /* Height in pixels */
        uint8_t bChannels;                         /* Number of channels e.g. YCbCr = 3 */
        uint8_t abChannelType[MAX_CHANNELS];
        uint8_t abChannelHSampFactor[MAX_CHANNELS];
        uint8_t abChannelVSampFactor[MAX_CHANNELS];
        uint8_t abChannelQuantTableMap[MAX_CHANNELS];

        /*********** From DQT ***********/
        uint8_t blQuantUses16bits;                 /* If flag is set, it is an error as 16 bit is not supported */
        uint16_t awQuantTable[MAX_CHANNELS][64];    /* Supports only 8 & 16 bit resolutions */

        /*********** From DRI ***********/
        uint16_t wRestartInterval;                  /* The restart interval in blocks */

        /*********** From DHT ***********/
        uint8_t bHuffTables;
        uint8_t abHuffAcSymLen[MAX_HUFF_TABLES][16];   /* Supports only 8 bit resolution */
        uint8_t abHuffAcSymbol[MAX_HUFF_TABLES][256];  /* Maximum possible symbols are 256 */
        uint8_t abHuffDcSymLen[MAX_HUFF_TABLES][16];   /* Supports only 8 bit resolution */
        uint8_t abHuffDcSymbol[MAX_HUFF_TABLES][16];   /* Maximum possible symbols are 16 for DC :-) */
        /*********** For Huffman-Decoding ***********/
        uint16_t awHuffAcSymStart[MAX_HUFF_TABLES][16]; /* Starting symbol for each length */
        uint16_t awHuffDcSymStart[MAX_HUFF_TABLES][16]; /* Starting symbol for each length */

        /*********** From SOS ***********/
        uint8_t abChannelHuffAcTableMap[MAX_CHANNELS];
        uint8_t abChannelHuffDcTableMap[MAX_CHANNELS];

        uint8_t bError;

        /*********** Work memory ***********/
        uint16_t wWorkBits;
        uint8_t bBitsAvailable;
        uint8_t bBlocksInOnePass;
        short asOneBlock[MAX_BLOCKS][64];     /* Temporary storage for a 8x8 block */
        uint16_t  wBlockNumber;
        uint8_t  abChannelMap[MAX_BLOCKS];
        uint8_t  bSubSampleType;
        short asPrevDcValue[MAX_CHANNELS];
        uint8_t *pbCurrentHuffSymLenTable;
        uint8_t *pbCurrentHuffSymbolTable;
        uint16_t *pwCurrentHuffSymStartTable;
        uint16_t *pwCurrentQuantTable;
        uint8_t abDataBuffer[MAX_DATA_BUF_LEN];
        uint16_t wBufferLen;
        uint16_t wBufferIndex;
        uint8_t bFirstBit;

        int32_t wStartX;
        int32_t wStartY;
        
        int32_t wDrawWidth;
        int32_t wDrawHeight;
        
        int32_t wDrawX;
        int32_t wDrawY;        
        
        int32_t wPrevX;
        int32_t wPrevY;
        uint8_t handle; //handle to the driver instance
} JPEGDECODER;

/*******************************************************************************
Function:       void JPEG_vResetDecoder(JPEGDECODER *pJpegDecoder)

Precondition:   None

Overview:       This function resets the variables so that new jpeg image can be
                decoded

Input:          JPEGDECODER

Output:         None
*******************************************************************************/
void JPEG_vResetDecoder(JPEGDECODER *decoder);

/*******************************************************************************
Function:       uint8_t JPEG_bReadHeader(JPEGDECODER *pJpegDecoder)

Precondition:   None

Overview:       This function reads the JPEG file header and 
                fills the data structure

Input:          JPEGDECODER

Output:         Error code - '0' means no error
*******************************************************************************/
uint8_t JPEG_bReadHeader(JPEGDECODER *pJpegDecoder);

/*******************************************************************************
Function:       uint8_t JPEG_bGenerateHuffmanTables(JPEGDECODER *pJpegDecoder)

Precondition:   None

Overview:       This function generated the table required for Huffman decoding
                from the data read from the header

Input:          JPEGDECODER

Output:         Error code - '0' means no error
*******************************************************************************/
uint8_t JPEG_bGenerateHuffmanTables(JPEGDECODER *pJpegDecoder);

/*******************************************************************************
Function:       void JPEG_vInitDisplay(JPEGDECODER *pJpegDecoder)

Precondition:   None

Overview:       Initializes the (x, y) co-ordinates to (0, 0)

Input:          JPEGDECODER

Output:         None
*******************************************************************************/
void JPEG_vInitDisplay(JPEGDECODER *pJpegDecoder);

/*******************************************************************************
Function:       uint8_t JPEG_bDecodeOneBlock(JPEGDECODER *pJpegDecoder)

Precondition:   File pointer must point to a new block of data

Overview:       Decodes the 8x8 pixel values of all the channels
                (A multiple of 8x8 block if subsampling is used)

Input:          JPEGDECODER

Output:         Error code - '0' means no error
*******************************************************************************/
uint8_t JPEG_bDecodeOneBlock(JPEGDECODER *pJpegDecoder);

/*******************************************************************************
Function:       uint8_t JPEG_bPaintOneBlock(JPEGDECODER *pJpegDecoder)

Precondition:   One block - 8x8 pixel data of all channels must be decoded

Overview:       Displays one 8x8 on the screen
                (A multiple of 8x8 block if subsampling is used)

Input:          JPEGDECODER

Output:         Error code - '0' means no error
*******************************************************************************/
uint8_t JPEG_bPaintOneBlock(JPEGDECODER *pJpegDecoder);



/**************************/
/**** CONSTANT TABLES  ****/
/**************************/
static const uint8_t abZigzag[64] =
{
  0,  1,  8,  16, 9,  2,  3,  10,
  17, 24, 32, 25, 18, 11, 4,  5,
  12, 19, 26, 33, 40, 48, 41, 34,
  27, 20, 13, 6,  7,  14, 21, 28,
  35, 42, 49, 56, 57, 50, 43, 36,
  29, 22, 15, 23, 30, 37, 44, 51,
  58, 59, 52, 45, 38, 31, 39, 46,
  53, 60, 61, 54, 47, 55, 62, 63
};

#endif // GFXU_IMAGE_JPG_COMMON_H