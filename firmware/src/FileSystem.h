/* 
 * File:   FileSystem.h
 * Author: Bob
 *
 * Created on October 7, 2020, 10:46 AM
 */

#ifndef FILESYSTEM_H
#define	FILESYSTEM_H

class MountDrive
{
public:
    MountDrive();
    ~MountDrive();
    
    const char *Name() {return "/mnt/filesystem";}
    
private:
    static int _mountCount;
};

#endif	/* FILESYSTEM_H */

