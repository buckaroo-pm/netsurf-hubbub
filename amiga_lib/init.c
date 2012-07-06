/* :ts=4
 *  $VER: init.c $Revision$ (19-Nov-2010)
 *
 *  This file is part of parserutils.
 *
 *  Copyright (c) 2010 Hyperion Entertainment CVBA.
 *  All Rights Reserved.
 *
 * $Id$
 *
 * $Log$
 *
 *
 */

#include <exec/exec.h>
#include <proto/exec.h>
#include <dos/dos.h>
#include <stddef.h>
#include "hubbub/parser.h"
#include <proto/parserutils.h>
#include <stdarg.h>
#include <proto/hubbub.h>

/* Version Tag */
#include "hubbub.library_rev.h"
STATIC CONST UBYTE USED verstag[] = VERSTAG;

struct Interface *INewlib __attribute__((force_no_baserel)) = NULL;
struct ParserUtilsIFace *IParserUtils __attribute__((force_no_baserel)) = NULL;

struct CLibrary
{
    struct Library libNode;
    BPTR segList;
    /* If you need more data fields, add them here */
};

/*
 * The system (and compiler) rely on a symbol named _start which marks
 * the beginning of execution of an ELF file. To prevent others from 
 * executing this library, and to keep the compiler/linker happy, we
 * define an empty _start symbol here.
 *
 * On the classic system (pre-AmigaOS 4.x) this was usually done by
 * moveq #0,d0
 * rts
 *
 */
int32 _start(void);

int32 _start(void)
{
    /* If you feel like it, open DOS and print something to the user */
    return RETURN_FAIL;
}

/*
void *hubbub_realloc(void *ptr, size_t size, void *pw)
{
	(void)pw;
	if (ptr == NULL)
	return size > 0 ? malloc(size) : NULL;
	if (size == 0) {
	free(ptr);
	return NULL;
	}
	return realloc(ptr, size);
}
*/

/* Open the library */
STATIC struct Library *libOpen(struct LibraryManagerInterface *Self, ULONG version)
{
    struct CLibrary *libBase = (struct CLibrary *)Self->Data.LibBase; 

    if (version > VERSION)
    {
        return NULL;
    }

    /* Add any specific open code here 
       Return 0 before incrementing OpenCnt to fail opening */


    /* Add up the open count */
    libBase->libNode.lib_OpenCnt++;
    return (struct Library *)libBase;

}


/* Close the library */
STATIC APTR libClose(struct LibraryManagerInterface *Self)
{
    struct Library *libBase = (struct Library *)Self->Data.LibBase;
    /* Make sure to undo what open did */


    /* Make the close count */
    ((struct Library *)libBase)->lib_OpenCnt--;

    return 0;
}


/* Expunge the library */
STATIC APTR libExpunge(struct LibraryManagerInterface *Self)
{
    /* If your library cannot be expunged, return 0 */
    struct ExecIFace *IExec
        = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
    APTR result = (APTR)0;
    struct CLibrary *libBase = (struct CLibrary *)Self->Data.LibBase;
    if (libBase->libNode.lib_OpenCnt == 0)
    {
	     result = (APTR)libBase->segList;

        /* Undo what the init code did */

	//	hubbub_finalise(hubbub_realloc, NULL);

        struct Library *ParserUtilsBase = IParserUtils->Data.LibBase;
        IExec->DropInterface(IParserUtils);
        IExec->CloseLibrary(ParserUtilsBase);

        struct Library *newlibbase = INewlib->Data.LibBase;
        IExec->DropInterface(INewlib);
        IExec->CloseLibrary(newlibbase);

        IExec->Remove((struct Node *)libBase);
        IExec->DeleteLibrary((struct Library *)libBase);
    }
    else
    {
        result = (APTR)0;
        libBase->libNode.lib_Flags |= LIBF_DELEXP;
    }
    return result;
}

