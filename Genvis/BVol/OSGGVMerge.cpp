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

#include "OSGGVMerge.h"
USING_GENVIS_NAMESPACE

#undef GV_COMPACTNESS_CRITERIUM

MergeLevels::MergeLevels (unsigned level)
  : m_level(level)
{
}

bool MergeLevels::compact(BVolGroupBase* parent, 
			  BVolAdapterBase* son1, BVolAdapterBase* son2)
{
   const BVol& bvol  = parent->getBoundingVolume();
   const BVol& bvol1 = son1->getBoundingVolume();
   const BVol& bvol2 = son2->getBoundingVolume();

   Real s  = bvol.getSurfaceArea();
   Real s1 = bvol1.getSurfaceArea();
   Real s2 = bvol2.getSurfaceArea();

   Real v  = bvol.getVolume();
   Real v1 = bvol1.getVolume();
   Real v2 = bvol2.getVolume();

   const Real c_s = 0.50;
   const Real c_v = 0.25;

   if (s1+s2 < c_s*s) 
      return false;
   if (v1+v2 < c_v*v) 
      return false;
   return true;
}

void MergeLevels::merge (BVolGroupBase* scene,
			 std::vector<BVolAdapterBase*>& addList,
			 unsigned level)
{
   const std::vector<BVolAdapterBase*>& sons = scene->getSons();
#ifdef GV_COMPACTNESS_CRITERIUM
   if (sons.size() == 2) {
      if (!compact(scene, sons[0], sons[1])) {
         addList.push_back(sons[0]);
         addList.push_back(sons[1]);
         return;
      }
   }
#endif

   for (std::vector<BVolAdapterBase*>::const_iterator iter = sons.begin();
	iter != sons.end(); 
	++iter) {
      BVolAdapterBase* subNode  = *iter;
      BVolGroupBase*  subScene = dynamic_cast<BVolGroupBase*>(subNode);
      if (subScene != NULL) {
 	 if (level < m_level) {
	    merge(subScene, addList, level+1);
	    subScene->getSons().clear();
	    //delete subScene;
	 } else {
	    addList.push_back(subScene);
	 }
      } else {
         addList.push_back(subNode);
      }
   }
}

void MergeLevels::merge (BVolGroupBase* scene)
{
   if (scene == NULL) {
      return;
   }
   // one merging step
   std::vector<BVolAdapterBase*> addList;
   merge(scene, addList, 0);
   std::vector<BVolAdapterBase*>& sons = scene->getSons();
   sons = addList;

   // merging of subhierarchy
   for (std::vector<BVolAdapterBase*>::const_iterator iter = sons.begin();
	iter != sons.end(); 
	++iter) {
      BVolAdapterBase* subNode = *iter;
      BVolGroupBase*  subScene = dynamic_cast<BVolGroupBase*>(subNode);
      if (subScene != NULL) {
	 merge(subScene);
      }
   }
}
