//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//                                License                                     
//                                                                            
//   This library is free software; you can redistribute it and/or modify it 
//   under the terms of the GNU Library General Public License as published  
//   by the Free Software Foundation, version 2.                             
//                                                                             
//   This library is distributed in the hope that it will be useful, but       
//   WITHOUT ANY WARRANTY; without even the implied warranty of                
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         
//   Library General Public License for more details.                          
//                                                                            
//   You should have received a copy of the GNU Library General Public         
//   License along with this library; if not, write to the Free Software       
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.1 $
//   $Date: 2003/09/11 16:20:30 $
//                                                                            
//=============================================================================

#ifndef OSGGVBASE_H
#define OSGGVBASE_H

#include "OSGVector.h"
OSG_USING_NAMESPACE

// Compile-time Defines
//
// OSGGVCache.h/.cpp:
// cache implementation uses HashMap instead of OpenSG Attachment
#define GV_CACHE_HASHMAP
// OSGGVDoubleTraverser.h/.cpp:
// case dispatch by dynamic_cast instead of virtual function
#undef  GV_DISPATCHBYCAST
// OSGGVDoubleTraverserFixed.h/.cpp:
// do depth counting at compile-time
#define GV_GENERIC_DEPTHCOUNT
// storing of bvol in adapter classes 
// instead of lazy generation
#undef  GV_STOREDBVOL_FACE
#undef  GV_STOREDBVOL_PRIMITIVE
// storing of point and normal fields in adapter classes 
// instead of lazy generation
#undef  GV_STOREDCOORDS_FACE
#undef  GV_STOREDCOORDS_PRIMITIVE
#undef  GV_STOREDCOORDS_TRIANGLE
// OSGGVBVolCollision.h/.cpp only:
// realign also dop1 in local-coordsystem of dop2
#define GV_WITH_CLASS2
// use bounding-volumes in mixed tests
#undef  GV_BVOLS_IN_MIXEDTESTS
// OSGGVTriangleAdapterAligned.h/.cpp:
// use subcones (currently 4 subcones per direction)
#define GV_WITH_SUBCONES
// OSGGVPrecomputedRealign.h/.cpp only:
// 32Bit/64Bit occupancy-table
#define GV_PRECOMPUTEDREALIGN_32BIT
// minimum distance computation:
// show minimum distance as line in getLineGeometry()
#define GV_WITH_LINEGEOMETRY
// some verbose output
#undef  GV_VERBOSE
// some debug output
#undef  GV_DEBUG
// OSGGVRAPID*.h/.cpp:
// activate code for collision detection with RAPID
#define GV_WITH_RAPID

#ifdef WIN32
// remark #383: value copied to temporary, reference to temporary used
#pragma warning (disable : 383)
// remark #869: parameter was never referenced
#pragma warning (disable : 869)

// Windows Style DEBUG or NDEBUG macros
# if defined(OSG_DEBUG) && !defined(_DEBUG)
#  define _DEBUG
# endif
# if !defined(OSG_DEBUG) && !defined(NDEBUG)
#  define NDEBUG
# endif
#endif

// Adapter constants
// constant with the size of the AdapterVectorArray
const unsigned GV_MAX_NUM_ADAPTER = 4;
// constant for the number of objects
const unsigned GV_MAX_NUM  = 100000; //10000;
const unsigned GV_MAX_NUM_OBJECTS  = 1000; //10000;
// constant for the number of faces
const unsigned GV_MAX_NUM_FACES    = 100000; //400000;
// constant for the number of faces
const unsigned GV_MAX_NUM_TRIANGLES= 100000; //400000;

// GENVIS namespace
#define BEGIN_GENVIS_NAMESPACE namespace genvis {
#define END_GENVIS_NAMESPACE   }
#define USING_GENVIS_NAMESPACE using namespace genvis;

// DLL generation
#if defined(WIN32) && defined(OSG_BUILD_DLL)
#   ifdef OSG_COMPILEGENVISLIB
#       define OSG_GENVISLIB_DLLMAPPING     __declspec(dllexport)
#       define OSG_GENVISLIB_DLLTMPLMAPPING __declspec(dllexport)
#   else
#       define OSG_GENVISLIB_DLLMAPPING     __declspec(dllimport)
#       define OSG_GENVISLIB_DLLTMPLMAPPING __declspec(dllimport)
#   endif
#else
#   define OSG_GENVISLIB_DLLMAPPING
#   define OSG_GENVISLIB_DLLTMPLMAPPING
#endif


BEGIN_GENVIS_NAMESPACE

#ifdef GV_VERBOSE
extern bool GV_verbose;
bool        evalVerbosity ();
#endif

// Specialities
#ifdef WIN32
extern "C++" {
 
void OSG_GENVISLIB_DLLTMPLMAPPING getProcessVMConsumption         
(SIZE_T& processVMSize, SIZE_T& processVMUsed);
bool OSG_GENVISLIB_DLLTMPLMAPPING getProcessHeapMemoryConsumption 
(unsigned int& heapBlocksUsed, unsigned int& heapMemoryUsed);

}
#endif

// Types
typedef Int64    i64;
typedef Int32    i32;
typedef Int16    i16;
typedef Int8     i8;
typedef UInt64   u64;
typedef UInt32   u32;
typedef UInt16   u16;
typedef UInt8    u8;
typedef Real32                                        Real;
typedef VectorInterface<u32, VecStorage3<u32> >       VectorClass3u;
typedef VectorInterface<i32, VecStorage3<i32> >       VectorClass3i;
typedef PointInterface<u32, VecStorage3<u32> >        PointClass3u;
typedef PointInterface<i32, VecStorage3<i32> >        PointClass3i;
typedef VectorInterface<Real32, VecStorage3<Real32> > VectorClass;
typedef PointInterface<Real32, VecStorage3<Real32> >  PointClass;
typedef VectorInterface<Real32, VecStorage3<Real32> > VectorClass3;
typedef PointInterface<Real32, VecStorage3<Real32> >  PointClass3;
typedef VectorInterface<Real32, VecStorage2<Real32> > VectorClass2;
typedef PointInterface<Real32, VecStorage2<Real32> >  PointClass2;

// Constants
const Real infinity = 1e16;

// Missing functions of namespace std
template<class TYPE> 
inline TYPE stdMin (const TYPE& a, const TYPE& b)
{ 
   return (a < b ? a : b); 
}

template<class TYPE> 
inline TYPE stdMax (const TYPE& a, const TYPE& b)
{ 
   return (a < b ? b : a); 
}

template<class TYPE> 
inline TYPE stdAbs (const TYPE& a)
{ 
   return (a >= 0 ? a : -a); 
}

template <class REAL>
inline REAL cos2sin (const REAL& v)
{
   return sqrt(1-v*v);
}

END_GENVIS_NAMESPACE
#endif
