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
//   $Revision: 1.4 $
//   $Date: 2006/06/08 16:57:46 $
//                                                                            
//=============================================================================

#ifndef OSGGVRAPIDCOLLISION_H
#define OSGGVRAPIDCOLLISION_H

#include <vector>
#include "OSGGVBase.h"
#include "OSGGVCollisionData.h"
#include "OSGGVBVolCollision.h"
#include "OSGGVRAPIDAdapter.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief DataBase for collision detection with RAPID. RAPID creates one datastructure
           per object used in collision detection and reports just the ids of colliding
           triangles. This class stores with a list of colliding object adapters (getContacts)
	   a list of colliding face adapters (getContactFaces). 
 */
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING RAPIDCollision 
: public BVolCollisionBase<BasicTraits>, public DoubleTraverserBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef BVolCollisionBase<BasicTraits>         Inherited;
   typedef RAPIDCollision<BasicTraits>            Self;
   typedef typename Inherited::Cache              Cache;
   typedef typename Inherited::CacheData          CacheData;
   typedef typename Inherited::TransformType      TransformType;
   typedef OpenSGRAPIDAdapter<BasicTraits>        AdapterType;
   typedef OpenSGRAPIDAdapter<BasicTraits>        GroupType;
   typedef typename Inherited::CollisionPair      CollisionPair;
   typedef typename Inherited::CollisionContainer CollisionContainer;
   typedef typename DoubleTraverserBase<BasicTraits>::ResultType ResultT;
   typedef OpenSGRAPIDFaceAdapter<BasicTraits>    FaceAdapterType;
   typedef std::vector<FaceAdapterType>           FaceContainer;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   RAPIDCollision ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Member.                                                      */
   /*! \{                                                                 */
   inline i32                       getMode () const;
   virtual inline bool              getStopFirst () const;
   virtual void                     setStopFirst (bool flag=false);
   virtual inline DataBase<BasicTraits>&       getData ()       { return *this; }
   virtual inline const DataBase<BasicTraits>& getData () const { return *this; }
   inline Self&                                getDataTyped ()  { return *this; }
   inline const Self&                          getDataTyped () const { return *this; }
   /*! Get the integer identifier used by the AdapterType. This virtual method 
       is used to propagate the identifier from the DoubleTraverser to the 
       AllTraverser. */
   virtual inline u32                   getAdapterId() const;
   /*! Get the bounding volume for an adapter object for use in AllTraverser. */
   virtual inline const BoundingVolume<Real>& getBoundingVolume (const Adapter* adapter) const;
   /*! */
   virtual inline CacheData& getCacheData (CacheData& data, const Adapter* adapter) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Collision results.                                           */
   /*! \{                                                                 */
   inline FaceContainer&            getContactFirst  ();
   inline const FaceContainer&      getContactFirst  () const;
   inline FaceContainer&            getContactSecond ();
   inline const FaceContainer&      getContactSecond () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Init.                                                        */
   /*! \{                                                                 */
   virtual bool Init (); 
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Traversal functions.                                         */
   /*! \{                                                                 */
   /*! Init for the single pair in the arguments.                         */
   bool InitDouble  (GroupType* root0, const TransformType& m0, const TransformType& f0, 
		     GroupType* root1, const TransformType& m1, const TransformType& f1);
   /*! Leave for the single pair in the arguments.                        */
   bool LeaveDouble (GroupType* root0, const TransformType& m0, const TransformType& f0, 
		     GroupType* root1, const TransformType& m1, const TransformType& f1);
   /*! Operation for inner nodes in the arguments.                        */
   ResultT BVolBVolCollision (GroupType* b0, GroupType* b1);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Apply.                                                       */
   /*! \{                                                                 */ 
   virtual bool apply (const GeomObjectType& node0, const GeomObjectType& node1);
   virtual bool apply (CacheData& parent0, CacheData& data0, BVolAdapterBase* node0, 
		       CacheData& parent1, CacheData& data1, BVolAdapterBase* node1, 
		       const TransformType& m0 = TransformType::identity(),
		       const TransformType& m1 = TransformType::identity());
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Dump.                                                        */
   /*! \{                                                                 */
   virtual void dump (std::ostream& os);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   i32                m_mode;
   i32                m_last;
   FaceContainer      m_first;
   FaceContainer      m_second;
   double             m_r0[3][3];
   double             m_r1[3][3];
   double             m_t0[3];
   double             m_t1[3];
   double             m_s0;
   double             m_s1;
};
typedef  RAPIDCollision<OpenSGTraits> OSGRAPIDCollision;

