    
#include "console.h"
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define countof(x) (sizeof(x) / sizeof((x)[0]))
bool ctrlC;

//static int mountCount = 0;

//SYS_FS_HANDLE fileDescriptorToHandle[] = {SYS_FS_HANDLE_INVALID, SYS_FS_HANDLE_INVALID, SYS_FS_HANDLE_INVALID, SYS_FS_HANDLE_INVALID, SYS_FS_HANDLE_INVALID,};
// Reserve some file handles for stdin, stdout, stderr, and a few more just in case
#define RESERVED_FILE_HANDLES 10

int _mon_getc(int canblock)
{
    int c;
    do 
    {
        if (ctrlC)
            c = '\x3';
        else
            c = CONSOLE_Read();
    } while (c == -1 && canblock);
    return c;
}

void _mon_putc(char c)
{
    do
    {
    } while (CONSOLE_Write(c) == 0);
}

void _putchar(char character)
{
    _mon_putc(character);
}

void _mon_write(const char *s, unsigned int count)
{
    while (count--)
        CONSOLE_Write(*s++);
}

void _mon_puts(const char * s)
{
    while (*s)
        CONSOLE_Write(*s++);
}

/*static int ToErrno(SYS_FS_ERROR err)
{
    switch (err)
    {
        case SYS_FS_ERROR_OK : return 0;
        case SYS_FS_ERROR_NO_FILE :
        case SYS_FS_ERROR_NO_PATH : 
        case SYS_FS_ERROR_INVALID_NAME  : return ENOENT;
        case SYS_FS_ERROR_INVALID_OBJECT : return EBADF;
        case SYS_FS_ERROR_TOO_MANY_OPEN_FILES : return EMFILE;
        case SYS_FS_ERROR_DENIED : 
        case SYS_FS_ERROR_WRITE_PROTECTED : 
        case SYS_FS_ERROR_LOCKED : return EACCES;
        case SYS_FS_ERROR_INVALID_PARAMETER : return EINVAL;
        
        default :
            return EIO;
    }
}

void MountDrive()
{
    SYS_INT_PROCESSOR_STATUS status = SYS_INT_StatusGetAndDisable();
    if (mountCount == 0)
        SYS_FS_Mount(SYS_FS_MEDIA_IDX0_DEVICE_NAME_VOLUME_IDX0, SYS_FS_MEDIA_IDX0_MOUNT_NAME_VOLUME_IDX0, FAT, 0, NULL);
    ++mountCount;
    SYS_INT_StatusRestore(status);
}

void UnmountDrive()
{
    SYS_INT_PROCESSOR_STATUS status = SYS_INT_StatusGetAndDisable();
    if (mountCount == 1)
        SYS_FS_Unmount(SYS_FS_MEDIA_IDX0_MOUNT_NAME_VOLUME_IDX0);
    --mountCount;
    SYS_INT_StatusRestore(status);
}

int open(const char *filename, int access, ...)
{
    SYS_FS_HANDLE file;
    SYS_FS_FILE_OPEN_ATTRIBUTES attr = 0;
    bool create = (access & O_CREAT) == O_CREAT;
    bool append = (access & O_APPEND) == O_APPEND;
    int fd;
    
    // Find an available file handle slot
    for (fd = 0; fd < countof(fileDescriptorToHandle); ++fd)
    {
        if (fileDescriptorToHandle[fd] == SYS_FS_HANDLE_INVALID)
            break;
    }
    if (fd == countof(fileDescriptorToHandle))
    {
        errno = EMFILE;
        return -1;
    }
    
    switch (access & O_ACCMODE)
    {
        case O_RDONLY :
            attr = SYS_FS_FILE_OPEN_READ;
            break;
        
        case O_WRONLY :
            if (create)
                attr = append ? SYS_FS_FILE_OPEN_APPEND : SYS_FS_FILE_OPEN_WRITE;
            else
                attr = append ? SYS_FS_FILE_OPEN_APPEND : SYS_FS_FILE_OPEN_READ_PLUS;
            break;
            
        case O_RDWR :
            if (create)
                attr = append ? SYS_FS_FILE_OPEN_APPEND_PLUS : SYS_FS_FILE_OPEN_WRITE_PLUS;
            else
                attr = append ? SYS_FS_FILE_OPEN_APPEND_PLUS : SYS_FS_FILE_OPEN_READ_PLUS;
            break;
    }
    MountDrive();
    file = SYS_FS_FileOpen(filename, attr);
    if (file == SYS_FS_HANDLE_INVALID)
    {
        UnmountDrive();
        errno = ToErrno(SYS_FS_Error());
        return -1;
    }
    fileDescriptorToHandle[fd] = file;
    return fd + RESERVED_FILE_HANDLES;
}

int close(int fd)
{
    int rtn = ToErrno(SYS_FS_FileClose(fileDescriptorToHandle[fd - RESERVED_FILE_HANDLES]));
    if (rtn == 0)
        fileDescriptorToHandle[fd - RESERVED_FILE_HANDLES] = SYS_FS_HANDLE_INVALID;
    UnmountDrive();
    return rtn;
}

long lseek(int handle, long offset, int whence)
{
    int32_t bytesMoved, pos;
    SYS_FS_HANDLE fsh;
    SYS_FS_FILE_SEEK_CONTROL anchor;
    
    switch (handle)
    {
        case 1 :
            errno = EBADF;
            return -1;
            break;
            
        default :
            fsh = fileDescriptorToHandle[handle - RESERVED_FILE_HANDLES];
            if (whence == SEEK_SET)
                anchor = SYS_FS_SEEK_SET;
            else if (whence == SEEK_CUR)
                anchor = SYS_FS_SEEK_CUR;
            else if (whence == SEEK_END)
                anchor = SYS_FS_SEEK_END;
            else
            {
                errno = EINVAL;
                return -1;
            }
            bytesMoved = SYS_FS_FileSeek(fsh, (int32_t) offset, anchor);
            if (bytesMoved == -1)
            {
                errno = ToErrno(SYS_FS_Error());
                return -1;
            }
            pos = SYS_FS_FileTell(fsh);
            if (pos == -1)
            {
                errno = ToErrno(SYS_FS_Error());
                return -1;
            }
            return pos;
    }
    
}

off_t lseekforgzip(int handle, off_t offset, int whence)
{
    return (off_t) lseek(handle, (long) offset, whence);
}*/

