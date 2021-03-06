//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.4 $
//   $Date: 2004/12/20 15:55:46 $
//                                                                            
//=============================================================================


template <class ADAPTER,class CONTAINER>
inline typename RegularGrid<ADAPTER,CONTAINER>::ContainerType& 
RegularGrid<ADAPTER,CONTAINER>::primitives (const u64& index)
{
   return m_voxel[index];
}
template <class ADAPTER,class CONTAINER>
inline typename RegularGrid<ADAPTER,CONTAINER>::ContainerType&
RegularGrid<ADAPTER,CONTAINER>::primitives (const RegularGridIter& iter)
{
   return m_voxel[iter.getIndex()];
}
template <class ADAPTER,class CONTAINER>
inline typename RegularGrid<ADAPTER,CONTAINER>::ContainerType&
RegularGrid<ADAPTER,CONTAINER>::primitives (u32 x, u32 y, u32 z)
{
   assert(x < getNumVoxelsDim()[0]);
   assert(y < getNumVoxelsDim()[1]);
   assert(z < getNumVoxelsDim()[2]);
   return m_voxel[x + y*getStrides()[1] + z*getStrides()[2]];
}
template <class ADAPTER,class CONTAINER>
inline typename RegularGrid<ADAPTER,CONTAINER>::ContainerType&
RegularGrid<ADAPTER,CONTAINER>::primitives (const VectorClass3u& index)
{
   assert(index[0] < getNumVoxelsDim()[0]);
   assert(index[1] < getNumVoxelsDim()[1]);
   assert(index[2] < getNumVoxelsDim()[2]);
   return m_voxel[index.dot(getStrides())];
}
template <class ADAPTER,class CONTAINER>
inline typename RegularGrid<ADAPTER,CONTAINER>::ContainerType&
RegularGrid<ADAPTER,CONTAINER>::primitives (PointClass p)
{
   toVoxel(p);
   return primitives(p[0], p[1], p[2]);
}

template <class ADAPTER,class CONTAINER>
inline std::vector<typename RegularGrid<ADAPTER,CONTAINER>::ContainerType>& 
RegularGrid<ADAPTER,CONTAINER>::getStore ()
{
   return m_voxel;
}


#if 0
template <class ADAPTER,class CONTAINER>
inline void RegularGrid<ADAPTER,CONTAINER>::scaleUp   ()
{
   // set new values
   m_num         *= 2;
   m_strides[1]  *= 2;
   m_strides[2]  *= 4;
   m_numVoxels   *= 8;
   m_vh          /= 2.0;
   m_vlength     /= 2.0;
   m_inv_vlength *= 2.0;
   m_vcen = PointClass(minVector()[0], minVector()[1], minVector()[2]) + m_vh;

   u64 i;
   // insert entry after each existing entry in all rows and all slices
   for (i=0; i<getNumVoxels(); i+=2) {
      m_voxel.insert(m_voxel.begin()+i, m_voxel[i]);     
   }
   // insert row after each existing row in all slices
   for (i=0; i<getNumVoxels(); i+=getNumVoxelsDim()[0]) {
      m_voxel.insert(m_voxel.begin()+(getNumVoxelsDim()[0]*(i+1)-1), 
			 m_voxel.begin()+(getNumVoxelsDim()[0]*i),  m_voxel.begin()+(getNumVoxelsDim()[0]*(i+1)));
   }
   // insert slice after each existing slice
   for (i=0; i<getNumVoxels(); i+=getNumVoxelsXY()) {
      m_voxel.insert(m_voxel.begin()+(getNumVoxelsXY()*(i+1)-1),     
			 m_voxel.begin()+(getNumVoxelsXY()*i),      m_voxel.begin()+(getNumVoxelsXY()*(i+1)));
   }
}

template <class ADAPTER,class CONTAINER>
inline void RegularGrid<ADAPTER,CONTAINER>::scaleDown ()
{
   // resort
   u32 x, y, z;
   for (z=0; z<getNumVoxelsDim()[2]; z+=2) {
      for (y=0; y<getNumVoxelsDim()[1]; y+=2) {
	 for (x=0; x<getNumVoxelsDim()[0]; x+=2) {
	    ContainerType& entry = primitives(x, y, z);
	    entry.insert(entry.end(), primitives(x+1, y, z).begin(), primitives(x+1, y, z).end());
	    entry.insert(entry.end(), primitives(x, y+1, z).begin(), primitives(x, y+1, z).end());
	    entry.insert(entry.end(), primitives(x+1, y+1, z).begin(), primitives(x+1, y+1, z).end());
	 }
      }
   }

   // set new values
   m_num         /= 2;
   m_strides[1]  /= 2;
   m_strides[2]  /= 4; 
   m_numVoxels   /= 8;
   m_vh          *= 2.0;
   m_vlength     *= 2.0;
   m_inv_vlength /= 2.0;
   m_vcen = PointClass(minVector()[0], minVector()[1], minVector()[2]) + m_vh;

   // erase unnecessary entries
   for (x=0; x<getNumVoxelsDim()[0]; ++x) {
      m_voxel.erase(m_voxel.begin()+x);     
   }
   for (y=0; y<getNumVoxelsDim()[1]; ++y) {
      m_voxel.erase(m_voxel.begin()+(getNumVoxelsDim()[0]*y), m_voxel.begin()+(getNumVoxelsDim()[0]*(y+2)));     
   }
   for (z=0; z<getNumVoxelsDim()[2]; ++z) {
      m_voxel.erase(m_voxel.begin()+(getNumVoxelsXY()*z),     m_voxel.begin()+(getNumVoxelsXY()*(z+2)));     
   }
}
#endif

