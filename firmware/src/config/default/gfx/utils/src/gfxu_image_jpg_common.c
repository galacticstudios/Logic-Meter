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


#include "gfx/utils/inc/gfxu_image_jpg_common.h"

void jpeg_idct_islow (short *inbuf, uint16_t *quantptr);


static void JPEG_Seek(JPEGDECODER* decoder, long offset, int whence)
{
    if(whence == 0)
    {
        decoder->fileIndex = offset;        
    }
    else if(whence == 1)
    {
        decoder->fileIndex += offset;        
    }
}

/*static size_t JPEG_Read(void *ptr, size_t size, size_t n, JPEGDECODER* decoder)
{
    size_t count = 0;
    uint8_t* pFile;
    uint8_t* bptr;
    
    pFile = (uint8_t*)decoder->pImageFile->header.dataAddress;
    bptr = (uint8_t *)ptr;
    
    for(count = 0; count < size * n; ++count)
    {
        bptr[count] = (uint8_t) pFile[decoder->fileIndex];
        ++decoder->fileIndex;
    }        
    return count;
}*/


void JPEG_vResetDecoder(JPEGDECODER *decoder)
{
    uint16_t wIndex;
    uint8_t *pbptr = (uint8_t*)decoder;
    for(wIndex = 0; wIndex < sizeof(JPEGDECODER); wIndex++)
    {
        pbptr[wIndex] = 0;
    }
}

/*******************************************************************************
Function:       uint16_t JPEG_wReadWord(GFXU_ImageAsset *pfile)

Precondition:   None

Overview:       This function reads and returns a single word obtained as
                Higher byte first followed by lower byte

Input:          Image file

Output:         One word
*******************************************************************************/
static uint16_t JPEG_wReadWord(JPEGDECODER *decoder)
{
     uint8_t btemp;
     uint16_t wData;
     
     decoder->readPtr(&btemp, sizeof(btemp), 1, decoder);
     wData = btemp;
     decoder->readPtr(&btemp, sizeof(btemp), 1, decoder);
     wData = (wData << 8) | btemp;
     
     return wData;
}

/*******************************************************************************
Function:       uint8_t JPEG_bReadByte(GFXU_ImageAsset *pfile)

Precondition:   None

Overview:       This function reads and returns a single byte from the file

Input:          Image file

Output:         One byte
*******************************************************************************/
static uint8_t JPEG_bReadByte(JPEGDECODER *decoder)
{
     uint8_t bData;
     
     decoder->readPtr(&bData, sizeof(bData), 1, decoder);

     return bData;
}

