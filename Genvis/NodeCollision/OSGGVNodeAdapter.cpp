//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.2 $
//   $Date: 2004/03/12 13:27:49 $
//                                                                            
//=============================================================================

#include "OSGGVNodeAdapter.h"

#include "OSGGVTraits.h"
#include "OSGVector.h"
#include "OSGFieldContainerPtr.h"
#include "OSGNode.h"
USING_GENVIS_NAMESPACE
OSG_USING_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING  OpenSGNodeAdapter<OpenSGTraits>;

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
void  OpenSGNodeAdapter<BasicTraits>::init (const GeomObjectType& obj)
{
   setOriginal(obj);
}

template <class BasicTraits>
std::ostream& OpenSGNodeAdapter<BasicTraits>::dump (std::ostream& os) const
{
   os << "OpenSGNodeAdapter";
   return os;
}

