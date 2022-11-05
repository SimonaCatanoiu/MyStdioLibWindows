#ifndef SO_UTILS_H
#define SO_UTILS_H
#include <Windows.h>

typedef enum{
    w,
    r,
    a,
    wplus,
    rplus,
    aplus,
    unset
} OPENMODE;

DWORD get_flags(const char* mode);
DWORD get_accessmode(const char* mode);
DWORD get_sharemode(const char* mode);
OPENMODE get_open_mode(const char* mode);
int is_read_flag_on(OPENMODE mode);
int is_write_flag_on(OPENMODE mode);
#endif