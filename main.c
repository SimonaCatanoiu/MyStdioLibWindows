#define DLL_EXPORTS
#include <stdio.h>
#include <string.h>
#include "so_stdio.h"
#include "ErrorCheck.h"

#define COMMAND_LINE_SIZE 8191
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
    new_file->buffer_length = 0;
    memset(new_file->buffer, 0, BUFFER_SIZE);
    new_file->last_operation = none_op;
    new_file->openmode = get_open_mode(mode);
    new_file->flags = get_flags(mode);
    new_file->file_offset = 0;
    new_file->bool_is_eof = 0;
    new_file->bool_is_error = 0;
    new_file->proc_info.hProcess = INVALID_HANDLE_VALUE;
    new_file->proc_info.hThread = INVALID_HANDLE_VALUE;

    DWORD access_mode = get_accessmode(mode);

    // deschiderea fisierului si intializare handle
    LPCTSTR filepath = (LPCSTR)pathname;
    new_file->handle = CreateFile(
        filepath,
        access_mode,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        new_file->flags,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    // verifica ca fisierul s-a deschis cu succes
    if (new_file->handle == INVALID_HANDLE_VALUE)
    {
        printf("Eroare la deschiderea fisierului\n");
        new_file->bool_is_error = 1;
        return NULL;
    }
    return new_file;
}

int so_fclose(SO_FILE *stream)
{
    // Inainte de a inchide fisierul, trebuie sa scriu ce am in buffer daca ultima operatie a fost de scriere

    if (stream->last_operation == write_op)
    {
        int ret_value = so_fflush(stream);
        if (ret_value < 0)
        {
            return SO_EOF;
        }
    }

    BOOL return_value = CloseHandle(stream->handle);

    if (return_value == FALSE)
    {
        stream->bool_is_error = 1;
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
        stream->bool_is_error = 1;
        return INVALID_HANDLE_VALUE;
    }
    return stream->handle;
}

int so_fflush(SO_FILE *stream)
{
    DWORD dwBytesWritten;
    if (check_error_so_fflush(stream) == THROW_SO_EOF)
    {
        stream->bool_is_error = 1;
        return SO_EOF;
    }

    BOOL returnValue = WriteFile(
        stream->handle,
        stream->buffer,
        stream->buffer_length,
        &dwBytesWritten,
        NULL);
    // DACA NU ARE SUCCES SCRIEREA, INTOARCE SO_EOF
    if (returnValue == FALSE)
    {
        stream->bool_is_error = 1;
        return SO_EOF;
    }

    // FFLUSH TREBUIE SA ASIGURE CA TOT BUFFERUL E SCRIS
    while (dwBytesWritten < stream->buffer_length)
    {
        DWORD dwBytesWritten2;
        BOOL returnValue2 = WriteFile(
            stream->handle,
            stream->buffer + dwBytesWritten,
            stream->buffer_length - dwBytesWritten,
            &dwBytesWritten2,
            NULL);
        if (returnValue2 == FALSE)
        {
            stream->bool_is_error = 1;
            return SO_EOF;
        }
        dwBytesWritten = dwBytesWritten + dwBytesWritten2;
    }

    // printf("FFlush trebuia sa scrie %d caractere. FFlush a scris %d caractere\n", stream->buffer_length, dwBytesWritten);

    // dupa ce a scris in fisier, punem valorile din buffer din nou pe 0 si mutam offset-ul buffer-ului.Ultima operatie se va muta pe fflush_op
    // invalidarea bufferului
    memset(stream->buffer, 0, BUFFER_SIZE);
    stream->buffer_offset = 0;
    stream->buffer_length = 0;
    stream->last_operation = fflush_op;
    return 0;
}