template <class ADAPTER,class CONTAINER>
inline void RegularGrid<ADAPTER,CONTAINER>::init (const K6Dop& box, Real voxels, InitMode mode)
{
   clear();

   static FloatingComparator<Real> comp;

   // compute grid extents
   if (voxels <= 0) {
      mode   = MaxVoxels;
      voxels = 1;
   }

   Real shx = stdMax(comp.getPrecision(), 0.5f*box.difference(0));
   Real shy = stdMax(comp.getPrecision(), 0.5f*box.difference(1));
   Real shz = stdMax(comp.getPrecision(), 0.5f*box.difference(2));

   u32 num_x, num_y, num_z;
   switch (mode) {
      case MaxVoxelsPerDim: {
         Real shmax = stdMax(shx, stdMax(shy, shz));
         Real edge = stdMax(comp.getPrecision(), shmax/voxels);
         num_x = (shmax == shx) ? u32(voxels) : u32(ceil(shx/edge));
         num_y = (shmax == shy) ? u32(voxels) : u32(ceil(shy/edge));
         num_z = (shmax == shz) ? u32(voxels) : u32(ceil(shz/edge));
         }
         break;
      case MinVoxelsPerDim: {
         Real shmin = stdMin(shx, stdMin(shy, shz));
         Real edge = stdMax(comp.getPrecision(), shmin/voxels);
         num_x = (shmin == shx) ? u32(voxels) : u32(ceil(shx/edge));
         num_y = (shmin == shy) ? u32(voxels) : u32(ceil(shy/edge));
         num_z = (shmin == shz) ? u32(voxels) : u32(ceil(shz/edge));
         }
         break;
      case VoxelsPerUnit: {
         num_x = u32(ceil(shx*voxels));
         num_y = u32(ceil(shy*voxels));
         num_z = u32(ceil(shz*voxels));
         }
 	 break;
      case MaxVoxels:
      default:
         computeExtends(shx,shy,shz, num_x,num_y,num_z, (u64)voxels, 1,(u64)voxels);
         break;
   }
   Inherited::init(box.getCenter(), shx,shy,shz, num_x,num_y,num_z);

   // resize voxel-array
   m_voxel.resize(getNumVoxels());
}
template <class ADAPTER,class CONTAINER>
inline void RegularGrid<ADAPTER,CONTAINER>::init (const K6Dop& box, u32 dimx, u32 dimy, u32 dimz)
{
   clear();

   // correct a dim = 0
   dimx = stdMax(dimx, 1);
   dimy = stdMax(dimy, 1);
   dimz = stdMax(dimz, 1);
   // calc side length
   Real shx = 0.5f*box.difference(0)/dimx;
   Real shy = 0.5f*box.difference(1)/dimx;
   Real shz = 0.5f*box.difference(2)/dimx;
   Inherited::init(box.getCenter(), shx,shy,shz, num_x,num_y,num_z);

   // resize voxel-array
   m_voxel.resize(getNumVoxels());
}

template <class ADAPTER,class CONTAINER>
inline void RegularGrid<ADAPTER,CONTAINER>::clear ()
{
   // clear voxel-array
   for (u64 i=0; i<getNumVoxels(); ++i) {
      m_voxel[i].clear();
   }
}

template <class ADAPTER,class CONTAINER>
inline void RegularGrid<ADAPTER,CONTAINER>::computeExtends(Real hlx, Real hly, Real hlz,
							   u32& dx, u32& dy, u32& dz, 
							   u64 maxVoxels,
							   u64 maxVoxelsInDim_from, 
							   u64 maxVoxelsInDim_to)
{
   static FloatingComparator<Real> comp;

   Real smax = stdMax(hlx, stdMax(hly, hlz));
   u64 maxVoxelsInDim = (maxVoxelsInDim_from + maxVoxelsInDim_to)/2;
   Real voxelEdge = stdMax(comp.getPrecision(), smax/Real(maxVoxelsInDim));

   dx = (smax == hlx) ? (int)maxVoxelsInDim : (int)ceil(hlx/voxelEdge);
   dy = (smax == hly) ? (int)maxVoxelsInDim : (int)ceil(hly/voxelEdge);
   dz = (smax == hlz) ? (int)maxVoxelsInDim : (int)ceil(hlz/voxelEdge);

   Real voxels = Real(dx) * Real(dy);
   if (voxels <= maxVoxels) {
      voxels *= Real(dz);
   }

   if (maxVoxelsInDim_from != maxVoxelsInDim_to) {
      if (voxels > Real(maxVoxels)) {
         computeExtends(hlx,hly,hlz, dx,dy,dz, maxVoxels,
                        maxVoxelsInDim_from, maxVoxelsInDim-1);
      } else if (voxels < (Real)maxVoxels) {
            if (maxVoxelsInDim_to - maxVoxelsInDim == 1) {
               maxVoxelsInDim_to = maxVoxelsInDim;
            }
            computeExtends(hlx,hly,hlz, dx,dy,dz, maxVoxels,
                           maxVoxelsInDim, maxVoxelsInDim_to);
      }
   }
}


