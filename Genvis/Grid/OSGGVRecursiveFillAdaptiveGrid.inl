//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.1 $
//   $Date: 2004/03/12 13:20:55 $
//                                                                            
//=============================================================================


template <class ADAPTER, class CONTAINER>
inline RecursiveFillAdaptiveGrid<ADAPTER,CONTAINER>::RecursiveFillAdaptiveGrid (GridType& grid)
  : m_grid(&grid)
{
}
template <class ADAPTER, class CONTAINER>
inline RecursiveFillAdaptiveGrid<ADAPTER,CONTAINER>::GridType& 
RecursiveFillAdaptiveGrid<ADAPTER,CONTAINER>::getGrid () const
{
   assert(m_grid != NULL);
   return *m_grid;
}

template <class ADAPTER, class CONTAINER>
inline void RecursiveFillAdaptiveGrid<ADAPTER,CONTAINER>::fillVoxels 
(const std::vector<Adapter*>& primitives)
{
   // fill octree
   for (u32 i=0; i<primitives.size(); ++i) {
      BVolAdapterBase* prim = static_cast<PointerType>(primitives[i]);
      CellType*        cell = getLeafCell(prim->getBoundingVolume());
      while (cell != NULL) {
	 cell->voxel.push_back(prim);
	 cell = cell->parent;
      }
   }
}

template <class ADAPTER, class CONTAINER>
inline void RecursiveFillAdaptiveGrid<ADAPTER,CONTAINER>::fillVoxels 
(const std::vector<PointerType>& primitives)
{
   // fill octree
   for (u32 i=0; i<primitives.size(); ++i) {
      PointerType prim = primitives[i];
      CellType*   cell = getLeafCell(prim->getBoundingVolume());
      while (cell != NULL) {
	 cell->voxel.push_back(prim);
	 cell = cell->parent;
      }
   }
}