/*******************************************************************************
Function:       uint8_t JPEG_bReadHeader(JPEGDECODER *pJpegDecoder)

Precondition:   None

Overview:       This function reads the JPEG file header and 
                fills the data structure

Input:          JPEGDECODER

Output:         Error code - '0' means no error
*******************************************************************************/
uint8_t JPEG_bReadHeader(JPEGDECODER *pJpegDecoder)
{
     GFX_Bool blSOSOver = 0;
     while(pJpegDecoder->fileIndex * sizeof(uint8_t)
             < pJpegDecoder->pImageFile->header.dataSize)
     {
             uint8_t btemp, bcount, bsection;
             uint16_t wSegLen, wOffset;
             
             if(blSOSOver == 1)
             {
                     if(pJpegDecoder->bChannels == 1)
                     {
                             pJpegDecoder->bBlocksInOnePass = 1;
                             pJpegDecoder->bSubSampleType = JPEG_SAMPLE_1x1;
                     }    
                     else if(pJpegDecoder->bChannels == 3)
                     {
                             if((pJpegDecoder->abChannelHSampFactor[0] == 1 && pJpegDecoder->abChannelVSampFactor[0] == 1) &&
                                (pJpegDecoder->abChannelHSampFactor[1] == 1 && pJpegDecoder->abChannelVSampFactor[1] == 1) &&
                                (pJpegDecoder->abChannelHSampFactor[2] == 1 && pJpegDecoder->abChannelVSampFactor[2] == 1))
                             {
                                     pJpegDecoder->bBlocksInOnePass = 3;
                                     pJpegDecoder->abChannelMap[0] = 0;
                                     pJpegDecoder->abChannelMap[1] = 1;
                                     pJpegDecoder->abChannelMap[2] = 2;
                                     pJpegDecoder->bSubSampleType = JPEG_SAMPLE_1x1;
                             }
                             else if((pJpegDecoder->abChannelHSampFactor[0] == 1 && pJpegDecoder->abChannelVSampFactor[0] == 2) &&
                                     (pJpegDecoder->abChannelHSampFactor[1] == 1 && pJpegDecoder->abChannelVSampFactor[1] == 1) &&
                                     (pJpegDecoder->abChannelHSampFactor[2] == 1 && pJpegDecoder->abChannelVSampFactor[2] == 1))
                             {
                                     pJpegDecoder->bBlocksInOnePass = 4;
                                     pJpegDecoder->abChannelMap[0] = 0;
                                     pJpegDecoder->abChannelMap[1] = 0;
                                     pJpegDecoder->abChannelMap[2] = 1;
                                     pJpegDecoder->abChannelMap[3] = 2;
                                     pJpegDecoder->bSubSampleType = JPEG_SAMPLE_1x2;
                             }
                             else if((pJpegDecoder->abChannelHSampFactor[0] == 2 && pJpegDecoder->abChannelVSampFactor[0] == 1) &&
                                     (pJpegDecoder->abChannelHSampFactor[1] == 1 && pJpegDecoder->abChannelVSampFactor[1] == 1) &&
                                     (pJpegDecoder->abChannelHSampFactor[2] == 1 && pJpegDecoder->abChannelVSampFactor[2] == 1))
                             {
                                     pJpegDecoder->bBlocksInOnePass = 4;
                                     pJpegDecoder->abChannelMap[0] = 0;
                                     pJpegDecoder->abChannelMap[1] = 0;
                                     pJpegDecoder->abChannelMap[2] = 1;
                                     pJpegDecoder->abChannelMap[3] = 2;
                                     pJpegDecoder->bSubSampleType = JPEG_SAMPLE_2x1;
                             }
                             else if((pJpegDecoder->abChannelHSampFactor[0] == 2 && pJpegDecoder->abChannelVSampFactor[0] == 2) &&
                                     (pJpegDecoder->abChannelHSampFactor[1] == 1 && pJpegDecoder->abChannelVSampFactor[1] == 1) &&
                                     (pJpegDecoder->abChannelHSampFactor[2] == 1 && pJpegDecoder->abChannelVSampFactor[2] == 1))
                             {
                                     pJpegDecoder->bBlocksInOnePass = 6;
                                     pJpegDecoder->abChannelMap[0] = 0;
                                     pJpegDecoder->abChannelMap[1] = 0;
                                     pJpegDecoder->abChannelMap[2] = 0;
                                     pJpegDecoder->abChannelMap[3] = 0;
                                     pJpegDecoder->abChannelMap[4] = 1;
                                     pJpegDecoder->abChannelMap[5] = 2;
                                     pJpegDecoder->bSubSampleType = JPEG_SAMPLE_2x2;
                             }
                             else
                             {
                                     JPEG_SendError(100);
                             }
                     }
                     return 0;
             }

             pJpegDecoder->readPtr(&btemp, sizeof(btemp), 1, pJpegDecoder);

             if(btemp != 0xFF)
             {
                     continue;
             }

             pJpegDecoder->readPtr(&bsection, sizeof(bsection), 1, pJpegDecoder);
             switch(bsection)
             {
               case SOI:
               case TEM:
               case EOI:
               case RST0:
               case RST1:
               case RST2:
               case RST3:
               case RST4:
               case RST5:
               case RST6:
               case RST7: break;

               case SOF0: /* Start of frame */
                          wSegLen = JPEG_wReadWord(pJpegDecoder);
                          if(wSegLen <= 8)
                          {
                                     JPEG_SendError(100);
                                     break;
                          }
                          pJpegDecoder->bDataBits = JPEG_bReadByte(pJpegDecoder);
                          pJpegDecoder->wHeight   = JPEG_wReadWord(pJpegDecoder);
                          pJpegDecoder->wWidth    = JPEG_wReadWord(pJpegDecoder);
                          pJpegDecoder->bChannels = JPEG_bReadByte(pJpegDecoder);
                          if(wSegLen < 8 + (pJpegDecoder->bChannels * 3))
                          {
                                     JPEG_SendError(100);
                                     break;
                          }
                          for(bcount = 0; bcount < pJpegDecoder->bChannels; bcount++)
                          {
                                     pJpegDecoder->abChannelType[bcount] = JPEG_bReadByte(pJpegDecoder);
                                     btemp = JPEG_bReadByte(pJpegDecoder);
                                     pJpegDecoder->abChannelHSampFactor[bcount] = btemp >> 4;
                                     pJpegDecoder->abChannelVSampFactor[bcount] = btemp & 0x0F;
                                     pJpegDecoder->abChannelQuantTableMap[bcount] = JPEG_bReadByte(pJpegDecoder);
                          }
                          break;

               case APP0: /* Start of Application */
                          wSegLen = JPEG_wReadWord(pJpegDecoder);
                          if(wSegLen < 16)
                          {
                                     JPEG_SendError(100);
                                     break;
                          }
                          else
                          {
                                     unsigned char buf[5];
                                     pJpegDecoder->readPtr(buf, sizeof(buf[0]), 5, pJpegDecoder);
                                     if(buf[0] == 'J' && buf[1] == 'F' && buf[2] == 'I' && buf[3] == 'F' && buf[4] == '\0')
                                     {
                                        pJpegDecoder->blJFIF = 1;
                                     }
                          }
                          pJpegDecoder->bMajorRev = JPEG_bReadByte(pJpegDecoder);
                          pJpegDecoder->bMinorRev = JPEG_bReadByte(pJpegDecoder);
                          if(pJpegDecoder->bMajorRev != 1)
                          {
                                     JPEG_SendError(100);
                                     break;
                          }
                          /* Ignore other bytes in this segment */
                          break;
                          
               case DRI: /* Start of Quantization table */
                    wSegLen = JPEG_wReadWord(pJpegDecoder);
                    if(wSegLen == 4)
                    {
                               pJpegDecoder->wRestartInterval = JPEG_wReadWord(pJpegDecoder);
                    }
                    break;
                    
               case DQT: /* Start of Quantization table */
                          wSegLen = JPEG_wReadWord(pJpegDecoder);
                          if(wSegLen < 67)
                          {
                                     JPEG_SendError(100);
                                     break;
                          }

                          do
                          {
                                     uint8_t bQTableIndex, bCounter;

                                     btemp = JPEG_bReadByte(pJpegDecoder);
                                     bQTableIndex = btemp & 0x0F;
                                     pJpegDecoder->blQuantUses16bits |= (btemp >> 4);
                                     
                                     for(bCounter = 0; bCounter < 64; bCounter++)
                                     {
                                               uint8_t bData1, bData2 = 0;
                                               if(pJpegDecoder->blQuantUses16bits == 0)
                                               {
                                                         pJpegDecoder->readPtr(&bData1, sizeof(uint8_t), 1, pJpegDecoder);
                                                         pJpegDecoder->awQuantTable[bQTableIndex][abZigzag[bCounter]] = bData1;
                                               }
                                               else
                                               {
                                                         pJpegDecoder->readPtr(&bData1, sizeof(uint8_t), 1, pJpegDecoder);
                                                         pJpegDecoder->readPtr(&bData2, sizeof(uint8_t), 1, pJpegDecoder);
                                                         pJpegDecoder->awQuantTable[bQTableIndex][abZigzag[bCounter]] = (((uint16_t)bData1) << 8) + (uint16_t)bData2;
                                               }
                                     }
                                     wSegLen -= (pJpegDecoder->blQuantUses16bits == 0)? 65: 129; /* Table length + information byte */
                          } while(wSegLen >= 67);
                          break;

               case DHT: /* Start of Huffmann table */
                          wSegLen = JPEG_wReadWord(pJpegDecoder);
                          if(wSegLen < 19)
                          {
                                     JPEG_SendError(100);
                                     break;
                          }

                          do
                          {
                                     uint8_t bHTableIndex, bCounter;
                                     uint16_t wNumOfSymbols;
                                     uint8_t blIsAc;
                                     uint8_t *pbLenTable, *pbSymTable;

                                     btemp = JPEG_bReadByte(pJpegDecoder);
                                     bHTableIndex = btemp & 0x0F;
                                     blIsAc = (btemp >> 4) & 0x01;
                                     
                                     if(blIsAc == 0)
                                     {
                                               pbLenTable = (uint8_t*)&pJpegDecoder->abHuffDcSymLen[bHTableIndex][0];
                                               pbSymTable = (uint8_t*)&pJpegDecoder->abHuffDcSymbol[bHTableIndex][0];
                                     }
                                     else
                                     {
                                               pbLenTable = (uint8_t*)&pJpegDecoder->abHuffAcSymLen[bHTableIndex][0];
                                               pbSymTable = (uint8_t*)&pJpegDecoder->abHuffAcSymbol[bHTableIndex][0];
                                     }

                                     pJpegDecoder->readPtr(pbLenTable, sizeof(uint8_t), 16, pJpegDecoder);

                                     for(bCounter = 0, wNumOfSymbols = 0; bCounter < 16; bCounter++)
                                     {
                                               wNumOfSymbols += pbLenTable[bCounter];
                                     }

                                     wSegLen -= 17; /* This table length + information byte */

                                     if(wSegLen < wNumOfSymbols || (blIsAc == 1 && wNumOfSymbols > 256) || (blIsAc == 0 && wNumOfSymbols > 16))
                                     {
                                               JPEG_SendError(100);
                                               break;
                                     }

                                     pJpegDecoder->readPtr(pbSymTable, sizeof(uint8_t), wNumOfSymbols, pJpegDecoder);
                                     wSegLen -= wNumOfSymbols; /* This table length + information byte */
                                     pJpegDecoder->bHuffTables++;
                          } while(wSegLen >= 19);
                          break;

               case SOS: /* Start of Scan parameters */
                          wSegLen = JPEG_wReadWord(pJpegDecoder);
                          if(wSegLen < 3)
                          {
                                     JPEG_SendError(100);
                                     break;
                          }

                          btemp = JPEG_bReadByte(pJpegDecoder);
                          wOffset = wSegLen - (3 + (btemp << 1));

                          if(pJpegDecoder->bChannels != btemp || wSegLen < 3 + (btemp << 1))
                          {
                                     JPEG_SendError(100);
                                     break;
                          }
                          else
                          {
                                     uint8_t bCounter, bChannelId = 0xFF;
                                     
                                     for(bCounter = 0; bCounter < pJpegDecoder->bChannels; bCounter++)
                                     {
                                               uint8_t bindex;

                                               btemp = JPEG_bReadByte(pJpegDecoder);
                                               for(bindex = 0; bindex < MAX_CHANNELS; bindex++)
                                               {
                                                          if(pJpegDecoder->abChannelType[bindex] == btemp)
                                                          {
                                                                    bChannelId = bindex;
                                                                    break;
                                                          }
                                               }

                                               if(bChannelId < MAX_CHANNELS)
                                               {
                                                          btemp = JPEG_bReadByte(pJpegDecoder);
                                                          pJpegDecoder->abChannelHuffAcTableMap[bChannelId] = btemp & 0x0F;
                                                          pJpegDecoder->abChannelHuffDcTableMap[bChannelId] = btemp >> 4;
                                               }
                                     }
                                     JPEG_Seek(pJpegDecoder, wOffset, 1);
                          }
                          blSOSOver = 1;
                          break;

               default: /* Any other segment with length */
                          wSegLen = JPEG_wReadWord(pJpegDecoder);
                          if(wSegLen < 2)
                          {
                                     JPEG_SendError(100);
                                     break;
                          }
                          JPEG_Seek(pJpegDecoder, wSegLen - 2, 1);
             }
     }
     return (pJpegDecoder->blJFIF == 1) ? 0 : 1;
}