/* The ROMTAG Init Function */
STATIC struct Library *libInit(struct Library *LibraryBase, APTR seglist, struct Interface *exec)
{
    struct CLibrary *libBase = (struct CLibrary *)LibraryBase;
    struct ExecIFace *IExec UNUSED = (struct ExecIFace *)exec;
    struct Library *newlibbase;
    struct Library *ParserUtilsBase;

    libBase->libNode.lib_Node.ln_Type = NT_LIBRARY;
    libBase->libNode.lib_Node.ln_Pri  = 0;
    libBase->libNode.lib_Node.ln_Name = "hubbub.library";
    libBase->libNode.lib_Flags        = LIBF_SUMUSED|LIBF_CHANGED;
    libBase->libNode.lib_Version      = VERSION;
    libBase->libNode.lib_Revision     = REVISION;
    libBase->libNode.lib_IdString     = VSTRING;

    libBase->segList = (BPTR)seglist;

    newlibbase = IExec->OpenLibrary("newlib.library", 3);
    if (newlibbase)
    {
       INewlib = IExec->GetInterface(newlibbase, "main", 1, NULL);
		if(!INewlib) return NULL;
    }

   ParserUtilsBase = IExec->OpenLibrary("parserutils.library", 1);
    if (ParserUtilsBase)
    {
       IParserUtils = IExec->GetInterface(ParserUtilsBase, "main", 1, NULL);
		if(!IParserUtils) return NULL;
    }

/*
	if(hubbub_initialise("L:CharSets/Aliases", hubbub_realloc, NULL) != HUBBUB_OK)
		return NULL;
*/

    /* Add additional init code here if you need it. For example, to open additional
       Libraries:
       libBase->UtilityBase = IExec->OpenLibrary("utility.library", 50L);
       if (libBase->UtilityBase)
       {
           libBase->IUtility = (struct UtilityIFace *)IExec->GetInterface(ElfBase->UtilityBase, 
              "main", 1, NULL);
           if (!libBase->IUtility)
               return NULL;
       } else return NULL; */

       return (struct Library *)libBase;
}

STATIC uint32 HubbubObtain(struct HubbubIFace *Self)
{
    return Self->Data.RefCount++;
}

STATIC uint32 HubbubRelease(struct HubbubIFace *Self)
{
    return Self->Data.RefCount--;
}

/* ------------------- Manager Interface ------------------------ */
/* These are generic. Replace if you need more fancy stuff */
STATIC uint32 _manager_Obtain(struct LibraryManagerInterface *Self)
{
	uint32 res;
	__asm__ __volatile__(
	"1:	lwarx	%0,0,%1\n"
	"addic	%0,%0,1\n"
	"stwcx.	%0,0,%1\n"
	"bne-	1b"
	: "=&r" (res)
	: "r" (&Self->Data.RefCount)
	: "cc", "memory");

	return res;
}

STATIC uint32 _manager_Release(struct LibraryManagerInterface *Self)
{
	uint32 res;
	__asm__ __volatile__(
	"1:	lwarx	%0,0,%1\n"
	"addic	%0,%0,-1\n"
	"stwcx.	%0,0,%1\n"
	"bne-	1b"
	: "=&r" (res)
	: "r" (&Self->Data.RefCount)
	: "cc", "memory");

	return res;
}

/* Manager interface vectors */
STATIC CONST APTR lib_manager_vectors[] =
{
	_manager_Obtain,
	_manager_Release,
	NULL,
	NULL,
	libOpen,
	libClose,
	libExpunge,
	NULL,
	(APTR)-1
};

/* "__library" interface tag list */
STATIC CONST struct TagItem lib_managerTags[] =
{
	{ MIT_Name,			(Tag)"__library"		},
	{ MIT_VectorTable,	(Tag)lib_manager_vectors},
	{ MIT_Version,		1						},
	{ TAG_DONE,			0						}
};

/* ------------------- Library Interface(s) ------------------------ */

#include "vectors.c"

/* Uncomment this line (and see below) if your library has a 68k jump table */
/* extern APTR VecTable68K[]; */

STATIC CONST struct TagItem mainTags[] =
{
	{ MIT_Name,			(Tag)"main"			},
	{ MIT_VectorTable,	(Tag)main_vectors	},
	{ MIT_Version,		1					},
	{ TAG_DONE,			0					}
};

STATIC CONST CONST_APTR libInterfaces[] =
{
	lib_managerTags,
	mainTags,
	NULL
};

STATIC CONST struct TagItem libCreateTags[] =
{
	{ CLT_DataSize,		sizeof(struct CLibrary)	},
	{ CLT_InitFunc,		(Tag)libInit			},
	{ CLT_Interfaces,	(Tag)libInterfaces		},
	/* Uncomment the following line if you have a 68k jump table */
	/* { CLT_Vector68K, (Tag)VecTable68K }, */
	{TAG_DONE,		 0 }
};


/* ------------------- ROM Tag ------------------------ */
STATIC CONST struct Resident lib_res USED =
{
	RTC_MATCHWORD,
	(struct Resident *)&lib_res,
	(APTR)(&lib_res + 1),
	RTF_NATIVE|RTF_AUTOINIT, /* Add RTF_COLDSTART if you want to be resident */
	VERSION,
	NT_LIBRARY, /* Make this NT_DEVICE if needed */
	0, /* PRI, usually not needed unless you're resident */
	"hubbub.library",
	VSTRING,
	(APTR)libCreateTags
};

