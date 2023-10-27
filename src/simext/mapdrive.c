#include "simext/mapdrive.h"
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define	MAXDRIVE	15
#define HSIZE		128

static pDriveData   map_new_drive_fd(int fd, int file_length);
static pDriveData   map_old_drive_fd(int fd);

pDriveData new_drive(int drive, int data_length)
{
    Cstr                bn;
    pDriveData          dp;

    bn = drive_base_file(drive);
    if (NULL == bn)
        return NULL;

    dp = new_drive_file(bn, data_length);
    free((void *)bn);
    return dp;
}

pDriveData new_drive_file(Cstr basename, int data_length)
{
    int                 fd;
    pDriveData          dp;
    int                 file_length;
    Cstr                fn;

    if (data_length < 0)
        return (errno = EINVAL), NULL;

    file_length = HSIZE + data_length;

    fn = drive_image_path(basename);
    fd = open(fn, O_RDWR | O_CREAT, 0666);
    free((void *)fn);

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
    Cstr                bn;
    Cstr                fn;
    int                 fd;
    pDriveData          dp;

    bn = drive_base_file(drive);
    fn = drive_image_path(bn);
    fd = open(fn, O_RDWR);

    free((void *)fn);
    free((void *)bn);

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

    int                 page_length;
    int                 file_length;
    int                 mmap_pages;
    int                 mmap_length;

    page_length = sysconf(_SC_PAGESIZE);
    if (page_length < 0)
        return;

    file_length = dd->data_offset + dd->data_length;

    mmap_pages = (file_length + page_length - 1) / page_length;
    mmap_length = mmap_pages * page_length;

    munmap(dd->data_offset + (char *)dd, mmap_length);
}

Cstr drive_base_file(int drive)
{
    Cstr                bn;

    if ((drive < 0) || (drive > MAXDRIVE))
        return (errno = EINVAL), NULL;

    bn = format("drive-%c", 'A' + drive);
    if (NULL == bn)
        return (errno = EINVAL), NULL;

    return bn;
}

Cstr drive_image_path(Cstr basename)
{
    Cstr                bdev_dir;
    Cstr                fn;

    ASSERT(NULL != basename, "detected NULL basename");
    ASSERT('\0' != basename[0], "detected EMPTY basename");
    if (NULL != strchr(basename, '/')) {
        STUB("'/' in basename '%s'", basename);
        return NULL;
    }
    if (NULL != strchr(basename, '.')) {
        STUB("'.' in basename '%s'", basename);
        return NULL;
    }

    bdev_dir = getenv("BDEV_DIR");
    if (NULL == bdev_dir)
        bdev_dir = "bdev";

    fn = format("%s/%s.mmap", bdev_dir, basename);
    if (NULL == fn)
        return (errno = EINVAL), NULL;
    return fn;
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
