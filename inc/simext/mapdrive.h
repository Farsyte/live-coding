#pragma once
#include "simext/support.h"

// === === === === === === === === === === === === === === === ===
//           SUPPORT CODE TO MAP MASS STORAGE INTO MEMORY
// === === === === === === === === === === === === === === === ===

// Reserve some storage at the start of the file to represent the
// geometry of the media.

typedef struct sDriveData {
    unsigned            data_offset;
    unsigned            data_length;
    unsigned            write_protect;
}                  *pDriveData;

// Access existing media in this numbered drive.

extern pDriveData   map_drive(int drive);

// Access existing media in this named drive image file.
// The media file is found in $BDEV_DIR (defaults to "./bdev").

extern pDriveData   map_drive_file(Cstr filename);

// Create new media in this numbered drive with the specified
// data length in bytes. Returns NULL if unable.

extern pDriveData   new_drive(int drive, int data_length);

// Create new media in this named drive image file with the specified
// data length in bytes. Returns NULL if unable.
// The media file is created in $BDEV_DIR (defaults to "./bdev").

extern pDriveData   new_drive_file(Cstr filename, int data_length);

// Given a drive number, return the basename of the drive's image file.
// Caller must free the pointer returned by this function.

extern Cstr         drive_base_file(int drive);

// Given a basename of a drive image, return an openable path.
// Caller must free the pointer returned by this function.

extern Cstr         drive_image_path(Cstr basename);

// Release resources for a mapped-in drive.

extern void         free_drive(pDriveData);