int so_fseek(SO_FILE *stream, long offset, int whence)
{
    if (check_error_so_fseek(stream, offset, whence) == THROW_ERR)
    {
        stream->bool_is_error = 1;
        return -1;
    }

    if (stream->last_operation == read_op)
    {
        // continutul bufferului trebuie invalidat
        memset(stream->buffer, 0, BUFFER_SIZE);
        stream->buffer_offset = 0;
        stream->buffer_length = 0;
    }

    if (stream->last_operation == write_op)
    {
        // continutul trebuie scris in fisier si apoi invalidat
        int ret_value = so_fflush(stream);
        if (ret_value < 0)
        {
            return -1;
        }
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
        stream->bool_is_error = 1;
        return -1;
    }

    // retine pozitia cursorului in fisier
    stream->file_offset = fseek_return;
    stream->bool_is_eof = 0;
    return 0;
}

long so_ftell(SO_FILE *stream)
{
    if (check_error_so_ftell(stream) == THROW_ERR)
    {
        stream->bool_is_error = 1;
        return -1;
    }
    return stream->file_offset;
}

int so_feof(SO_FILE *stream)
{
    if (check_error_so_feof(stream) == THROW_ERR)
    {
        stream->bool_is_error = 1;
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
        stream->bool_is_error = 1;
        return -1;
    }

    if (stream->bool_is_error == 1)
    {
        return 1;
    }
    return 0;
}

int so_fgetc(SO_FILE *stream)
{
    if (stream == NULL)
    {
        stream->bool_is_error = 1;
        return SO_EOF;
    }

    // verifica daca nu poate sa citeasca
    if (is_read_flag_on(stream->openmode) == 0)
    {
        stream->bool_is_error = 1;
        return SO_EOF;
    }

    // verifica daca a ajuns la finalul fisierului
    if (stream->bool_is_eof == 1)
    {
        stream->bool_is_error = 1;
        return SO_EOF;
    }

    // verifica daca trebuie incarcat bufferul
    if (stream->buffer_length == 0 || stream->buffer_offset == stream->buffer_length)
    {
        // incarc buffer-ul
        DWORD bytesReaded;
        BOOL ret_value = ReadFile(
            stream->handle,
            stream->buffer,
            BUFFER_SIZE,
            &bytesReaded,
            NULL);
        // verific daca a citit cu succes
        if (ret_value == FALSE)
        {
            if (bytesReaded == 0)
            {
                stream->bool_is_eof = 1;
            }
            else
            {
                stream->bool_is_error = 1;
            }
            return SO_EOF;
        }

        // actualizez informatiile despre buffer odata ce l-am incarcat
        stream->buffer_length = bytesReaded;
        stream->buffer_offset = 0;
    }
    stream->file_offset++;
    stream->buffer_offset++;
    stream->last_operation = read_op;

    return (int)stream->buffer[stream->buffer_offset - 1];
}

int so_fputc(int c, SO_FILE *stream)
{
    if (stream == NULL)
    {
        stream->bool_is_error = 1;
        return SO_EOF;
    }

    // verifica daca nu poate sa scrie
    if (is_write_flag_on(stream->openmode) == 0)
    {
        stream->bool_is_error = 1;
        return SO_EOF;
    }

    // daca bufferul este full, atunci va apela fflush
    if (stream->buffer_offset == BUFFER_SIZE)
    {
        int retValue = so_fflush(stream);
        if (retValue < 0)
        {
            stream->bool_is_error = 1;
            return SO_EOF;
        }
    }
    // va scrie in buffer si va muta cursorul
    stream->buffer[stream->buffer_offset] = (unsigned char)c;
    stream->buffer_offset++;
    stream->buffer_length++;
    stream->file_offset++;
    stream->last_operation = write_op;
    return c;
}

