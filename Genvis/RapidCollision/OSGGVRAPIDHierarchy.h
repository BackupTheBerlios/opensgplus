// ---------------------------------------------------------------
// File:    $Id: OSGGVRAPIDHierarchy.h,v 1.1 2003/09/11 16:20:31 fuenfzig Exp $
// Author:  Christoph Fuenfzig, <c.fuenfzig@cg.cs.tu-bs.de>
// ---------------------------------------------------------------
// hierarchy builder

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
typedef  RAPIDHierarchy<OpenSGTraits>   OSGRAPIDHierarchy;

END_GENVIS_NAMESPACE
#endif



