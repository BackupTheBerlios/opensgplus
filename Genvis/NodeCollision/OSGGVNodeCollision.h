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
//   $Revision: 1.2 $
//   $Date: 2004/03/12 13:27:49 $
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
class OSG_GENVISLIB_DLLMAPPING NodeCollision : public DataBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef DataBase<BasicTraits>               Inherited;
   typedef typename Inherited::Cache           Cache;
   typedef typename Inherited::CacheData       CacheData;

   typedef typename BasicTraits::TransformType TransformType;
   typedef OpenSGNodeAdapter<BasicTraits>      AdapterType;
   typedef CollisionData<AdapterType>          CollisionPair;
   typedef std::vector<CollisionPair>          CollisionContainer;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline NodeCollision ();
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
   inline u32                  getNumContacts () const;
   inline CollisionContainer&       getContacts ();
   inline const CollisionContainer& getContacts () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   u32           m_numTests;
   CollisionContainer m_contacts;
};
typedef  NodeCollision<OpenSGTraits>    OSGNodeCollision;

template <class BasicTraits>
inline NodeCollision<BasicTraits>::NodeCollision ()
{
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

END_GENVIS_NAMESPACE
#endif

