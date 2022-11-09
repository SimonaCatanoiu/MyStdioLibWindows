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

#define BUFFER_SIZE 4096

typedef enum{
    read_op,
    write_op,
    fflush_op,
    fseek_op,
    none_op
} LOPERATION;

struct _so_file{
    HANDLE handle;
    DWORD flags;
    char buffer[BUFFER_SIZE];
    int buffer_offset; 
    int buffer_length;
    int file_offset;
    int bool_is_eof;
    int bool_is_error;
    LOPERATION last_operation;
    OPENMODE openmode;
    PROCESS_INFORMATION proc_info;
};

DWORD get_flags(const char* mode);
DWORD get_accessmode(const char* mode);
DWORD get_sharemode(const char* mode);
OPENMODE get_open_mode(const char* mode);
int is_read_flag_on(OPENMODE mode);
int is_write_flag_on(OPENMODE mode);
#endif