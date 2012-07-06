#ifndef PROTO_PARSERUTILS_H
#define PROTO_PARSERUTILS_H

/*
**	$Id$
**	Includes Release 50.1
**
**	Prototype/inline/pragma header file combo
**
**	Copyright (c) 2010 Hyperion Entertainment CVBA.
**	All Rights Reserved.
*/

/****************************************************************************/

#ifndef __NOLIBBASE__
 #ifndef __USE_BASETYPE__
  extern struct Library * parserutils;
 #else
  extern struct Library * parserutils;
 #endif /* __USE_BASETYPE__ */
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/parserutils.h>
 #ifdef __USE_INLINE__
  #include <inline4/parserutils.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_PARSERUTILS_PROTOS_H
  #define CLIB_PARSERUTILS_PROTOS_H 1
 #endif /* CLIB_PARSERUTILS_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct ParserUtilsIFace *IParserUtils;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_PARSERUTILS_PROTOS_H
  #include <clib/parserutils_protos.h>
 #endif /* CLIB_PARSERUTILS_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/parserutils.h>
  #else
   #include <ppcinline/parserutils.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/parserutils_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/parserutils_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_PARSERUTILS_H */
