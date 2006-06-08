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
//   $Revision: 1.5 $
//   $Date: 2006/06/08 16:55:26 $
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
// storing of bvol in adapter classes 
// instead of lazy generation
#undef  GV_STOREDBVOL_FACE
#undef  GV_STOREDBVOL_PRIMITIVE
// storing of point and normal fields in adapter classes 
// instead of lazy generation
#undef  GV_STOREDCOORDS_FACE
#undef  GV_STOREDCOORDS_PRIMITIVE
#undef  GV_STOREDCOORDS_TRIANGLE
// use bounding-volumes in mixed tests
#define GV_BVOLS_IN_MIXEDTESTS
// OSGGVBVolCollision.h/.cpp:
// dynamic collision detection for translations
#define GV_COLDYNAMIC
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
#undef  GV_WITH_RAPID

// Windows Style DEBUG or NDEBUG macros
#ifdef WIN32
# if defined(OSG_DEBUG) && !defined(_DEBUG)
#  define _DEBUG
# endif
# if !defined(OSG_DEBUG) && !defined(NDEBUG)
#  define NDEBUG
# endif
#endif

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

extern std::ostream& GV_stream;
#ifdef GV_VERBOSE
extern bool          GV_verbose;
bool                 evalVerbosity ();
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
// Adapter Constants
// constant with the size of the AdapterVectorArray
const u32  GV_MAX_NUM_ADAPTER = 4;
// OSGGVAdapterFactory.h
// capacity of factory FactoryArray<AdapterType>
const u32  GV_MAX_NUM      = 100000;
// OSGGVRapidCollision.h/.cpp
// capacity of contact containers
const u32  GV_MAX_CONTACTS = 1000;

// Missing functions of namespace std
/** Minimum of two arguments.
 */
template<class TYPE> 
inline TYPE stdMin (const TYPE& a, const TYPE& b)
{ 
   return (a < b ? a : b); 
}

/** Maximum of two arguments.
 */
template<class TYPE> 
inline TYPE stdMax (const TYPE& a, const TYPE& b)
{ 
   return (a < b ? b : a); 
}

/** Clamping of second argument to interval defined by first and third argument.
 */
template <class TYPE, class TYPE2>
inline TYPE2 stdClamp (const TYPE& l, const TYPE2& value, const TYPE& h)
{
   if (value < l) {
      return TYPE2(l);
   }
   if (h < value) {
      return TYPE2(h);
   }
   return value;
}

/*! Comparison functor for C pointer types.
 */
template <class T>
struct PtrLess {
  typedef T* PtrType;
  static size_t value (const PtrType& key) {
#ifdef WIN32
    union Caster {
      PtrType pointer;
      size_t  address;
    };
    Caster result;
    result.pointer = key;
    return result.address;
#else
    return reinterpret_cast<size_t>(key); 
#endif
  }
  bool operator() (const PtrType& a, const PtrType& b) const {
    return value(a) < value(b);
  } 
};

/** Absolute value of argument.
 */
template<class TYPE> 
inline TYPE stdAbs (const TYPE& a)
{ 
   return (a >= 0 ? a : -a); 
}

/** Calculate cosine from sine.
 */
template <class REAL>
inline REAL cos2sin (const REAL& v)
{
   return sqrt(1-v*v);
}

/** Logarithm (of base 2) of argument.
 */
template <class TYPE>
inline u32 stdLog2 (TYPE arg)
{
   u32 result = 0;
   while (arg > 1) {
      arg >>= 1;
      ++result;
   }
   return result;
}
/** Power of two less than or equal to argument.
 */
template <class TYPE>
inline u32 stdFloorPower2 (TYPE arg)
{
   return 1 << (stdLog2(arg));
}
/** Power of two greater than argument.
 */
template <class TYPE>
inline u32 stdCeilPower2 (TYPE arg)
{
   return 1 << (stdLog2(arg)+1);
}

/** Double of argument. If argument is zero then result is one.
 */
template <class TYPE>
inline TYPE stdDouble (const TYPE& arg)
{
   return (arg==0 ? 1 : arg*2);
}
/** Half of argument.
 */
template <class TYPE>
inline TYPE stdHalf (const TYPE& arg)
{
   return arg/2;
}
/** n-times Double of argument. If argument is zero then result is zero.
 */
template <class TYPE>
inline TYPE stdDouble (const TYPE& arg, u32 n)
{
   return arg<<n;
}
/** n-times Half of argument.
 */
template <class TYPE>
inline TYPE stdHalf (const TYPE& arg, u32 n)
{
   return arg>>i;
}

/** Square of argument.
 */
template <class TYPE>
inline TYPE stdSqr (const TYPE& arg)
{
   return arg*arg;
}

/** Unequality compare operator, derived from equality compare operator.
 */
template <class TYPE>
inline bool operator!= (const TYPE& a, const TYPE& b)
{
   return !(a == b);
}

END_GENVIS_NAMESPACE
#endif
