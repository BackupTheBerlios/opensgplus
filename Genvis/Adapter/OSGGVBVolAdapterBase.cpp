//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.2 $
//   $Date: 2004/03/12 13:12:36 $
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


u32 BVolAdapterBase::getAdapterId ()
{
   static u32 id = getNextId();
   return id;
}

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

bool BVolAdapterBase::calcIntersect (Intersection& in)
{
   const Ray& ray = in.getRay();
   if (getBoundingVolume().calcIntersect(ray.getOrigin(), ray.getDirection(), in.getDist())) { 
      // intersection on this object
      in.setTo(this);
      in.setData(NULL);
      return true;
   }
   return false;

}
bool BVolAdapterBase::checkIntersect (const Intersection& in)
{
   return getBoundingVolume()
     .checkIntersect(in.getRay().getOrigin(), in.getRay().getDirection(), in.getDist());
}
VectorClass BVolAdapterBase::getSurfaceNormal (const Intersection& in)
{
   IntersectionNormal* data = dynamic_cast<IntersectionNormal*>(in.getData());
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

