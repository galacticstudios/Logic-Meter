/* 
 * File:   ToolUtility.cpp
 * Author: Bob
 * 
 * Created on September 23, 2020, 1:22 PM
 */

extern "C" 
{
#include "configuration.h"
#include "definitions.h"
}
#include "Menu.h"
#include "ToolUtility.h"
#include "UtilityPane.h"
#include "Console.h"

extern "C" int DumpDisk(int start, size_t maxBytes, bool showAscii, bool showHex);
extern "C" int DiskNonZeroSize();
extern const DRV_USBHS_INIT drvUSBInit;

static const Help help(NULL, NULL, NULL, 
        "Utility functions.");

static const MenuItem menuItems[5] = {
    MenuItem(), 
    MenuItem(), 
    MenuItem(),
    MenuItem(),
#ifdef __DEBUG
    MenuItem("DskDmp", MenuType::NoChange, NULL, CB(&ToolUtility::DumpDisk)), 
#else
    MenuItem(), 
#endif    
};

static const Menu menu(menuItems);

ToolUtility::ToolUtility() :
    Tool("Utilities", new UtilityPane, menu, help), _diskOffset(-1)
{
	 /* Initialize the USB device layer */
    sysObj.usbDevObject0 = USB_DEVICE_Initialize (USB_DEVICE_INDEX_0 , ( SYS_MODULE_INIT* ) & usbDevInitData);
	
	

	/* Initialize USB Driver */ 
    sysObj.drvUSBHSObject = DRV_USBHS_Initialize(DRV_USBHS_INDEX_0, (SYS_MODULE_INIT *) &drvUSBInit);	
}


ToolUtility::~ToolUtility() 
{
    // Harmony doesn't really deinitialize USB. So for now, we just do a reset (!!)
    SYS_INT_Disable();
    /* perform a system unlock sequence ,starting critical sequence*/
    SYSKEY = 0x00000000; //write invalid key to force lock
    SYSKEY = 0xAA996655; //write key1 to SYSKEY
    SYSKEY = 0x556699AA; //write key2 to SYSKEY
    /* set SWRST bit to arm reset */
    RSWRSTSET = 1;
    /* read RSWRST register to trigger reset */
    unsigned int dummy;
    dummy = RSWRST;
    /* prevent any unwanted code execution until reset occurs*/
    while(1);
}

void ToolUtility::DumpDisk()
{
    if (_diskOffset == -1)
    {
        _diskOffset = 0;
        _diskSize = (DiskNonZeroSize() + 15) & ~0xf;
    }
}

void ToolUtility::OnIdle()
{
    if (_diskOffset != -1 && _diskOffset < _diskSize && CONSOLE_WriteBufferEmpty())
    {
        if ((_diskOffset & 511) == 0)
        {
            printf("\r\n/* Sector %d */\r\n", (_diskOffset / 512));
        }
        _diskOffset = ::DumpDisk(_diskOffset, 16, false, true);
    }
}