template <class BasicTraits>
inline i32 RAPIDCollision<BasicTraits>::getMode () const
{
  return m_mode;
}
template <class BasicTraits>
inline bool RAPIDCollision<BasicTraits>::getStopFirst () const
{
  return Inherited::getStopFirst();
}

template <class BasicTraits>
inline typename RAPIDCollision<BasicTraits>::FaceContainer&       
RAPIDCollision<BasicTraits>::getContactFirst ()
{
   return m_first;
}
template <class BasicTraits>
inline const typename RAPIDCollision<BasicTraits>::FaceContainer& 
RAPIDCollision<BasicTraits>::getContactFirst () const
{
   return m_first;
}
template <class BasicTraits>
inline typename RAPIDCollision<BasicTraits>::FaceContainer&       
RAPIDCollision<BasicTraits>::getContactSecond ()
{
   return m_second;
}
template <class BasicTraits>
inline const typename RAPIDCollision<BasicTraits>::FaceContainer& 
RAPIDCollision<BasicTraits>::getContactSecond () const
{
   return m_second;
}

template <class BasicTraits>
u32 RAPIDCollision<BasicTraits>::getAdapterId() const
{
   return AdapterType::getAdapterId();
}
template <class BasicTraits>
inline const BoundingVolume<Real>& 
RAPIDCollision<BasicTraits>::getBoundingVolume (const Adapter* adapter) const
{
   const AdapterType* badapter = static_cast<const AdapterType*>(adapter);
   return badapter->getBoundingVolume();
}
template <class BasicTraits>
inline typename RAPIDCollision<BasicTraits>::CacheData& 
RAPIDCollision<BasicTraits>::getCacheData (CacheData&, const Adapter* adapter) const
{
   const AdapterType* badapter = static_cast<const AdapterType*>(adapter);
   return badapter->getObjectAdapter();
}


/*! \brief Traits class for usage of RAPIDCollision with double traverser.
 */
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING RAPIDCollisionTraits
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef typename RAPIDCollision<BasicTraits>::CacheData     CacheData;
   typedef typename RAPIDCollision<BasicTraits>::ResultT       ResultT;
   typedef RAPIDCollision<BasicTraits>                         ObjectT;
   typedef typename RAPIDCollision<BasicTraits>::AdapterType   GeneralType;
   typedef typename RAPIDCollision<BasicTraits>::AdapterType   GroupType;
   typedef typename RAPIDCollision<BasicTraits>::AdapterType   AdapterType;
   typedef K6Dop                                              BVol;
   typedef typename RAPIDCollision<BasicTraits>::TransformType TransformType;

   typedef InitSingleFunctor<bool,ObjectT>                    InitFunctorT;
   typedef InitDoubleFunctor<bool,GroupType,TransformType,GroupType,TransformType,ObjectT> InitDoubleFunctorT;
   typedef InitDoubleFunctor<bool,GroupType,TransformType,GroupType,TransformType,ObjectT> LeaveDoubleFunctorT;
   typedef DispatchFunctor<ResultT,GroupType,GroupType,ObjectT> BVolBVolFunctorT;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Functor creation.                                            */
   /*! \{                                                                 */
   static  InitFunctorT        createInitFunctor        (ObjectT* obj);
   static  InitDoubleFunctorT  createInitDoubleFunctor  (ObjectT* obj);
   static  LeaveDoubleFunctorT createLeaveDoubleFunctor (ObjectT* obj);
   static  BVolBVolFunctorT    createBVolBVolFunctor (ObjectT* obj);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};

END_GENVIS_NAMESPACE
#endif