/*******************************************************************************
Function:       uint8_t JPEG_bGenerateHuffmanTables(JPEGDECODER *pJpegDecoder)

Precondition:   None

Overview:       This function generated the table required for Huffman decoding
                from the data read from the header

Input:          JPEGDECODER

Output:         Error code - '0' means no error
*******************************************************************************/
uint8_t JPEG_bGenerateHuffmanTables(JPEGDECODER *pJpegDecoder)
{
     uint8_t bLength, bTable;

     for(bTable = 0; bTable < pJpegDecoder->bHuffTables / 2; bTable++)
     {
            pJpegDecoder->awHuffAcSymStart[bTable][0] = 0;
            pJpegDecoder->awHuffDcSymStart[bTable][0] = 0;

            for(bLength = 1; bLength < 16; bLength++)
            {
                   pJpegDecoder->awHuffAcSymStart[bTable][bLength] = (pJpegDecoder->awHuffAcSymStart[bTable][bLength - 1] + pJpegDecoder->abHuffAcSymLen[bTable][bLength - 1]) << 1;
                   pJpegDecoder->awHuffDcSymStart[bTable][bLength] = (pJpegDecoder->awHuffDcSymStart[bTable][bLength - 1] + pJpegDecoder->abHuffDcSymLen[bTable][bLength - 1]) << 1;
            }
     }
     return 0;
}

