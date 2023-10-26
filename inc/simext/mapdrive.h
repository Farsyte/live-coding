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

// Create new media in this numbered drive with the specified
// data length in bytes. Returns NULL if unable.

extern pDriveData   new_drive(int drive, int data_length);

// Release resources for a mapped-in drive.

extern void         free_drive(pDriveData);
