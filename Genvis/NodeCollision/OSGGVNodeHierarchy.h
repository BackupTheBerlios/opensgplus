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

#ifndef OSGGVNODEHIERARCHY_H
#define OSGGVNODEHIERARCHY_H

#include "OSGGVBase.h"
#include "OSGGVStaticInput.h"
#include "OSGGVTraits.h"
#include "OSGGVInput.h"
#include "OSGGVSingleBVolHierarchy.h"
#include "OSGGVNodeAdapter.h"
#include "OSGGVAdapterFactory.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Adapter/Hierarchy management for collision detection with the 
    bounding volume in OpenSG nodes.
 */
template<class BasicTraits> 
class OSG_GENVISLIB_DLLMAPPING NodeHierarchy 
: public SingleBVolHierarchyBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef SingleBVolHierarchyBase<BasicTraits>     Inherited;
   typedef OpenSGNodeAdapter<BasicTraits>           AdapterType;
   typedef typename AdapterType::FactoryType        FactoryType;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   NodeHierarchy ();
   virtual ~NodeHierarchy ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Adapter creation.                                            */
   /*! \{                                                                 */
   void addAdapter (const GeomObjectType& obj);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Memory management.                                           */
   /*! \{                                                                 */
   virtual void destroy ();
   virtual void clear   ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Hierarchy creation.                                          */
   /*! \{                                                                 */
   virtual void hierarchy ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Registration for traversal with cache.                       */
   /*! \{                                                                 */
   virtual void registerFunctors ();
   static void staticEnterGeometry (osg::CNodePtr&, OSGStaticInput*);
#ifndef OSG_NOFUNCTORS
   void functorEnterGeometry       (osg::CNodePtr&, OSGStaticInput*);
#endif
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   std::vector<AdapterType*>     m_leaf;
};
typedef  NodeHierarchy<OpenSGTraits> OSGNodeHierarchy;

END_GENVIS_NAMESPACE
#endif



