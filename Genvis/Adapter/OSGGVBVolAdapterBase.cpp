//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.1 $
//   $Date: 2003/09/11 16:20:29 $
//                                                                            
//=============================================================================

#include "OSGGVBVolAdapterBase.h"
USING_GENVIS_NAMESPACE

// explicit template instantiations
template class BVolAdapter<K6Dop>;
template class BVolAdapter<K14Dop>;
template class BVolAdapter<K18Dop>;
template class BVolAdapter<K26Dop>;
template class BVolAdapter<K12Dop>;
template class BVolAdapter<K20Dop>;


Real BVolAdapterBase::getSurfaceArea ()
{
  return getBoundingVolume().getSurfaceArea();
}

PointClass BVolAdapterBase::getCenter()
{
  return getBoundingVolume().getCenter();   
}

std::ostream& BVolAdapterBase::dump (std::ostream& os) const
{
   os << getBoundingVolume();
   return os;
}

bool BVolAdapterBase::calcIntersect (Intersection& hit)
{
   const Ray& ray = hit.getRay();
   Real t = hit.getDist();
   if (getBoundingVolume().testIntersect(ray.getOrigin(), ray.getDirection(), t)) { 
      // intersection on this object
      hit.setDist(t);
      hit.setTo(this);
      hit.setData(NULL);
      return true;
   }
   return false;

}
bool BVolAdapterBase::checkIntersect (const Ray& ray)
{
   Real t = -1;
   return getBoundingVolume().testIntersect(ray.getOrigin(), ray.getDirection(), t);
}
VectorClass BVolAdapterBase::getSurfaceNormal (const Intersection& hit)
{
   IntersectionNormal* data 
     = dynamic_cast<IntersectionNormal*>(hit.getData());
   assert(data != NULL);
   return data->getSurfaceNormal();
}

void BVolAdapterBase::update ()
{
}

void BVolAdapterBase::draw          ()
{
}

void BVolAdapterBase::drawWireframe ()
{
}
