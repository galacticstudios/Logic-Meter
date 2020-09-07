/* 
 * File:   Settings.cpp
 * Author: Bob
 * 
 * Created on July 27, 2020, 6:16 PM
 */

extern "C"
{
#include "definitions.h"
}
#include "Settings.h"

Settings settings;

static volatile bool flashBlock[NVM_FLASH_PAGESIZE / sizeof(bool)] 
    __attribute__((aligned(NVM_FLASH_PAGESIZE), space(prog)));

static bool isDirty = false;
static uint32_t dirtyTime;

// Returns the last Settings struct in flash memory that has valid set. If
// there are only invalid settings, returns -1
static int GetLatestFlashSettingsIndex()
{
    const Settings *s = (const Settings *) flashBlock;
    size_t settingsCount = sizeof(flashBlock) / sizeof(Settings);
    size_t i;
    for (i = 0; i < settingsCount && s[i].valid; ++i)
    {
    }
    return int(i) - 1;
}

void SettingsInitialize()
{
    int i = GetLatestFlashSettingsIndex();
    if (i >= 0)
        settings = *((Settings *) KVA0_TO_KVA1(flashBlock) + i);
}

void SettingsOnIdle()
{
    if (isDirty && SYS_TIME_CountToMS(SYS_TIME_CounterGet() - dirtyTime) > SettingsWriteDelayMS)
    {
        isDirty = false;
        
        // Find the next available spot to write the settings
        int validSettingsIndex = GetLatestFlashSettingsIndex();
        // If validSettingsIndex == -1, there are no valid settings in flash. That means it's a brand
        // new burn of flash memory. We need to do a Page Erase before writing settings.
        // If validSettingsIndex is the last settings block that fits in flash, then we also need to do
        // Page Erase(s) and start writing settings at the beginning of the block.
        if (validSettingsIndex == -1 || validSettingsIndex >= sizeof(flashBlock) / sizeof(Settings) - 1)
        {
            uint32_t page = uint32_t(flashBlock);
            for (int i = 0; i < sizeof(flashBlock) / NVM_FLASH_PAGESIZE; ++i, page += NVM_FLASH_PAGESIZE)
            {
                while (NVM_IsBusy()) {}
                NVM_PageErase(uint32_t(page));
            }
            validSettingsIndex = -1;
        }
        
        //  Now write the settings
        uint32_t dest = uint32_t((Settings *) flashBlock + validSettingsIndex + 1);
        for (int i = 0; i < sizeof(Settings) / sizeof(uint32_t); ++i, dest += sizeof(uint32_t))
        {
            while (NVM_IsBusy()) {}
            NVM_WordWrite(*((uint32_t *) &settings + i), dest);
        }
    }
}

void SettingsModified()
{
    dirtyTime = SYS_TIME_CounterGet();
    isDirty = true;
}
