#include <proto/exec.h>
#include <proto/dos.h>
#include <stdio.h>
#include <stdlib.h>

struct Interface *IHubbub = NULL;

void __init_hubbub_IHubbub(void) __attribute__((constructor));
void __exit_hubbub_IHubbub(void) __attribute__((destructor));

void __init_hubbub_IHubbub(void)
{
    struct Library * LibBase = IExec->OpenLibrary("hubbub.library", 2L);

    if (LibBase)
    {
       IHubbub = IExec->GetInterface(LibBase, "main", 1, NULL);
    }

    if (!IHubbub)
    {
       fprintf(stderr, "Could not open hubbub.library v2\n");

       IExec->CloseLibrary(LibBase);
       exit(EXIT_FAILURE);
    }
}

void __exit_hubbub_IHubbub(void)
{
    if (IHubbub)
    {
       struct Library * LibBase = IHubbub->Data.LibBase;

       IExec->DropInterface(IHubbub);
       IExec->CloseLibrary(LibBase);
       IHubbub = NULL;
    }
}
