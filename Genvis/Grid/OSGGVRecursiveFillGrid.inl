//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.2 $
//   $Date: 2004/03/12 13:21:21 $
//                                                                            
//=============================================================================


template <class ADAPTER, class CONTAINER>
inline RecursiveFillGrid<ADAPTER,CONTAINER>::RecursiveFillGrid (GridType& grid)
  : m_grid(&grid)
{
}
template <class ADAPTER, class CONTAINER>
inline u32        RecursiveFillGrid<ADAPTER,CONTAINER>::getNumNonEmptyVoxels () const
{
   return m_numNonEmpty;
}
template <class ADAPTER, class CONTAINER>
inline typename RecursiveFillGrid<ADAPTER,CONTAINER>::GridType& 
RecursiveFillGrid<ADAPTER,CONTAINER>::getGrid () const
{
   assert(m_grid != NULL);
   return *m_grid;
}

template <class ADAPTER, class CONTAINER>
inline void     RecursiveFillGrid<ADAPTER,CONTAINER>::fillVoxels 
(const std::vector<Adapter*>& primitives,
 u32 from_x, u32 to_x, 
 u32 from_y, u32 to_y, 
 u32 from_z, u32 to_z)
{
   static FloatingComparator<Real> comp;

   u32 dx = to_x - from_x;   // voxel area extends
   u32 dy = to_y - from_y;
   u32 dz = to_z - from_z;

   if (primitives.size() == 0) {
      return;
   }

   // compute bounding box for current voxel area
   VectorClass offset(getGrid().getLength()[0] * ((Real)from_x + (Real)dx/2),
		      getGrid().getLength()[1] * ((Real)from_y + (Real)dy/2),
		      getGrid().getLength()[2] * ((Real)from_z + (Real)dz/2));

   // remove all objects outside current bounding box
   K6Dop voxelBox (getGrid().getRefCenter() + offset,
		   (Real)getGrid().getHalfLength()[0]*(dx+1),
		   (Real)getGrid().getHalfLength()[1]*(dy+1),
		   (Real)getGrid().getHalfLength()[2]*(dz+1));

   if (dx == 0 && dy == 0 && dz == 0) { // voxel area is single voxel !
       //!(dx|dy|dz)) {
       ++m_numNonEmpty;
       for (i64 i=0; i<primitives.size(); ++i) {
	  BVolAdapterBase* prim = static_cast<BVolAdapterBase*>(primitives[i]);
	  if (prim->getBoundingVolume().checkIntersect(voxelBox)) {
	     getGrid().primitives(from_x,from_y,from_z).push_back(prim);
	  }
       }
   } else {
      // filter primitives into inVoxelPrimitives
      std::vector<Adapter*> inVoxelPrimitives;
      for (i64 i=0; i<primitives.size(); ++i) {
	 BVolAdapterBase* prim = static_cast<BVolAdapterBase*>(primitives[i]);
	 if (prim->getBoundingVolume().checkIntersect(voxelBox)) {
	    inVoxelPrimitives.push_back(prim);
	 }
      }

      // find largest extend
      u32 maxdir = 2;
      if (dx < dy) {
         if(dy > dz) maxdir = 1;
      } else {
         if(dx > dz) maxdir = 0;
      }

      // prepare voxel split along largest extends
      u32 t1x=to_x, t1y=to_y, t1z=to_z;
      u32 f2x=from_x, f2y=from_y, f2z=from_z;
      switch (maxdir) {
      case 0: 
	t1x = from_x + dx/2;
	f2x = t1x + 1;
	break;
      case 1: 
	t1y = from_y + dy/2;
	f2y = t1y + 1;
	break;
      case 2: 
	t1z = from_z + dz/2;
	f2z = t1z + 1;
	break;
      }
      // perform voxel split
      fillVoxels(inVoxelPrimitives, from_x,t1x, from_y,t1y, from_z,t1z);
      fillVoxels(inVoxelPrimitives, f2x,to_x, f2y,to_y, f2z,to_z);
   }
}
template <class ADAPTER, class CONTAINER>
inline u32 RecursiveFillGrid<ADAPTER,CONTAINER>::fillVoxels 
(const std::vector<Adapter*>& primitives)
{
   m_numNonEmpty = 0;
   // fill voxel-array
   fillVoxels(primitives,
	      0, getGrid().getNumVoxelsDim()[0]-1, 
	      0, getGrid().getNumVoxelsDim()[1]-1, 
	      0, getGrid().getNumVoxelsDim()[2]-1);
   return m_numNonEmpty;
}


