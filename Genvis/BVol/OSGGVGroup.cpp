//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.3 $
//   $Date: 2004/12/20 15:57:11 $
//                                                                            
//=============================================================================

#include "OSGGVGroup.h"
USING_GENVIS_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING BVolGroup<K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING BVolGroup<K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING BVolGroup<K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING BVolGroup<K26Dop>;
template class OSG_GENVISLIB_DLLMAPPING BVolGroup<K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING BVolGroup<K20Dop>;


// class BVolGroupInterface
bool BVolGroupInterface::isLeaf () const
{
   return false;
}


// class BVolGroupBase
BVolGroupBase::~BVolGroupBase ()
{
}
BVolAdapterBase* BVolGroupBase::getSon (u32 i) const
{
   assert(i < m_sons.size());
   return m_sons[i];
}
u32 BVolGroupBase::size () const
{
   return m_sons.size();
}

std::ostream& BVolGroupBase::dump (std::ostream& os) const
{
   os << "BVolGroupBase(";
   Inherited::dump(os) << ", " << getSons().size() 
		       << " childs={" << std::endl;
   for (ConstIterator iter = getSons().begin();
	iter != getSons().end(); 
	++iter) {
     os << ' ' << (*iter);
   }
   os << std::endl << "})" << std::endl;
   return os;
}


// template class BVolGroup<BVOL>
template <class BVOL>
bool    BVolGroup<BVOL>::calcIntersect     (Intersection& in)
{
   if (checkIntersect(in)) {
      in.setDist(-1);
      bool result = false;
      for (Iterator iter = getSons().begin();   
	   iter != getSons().end(); 
	   ++iter) {
	 if ((*iter)->calcIntersect(in)) {
	    result = true;
	 }
      }
      return result;
   }
   return false;
}

template <class BVOL>
void BVolGroup<BVOL>::draw          ()
{
#if 0
   // fixed grey color
   glColor3f(0.5f, 0.5f, 0.5f);
   getBoundingVolume().draw();
#else
   // random color
   glColor3f(GLfloat(rand())/RAND_MAX, 
	     GLfloat(rand())/RAND_MAX, 
	     GLfloat(rand())/RAND_MAX);
#endif
}
template <class BVOL>
void BVolGroup<BVOL>::drawWireframe ()
{
   // fixed grey color
   //glColor3f(0.8f, 0.8f, 0.8f);
   getBoundingVolume().drawWireframe();
}

