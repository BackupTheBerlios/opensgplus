//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.1 $
//   $Date: 2003/09/11 16:20:30 $
//                                                                            
//=============================================================================

#include "OSGGVRAPIDCollision.h"
#ifdef GV_WITH_RAPID
# include "RAPID.H"
#endif
USING_GENVIS_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING RAPIDCollision<OpenSGTraits>;

template <class BasicTraits>
RAPIDCollision<BasicTraits>::RAPIDCollision ()
  : Inherited()
{
#ifdef GV_WITH_RAPID
   m_mode = RAPID_ALL_CONTACTS;
#endif
}

template <class BasicTraits>
void RAPIDCollision<BasicTraits>::setStopFirst (bool flag)
{
   Inherited::setStopFirst(flag);
#ifdef GV_WITH_RAPID
   m_mode = (flag ? RAPID_FIRST_CONTACT : RAPID_ALL_CONTACTS);
#endif
}

