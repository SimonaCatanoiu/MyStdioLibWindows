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
    new_file->buffer_offset = -1;
    memset(new_file->buffer, 0, BUFFER_SIZE);
    new_file->last_operation = none_op;
    new_file->openmode = get_open_mode(mode);
    new_file->flags = get_flags(mode);
    new_file->file_offset = 0;
    new_file->bool_is_eof = 0;
    new_file->bool_is_error = 0;

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
        new_file->bool_is_error=1;
        return NULL;
    }
    return new_file;
}

int so_fclose(SO_FILE *stream)
{
    BOOL return_value = CloseHandle(stream->handle);

    if (return_value == FALSE)
    {
        stream->bool_is_error=1;
        return SO_EOF;
    }
    free(stream);
    stream = NULL;
    return 0;
}

HANDLE so_fileno(SO_FILE *stream)
{
    if (check_error_so_fileno(stream) == THROW_ERR)
    {
        stream->bool_is_error=1;
        return INVALID_HANDLE_VALUE;
    }
    return stream->handle;
}

int so_fflush(SO_FILE *stream)
{
    DWORD dwBytesWritten;
    if (check_error_so_fflush(stream) == THROW_SO_EOF)
    {
        stream->bool_is_error=1;
        return SO_EOF;
    }

    BOOL returnValue = WriteFile(
        stream->handle,
        stream->buffer,
        stream->buffer_offset,
        &dwBytesWritten,
        NULL);
    // DACA NU ARE SUCCES SCRIEREA, INTOARCE SO_EOF
    if (returnValue == FALSE)
    {
        stream->bool_is_error=1;
        return SO_EOF;
    }
    printf("FFlush trebuia sa scrie %d caractere. FFlush a scris %d caractere\n", stream->buffer_offset + 1, dwBytesWritten);

    // dupa ce a scris in fisier, punem valorile din buffer din nou pe 0 si mutam offset-ul buffer-ului.Ultima operatie se va muta pe fflush_op
    // invalidarea bufferului
    memset(stream->buffer, 0, BUFFER_SIZE);
    stream->buffer_offset = -1;
    stream->last_operation = fflush_op;

    return 0;
}

int so_fseek(SO_FILE *stream, long offset, int whence)
{
    if (check_error_so_fseek(stream, offset, whence) == THROW_ERR)
    {
        stream->bool_is_error=1;
        return -1;
    }

    if (stream->last_operation == read_op)
    {
        // continutul bufferului trebuie invalidat
        memset(stream->buffer, 0, BUFFER_SIZE);
        stream->buffer_offset = -1;
    }

    if (stream->last_operation == write_op)
    {
        // continutul trebuie scris in fisier si apoi invalidat
        so_fflush(stream);
    }

    DWORD file_mode;
    if (whence == SEEK_SET)
    {
        file_mode = FILE_BEGIN;
    }
    if (whence == SEEK_CUR)
    {
        file_mode = FILE_CURRENT;
    }
    if (whence == SEEK_END)
    {
        file_mode = FILE_END;
    }

    DWORD fseek_return = SetFilePointer(
        stream->handle,
        offset,
        NULL,
        file_mode);
    // verifica daca pozitionarea cursorului a avut succes
    if (fseek_return == INVALID_SET_FILE_POINTER)
    {
        stream->bool_is_error=1;
        return -1;
    }

    // retine pozitia cursorului in fisier si ultima operatie
    stream->file_offset = fseek_return;
    stream->last_operation = fseek_op;
    return 0;
}
long so_ftell(SO_FILE *stream)
{
    if (check_error_so_ftell(stream) == THROW_ERR)
    {
        stream->bool_is_error=1;
        return -1;
    }
    return stream->file_offset;
}

int so_feof(SO_FILE *stream)
{
    if (check_error_so_feof(stream) == THROW_ERR)
    {
        stream->bool_is_error=1;
        return -1;
    }
    if (stream->bool_is_eof == 1)
    {
        return 1;
    }
    return 0;
}
int so_ferror(SO_FILE *stream)
{
    if (check_error_so_ferror(stream) == THROW_ERR)
    {
        stream->bool_is_error=1;
        return -1;
    }

    if (stream->bool_is_error == 1)
    {
        return 1;
    }
    return 0;
}

int main()
{
   SO_FILE *file = so_fopen("maine", "w");

    strcpy(file->buffer, "Acesta este textul de afisat\n");
    file->buffer_offset = strlen(file->buffer) - 1;
    file->last_operation = write_op;

    so_fflush(file);

    file->last_operation = write_op;
    
    so_fflush(file);

   so_fclose(file);
}