/** Specialization for ADAPTER==BVolAdapterBase uses calcIntersect
    for each voxel entry.
 */
inline bool RegularGrid<BVolAdapterBase,std::vector<BVolAdapterBase*> >::calcIntersect (Intersection& in)
{
   static FloatingComparator<Real> comp;
   RegularGridIter iter(*this, in.getRay());
   bool  result = false;
   while (iter()) {
      const std::vector<AdapterType*>& list = m_voxel[iter.getIndex()];
      for (u32 i=0; i<list.size(); ++i) {
	 result = list[i]->calcIntersect(in) || result;
      }
      if (result && comp.leq(in.getDist(), iter.getOutDist())) {
	 return true;      // return true only if hit is inside voxel
      }
   }
   return result;
}
/** Specialization for ADAPTER==BVolAdapterBase uses checkIntersect
    for each voxel entry.
 */
inline bool RegularGrid<BVolAdapterBase,std::vector<BVolAdapterBase*> >::checkIntersect (const Intersection& in)
{
   RegularGridIter iter(*this, in.getRay());
   while (iter()) {
      const std::vector<AdapterType*>& list = m_voxel[iter.getIndex()];
      for (u32 i=0; i<list.size(); ++i) {
	 if (list[i]->checkIntersect(in)) {// ray did hit an item
	    return true;
	 }
      }
   }      
   return false;
}

/** Default implementation for all template arguments does nothing.
 */
template <class ADAPTER,class CONTAINER>
inline bool RegularGrid<ADAPTER,CONTAINER>::calcIntersect (Intersection& in)
{
   return false;
}
/** Default implementation for all template arguments does nothing.
 */
template <class ADAPTER,class CONTAINER>
inline bool RegularGrid<ADAPTER,CONTAINER>::checkIntersect (const Intersection& in)
{
   return false;
}

template <class ADAPTER,class CONTAINER>
inline bool RegularGrid<ADAPTER,CONTAINER>::calcIntersect (const PointClass&  origin, 
							   const VectorClass& dir,
							   Real& dist) const
{
   return Inherited::calcIntersect(origin, dir, dist);
}
template <class ADAPTER,class CONTAINER>
inline bool RegularGrid<ADAPTER,CONTAINER>::checkIntersect (const PointClass&  origin, 
							    const VectorClass& dir,
							    const Real& dist) const
{
   return Inherited::checkIntersect(origin, dir, dist);
}
template <class ADAPTER,class CONTAINER>
inline bool RegularGrid<ADAPTER,CONTAINER>::checkIntersect (const PointClass& point) const
{
   return Inherited::checkIntersect(point);
}
template <class ADAPTER,class CONTAINER>
inline bool RegularGrid<ADAPTER,CONTAINER>::checkIntersect (const BoundingVolume<Real>& bvol) const
{
   return Inherited::checkIntersect(bvol);
}
template <class ADAPTER,class CONTAINER>
inline bool RegularGrid<ADAPTER,CONTAINER>::checkIntersect (const PointClass& p1,
							    const PointClass& p2,
							    const PointClass& p3) const
{
   return Inherited::checkIntersect(p1, p2, p3);
}


template <class ADAPTER,class CONTAINER>
inline RegularGrid<ADAPTER,CONTAINER>::RegularGrid()
  : Inherited(), m_voxel(NULL)
{
}
template <class ADAPTER,class CONTAINER>
inline RegularGrid<ADAPTER,CONTAINER>::RegularGrid (const AABox& box,
						    Real     voxels, 
						    InitMode mode)
  : Inherited()
{
   init(box, voxels, mode);
}
template <class ADAPTER,class CONTAINER>
inline RegularGrid<ADAPTER,CONTAINER>::RegularGrid (const AABox& box,
						    u32 dimx, u32 dimy, u32 dimz)
  : Inherited()
{
   init(box, dimx, dimy, dimz);
}

template <class ADAPTER,class CONTAINER>
inline RegularGrid<ADAPTER,CONTAINER>::~RegularGrid()
{
   clear();
}
