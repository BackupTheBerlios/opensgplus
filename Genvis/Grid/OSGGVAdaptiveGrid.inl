//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.1 $
//   $Date: 2004/03/12 13:20:42 $
//                                                                            
//=============================================================================

//-------------------------------------------------------------------------------
//  Macro to traverse a quadtree from a specified cell (typically the root cell) 
//  to a leaf cell by following the x and y locational codes, xLocCode and 
//  yLocCode. Upon entering, cell is the specified cell and nextLevel is one less 
//  than the level of the specified cell. Upon termination, cell is the leaf cell
//  and nextLevel is one less than the level of the leaf cell. 
//-------------------------------------------------------------------------------
#define GV_TRAVERSE(cell,nextLevel,xLocCode,yLocCode,zLocCode)                    \
{                                                                                 \
    while ((cell)->children[0] != NULL) {                                         \
        u32 childBranchBit = 1 << (nextLevel);                                    \
        u32 childIndex = ((((xLocCode) & childBranchBit) >> (nextLevel))          \
        + (((yLocCode) & childBranchBit) >> ((nextLevel)-1))                      \
	+ (((zLocCode) & childBranchBit) >> ((nextLevel)-2)) );                   \
        --nextLevel;                                                              \
        (cell) = ((cell)->children)[childIndex];                                  \
    }                                                                             \
}

//-------------------------------------------------------------------------------
//  Macro to traverse a quadtree from a specified cell to an offspring cell by 
//  following the x and y locational codes, xLocCode and yLocCode. The offpring 
//  cell is either at a specified level or is a leaf cell if a leaf cell is 
//  reached before the specified level. Upon entering, cell is the specified 
//  cell and nextLevel is one less than the level of the specified cell. Upon 
//  termination, cell is the offspring cell and nextLevel is one less than the 
//  level of the offspring cell.
//-------------------------------------------------------------------------------
#define GV_TRAVERSE_TO_LEVEL(cell,nextLevel,xLocCode,yLocCode,zLocCode,level)     \
{                                                                                 \
    i32 n = (nextLevel) - (level) + 1;                                            \
    while (n--) {                                                                 \
        u32 childBranchBit = 1 << (nextLevel);                                    \
        u32 childIndex = ((((xLocCode) & childBranchBit) >> (nextLevel))          \
        + (((yLocCode) & childBranchBit) >> ((nextLevel)-1))                      \
        + (((zLocCode) & childBranchBit) >> ((nextLevel)-2)));                    \
        --nextLevel;                                                              \
        (cell) = ((cell)->children)[childIndex];                                  \
        if ((cell)->children[0]==NULL) break;                                     \
    }                                                                             \
}

//-------------------------------------------------------------------------------
//  Macro for traversing a quadtree to a common ancestor of a specified cell 
//  and its neighbor, whose x or y locational code differs from the cell's
//  corresponding x or y locational code by binaryDiff (determined by XOR'ing the 
//  appropriate pair of x or y locational codes). Upon entering, cell is the 
//  specified cell and cellLevel is the cell's level. Upon termination, cell is 
//  the common ancestor and cellLevel is the common ancestor's level.
//-------------------------------------------------------------------------------
#define GV_GET_COMMON_ANCESTOR(cell,cellLevel,binaryDiff)                         \
{                                                                                 \
    while ((binaryDiff) & (1 << (cellLevel))) {                                   \
        (cell) = (cell)->parent;                                                  \
        (cellLevel)++;                                                            \
    }                                                                             \
}


template <class ADAPTER,class CONTAINER>
inline AdaptiveGrid<ADAPTER,CONTAINER>::AdaptiveGrid 
(const AABox& box, Real voxels, InitMode mode)
  : Inherited(), m_root(NULL)
{
   init(box, voxels, mode);
}

template <class ADAPTER,class CONTAINER>
inline typename AdaptiveGrid<ADAPTER,CONTAINER>::CellType*
AdaptiveGrid<ADAPTER,CONTAINER>::getRoot ()
{
   return m_root;
}
template <class ADAPTER,class CONTAINER>
inline u32  AdaptiveGrid<ADAPTER,CONTAINER>::getDepth () const
{
   return m_depth;
}
template <class ADAPTER,class CONTAINER>
inline u32  AdaptiveGrid<ADAPTER,CONTAINER>::getMaxCode () const
{
   return m_maxValue;
}