template <class ADAPTER, class CONTAINER>
inline void     RecursiveFillGrid<ADAPTER,CONTAINER>::fillVoxels 
(const std::vector<PointerType>& primitives,
 u32 from_x, u32 to_x, 
 u32 from_y, u32 to_y, 
 u32 from_z, u32 to_z)
{
   static FloatingComparator<Real> comp;

   u32 dx = to_x - from_x;   // voxel area extends
   u32 dy = to_y - from_y;
   u32 dz = to_z - from_z;

   if (primitives.size() == 0) {
      return;
   }

   // compute bounding box for current voxel area
   VectorClass offset(getGrid().getLength()[0] * ((Real)from_x + (Real)dx/2),
		      getGrid().getLength()[1] * ((Real)from_y + (Real)dy/2),
		      getGrid().getLength()[2] * ((Real)from_z + (Real)dz/2));

   // remove all objects outside current bounding box
   K6Dop voxelBox (getGrid().getRefCenter() + offset,
		   (Real)getGrid().getHalfLength()[0]*(dx+1),
		   (Real)getGrid().getHalfLength()[1]*(dy+1),
		   (Real)getGrid().getHalfLength()[2]*(dz+1));

   if (dx == 0 && dy == 0 && dz == 0) { // voxel area is single voxel !
       //!(dx|dy|dz)) {
       ++m_numNonEmpty;
       for (i64 i=0; i<primitives.size(); ++i) {
	  if (primitives[i]->getBoundingVolume().checkIntersect(voxelBox)) {
	     getGrid().primitives(from_x,from_y,from_z).push_back(primitives[i]);
	  }
       }
   } else {
      // filter primitives into inVoxelPrimitives
      std::vector<PointerType> inVoxelPrimitives;
      for (i64 i=0; i<primitives.size(); ++i) {
	 if (primitives[i]->getBoundingVolume().checkIntersect(voxelBox)) {
	    inVoxelPrimitives.push_back(primitives[i]);
	 }
      }

      // find largest extend
      u32 maxdir = 2;
      if (dx < dy) {
         if(dy > dz) maxdir = 1;
      } else {
         if(dx > dz) maxdir = 0;
      }

      // prepare voxel split along largest extends
      u32 t1x=to_x, t1y=to_y, t1z=to_z;
      u32 f2x=from_x, f2y=from_y, f2z=from_z;
      switch (maxdir) {
      case 0: 
	t1x = from_x + dx/2;
	f2x = t1x + 1;
	break;
      case 1: 
	t1y = from_y + dy/2;
	f2y = t1y + 1;
	break;
      case 2: 
	t1z = from_z + dz/2;
	f2z = t1z + 1;
	break;
      }
      // perform voxel split
      fillVoxels(inVoxelPrimitives, from_x,t1x, from_y,t1y, from_z,t1z);
      fillVoxels(inVoxelPrimitives, f2x,to_x, f2y,to_y, f2z,to_z);
   }
}
template <class ADAPTER, class CONTAINER>
inline u32 RecursiveFillGrid<ADAPTER,CONTAINER>::fillVoxels 
(const std::vector<PointerType>& primitives)
{
   m_numNonEmpty = 0;
   // fill voxel-array
   fillVoxels(primitives,
	      0, getGrid().getNumVoxelsDim()[0]-1, 
	      0, getGrid().getNumVoxelsDim()[1]-1, 
	      0, getGrid().getNumVoxelsDim()[2]-1);
   return m_numNonEmpty;
}
