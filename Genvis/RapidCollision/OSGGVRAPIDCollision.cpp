//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.2 $
//   $Date: 2004/03/12 13:28:31 $
//                                                                            
//=============================================================================

#include <assert.h>
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
   m_last = 0;
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

template<class BasicTraits>
bool RAPIDCollision<BasicTraits>::Init ()
{
   // clears getContacts()
   bool result = Inherited::Init();
   // clear contact containers
   getContactFirst().reserve(GV_MAX_CONTACTS);
   getContactSecond().reserve(GV_MAX_CONTACTS);
   if (m_last > 0) {
      getContactFirst().erase (getContactFirst().begin(),  getContactFirst().begin()+(m_last)); 
      getContactSecond().erase(getContactSecond().begin(), getContactSecond().begin()+(m_last)); 
   }
   assert(getContactFirst().size() == getContactSecond().size());
   m_last = getContactFirst().size();
   return result;
}

