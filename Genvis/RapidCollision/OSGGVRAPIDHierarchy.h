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

#ifndef OSGGVRAPIDHIERARCHY_H
#define OSGGVRAPIDHIERARCHY_H

#include "OSGGVBase.h"
#include "OSGGVStaticInput.h"
#include "OSGGVTraits.h"
#include "OSGGVInput.h"
#include "OSGGVSingleBVolHierarchy.h"
#include "OSGGVRAPIDAdapter.h"
#include "OSGGVNodeAdapter.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Adapter/Hierarchy management for collision detection with RAPID. 
 */
template<class BasicTraits> 
class OSG_GENVISLIB_DLLMAPPING RAPIDHierarchy 
: public SingleBVolHierarchyBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef SingleBVolHierarchyBase<BasicTraits>     Inherited;
   typedef typename Inherited::TransformType        TransformType;
   typedef typename Inherited::GeomObjectType       GeomObjectType;
   typedef typename Inherited::Cache                Cache;
   typedef typename Inherited::CacheData            CacheData;

   typedef OpenSGRAPIDAdapter<BasicTraits>          AdapterType;
   typedef typename AdapterType::FactoryType        FactoryType;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   RAPIDHierarchy ();
   virtual ~RAPIDHierarchy ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Adapter creation.                                            */
   /*! \{                                                                 */
   void         addAdapter (const GeomObjectType& obj);
   virtual void addAdapter (const TransformType&  m2w,
			    const GeomObjectType& obj);
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
   virtual void process (const GeomObjectType& node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Registration for traversal with cache.                       */
   /*! \{                                                                 */
   virtual void registerFunctors ();
   static bool staticEnterGeometry (osg::CNodePtr& cnode, OSGStaticInput* input);
#ifndef OSG_NOFUNCTORS
   bool functorEnterGeometry       (osg::CNodePtr& cnode, OSGStaticInput* input);
#endif
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   void    hierarchyLocal  (const GeomObjectType& node);
   void    hierarchyGlobal (const GeomObjectType& node);

   std::vector<AdapterType*> m_leaf;
   AdapterType*              m_current;
};
typedef  RAPIDHierarchy<OpenSGTraits>   OSGRAPIDHierarchy;

END_GENVIS_NAMESPACE
#endif



