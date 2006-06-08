//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.4 $
//   $Date: 2006/06/08 17:03:30 $
//                                                                            
//=============================================================================

#include "OSGGVNodeAdapter.h"

#include "OSGGVTraits.h"
#include "OSGVector.h"
#include "OSGFieldContainerPtr.h"
#include "OSGNode.h"
OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

BEGIN_GENVIS_NAMESPACE

template <class BasicTraits>
OpenSGNodeAdapter<BasicTraits>::OpenSGNodeAdapter ()
  : Inherited()
{
}
template <class BasicTraits>
OpenSGNodeAdapter<BasicTraits>::~OpenSGNodeAdapter ()
{
}

template <class BasicTraits>
u32 OpenSGNodeAdapter<BasicTraits>::getAdapterId ()
{
   static u32 id = Adapter::getNextId();
   return id;
}

template <class BasicTraits>
OpenSGNodeAdapter<BasicTraits>::OpenSGNodeAdapter (const GeomObjectType& obj)
  : Inherited()
{
   init(obj);
}

template <class BasicTraits>
const BoundingVolume<Real>& OpenSGNodeAdapter<BasicTraits>::getBoundingVolume () const
{
   static K6Dop bvol;

   // get scenegraph bounding volume
   VectorClass minBound, maxBound;
   getObjectAdapter().getNode()->getVolume().getBounds(minBound, maxBound); 
   bvol.minVector()[0] = minBound[0];
   bvol.minVector()[1] = minBound[1];
   bvol.minVector()[2] = minBound[2];
   bvol.maxVector()[0] = maxBound[0];
   bvol.maxVector()[1] = maxBound[1];
   bvol.maxVector()[2] = maxBound[2];

   return bvol;
}

template <class BasicTraits>
void  OpenSGNodeAdapter<BasicTraits>::init (const GeomObjectType& obj)
{
   setOriginal(obj);
}

template <class BasicTraits>
std::ostream& OpenSGNodeAdapter<BasicTraits>::dump (std::ostream& os) const
{
   os << "OpenSGNodeAdapter(" << getOriginal() << ")";
   return os;
}

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING  OpenSGNodeAdapter<OpenSGTraits>;

END_GENVIS_NAMESPACE
