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
//   $Date: 2004/12/20 15:59:08 $
//                                                                            
//=============================================================================

#ifndef OSGGVNODETRAVERSER_H
#define OSGGVNODETRAVERSER_H

#include "OSGGVBase.h"
#include "OSGGVTraits.h"
#include "OSGGVStaticInput.h"
#include "OSGGVInput.h"
#include "OSGGVAllTraverser.h"
#include "OSGGVNodeCollision.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Traverser for collision detection with the bounding volume 
    in OpenSG nodes (without any hierarchy). 
    Mention that this cannot be very precise! 
 */
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING NodeAllTraverser : public AllTraverserBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef AllTraverserBase<BasicTraits>        Inherited;
   typedef typename Inherited::Cache            Cache;
   typedef typename Inherited::CacheData        CacheData;

   typedef typename BasicTraits::TransformType  TransformType;
   typedef NodeCollision<BasicTraits>           ObjectT;
   typedef typename ObjectT::AdapterType        AdapterType;
   typedef typename ObjectT::CollisionPair      CollisionPair;
   typedef typename ObjectT::CollisionContainer CollisionContainer;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline NodeAllTraverser ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name DataBase for collision result.                               */
   /*! \{                                                                 */   
   virtual inline DataBase<BasicTraits>&       getData ();
   virtual inline const DataBase<BasicTraits>& getData () const;
   inline ObjectT&                             getDataTyped ();
   inline const ObjectT&                       getDataTyped () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Apply.                                                       */
   /*! \{                                                                 */   
   virtual bool apply   (const OSG::NodePtr& node0, const OSG::NodePtr& node1);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   ObjectT m_data;
};
typedef  NodeAllTraverser<OpenSGTraits> OSGNodeTraverser;

template <class BasicTraits>
inline NodeAllTraverser<BasicTraits>::NodeAllTraverser ()
  : Inherited()
{
}

template <class BasicTraits>
inline typename NodeAllTraverser<BasicTraits>::ObjectT&       
NodeAllTraverser<BasicTraits>::getDataTyped ()
{
   return m_data;
}
template <class BasicTraits>
inline const typename NodeAllTraverser<BasicTraits>::ObjectT& 
NodeAllTraverser<BasicTraits>::getDataTyped () const
{
   return m_data;
}
template <class BasicTraits>
inline DataBase<BasicTraits>&       
NodeAllTraverser<BasicTraits>::getData ()
{
   return m_data;
}
template <class BasicTraits>
inline const DataBase<BasicTraits>& 
NodeAllTraverser<BasicTraits>::getData () const
{
   return m_data;
}

END_GENVIS_NAMESPACE
#endif