/*******************************************************************************
Function:       uint8_t JPEG_bGet1Bit(JPEGDECODER *pJpegDecoder)

Precondition:   None

Overview:       This function returns 1 bit as the lsb of the returned byte.
                It automatically fills the buffer if it becomes empty.
                It also converts 0xFF00 into 0.

Input:          JPEGDECODER

Output:         One bit
*******************************************************************************/
static uint8_t JPEG_bGet1Bit(JPEGDECODER *pJpegDecoder)
{
     uint8_t bBit = 0;

     if(pJpegDecoder->wBufferIndex >= pJpegDecoder->wBufferLen)
     {
            pJpegDecoder->wBufferLen = pJpegDecoder->readPtr(&pJpegDecoder->abDataBuffer[0], sizeof(uint8_t), MAX_DATA_BUF_LEN, pJpegDecoder);
            while(pJpegDecoder->abDataBuffer[pJpegDecoder->wBufferLen - 1] == 0xFF)
            {
                   pJpegDecoder->wBufferLen--;
                   JPEG_Seek(pJpegDecoder, -1, 1);
            }
            pJpegDecoder->wBufferIndex = 0;
     }

     while(pJpegDecoder->bBitsAvailable == 0) /* Be very careful to touch this part of code! You must know exactly what you are doing */
     {
            pJpegDecoder->bBitsAvailable = 16;
            pJpegDecoder->wWorkBits = pJpegDecoder->abDataBuffer[pJpegDecoder->wBufferIndex++];
            pJpegDecoder->wWorkBits = (pJpegDecoder->wWorkBits << 8) + pJpegDecoder->abDataBuffer[pJpegDecoder->wBufferIndex++];
            if(pJpegDecoder->wBufferIndex > pJpegDecoder->wBufferLen) /* wBufferIndex need not be even because of the below condition */
            {
                   pJpegDecoder->bBitsAvailable = 8;
            }
            else if((pJpegDecoder->wWorkBits & 0x00FF) == 0x00FF)
            {
                   pJpegDecoder->bBitsAvailable = 8;
                   pJpegDecoder->wBufferIndex--;
            }
            else if(pJpegDecoder->wWorkBits >= 0xFF00)
            {
                   if(pJpegDecoder->wWorkBits == 0xFF00)
                   {
                         pJpegDecoder->bBitsAvailable = 8;
                   }
            }
     }

     bBit = (pJpegDecoder->wWorkBits & 0x8000)? 0x01: 0;

     pJpegDecoder->wWorkBits <<= 1;
     pJpegDecoder->bBitsAvailable--;

     return bBit;
}

