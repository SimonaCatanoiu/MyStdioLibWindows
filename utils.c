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