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
//   $Date: 2004/03/12 13:28:31 $
//                                                                            
//=============================================================================

#ifndef OSGGVRAPIDTRAVERSER_H
#define OSGGVRAPIDTRAVERSER_H

#include "OSGGVBase.h"
#include "OSGGVTraits.h"
#include "OSGGVStaticInput.h"
#include "OSGGVInput.h"
#include "OSGGVAllTraverser.h"
#include "OSGGVRAPIDCollision.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Traverser for collision detection with RAPID. 
 */
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING RAPIDAllTraverser : public AllTraverserBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef AllTraverserBase<BasicTraits>        Inherited;
   typedef typename Inherited::Cache            Cache;
   typedef typename Inherited::CacheData        CacheData;

   typedef typename BasicTraits::TransformType  TransformType;
   typedef RAPIDCollision<BasicTraits>          ObjectT;
   typedef typename ObjectT::ObjectAdapterType  ObjectAdapterType;
   typedef typename ObjectT::FaceAdapterType    FaceAdapterType;
   typedef typename ObjectT::FaceContainer      FaceContainer;
   typedef typename ObjectT::CollisionPair      CollisionPair;
   typedef typename ObjectT::CollisionContainer CollisionContainer;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline RAPIDAllTraverser ();
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
   virtual bool apply (const OSG::NodePtr& node0, const OSG::NodePtr& node1);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   ObjectT m_data;
};
typedef RAPIDAllTraverser<OpenSGTraits> OSGRAPIDTraverser;

template <class BasicTraits>
inline RAPIDAllTraverser<BasicTraits>::RAPIDAllTraverser ()
  : Inherited()
{
}

template <class BasicTraits>
inline typename RAPIDAllTraverser<BasicTraits>::ObjectT&       
RAPIDAllTraverser<BasicTraits>::getDataTyped ()
{
   return m_data;
}
template <class BasicTraits>
inline const typename RAPIDAllTraverser<BasicTraits>::ObjectT& 
RAPIDAllTraverser<BasicTraits>::getDataTyped () const
{
   return m_data;
}
template <class BasicTraits>
inline DataBase<BasicTraits>&       
RAPIDAllTraverser<BasicTraits>::getData ()
{
   return m_data;
}
template <class BasicTraits>
inline const DataBase<BasicTraits>& 
RAPIDAllTraverser<BasicTraits>::getData () const
{
   return m_data;
}

END_GENVIS_NAMESPACE
#endif



