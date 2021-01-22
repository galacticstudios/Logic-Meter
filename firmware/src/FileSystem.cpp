/* 
 * File:   FileSystem.cpp
 * Author: Bob
 * 
 * Created on October 7, 2020, 10:46 AM
 */

extern "C" 
{
#include "definitions.h"
}
#include "Utility.h"
#include "FileSystem.h"

int MountDrive::_mountCount;

MountDrive::MountDrive()
{
    DisableInterrupts di;
    if (_mountCount == 0)
        SYS_FS_Mount("/dev/nvma1", Name(), FAT, 0, NULL);
    ++_mountCount;
}

MountDrive::~MountDrive()
{
    DisableInterrupts di;
    if (_mountCount == 1)
        SYS_FS_Unmount(Name());
    --_mountCount;
}
