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
//   $Date: 2004/12/20 15:59:31 $
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
class OSG_GENVISLIB_DLLMAPPING RAPIDCollision : public BVolCollisionBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef BVolCollisionBase<BasicTraits>         Inherited;
   typedef typename Inherited::Cache              Cache;
   typedef typename Inherited::CacheData          CacheData;
   typedef typename Inherited::TransformType      TransformType;
   typedef typename Inherited::AdapterType        AdapterType;
   typedef typename Inherited::CollisionPair      CollisionPair;
   typedef typename Inherited::CollisionContainer CollisionContainer;

   typedef OpenSGRAPIDAdapter<BasicTraits>        ObjectAdapterType;
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
   virtual bool              Init (); 
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   i32                m_mode;
   i32                m_last;
   FaceContainer      m_first;
   FaceContainer      m_second;
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

END_GENVIS_NAMESPACE
#endif

