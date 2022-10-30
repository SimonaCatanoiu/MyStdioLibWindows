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
OPENMODE get_open_mode(const char* mode);

#endif