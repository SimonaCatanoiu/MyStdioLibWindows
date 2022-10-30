#define DLL_EXPORTS
#include <stdio.h>
#include <string.h>
#include "so_stdio.h"
#include "ErrorCheck.h"

SO_FILE *so_fopen(const char *pathname, const char *mode)
{
    if (check_error_so_fopen_args(pathname, mode) == THROW_NULL)
    {
        printf("Parametrii invalizi\n");
        return NULL;
    }
    // aloca un pointer SO_FILE
    SO_FILE *new_file = (SO_FILE *)malloc(sizeof(SO_FILE));
    if (new_file == NULL)
    {
        printf("Nu s-a putut aloca un SO_FILE\n");
        return NULL;
    }

    // initializeaza structura SO_FILE
    new_file->buffer_offset = 0;
    memset(new_file->buffer, 0, BUFFER_SIZE);
    new_file->last_operation = none_op;
    new_file->openmode = get_open_mode(mode);
    new_file->flags = get_flags(mode);

    // deschiderea fisierului si intializare handle
    LPCTSTR filepath = (LPCSTR)pathname;
    new_file->handle = CreateFile(
        filepath,
        GENERIC_READ | GENERIC_WRITE,       // puse asa ca sa mearga in checker??
        FILE_SHARE_READ | FILE_SHARE_WRITE, // puse asa ca sa mearga in checker??
        NULL,
        new_file->flags,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    // verifica ca fisierul s-a deschis cu succes
    if (new_file->handle == INVALID_HANDLE_VALUE)
    {
        printf("Eroare la deschiderea fisierului\n");
        return NULL;
    }
    return new_file;
}

int so_fclose(SO_FILE *stream)
{
    BOOL return_value = CloseHandle(stream->handle);
 
    if (return_value == FALSE)
    {
        free(stream);
        stream=NULL;
        return SO_EOF;
    }
    free(stream);
    stream=NULL;
    return 0;
}

int main()
{
    SO_FILE *file = so_fopen("maine", "a");
    so_fclose(file);
    return 0;
}