template <class ADAPTER,class CONTAINER>
inline typename AdaptiveGrid<ADAPTER,CONTAINER>::CellType*
AdaptiveGrid<ADAPTER,CONTAINER>::getLeafCell (u32 x, u32 y, u32 z)
{
   assert(x < getNumVoxelsDim()[0]);
   assert(y < getNumVoxelsDim()[1]);
   assert(z < getNumVoxelsDim()[2]);
   //----Determine the locational codes of the point's position.
   u32 xLocCode = (u32) (Real(x)/getNumVoxelsDim()[0] * getMaxCode()); assert(xLocCode < getMaxCode());
   u32 yLocCode = (u32) (Real(y)/getNumVoxelsDim()[1] * getMaxCode()); assert(yLocCode < getMaxCode());
   u32 zLocCode = (u32) (Real(z)/getNumVoxelsDim()[2] * getMaxCode()); assert(zLocCode < getMaxCode());

   //----Follow the branching patterns of the locational codes from the root cell
   //----to locate the leaf cell containing p
   CellType* cell = getRoot();
   i32  nextLevel = getDepth()-1;
   GV_TRAVERSE(cell,nextLevel,xLocCode,yLocCode,zLocCode);
   return cell;
}
template <class ADAPTER,class CONTAINER>
inline typename AdaptiveGrid<ADAPTER,CONTAINER>::CellType*
AdaptiveGrid<ADAPTER,CONTAINER>::getLeafCell (const VectorClass3i& index)
{
   return getLeafCell(index[0], index[1], index[2]);
}
template <class ADAPTER,class CONTAINER>
inline typename AdaptiveGrid<ADAPTER,CONTAINER>::CellType*
AdaptiveGrid<ADAPTER,CONTAINER>::getLeafCell (PointClass p)
{
   toVoxel(p);
   //----Determine the x and y locational codes of the point's position. Refer 
   //----to [King2001] for more efficient methods for converting floating point 
   //----numbers to integers.
   u32 xLocCode = (u32) (p[0]/getNumVoxelsDim()[0] * getMaxCode()); assert(xLocCode < getMaxCode());
   u32 yLocCode = (u32) (p[1]/getNumVoxelsDim()[1] * getMaxCode()); assert(yLocCode < getMaxCode());
   u32 zLocCode = (u32) (p[2]/getNumVoxelsDim()[2] * getMaxCode()); assert(zLocCode < getMaxCode());

   //----Follow the branching patterns of the locational codes from the root cell
   //----to locate the leaf cell containing p
   CellType* cell = getRoot();
   i32  nextLevel = getDepth()-1;
   GV_TRAVERSE(cell,nextLevel,xLocCode,yLocCode,zLocCode);
   return cell;
}
template <class ADAPTER,class CONTAINER>
inline typename AdaptiveGrid<ADAPTER,CONTAINER>::CellType* 
AdaptiveGrid<ADAPTER,CONTAINER>::getLeafCell (const BoundingVolume<Real>& r)
{
    //----Determine the x and y locational codes of the region boundaries. Refer 
    //----to [King2001] for more efficient methods for converting floating point 
    //----numbers to integers.
    u32 x0LocCode = (u32) (r.minVector()[0]/getNumVoxelsDim()[0] * getMaxCode()); assert(x0LocCode < getMaxCode());
    u32 y0LocCode = (u32) (r.minVector()[1]/getNumVoxelsDim()[1] * getMaxCode()); assert(y0LocCode < getMaxCode());
    u32 z0LocCode = (u32) (r.minVector()[2]/getNumVoxelsDim()[2] * getMaxCode()); assert(z0LocCode < getMaxCode());
    u32 x1LocCode = (u32) (r.maxVector()[0]/getNumVoxelsDim()[0] * getMaxCode()); assert(x1LocCode < getMaxCode());
    u32 y1LocCode = (u32) (r.maxVector()[1]/getNumVoxelsDim()[1] * getMaxCode()); assert(y1LocCode < getMaxCode());
    u32 z1LocCode = (u32) (r.maxVector()[2]/getNumVoxelsDim()[2] * getMaxCode()); assert(z1LocCode < getMaxCode());

    //----Determine the XOR'ed pairs of locational codes of the region boundaries
    u32 xDiff = x0LocCode ^ x1LocCode;
    u32 yDiff = y0LocCode ^ y1LocCode;
    u32 zDiff = z0LocCode ^ z1LocCode;

    //----Determine the level of the smallest possible cell entirely containing 
    //----the region
    CellType* cell = getRoot();
    u32 level  = getDepth();
    u32 ylevel = getDepth();
    u32 zlevel = getDepth();
    while (!(xDiff & (1 << level))  && level > 0)         level--;
    while (!(yDiff & (1 << ylevel)) && (ylevel > level))  ylevel--;
    while (!(zDiff & (1 << zlevel)) && (zlevel > ylevel)) zlevel--;
    zlevel++;

    //----Follow the branching patterns of the locational codes of v0 from the 
    //----root cell to the smallest cell entirely containing the region
    level = getDepth()-1;
    GV_TRAVERSE_TO_LEVEL(cell,level,x0LocCode,y0LocCode,z0LocCode,zlevel);
    return cell;
}