/*******************************************************************************
Function:       uint16_t JPEG_wGetBits(JPEGDECODER *pJpegDecoder, uint8_t bLen)

Precondition:   None

Overview:       This function returns bLen number of bits as the lsb of the
                returned word and it automatically fills the buffer
                if it becomes empty.

Input:          JPEGDECODER, Number of bits

Output:         Requested number of bits
*******************************************************************************/
static uint16_t JPEG_wGetBits(JPEGDECODER *pJpegDecoder, uint8_t bLen)
{
     uint16_t wVal = 0;

     wVal = pJpegDecoder->bFirstBit = JPEG_bGet1Bit(pJpegDecoder);
     bLen--;

     while(bLen)
     {
            wVal = (wVal << 1) + JPEG_bGet1Bit(pJpegDecoder);
            bLen--;
     }

     return wVal;   
}

/*******************************************************************************
Function:       uint16_t JPEG_wGetRestartWord(JPEGDECODER *pJpegDecoder)

Precondition:   File pointer must point to the restart word

Overview:       Returns the restart word

Input:          JPEGDECODER

Output:         Restart word
*******************************************************************************/
static uint16_t JPEG_wGetRestartWord(JPEGDECODER *pJpegDecoder)
{
     uint16_t wRestartWord;
     while((pJpegDecoder->bBitsAvailable & 0x07) != 0) /* This is to clearoff wnwanted bits to go to fresh byte */
     {
            JPEG_bGet1Bit(pJpegDecoder);
     }
     wRestartWord = JPEG_wGetBits(pJpegDecoder, 16);
     return(wRestartWord);
}

/*******************************************************************************
Function:       int16_t JPEG_sGetBitsValue(JPEGDECODER *pJpegDecoder, uint8_t bLen)

Precondition:   None

Overview:       Returns the signed value of the bLen bits of data

Input:          JPEGDECODER, Number of bits

Output:         Signed number
*******************************************************************************/
static int16_t JPEG_sGetBitsValue(JPEGDECODER *pJpegDecoder, uint8_t bLen)
{
     uint16_t wVal = 0;

     if(bLen != 0)
     {  
            wVal = JPEG_wGetBits(pJpegDecoder, bLen);
            if(pJpegDecoder->bFirstBit == 0)
            {
                   wVal = ~(wVal | (0xFFFF << bLen));
                   return ((int16_t)wVal * -1);
            }
     }
     return (int16_t)wVal;
}

/*******************************************************************************
Function:       uint8_t JPEG_bGetNextHuffByte(JPEGDECODER *pJpegDecoder)

Precondition:   File pointer must point to the Huffman data

Overview:       Returns the Huffman decoded data

Input:          JPEGDECODER

Output:         Huffman decoded data
*******************************************************************************/
static uint8_t JPEG_bGetNextHuffByte(JPEGDECODER *pJpegDecoder)
{
     uint8_t bBits, bSymbol = 0;
     uint16_t wBitPattern = 0, wSymbolOffset = 0;

     for(bBits = 0; bBits < 16; bBits++)
     {
            uint8_t bSymbols;
            uint16_t wDiff;

            wBitPattern = (wBitPattern << 1) + JPEG_bGet1Bit(pJpegDecoder);
            bSymbols = pJpegDecoder->pbCurrentHuffSymLenTable[bBits];
            if(bSymbols == 0)
            {
                   continue;
            }

            wDiff = wBitPattern - pJpegDecoder->pwCurrentHuffSymStartTable[bBits];
            if(wDiff < bSymbols)
            {
                   bSymbol = pJpegDecoder->pbCurrentHuffSymbolTable[wSymbolOffset + wDiff];
                   break;
            }
            wSymbolOffset += bSymbols;
     }
     return bSymbol;
}

