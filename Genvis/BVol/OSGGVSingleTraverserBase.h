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
//   $Date: 2004/12/20 15:57:11 $
//                                                                            
//=============================================================================

#ifndef OSGGVSINGLETRAVERSERBASE_H
#define OSGGVSINGLETRAVERSERBASE_H

#include "OSGGVBase.h"
#include "OSGGVFunctors.h"
#include "OSGGVTraits.h"
#include "OSGGVInput.h"
#include "OSGGVGroup.h"
#include "OSGGVBVolAdapterExt.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Base single traverser.
 */
template <class BasicTraits>
class SingleTraverserBase
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef typename BasicTraits::Cache          Cache;
   typedef typename BasicTraits::CacheData      CacheData;
   typedef typename BasicTraits::GeomObjectType GeomObjectType;
   typedef typename BasicTraits::TransformType  TransformType;
   /*! \name Result type.                                                 */
   enum ResultType {
      CONTINUE, // continue traversal
      QUIT      // quit traversal
   };
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline SingleTraverserBase ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Apply.                                                       */
   /*! \{                                                                 */   
   virtual bool apply (const GeomObjectType& node) = 0;
   virtual bool apply (CacheData& data, BVolAdapterBase* node, 
		       const TransformType& m=TransformType::identity()) = 0;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   static inline CacheData& getCacheData (CacheData& data, BVolAdapterBase* adapter);
};
typedef  SingleTraverserBase<OpenSGTraits>  OSGBVolTraverser;


template <class BasicTraits>
inline SingleTraverserBase<BasicTraits>::SingleTraverserBase ()
{
}

template <class BasicTraits>
inline typename SingleTraverserBase<BasicTraits>::CacheData& 
SingleTraverserBase<BasicTraits>::getCacheData (CacheData& data, BVolAdapterBase* group)
{
   if (group->getParent() == NULL) { // no parent
      return data;
   }
   // triangle adapter
   OpenSGTriangleBase<BasicTraits>*  tri = 
     dynamic_cast<OpenSGTriangleBase<BasicTraits>*>(group->getParent());
   if (tri != NULL) {
      return tri->getObjectAdapter();
   }
   // object adapter
   OpenSGObjectBase<BasicTraits>* obj = 
     dynamic_cast<OpenSGObjectBase<BasicTraits>*>(group->getParent());
   if (obj != NULL) {
      return obj->getObjectAdapter();
   }
   // no adapter
   return data;
}

END_GENVIS_NAMESPACE
#endif
