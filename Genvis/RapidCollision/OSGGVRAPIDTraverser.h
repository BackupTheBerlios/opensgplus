// ---------------------------------------------------------------
// File:    $Id: OSGGVRAPIDTraverser.h,v 1.1 2003/09/11 16:20:31 fuenfzig Exp $
// Author:  Christoph Fuenfzig, <c.fuenfzig@cg.cs.tu-bs.de>
// ---------------------------------------------------------------

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
   typedef AllTraverserBase<BasicTraits>   Inherited;
   typedef typename BasicTraits::TransformType TransformType;
   typedef OpenSGRAPIDAdapter<BasicTraits> ObjectAdapterType;
   typedef RAPIDCollision<BasicTraits>     ObjectT;
   typedef ObjectT::FaceAdapterType        FaceAdapterType;
   typedef ObjectT::FaceContainer          FaceContainer;
   typedef ObjectT::CollisionPair          CollisionPair;
   typedef ObjectT::CollisionContainer     CollisionContainer;

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
inline RAPIDAllTraverser<BasicTraits>::ObjectT&       
RAPIDAllTraverser<BasicTraits>::getDataTyped ()
{
   return m_data;
}
template <class BasicTraits>
inline const RAPIDAllTraverser<BasicTraits>::ObjectT& 
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