template <class ADAPTER,class CONTAINER>
inline typename AdaptiveGrid<ADAPTER,CONTAINER>::ContainerType&
AdaptiveGrid<ADAPTER,CONTAINER>::primitives (u32 x, u32 y, u32 z)
{
   return getLeafCell(x,y,z)->voxel;
}
template <class ADAPTER,class CONTAINER>
inline typename AdaptiveGrid<ADAPTER,CONTAINER>::ContainerType&
AdaptiveGrid<ADAPTER,CONTAINER>::primitives (const VectorClass3i& index)
{
   return getLeafCell(index)->voxel;
}
template <class ADAPTER,class CONTAINER>
inline typename AdaptiveGrid<ADAPTER,CONTAINER>::ContainerType&
AdaptiveGrid<ADAPTER,CONTAINER>::primitives (PointClass p)
{
   return getLeafCell(p)->voxel;
}
template <class ADAPTER,class CONTAINER>
inline typename AdaptiveGrid<ADAPTER,CONTAINER>::ContainerType&
AdaptiveGrid<ADAPTER,CONTAINER>::primitives (const BoundingVolume<Real>& r)
{
    return getLeafCell(r)->voxel;
}

template <class ADAPTER,class CONTAINER>
inline typename AdaptiveGrid<ADAPTER,CONTAINER>::CellType*
AdaptiveGrid<ADAPTER,CONTAINER>::getCell (u32 xCode, u32 yCode, u32 zCode)
{
   //----Follow the branching patterns of the locational codes from the root cell
   CellType* cell = getRoot();
   i32  nextLevel = getDepth()-1;
   GV_TRAVERSE(cell, nextLevel, xCode, yCode, zCode);
   return (cell);
}

template <class ADAPTER,class CONTAINER>
inline AdaptiveGrid<ADAPTER,CONTAINER>::AdaptiveGrid()
  : Inherited(), m_root(NULL), m_depth(0), m_maxValue(0)
{
}

template <class ADAPTER,class CONTAINER>
inline void AdaptiveGrid<ADAPTER,CONTAINER>::clear ()
{
   if (getRoot() != NULL) {
      getRoot()->clearComplete();
   }
}
template <class ADAPTER,class CONTAINER>
inline AdaptiveGrid<ADAPTER,CONTAINER>::~AdaptiveGrid()
{
   clear();
}

template <class ADAPTER,class CONTAINER>
inline void AdaptiveGrid<ADAPTER,CONTAINER>::init 
(const K6Dop& box, Real voxels, InitMode mode)
{
   if (voxels <= 0) { return; }

   clear();
   static FloatingComparator<Real> comp;
   // compute grid extents
   Real shx = stdMax(comp.getPrecision(), 0.5f*box.difference(0));
   Real shy = stdMax(comp.getPrecision(), 0.5f*box.difference(1));
   Real shz = stdMax(comp.getPrecision(), 0.5f*box.difference(2));

   u32 num_x, num_y, num_z;
   switch (mode) {
      case MaxVoxelsPerDim: {
	 m_depth = stdLog2(u32(voxels))+1;
         num_x = num_y = num_z = (1 << m_depth);
         break;
      }
      case MinVoxelsPerDim: {
         Real shmin = stdMin(shx, stdMin(shy, shz));
         Real shmax = stdMax(shx, stdMax(shy, shz));
	 m_depth = stdLog2(u32(voxels*shmax/shmin))+1;
         num_x = num_y = num_z = (1 << m_depth);
         break;
      }
      case VoxelsPerUnit: {
         Real shmax = stdMax(shx, stdMax(shy, shz));
	 m_depth = stdLog2(u32(voxels*shmax))+1;
         num_x = num_y = num_z = (1 << m_depth);
 	 break;
      }
      default: 
      case MaxVoxels: {
	 m_depth = u32(voxels);
         num_x = num_y = num_z = (1 << m_depth);
	 break;
      }	
   }
   Inherited::init(box.getCenter(), shx,shy,shz, num_x,num_y,num_z);
   // for locational codes m_depth has to be smaller than 32
   SLOG << num_x << "*" << num_y << "*" << num_z << ", depth=" << m_depth << std::endl;
   assert(0 < m_depth && m_depth < 32);
   m_maxValue = (1 << m_depth);
   SLOG << "maxValue=" << m_maxValue << std::endl;
   // create root 
   m_root = FactoryType::the().newObject();
   m_root->level = m_depth;
   m_root->subdivide();
}