size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
    int total_read_elements = 0;
    // verifica daca argumentele sunt valide
    if (check_error_so_fread_fwrite(stream, size, nmemb) == THROW_SO_EOF)
    {
        stream->bool_is_error = 1;
        return SO_EOF;
    }

    // verifica daca se poate citi din fisier
    if (is_read_flag_on(stream->openmode) == 0)
    {
        stream->bool_is_error = 1;
        return SO_EOF;
    }
    int index = 0;
    for (int i = 0; i < nmemb; i++)
    {
        for (int j = 0; j < size; j++)
        {
            // pentru fiecare byte citit, apelam so_fgetc
            int ret_char = so_fgetc(stream);
            // daca a ajuns la finalul fisierului, intoarce cate elemente a citit
            if (ret_char < 0 && stream->bool_is_eof == 1)
            {
                return total_read_elements;
            }
            else
            { // daca a dat eroare la fgetc, intoarce SO_EOF
                if (ret_char < 0 && stream->bool_is_error == 1)
                {
                    return SO_EOF;
                }
            }
            // pune byte in ptr
            *(((unsigned char *)ptr) + index) = (unsigned char)ret_char;
            index++;
        }
        total_read_elements++;
    }
    // intoarce cate elemente a citit
    stream->last_operation = read_op;
    return total_read_elements;
}

size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
    int total_elements_written = 0;
    // verifica daca argumentele sunt valide
    if (check_error_so_fread_fwrite(stream, size, nmemb) == THROW_SO_EOF)
    {
        stream->bool_is_error = 1;
        return SO_EOF;
    }

    // verifica daca poate scrie in fisier
    if (is_write_flag_on(stream->openmode) == 0)
    {
        stream->bool_is_error = 1;
        return SO_EOF;
    }

    for (int i = 0; i < nmemb; i++)
    {
        for (int j = 0; j < size; j++)
        {
            // pentru fiecare byte din ptr, apelam so_fputc
            int index = i * size + j;
            int char_to_put = *(((unsigned char *)ptr) + i);
            int ret_value = so_fputc(char_to_put, stream);
            // verificam daca a intampinat vreo eroare
            if (ret_value < 0 && stream->bool_is_error == 1)
            {
                return SO_EOF;
            }
        }
        total_elements_written++;
    }
    stream->last_operation = write_op;
    return total_elements_written;
}

