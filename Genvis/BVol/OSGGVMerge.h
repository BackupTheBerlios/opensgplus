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
//   $Revision: 1.3 $
//   $Date: 2004/12/20 15:57:11 $
//                                                                            
//=============================================================================

#ifndef OSGGVMERGE_H
#define OSGGVMERGE_H

#include <stack>
#include <vector>
#include "OSGGVBase.h"
#include "OSGGVGroup.h"
#include "OSGGVSingleTraverser.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Merge levels of a bounding volume hierarchy. For example
    Merge with setNumLevels(1) on a binary hierarchy creates a 4-ary hierarchy.
 */
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING Merge
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef typename BasicTraits::GeomObjectType                  GeomObjectType;
   typedef typename BasicTraits::GeomFaceType                    GeomFaceType;
   typedef typename BasicTraits::TransformType                   TransformType;
   typedef BVolAdapterBase                                       GeneralType; 
   typedef BoundingVolume<Real>                                  BVol;
   typedef typename SingleTraverserBase<BasicTraits>::ResultType ResultT;
   typedef BVolAdapterBase                                       AdapterType;
   typedef BVolGroupBase                                         GroupType;
   typedef std::vector<GeneralType*>                             AddList;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   inline Merge ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */
   inline u32      getNumLevels () const;
   inline void     setNumLevels (u32 level);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Traversal functions.                                         */
   /*! \{                                                                 */   
   bool     InitMerge  (GeneralType* a, const TransformType& m);
   bool     LeaveMerge (GeneralType* a, const TransformType& m);
   ResultT  InnerMerge (GroupType*   b);
   ResultT  InnerLeaveMerge (GroupType*   b);
   ResultT  LeafMerge  (AdapterType* p);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   u32 m_level;
   u32 m_current;
   std::stack<AddList> m_addStack;
};

template <class BasicTraits>
inline Merge<BasicTraits>::Merge ()
  : m_level(0)
{
}
template <class BasicTraits>
inline u32 Merge<BasicTraits>::getNumLevels () const
{
   return m_level;
}
template <class BasicTraits>
inline void     Merge<BasicTraits>::setNumLevels (u32 level)
{
   m_level = level;
}

/*! \brief Traits class for Merge.
*/
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING MergeTraits
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef typename Merge<BasicTraits>::BVol          BVol;
   typedef typename Merge<BasicTraits>::ResultT       ResultT;
   typedef Merge<BasicTraits>                         ObjectT;
   typedef typename Merge<BasicTraits>::GeneralType   GeneralType;
   typedef typename Merge<BasicTraits>::GroupType     GroupType;
   typedef typename Merge<BasicTraits>::AdapterType   AdapterType;
   typedef typename Merge<BasicTraits>::TransformType TransformType;

   typedef InitSingleFunctor2<bool,GeneralType,TransformType,ObjectT> InitFunctorT;
   typedef SingleFunctor<ResultT,GroupType,ObjectT>                   InnerFunctorT;
   typedef SingleFunctor<ResultT,AdapterType,ObjectT>                 LeafFunctorT;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Functor creation.                                            */
   /*! \{                                                                 */
   static  InitFunctorT  createInitFunctor      (ObjectT* obj);
   static  InitFunctorT  createInitLeaveFunctor (ObjectT* obj);
   static  InnerFunctorT createInnerFunctor     (ObjectT* obj);
   static  InnerFunctorT createInnerLeaveFunctor(ObjectT* obj);
   static  LeafFunctorT  createLeafFunctor      (ObjectT* obj);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};

END_GENVIS_NAMESPACE
#endif
