#include "simext/mapdrive.h"
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define	MAXDRIVE	15

#define HSIZE		128

static int          open_drive(int drive, int creat);
static pDriveData   map_new_drive_fd(int fd, int data_length);
static pDriveData   map_old_drive_fd(int fd);

pDriveData new_drive(int drive, int data_length)
{
    int                 fd;
    pDriveData          dp;
    int                 file_length;

    file_length = HSIZE + data_length;

    if (data_length < 0)
        return (errno = EINVAL), NULL;

    fd = open_drive(drive, O_RDWR | O_CREAT);
    if (fd < 0)
        return NULL;

    dp = map_new_drive_fd(fd, file_length);

    close(fd);

    if (NULL == dp)
        return NULL;

    memset(dp, 0, file_length);

    dp->data_offset = HSIZE;
    dp->data_length = data_length;
    dp->write_protect = 0;

    return dp;
}

pDriveData map_drive(int drive)
{
    int                 fd;
    pDriveData          dp;

    fd = open_drive(drive, O_RDWR);
    if (fd < 0)
        return NULL;

    dp = map_old_drive_fd(fd);

    close(fd);

    return dp;
}

void free_drive(pDriveData dd)
{
    if (NULL == dd)
        return;

    int                 file_length;
    int                 page_length;
    int                 mmap_pages;
    int                 mmap_length;

    file_length = dd->data_offset + dd->data_length;

    page_length = sysconf(_SC_PAGESIZE);
    if (page_length < 0)
        return;

    mmap_pages = (file_length + page_length - 1) / page_length;
    mmap_length = mmap_pages * page_length;

    munmap(dd->data_offset + (char *)dd, mmap_length);
}

static int open_drive(int drive, int mode)
{
    Cstr                bdev_dir;
    Cstr                fn;
    int                 fd;

    if ((drive < 0) || (drive > MAXDRIVE))
        return (errno = EINVAL), -1;

    bdev_dir = getenv("BDEV_DIR");
    if (NULL == bdev_dir)
        bdev_dir = "bdev";

    fn = format("%s/drive-%c.mmap", bdev_dir, 'A' + drive);
    if (NULL == fn)
        return (errno = EINVAL), -1;

    fd = open(fn, mode, 0666);
    free((void *)fn);
    return fd;
}

static pDriveData map_new_drive_fd(int fd, int file_length)
{
    int                 page_length;
    int                 mmap_pages;
    int                 mmap_length;
    void               *vp;

    page_length = sysconf(_SC_PAGESIZE);
    if (page_length < 0) {
        return (errno = EINVAL), NULL;
    }

    mmap_pages = (file_length + page_length - 1) / page_length;
    mmap_length = mmap_pages * page_length;

    if (ftruncate(fd, mmap_length) < 0)
        return NULL;

    vp = mmap(NULL, mmap_length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (((void *)-1) == vp)
        return NULL;

    return vp;
}

static pDriveData map_old_drive_fd(int fd)
{
    struct stat         sb[1];
    int                 page_length;
    int                 file_length;
    struct sDriveData   s[1];
    int                 rs;
    int                 data_offset;
    int                 data_length;
    int                 write_protect;
    int                 mmap_pages;
    int                 mmap_length;
    int                 mmap_prot;
    void               *vp;

    if (fstat(fd, sb) < 0)
        return NULL;

    page_length = sysconf(_SC_PAGESIZE);
    if (page_length < 0)
        return (errno = EINVAL), NULL;

    file_length = sb->st_size;
    if (file_length < (int)HSIZE)
        return NULL;

    rs = read(fd, s, sizeof(*s));
    if (rs < 0)
        return NULL;

    data_offset = s->data_offset;
    data_length = s->data_length;
    write_protect = s->write_protect;

    mmap_pages = (data_offset + data_length + page_length - 1) / page_length;
    mmap_length = mmap_pages * page_length;

    if (mmap_length > sb->st_size)
        if (ftruncate(fd, mmap_length) < 0)
            return NULL;

    mmap_prot = write_protect ? PROT_READ : (PROT_READ | PROT_WRITE);

    vp = mmap(NULL, mmap_length, mmap_prot, MAP_SHARED, fd, 0);
    if (((void *)-1) == vp)
        return NULL;

    return vp;
}
