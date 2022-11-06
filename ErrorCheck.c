#include "ErrorCheck.h"
#include <stdio.h>
#include <string.h>

int check_error_so_fopen_args(const char *pathname, const char *mode)
{
    if (pathname == NULL || mode == NULL)
    {
        return THROW_NULL;
    }
    if (strcmp(pathname, "") == 0)
    {
        return THROW_NULL;
    }
    if ((strcmp(mode, "w") != 0) && (strcmp(mode, "r") != 0) && (strcmp(mode, "a") != 0) && (strcmp(mode, "w+") != 0) && (strcmp(mode, "r+") != 0) && (strcmp(mode, "a+") != 0))
    {
        return THROW_NULL;
    }
    return MOVEON;
}

int check_error_so_fileno(SO_FILE *stream)
{
    if (stream == NULL)
    {
        printf("Fileno error: NULL STREAM\n");
        return THROW_ERR;
    }
    if (stream->handle == INVALID_HANDLE_VALUE)
    {
        printf("Fileno error: descriptor invalid\n");
        return THROW_ERR;
    }
    return MOVEON;
}

int check_error_so_fflush(SO_FILE *stream)
{
    if (stream == NULL)
    {
        printf("Flush error: NULL STREAM\n");
        return THROW_SO_EOF;
    }
    // FFLUSH ARE SENS DOAR DACA ULTIMA OPERATIE A FOST DE SCRIERE
    if (stream->last_operation != write_op)
    {
        printf("FFlush nu va face nimic pentru ca ultima operatie nu este de scriere\n");
        return THROW_SO_EOF;
    }
    return MOVEON;
}

int check_error_so_fseek(SO_FILE* stream,long offset,int whence)
{
    if(stream==NULL)
    {
        printf("Fseek:null stream\n");
        return THROW_ERR;
    }
    if((whence!=SEEK_SET)&&(whence!=SEEK_END)&&(whence!=SEEK_CUR))
    {
        printf("Fseek:whence nu e o valoare valida\n");
        return THROW_ERR;
    }
    if((whence==SEEK_SET) && (offset<0))
    {
        printf("Fseek:offset negativ pentru SEEK_SET");
        return THROW_ERR;
    }
    return MOVEON;
}

int check_error_so_ftell(SO_FILE* stream)
{
    if(stream==NULL)
    {
        printf("Ftell: argument null\n");
        return THROW_ERR;
    }
    return MOVEON;
}

int check_error_so_feof(SO_FILE* stream)
{
    if(stream==NULL)
    {
        printf("Feof:null argument\n");
        return THROW_ERR;
    }
    return MOVEON;
}

int check_error_so_ferror(SO_FILE* stream)
{
    if(stream==NULL)
    {
        printf("Ferror: null argument\n");
        return THROW_ERR;
    }
    return MOVEON;
}

int check_error_so_fread_fwrite(SO_FILE *stream,size_t size, size_t nmemb)
{
    if(stream==NULL)
    {
        return THROW_SO_EOF;
    }
    if(size<=0)
    {
        return THROW_SO_EOF;
    }
    if(nmemb<=0)
    {
        return THROW_SO_EOF;
    }
    return MOVEON;
}