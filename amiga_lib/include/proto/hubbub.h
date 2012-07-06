#ifndef PROTO_HUBBUB_H
#define PROTO_HUBBUB_H

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
  extern struct Library * hubbub;
 #else
  extern struct Library * hubbub;
 #endif /* __USE_BASETYPE__ */
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/hubbub.h>
 #ifdef __USE_INLINE__
  #include <inline4/hubbub.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_HUBBUB_PROTOS_H
  #define CLIB_HUBBUB_PROTOS_H 1
 #endif /* CLIB_HUBBUB_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct HubbubIFace *IHubbub;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_HUBBUB_PROTOS_H
  #include <clib/hubbub_protos.h>
 #endif /* CLIB_HUBBUB_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/hubbub.h>
  #else
   #include <ppcinline/hubbub.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/hubbub_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/hubbub_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_HUBBUB_H */
