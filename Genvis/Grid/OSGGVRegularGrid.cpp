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

#include "OSGGVRegularGrid.h"
#include "OSGGVBVolAdapterBase.h"
#include "OSGGVIntersection.h"
#include "OSGGVBVolFunctions.h"
USING_GENVIS_NAMESPACE

/** Specialization for ADAPTER==BVolAdapterBase uses calcIntersect
    for each voxel entry.
 */
bool RegularGrid<BVolAdapterBase,std::vector<BVolAdapterBase*> >::calcIntersect (Intersection& hit)
{
   static FloatingComparator<Real> comp;
   RegularGridIter iter(*this, hit.getRay());
   bool  result = false;
   while (iter()) {
      const std::vector<AdapterType*>& list = m_voxel[iter.getIndex()];
      for (unsigned i=0; i<list.size(); ++i) {
	 result = list[i]->calcIntersect(hit) || result;
      }
      if (result && comp.leq(hit.getDist(), iter.getOutDist())) {
	 return true;      // return true only if hit is inside voxel
      }
   }
   return result;
}
/** Specialization for ADAPTER==BVolAdapterBase uses checkIntersect
    for each voxel entry.
 */
bool RegularGrid<BVolAdapterBase,std::vector<BVolAdapterBase*> >::checkIntersect (const Ray& ray)
{
   RegularGridIter iter(*this, ray);
   while (iter()) {
      const std::vector<AdapterType*>& list = m_voxel[iter.getIndex()];
      for (unsigned i=0; i<list.size(); ++i) {
	 if (list[i]->checkIntersect(ray)) {// ray did hit an item
	    return true;
	 }
      }
   }      
   return false;
}