SO_FILE *so_popen(const char *command, const char *type)
{
    // verifica daca type e r sau w
    if ((strcmp(type, "r") != 0) && (strcmp(type, "w") != 0))
    {
        printf("Popen: argument type invalid\n");
        return NULL;
    }

    STARTUPINFO start_info;
    PROCESS_INFORMATION prcess_info;
    SECURITY_ATTRIBUTES si_pipe;

    // Pregatire structura STARTUPINFO pentru noul proces
    ZeroMemory(&start_info, sizeof(start_info));
    start_info.cb = sizeof(start_info);
    // pregatire structura PROCESS_INFORMATION pentru noul proces
    ZeroMemory(&prcess_info, sizeof(prcess_info));
    // marcam handle-urile pipe-ului ca fiind mostenibile
    ZeroMemory(&si_pipe, sizeof(si_pipe));
    si_pipe.bInheritHandle = TRUE;
    si_pipe.nLength = sizeof(si_pipe);

    // Creare pipe
    HANDLE hReadPipe, hWritePipe; // capetele pipe-ului
    BOOL ret_value = CreatePipe(&hReadPipe, &hWritePipe, &si_pipe, 0);
    if (ret_value == FALSE)
    {
        printf("Popen: eroare la crearea pipe-ului\n");
        return NULL;
    }

    // Setam Handle-urile pentru stdin, stdout, stderr pentru procesul copil
    if (strcmp(type, "r") == 0)
    {
        start_info.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
        start_info.hStdOutput = hWritePipe;
        start_info.hStdError = GetStdHandle(STD_ERROR_HANDLE);
        start_info.dwFlags |= STARTF_USESTDHANDLES;
        // Marcam nemostenibil capatul de pipe nefolosit de copil
        SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);
    }
    else
    {
        start_info.hStdInput = hReadPipe;
        start_info.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        start_info.hStdError = GetStdHandle(STD_ERROR_HANDLE);
        start_info.dwFlags |= STARTF_USESTDHANDLES;
        // Marcam nemostenibil capatul de pipe nefolosit de copil
        SetHandleInformation(hWritePipe, HANDLE_FLAG_INHERIT, 0);
    }

    // Initializare comanda de executat de copil
    CHAR argproc[COMMAND_LINE_SIZE];
    strcpy(argproc, "cmd /C ");
    strcat(argproc, command);

    // Creare proces copil
    BOOL create_res = CreateProcess(
        NULL,           /* No module name (use command line) */
        (LPSTR)argproc, /* Command line */
        NULL,           /* Process handle not inheritable */
        NULL,           /* Thread handle not inheritable */
        TRUE,           /* Set handle inheritance to TRUE */
        0,              /* No creation flags */
        NULL,           /* Use parent's environment block */
        NULL,           /* Use parent's starting directory */
        &start_info,    /* Pointer to STARTUPINFO structure */
        &prcess_info    /* Pointer to PROCESS_INFORMATION structure */
    );

    if (create_res == FALSE)
    {
        // eroare la crearea procesului. Inchid capetele pipe-ului
        printf("Popen: eroare la crearea procesului copil\n");
        CloseHandle(hWritePipe);
        CloseHandle(hReadPipe);
        return NULL;
    }

    // inchidem capatul nefolosit de parinte si setam handle-ul fisierului
    HANDLE file_handle;
    HANDLE child_handle;
    if (strcmp(type, "r") == 0)
    {
        file_handle = hReadPipe;
        CloseHandle(hWritePipe);
    }
    else
    {
        file_handle = hWritePipe;
        CloseHandle(hReadPipe);
    }

    // initializare structura SO_FILE
    // aloca fisier so_file
    SO_FILE *file = (SO_FILE *)malloc(sizeof(SO_FILE));
    // verifica alocarea
    if (file == NULL)
    {
        printf("Popen:eroare la alocarea SO_FILE\n");
        CloseHandle(file_handle);
        return NULL;
    }

    // initializeaza structura SO_FILE
    file->flags = get_flags(type);
    file->buffer_offset = 0;
    file->buffer_length = 0;
    file->file_offset = 0;
    file->last_operation = none_op;
    file->openmode = get_open_mode(type);
    file->bool_is_eof = 0;
    file->bool_is_error = 0;
    memset(file->buffer, 0, BUFFER_SIZE);
    file->proc_info.hProcess = prcess_info.hProcess;
    file->proc_info.hThread = prcess_info.hThread;
    file->handle = file_handle;

    return file;
}

int so_pclose(SO_FILE *stream)
{
    if (stream->proc_info.hThread == INVALID_HANDLE_VALUE || stream->proc_info.hProcess == INVALID_HANDLE_VALUE)
    {
        printf("Pclose: nu exista proces deschis cu un popen asociat stream-ului\n");
        return -1;
    }

    PROCESS_INFORMATION process_info;
    process_info = stream->proc_info;

    // inchidem si eliberam memoria ocupata de SO_FILE
    int ret_fclose = so_fclose(stream);
    if (ret_fclose < 0)
    {
        printf("Pclose: eroare la inchiderea stream-ului\n");
        return -1;
    }

    /* Asteptam finalizarea copilului */
    DWORD dwRes = WaitForSingleObject(process_info.hProcess, INFINITE);

    if (dwRes == WAIT_FAILED)
    {
        printf("Pclose: eroare la wait\n");
        return -1;
    }

    BOOL ret = GetExitCodeProcess(process_info.hProcess, &dwRes);
    if(ret==FALSE)
    {
        printf("Pclose: eroare la GetExitCodeProcess\n");
        return -1;
    }
    CloseHandle(process_info.hProcess);
    CloseHandle(process_info.hThread);
    return (int)dwRes;
}

int main()
{
    SO_FILE *f;
    char line[11];
    f = so_popen("dir", "r");
    int total = 0;
    while (so_feof(f) == 0)
    {
        size_t ret = so_fread(&line[total], 1, 10, f);
        if (ret > 0)
        {
            line[ret] = '\0';
            printf("%s", line);
        }
    }
    so_pclose(f);
    return 0;
}