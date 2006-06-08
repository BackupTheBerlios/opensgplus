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
//   $Date: 2006/06/08 17:03:30 $
//                                                                            
//=============================================================================

#ifndef OSGGVNODECOLLISION_H
#define OSGGVNODECOLLISION_H

#include <vector>
#include "OSGGVBase.h"
#include "OSGGVCollisionData.h"
#include "OSGGVBVolCollision.h"
#include "OSGGVNodeAdapter.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief DataBase for collision detection with the bounding volumes 
    in OpenSG nodes (without any hierarchy).
 */
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING NodeCollision 
: public DataBase<BasicTraits>, public DoubleTraverserBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef NodeCollision<BasicTraits>          Self;
   typedef DataBase<BasicTraits>               Inherited;
   typedef typename Inherited::Cache           Cache;
   typedef typename Inherited::CacheData       CacheData;

   typedef typename DoubleTraverserBase<BasicTraits>::ResultType ResultT;
   typedef typename BasicTraits::TransformType TransformType;
   typedef OpenSGNodeAdapter<BasicTraits>      AdapterType;
   typedef OpenSGNodeAdapter<BasicTraits>      GroupType;
   typedef CollisionData<AdapterType>          CollisionPair;
   typedef std::vector<CollisionPair>          CollisionContainer;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline NodeCollision ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Member.                                                      */
   /*! \{                                                                 */
   inline bool                      getStopFirst () const;
   inline void                      setStopFirst (bool flag=false);
   virtual inline DataBase<BasicTraits>&       getData ()       { return *this; }
   virtual inline const DataBase<BasicTraits>& getData () const { return *this; }
   inline Self&                                getDataTyped ()  { return *this; }
   inline const Self&                          getDataTyped () const { return *this; }
   /*! Get the integer identifier used by the AdapterType. This virtual method 
       is used to propagate the identifier from the DoubleTraverser to the 
       AllTraverser. */
   virtual inline u32                         getAdapterId() const;
   /*! Get the bounding volume for an adapter object for use in AllTraverser. */
   virtual inline const BoundingVolume<Real>& getBoundingVolume (const Adapter* adapter) const;
   /*! */
   virtual inline CacheData& getCacheData (CacheData& data, const Adapter* adapter) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Collision statistics.                                        */
   /*! \{                                                                 */   
   inline u32                  getNumPrimTests  () const;
   inline u32                  getNumMixedTests () const;
   inline u32&                 getNumBVolTests  ();
   inline const u32&           getNumBVolTests  () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Collision results.                                           */
   /*! \{                                                                 */   
   inline u32                       getNumContacts () const;
   inline CollisionContainer&       getContacts ();
   inline const CollisionContainer& getContacts () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Init.                                                        */
   /*! \{                                                                 */   
   virtual bool  Init ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Traversal functions.                                         */
   /*! \{                                                                 */
   /*! Init for the single pair in the arguments.                         */
   bool        InitDouble  (GroupType* root0, const TransformType& m0, const TransformType& f0, 
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
   typedef std::map<AdapterType*, OSG::DynamicVolume, PtrLess<AdapterType> > VolumeMap;
   VolumeMap          m_volume;
   bool               m_stopFirst;
   u32                m_numTests;
   CollisionContainer m_contacts;
};
typedef  NodeCollision<OpenSGTraits>    OSGNodeCollision;

template <class BasicTraits>
inline NodeCollision<BasicTraits>::NodeCollision ()
{
}

template <class BasicTraits>
inline bool                      NodeCollision<BasicTraits>::getStopFirst () const
{
   return m_stopFirst;
}
template <class BasicTraits>
inline void                      NodeCollision<BasicTraits>::setStopFirst (bool flag)
{
   m_stopFirst = flag;
}

template <class BasicTraits>
inline u32 NodeCollision<BasicTraits>::getNumPrimTests () const
{
   return 0;
}
template <class BasicTraits>
inline u32 NodeCollision<BasicTraits>::getNumMixedTests () const
{
   return 0;
}
template <class BasicTraits>
inline u32& NodeCollision<BasicTraits>::getNumBVolTests ()
{
   return m_numTests;
}
template <class BasicTraits>
inline const u32& NodeCollision<BasicTraits>::getNumBVolTests () const
{
   return m_numTests;
}
template <class BasicTraits>
inline u32 NodeCollision<BasicTraits>::getNumContacts () const
{
   return m_contacts.size();
}
template <class BasicTraits>
inline typename NodeCollision<BasicTraits>::CollisionContainer& 
NodeCollision<BasicTraits>::getContacts ()
{
   return m_contacts;
}
template <class BasicTraits>
inline const typename NodeCollision<BasicTraits>::CollisionContainer& 
NodeCollision<BasicTraits>::getContacts () const
{
   return m_contacts;
}

template <class BasicTraits>
inline u32 NodeCollision<BasicTraits>::getAdapterId() const
{
   return AdapterType::getAdapterId();
}
template <class BasicTraits>
inline const BoundingVolume<Real>& NodeCollision<BasicTraits>::getBoundingVolume (const Adapter* adapter) const
{
   const AdapterType* badapter = static_cast<const AdapterType*>(adapter);
   return badapter->getBoundingVolume();
}
template <class BasicTraits>
inline typename NodeCollision<BasicTraits>::CacheData& 
NodeCollision<BasicTraits>::getCacheData (CacheData&, const Adapter* adapter) const
{
   const AdapterType* badapter = static_cast<const AdapterType*>(adapter);
   return badapter->getObjectAdapter();
}



/*! \brief Traits class for usage of NodeCollision with double traverser.
 */
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING NodeCollisionTraits
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef typename NodeCollision<BasicTraits>::CacheData     CacheData;
   typedef typename NodeCollision<BasicTraits>::ResultT       ResultT;
   typedef NodeCollision<BasicTraits>                         ObjectT;
   typedef typename NodeCollision<BasicTraits>::AdapterType   GeneralType;
   typedef typename NodeCollision<BasicTraits>::AdapterType   GroupType;
   typedef typename NodeCollision<BasicTraits>::AdapterType   AdapterType;
   typedef K6Dop                                              BVol;
   typedef typename NodeCollision<BasicTraits>::TransformType TransformType;

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