#define range_limit(x) (x<0)?0:(x>0xFF)?0xFF:x
/*******************************************************************************
Function:       uint8_t JPEG_bDecodeOneBlock(JPEGDECODER *pJpegDecoder)

Precondition:   File pointer must point to a new block of data

Overview:       Decodes the 8x8 pixel values of all the channels
                (A multiple of 8x8 block if subsampling is used)

Input:          JPEGDECODER

Output:         Error code - '0' means no error
*******************************************************************************/
uint8_t JPEG_bDecodeOneBlock(JPEGDECODER *pJpegDecoder)
{
     uint8_t bBlock, bCounter;


     for(bBlock = 0; bBlock < pJpegDecoder->bBlocksInOnePass; bBlock++)
     {
            uint8_t bByteCount, bHuffbyte;

            if((pJpegDecoder->wRestartInterval > 0) && (pJpegDecoder->wBlockNumber == pJpegDecoder->wRestartInterval * pJpegDecoder->bBlocksInOnePass))
            {
                   uint16_t wRestartWord = JPEG_wGetRestartWord(pJpegDecoder);
                   if(wRestartWord < 0xFFD0 || wRestartWord > 0xFFD7)
                   {
                            JPEG_SendError(100);
                   }                          
                   for(bCounter = 0; bCounter < MAX_CHANNELS; bCounter++)
                   {
                            pJpegDecoder->asPrevDcValue[bCounter] = 0;
                   }                   
                   pJpegDecoder->wBlockNumber = 0;
            }

            for(bCounter = 0; bCounter < 64; bCounter++)
            {
                   pJpegDecoder->asOneBlock[bBlock][bCounter] = 0;
            }

            pJpegDecoder->pwCurrentQuantTable = &pJpegDecoder->awQuantTable[pJpegDecoder->abChannelQuantTableMap[pJpegDecoder->abChannelMap[bBlock]]][0];

            /* Decode DC value */
            bByteCount = 0;
            pJpegDecoder->pbCurrentHuffSymLenTable = &pJpegDecoder->abHuffDcSymLen[pJpegDecoder->abChannelHuffDcTableMap[pJpegDecoder->abChannelMap[bBlock]]][0];
            pJpegDecoder->pbCurrentHuffSymbolTable = &pJpegDecoder->abHuffDcSymbol[pJpegDecoder->abChannelHuffDcTableMap[pJpegDecoder->abChannelMap[bBlock]]][0];
            pJpegDecoder->pwCurrentHuffSymStartTable = &pJpegDecoder->awHuffDcSymStart[pJpegDecoder->abChannelHuffDcTableMap[pJpegDecoder->abChannelMap[bBlock]]][0];
            bHuffbyte = JPEG_bGetNextHuffByte(pJpegDecoder);
            pJpegDecoder->asOneBlock[bBlock][0] = JPEG_sGetBitsValue(pJpegDecoder, bHuffbyte & 0x0F) + pJpegDecoder->asPrevDcValue[pJpegDecoder->abChannelMap[bBlock]];
            pJpegDecoder->asPrevDcValue[pJpegDecoder->abChannelMap[bBlock]] = pJpegDecoder->asOneBlock[bBlock][0];

            /* Decode AC value */
            bByteCount = 1;
            pJpegDecoder->pbCurrentHuffSymLenTable = &pJpegDecoder->abHuffAcSymLen[pJpegDecoder->abChannelHuffAcTableMap[pJpegDecoder->abChannelMap[bBlock]]][0];
            pJpegDecoder->pbCurrentHuffSymbolTable = &pJpegDecoder->abHuffAcSymbol[pJpegDecoder->abChannelHuffAcTableMap[pJpegDecoder->abChannelMap[bBlock]]][0];
            pJpegDecoder->pwCurrentHuffSymStartTable = &pJpegDecoder->awHuffAcSymStart[pJpegDecoder->abChannelHuffAcTableMap[pJpegDecoder->abChannelMap[bBlock]]][0];
            while(bByteCount < 64)
            {
                   bHuffbyte = JPEG_bGetNextHuffByte(pJpegDecoder);
                   bByteCount += (bHuffbyte >> 4);
                   if(bHuffbyte == 0 /* EOB */)
                   {
                          break;
                   }
                   if(bByteCount > 63)
                   {
                          JPEG_SendError(100);
                   }
                   pJpegDecoder->asOneBlock[bBlock][abZigzag[bByteCount++]] = JPEG_sGetBitsValue(pJpegDecoder, bHuffbyte & 0x0F);
            }
            pJpegDecoder->wBlockNumber++;
            jpeg_idct_islow(&pJpegDecoder->asOneBlock[bBlock][0],pJpegDecoder->pwCurrentQuantTable);
     }

     return 0;
}

#define JPEG_WRITE_TO_DISPLAY
/*******************************************************************************
Function:       void JPEG_vInitDisplay(JPEGDECODER *pJpegDecoder)

Precondition:   None

Overview:       Initializes the (x, y) co-ordinates to (0, 0)

Input:          JPEGDECODER

Output:         None
*******************************************************************************/
void JPEG_vInitDisplay(JPEGDECODER *pJpegDecoder)
{
    pJpegDecoder->wPrevX = 0;
    pJpegDecoder->wPrevY = 0;
}