int write(int handle, void *buffer, size_t nbyte)
{
    const char *cbuffer = (char *) buffer;
    int written = 0;
    /* Do not try to write from a NULL buffer */
    if (buffer == NULL)
        return 0;
    
    switch (handle)
    {
        case 1 :
            while (nbyte--)
            {
                CONSOLE_Write(*cbuffer++);
                ++written;
            }
            break;
            
        default :
            //written = (int) SYS_FS_FileWrite(fileDescriptorToHandle[handle - RESERVED_FILE_HANDLES], buffer, nbyte);
            //errno = ToErrno(SYS_FS_Error());
            break;
    }
    
    return written;
}

int read(int handle, void *buffer, size_t len)
{
    int i;

    if (len == 0)
        return 0;
    
    switch (handle)
    {
        case 0:
            *(char*) buffer = _mon_getc(1);
            i = 1;
            break;

        default:
            //i = SYS_FS_FileRead(fileDescriptorToHandle[handle - RESERVED_FILE_HANDLES], buffer, len);
            //errno = ToErrno(SYS_FS_Error());
            break;
    }
    
    return i;
}

/*
int link(const char *oldpath, const char *newpath)
{
    int rtn;
    MountDrive();
    rtn = SYS_FS_FileDirectoryRenameMove(oldpath, newpath) == SYS_FS_RES_SUCCESS ? 0 : -1;
    UnmountDrive();
    return rtn;
}

int unlink(const char *name)
{
    int rtn;
    MountDrive();
    rtn = SYS_FS_FileDirectoryRemove(name) == SYS_FS_RES_SUCCESS ? 0 : -1;
    UnmountDrive();
    return rtn;
}

typedef struct Dir
{
    SYS_FS_HANDLE dir;
    struct dirent de;
    SYS_FS_FSTAT stat;
    char lfname[FAT_FS_MAX_LFN];
} Dir_t;

Dir_t *opendir(const char *path)
{
    SYS_FS_HANDLE h;
    Dir_t *dir;
    
    MountDrive();
    h  = SYS_FS_DirOpen(path);
    if (h == SYS_FS_HANDLE_INVALID)
    {
        errno = ToErrno(SYS_FS_Error());
        return 0;
    }
    dir = malloc(sizeof(Dir_t));
    if (dir == 0)
    {
        errno = ENOMEM;
        return 0;
    }
    dir->dir = h;
    return dir;
}

struct dirent *readdir(Dir_t *dirp)
{
    SYS_FS_RESULT res;
    memset(&dirp->stat, 0, sizeof(SYS_FS_FSTAT));
    dirp->stat.lfname = dirp->lfname;
    dirp->stat.lfsize = sizeof(dirp->lfname);
    res = SYS_FS_DirRead(dirp->dir, &dirp->stat);
    if (res == SYS_FS_RES_FAILURE)
    {
        errno = ToErrno(SYS_FS_Error());
        return 0;
    }
    if (*dirp->stat.lfname)
        dirp->de.d_name = dirp->stat.lfname;
    else
        dirp->de.d_name = dirp->stat.fname;
    if (*dirp->de.d_name)
    {
        dirp->de.d_type = (dirp->stat.fattrib & 0x10) ? DT_DIR : DT_REG;
        return &dirp->de;
    }
    else
        return 0;
}

int closedir(Dir_t *dirp)
{
    SYS_FS_DirClose(dirp->dir);
    free(dirp);
    UnmountDrive();
    return 0;
}

void rewinddir(Dir_t *dirp)
{
    SYS_FS_DirRewind(dirp->dir);
}

const char *CurrentDirectoryName()
{
    return SYS_FS_MEDIA_IDX0_MOUNT_NAME_VOLUME_IDX0 "/";
}

*/