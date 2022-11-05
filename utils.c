#include "utils.h"
#include <string.h>
#include <Windows.h>

DWORD get_flags(const char* mode)
{
    DWORD flags=0;
    // deschide fisierul pentru citire. Esueaza daca nu exista
    if (strcmp(mode, "r") == 0)
    {
        flags=OPEN_EXISTING;
    }
    //deschide fisierul pentru scriere. Daca nu exista, creeaza. Daca exista, truncheaza la 0
    if (strcmp(mode, "w") == 0)
    {
        flags=CREATE_ALWAYS;
    }
    //deschide in append(scriere la final).Daca nu exista,creeaza
    if (strcmp(mode, "a") == 0)
    {
        flags=OPEN_ALWAYS;
    }
    //deschide pentru citire si scriere. Esueaza daca nu exista
    if (strcmp(mode, "r+") == 0)
    {
        flags=OPEN_EXISTING;
    }
    //deschide pentru citire si scriere. Daca nu exista,creeaza. Daca exista, truncheaza la 0
    if (strcmp(mode, "w+") == 0)
    {
        flags=CREATE_ALWAYS;
    }
    //deschide in append + read. Daca nu exista,creeaza
    if (strcmp(mode, "a+") == 0)
    {
        flags=OPEN_ALWAYS;
    }
    return flags;
}

DWORD get_accessmode(const char* mode)
{
    DWORD desired_access=0;
    if (strcmp(mode, "r") == 0)
    {
        desired_access=GENERIC_READ;
    }
    if (strcmp(mode, "w") == 0)
    {
        desired_access=GENERIC_WRITE;
    }
    if (strcmp(mode, "a") == 0)
    {
        desired_access=FILE_APPEND_DATA;
    }
    if (strcmp(mode, "r+") == 0)
    {
        desired_access=GENERIC_READ | GENERIC_WRITE;
    }
    if (strcmp(mode, "w+") == 0)
    {
        desired_access=GENERIC_READ | GENERIC_WRITE;
    }
    if (strcmp(mode, "a+") == 0)
    {
        desired_access=GENERIC_READ|FILE_APPEND_DATA;
    }
    return desired_access;
}

OPENMODE get_open_mode(const char* mode)
{
    if (strcmp(mode, "r") == 0)
    {
        return r;
    }
    if (strcmp(mode, "w") == 0)
    {
        return w;
    }
    if (strcmp(mode, "a") == 0)
    {
        return a;
    }
    if (strcmp(mode, "r+") == 0)
    {
        return rplus;
    }
    if (strcmp(mode, "w+") == 0)
    {
        return wplus;
    }
    if (strcmp(mode, "a+") == 0)
    {
        return aplus;
    }
    return unset;
}

int is_read_flag_on(OPENMODE mode)
{
    if(mode==r || mode==rplus || mode==aplus||mode==wplus)
    {
        return 1;
    }
    return 0;
}

int is_write_flag_on(OPENMODE mode)
{
    if(mode==w || mode==wplus || mode==aplus||mode==a||mode==rplus)
    {
        return 1;
    }
    return 0;
}