/*******************************************************************************
Function:       uint8_t JPEG_bPaintOneBlock(JPEGDECODER *pJpegDecoder)

Precondition:   One block - 8x8 pixel data of all channels must be decoded

Overview:       Displays one 8x8 on the screen
                (A multiple of 8x8 block if subsampling is used)

Input:          JPEGDECODER

Output:         Error code - '0' means no error
*******************************************************************************/
uint8_t JPEG_bPaintOneBlock(JPEGDECODER *pJpegDecoder)
{
    static uint16_t wX, wY, wX2, psCxIndex;
    static uint8_t r,g,b;
    static short *psY,*psCb,*psCr;
    static int32_t s1,s2,s3;
    static uint8_t bBlock, bOffsetY[2] = {0,8}, bOffsetX[4] = {0,8,0,8}, bCbCrOffset[4] = {0,4,32,36};
    static uint16_t wXPos, wYPos,wYPos2;
    static uint8_t shiftX, shiftY;
    static GFX_Point pnt;
    static GFX_Color color;
    uint8_t *pixels;

    if(pJpegDecoder->bSubSampleType == JPEG_SAMPLE_1x1)
    {
        shiftX = 8; 
        shiftY = 8;

        GFX_PixelBufferCreate(shiftX,
                              shiftY,
                              GFX_COLOR_MODE_RGB_888,
                              &pixels,
                              &pJpegDecoder->pixelBlockBuffer);
        
        pJpegDecoder->pixelBlockBuffer.pixels = pJpegDecoder->memIntf->heap.malloc(pJpegDecoder->pixelBlockBuffer.buffer_length);
        
        psY = &pJpegDecoder->asOneBlock[0][0]; 
        psCb = &pJpegDecoder->asOneBlock[1][0]; 
        psCr = &pJpegDecoder->asOneBlock[2][0];

        for(wY = 0; wY < 8; wY++)
        {
            for(wX = 0; wX < 8; wX++)
            {
                s1 = ((*psY) + 128)*128, s2 = (*psCb), s3 = (*psCr);
                r = range_limit((s1 + 180*s3)>>7);
                g = range_limit((s1 - 44*s2 - 91*s3)>>7);
                b = range_limit((s1 + 227*s2)>>7);
                
//              IMG_vSetColor(r, g, b);
//              IMG_vPixelArray(wX + (wY<<3));
//                pixel[wX + (wY<<3)] = GFX_RGBConvert(r, g, b);
                
                color = r << 16 | g << 8 | b;

                pnt.x = wX;
                pnt.y = wY; //(wY << 3);
                
                GFX_PixelBufferSet_Unsafe(&pJpegDecoder->pixelBlockBuffer,
                                          &pnt,
                                          color);
                
                psY++; psCb++; psCr++;
            }  
         }
     }       
     else if(pJpegDecoder->bSubSampleType == JPEG_SAMPLE_1x2)
     {
        shiftX = 8; 
        shiftY = 16;

        GFX_PixelBufferCreate(shiftX,
                              shiftY,
                              GFX_COLOR_MODE_RGB_888,
                              &pixels,
                              &pJpegDecoder->pixelBlockBuffer);
        
        pJpegDecoder->pixelBlockBuffer.pixels = pJpegDecoder->memIntf->heap.malloc(pJpegDecoder->pixelBlockBuffer.buffer_length);

        for(bBlock = 0; bBlock < 2; bBlock++)
        {
            psY = &pJpegDecoder->asOneBlock[bBlock][0];
            psCb = &pJpegDecoder->asOneBlock[2][bBlock<<5];
            psCr = &pJpegDecoder->asOneBlock[3][bBlock<<5];

            for(wY = 0; wY < 8; wY++)
            {
                for(wX = 0; wX < 8; wX++)
                {
                    s1 = ((*psY) + 128)*128;
                    s2 = psCb[(wY>>1)*8+wX];
                    s3 = psCr[(wY>>1)*8+wX];
                    r = range_limit((s1 + 180*s3)>>7);
                    g = range_limit((s1 - 44*s2 - 91*s3)>>7);
                    b = range_limit((s1 + 227*s2)>>7);                    
                    //pixel[wX + ((bOffsetY[bBlock] + wY)<<3)] = GFX_RGBConvert(r, g, b);
                    
                    color = r << 16 | g << 8 | b;

                    pnt.x = wX;
                    pnt.y = ((bOffsetY[bBlock] + wY) << 3);

                    GFX_PixelBufferSet_Unsafe(&pJpegDecoder->pixelBlockBuffer,
                                              &pnt,
                                              color);

                    psY++;
                }
            }
        }
     }       
     else if(pJpegDecoder->bSubSampleType == JPEG_SAMPLE_2x1)
     {
        shiftX = 16; 
        shiftY = 8;

        GFX_PixelBufferCreate(shiftX,
                              shiftY,
                              GFX_COLOR_MODE_RGB_888,
                              &pixels,
                              &pJpegDecoder->pixelBlockBuffer);
        
        pJpegDecoder->pixelBlockBuffer.pixels = pJpegDecoder->memIntf->heap.malloc(pJpegDecoder->pixelBlockBuffer.buffer_length);

        for(bBlock = 0; bBlock < 2; bBlock++)
        {
            psY = &pJpegDecoder->asOneBlock[bBlock][0];
            psCb = &pJpegDecoder->asOneBlock[2][bBlock<<2];
            psCr = &pJpegDecoder->asOneBlock[3][bBlock<<2];

            for(wY = 0; wY < 8; wY++)
            {
                for(wX = 0; wX < 8; wX++)
                {
                    s1 = ((*psY) + 128)*128;
                    s2 = psCb[(wY*8)+(wX>>1)];
                    s3 = psCr[(wY*8)+(wX>>1)];
                    r = range_limit((s1 + 180*s3)>>7);
                    g = range_limit((s1 - 44*s2 - 91*s3)>>7);
                    b = range_limit((s1 + 227*s2)>>7);
//                    pixel[bOffsetX[bBlock] + wX + (wY<<4)] = GFX_RGBConvert(r, g, b);
                    
                    color = r << 16 | g << 8 | b;

                    pnt.x = bOffsetX[bBlock] + wX;
                    pnt.y = (wY << 4);

                    GFX_PixelBufferSet_Unsafe(&pJpegDecoder->pixelBlockBuffer,
                                              &pnt,
                                              color);

                    psY++;
                }
            }
        }
     }
     else if(pJpegDecoder->bSubSampleType == JPEG_SAMPLE_2x2)
     {
        shiftX = 16; 
        shiftY = 16;

        GFX_PixelBufferCreate(shiftX,
                              shiftY,
                              GFX_COLOR_MODE_RGB_888,
                              &pixels,
                              &pJpegDecoder->pixelBlockBuffer);
        
        pJpegDecoder->pixelBlockBuffer.pixels = pJpegDecoder->memIntf->heap.malloc(pJpegDecoder->pixelBlockBuffer.buffer_length);

        for(bBlock = 0; bBlock < 4; bBlock++)
        {
            wXPos = bOffsetX[bBlock];
            wYPos = bOffsetY[bBlock>>1];

            psY = &pJpegDecoder->asOneBlock[bBlock][0];
            psCb = &pJpegDecoder->asOneBlock[4][bCbCrOffset[bBlock]];
            psCr = &pJpegDecoder->asOneBlock[5][bCbCrOffset[bBlock]];

            for(wY = 0; wY < 8; wY++)
            {
                wYPos2 = wYPos + wY;

                for(wX = 0; wX < 4; wX++)
                {
                    psCxIndex = (wY>>1)*8 + wX;
                    s2 = psCb[psCxIndex];
                        s3 = psCr[psCxIndex];

                    for(wX2 = 0; wX2 < 2; wX2++)
                    {
                        s1 = ((*psY++) + 128)*128;
                        r = range_limit((s1 + 180*s3)>>7);
                        g = range_limit((s1 - 44*s2 - 91*s3)>>7);
                        b = range_limit((s1 + 227*s2)>>7);
                        //pixel[wXPos + (wX<<1) + wX2 + (wYPos2<<4)] = GFX_RGBConvert(r, g, b);

                        color = r << 16 | g << 8 | b;

                        pnt.x = wXPos + (wX<<1) + wX2;
                        pnt.y = wYPos2;

                        GFX_PixelBufferSet_Unsafe(&pJpegDecoder->pixelBlockBuffer,
                                                  &pnt,
                                                  color);
                    }    
                }
            }
        }
    }
    
    GFX_Set(GFXF_DRAW_PIPELINE_MODE, GFX_PIPELINE_GCU);
    GFX_DrawBlit(&pJpegDecoder->pixelBlockBuffer,
        0, 0, shiftX, shiftY, pJpegDecoder->wPrevX + pJpegDecoder->wDrawX, pJpegDecoder->wPrevY + pJpegDecoder->wDrawY);
    GFX_Set(GFXF_DRAW_PIPELINE_MODE, GFX_PIPELINE_GCUGPU);
    
    pJpegDecoder->wPrevX += shiftX;

    if (pJpegDecoder->wPrevX >= pJpegDecoder->wWidth + pJpegDecoder->wStartX)
    {
        pJpegDecoder->wPrevX = pJpegDecoder->wStartX;
        pJpegDecoder->wPrevY += shiftY;
    }
    
    pJpegDecoder->memIntf->heap.free(pJpegDecoder->pixelBlockBuffer.pixels);
    
    return 0;
}

int8_t JPEG_bInterpretResourceHeader(GFXU_ImageAsset* img)
{
    JPEGDECODER JPEG_JpegDecoder;
    
    if (img == NULL)
        return -1;

    JPEG_vResetDecoder(&JPEG_JpegDecoder);
    JPEG_JpegDecoder.pImageFile = img;

    if(JPEG_bReadHeader(&JPEG_JpegDecoder) != 0)
    {
        return(1);
    }

    return 0;
}
