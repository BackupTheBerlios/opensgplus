//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//                                License                                     
//                                                                            
//   This library is free software; you can redistribute it and/or modify it 
//   under the terms of the GNU Library General Public License as published  
//   by the Free Software Foundation, version 2.                             
//                                                                             
//   This library is distributed in the hope that it will be useful, but       
//   WITHOUT ANY WARRANTY; without even the implied warranty of                
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         
//   Library General Public License for more details.                          
//                                                                            
//   You should have received a copy of the GNU Library General Public         
//   License along with this library; if not, write to the Free Software       
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.1 $
//   $Date: 2004/03/12 13:20:42 $
//                                                                            
//=============================================================================

#ifndef OSGGVADAPTIVEGRID_H
#define OSGGVADAPTIVEGRID_H

#include <vector>
#include "OSGGVBase.h"
#include "OSGGVRegularGridBase.h"
//#include "OSGGVAdaptiveGridIter.h"
#include "OSGGVAdapterFactory.h"
#include "OSGGVBVolAdapterBase.h"
#include "OSGGVBVolFunctions.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Adaptive grid.
 */
template <class ADAPTER,
          class CONTAINER = std::vector<ADAPTER*> >
class AdaptiveGrid : public RegularGridBase
{
public:
   //-------------------------------------------------------------------------------
   //  Generic octree cell. Note that the locational codes and the cell level are 
   //  only used in neighbor searching; they are not necessary for point or region 
   //  location.
   //-------------------------------------------------------------------------------
   template <class CELLCONTAINER>
   class OSG_GENVISLIB_DLLMAPPING OTCell 
   {
      friend class AdaptiveGrid<CELLCONTAINER>;
   public:
      typedef OTCell<CELLCONTAINER>     Self;
      typedef CELLCONTAINER             ContainerType;
      typedef FactoryHeap<Self>     FactoryType;

      inline OTCell () {
         parent = NULL;
	 xLocCode = yLocCode = zLocCode = 0;
	 children[0] = NULL;
	 children[1] = NULL;
	 children[2] = NULL;
	 children[3] = NULL;
	 children[4] = NULL;
	 children[5] = NULL;
	 children[6] = NULL;
	 children[7] = NULL;
      }
      inline void subdivide () {
	 if (level > 0) {
	   u32 newLevel = level-1;
	   u32 newValue = (1 << newLevel);
	   children[0] = FactoryType::the().newObject();
	   children[0]->parent = this;
	   children[0]->level  = newLevel;
	   children[0]->xLocCode = xLocCode;
	   children[0]->yLocCode = yLocCode;
	   children[0]->zLocCode = zLocCode;
	   children[0]->subdivide();
	   children[1] = FactoryType::the().newObject();
	   children[1]->parent = this;
	   children[1]->level  = newLevel;
	   children[1]->xLocCode = xLocCode | newValue;
	   children[1]->yLocCode = yLocCode;
	   children[1]->zLocCode = zLocCode;
	   children[1]->subdivide();
	   children[2] = FactoryType::the().newObject();
	   children[2]->parent = this;
	   children[2]->level  = newLevel;
	   children[2]->xLocCode = xLocCode;
	   children[2]->yLocCode = yLocCode | newValue;
	   children[2]->zLocCode = zLocCode;
	   children[2]->subdivide();
	   children[3] = FactoryType::the().newObject();
	   children[3]->parent = this;
	   children[3]->level  = newLevel;
	   children[3]->xLocCode = xLocCode | newValue;
	   children[3]->yLocCode = yLocCode | newValue;
	   children[3]->zLocCode = zLocCode;
	   children[3]->subdivide();
	   children[4] = FactoryType::the().newObject();
	   children[4]->parent = this;
	   children[4]->level  = newLevel;
	   children[4]->xLocCode = xLocCode;
	   children[4]->yLocCode = yLocCode;
	   children[4]->zLocCode = zLocCode | newValue;
	   children[4]->subdivide();
	   children[5] = FactoryType::the().newObject();
	   children[5]->parent = this;
	   children[5]->level  = newLevel;
	   children[5]->xLocCode = xLocCode | newValue;
	   children[5]->yLocCode = yLocCode;
	   children[5]->zLocCode = zLocCode | newValue;
	   children[5]->subdivide();
	   children[6] = FactoryType::the().newObject();
	   children[6]->parent = this;
	   children[6]->level  = newLevel;
	   children[6]->xLocCode = xLocCode;
	   children[6]->yLocCode = yLocCode | newValue;
	   children[6]->zLocCode = zLocCode | newValue;
	   children[6]->subdivide();
	   children[7] = FactoryType::the().newObject();
	   children[7]->parent = this;
	   children[7]->level  = newLevel;
	   children[7]->xLocCode = xLocCode | newValue;
	   children[7]->yLocCode = yLocCode | newValue;
	   children[7]->zLocCode = zLocCode | newValue;
	   children[7]->subdivide();
	 }
      }
      inline void clear () {
 	 if (children[0]!=NULL) { 
	    children[0]->clear(); FactoryType::the().deleteObject(children[0]); 
	    children[0] = NULL;
	 }
	 if (children[1]!=NULL) { 
	    children[1]->clear(); FactoryType::the().deleteObject(children[1]); 
	    children[1] = NULL;
	 }
	 if (children[2]!=NULL) { 
	    children[2]->clear(); FactoryType::the().deleteObject(children[2]); 
	    children[2] = NULL;
	 }
	 if (children[3]!=NULL) { 
	    children[3]->clear(); FactoryType::the().deleteObject(children[3]); 
	    children[3] = NULL;
	 }
	 if (children[4]!=NULL) { 
	    children[4]->clear(); FactoryType::the().deleteObject(children[4]); 
	    children[4] = NULL;
	 }
	 if (children[5]!=NULL) { 
	    children[5]->clear(); FactoryType::the().deleteObject(children[5]); 
	    children[5] = NULL;
	 }
	 if (children[6]!=NULL) { 
	    children[6]->clear(); FactoryType::the().deleteObject(children[6]); 
	    children[6] = NULL;
	 }
	 if (children[7]!=NULL) { 
	    children[7]->clear(); FactoryType::the().deleteObject(children[7]); 
	    children[7] = NULL;
	 }
      }
      inline void clearComplete () {
	 clear();
	 if (parent!=NULL) {
 	    if (parent->children[0]==this) parent->children[0] = NULL;
	    if (parent->children[1]==this) parent->children[1] = NULL;
	    if (parent->children[2]==this) parent->children[2] = NULL;
	    if (parent->children[3]==this) parent->children[3] = NULL;
	    if (parent->children[4]==this) parent->children[4] = NULL;
	    if (parent->children[5]==this) parent->children[5] = NULL;
	    if (parent->children[6]==this) parent->children[6] = NULL;
	    if (parent->children[7]==this) parent->children[7] = NULL;
	 }
	 FactoryType::the().deleteObject(this);
      }
      inline Self* getParent () const {
         return parent;
      }
      inline Self* getChildren (u16 id) const {
 	 assert(id < 8);
         return children[id];
      }
      inline ContainerType& getContainer () {
	 return voxel;
      }
      inline const ContainerType& getContainer () const {
	 return voxel;
      }
      inline u32 getCodeX () {
	 return xLocCode;
      }
      inline u32 getCodeY () {
	 return yLocCode;
      }
      inline u32 getCodeZ () {
	 return zLocCode;
      }

      u32           xLocCode;     // X locational code
      u32           yLocCode;     // Y locational code
      u32           zLocCode;     // Z locational code
      u32           level;        // Cell level in hierarchy (smallest cell has level 0)
      Self*         parent;       // Pointer to parent cell
      Self*         children[8];  // Pointer to first of 8 contiguous child cells
      ContainerType voxel;
   };
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */   
   typedef AdaptiveGrid<ADAPTER,CONTAINER> Self;
   typedef RegularGridBase                 Inherited;
   typedef ADAPTER                         AdapterType;
   typedef ADAPTER*                        PointerType;
   typedef CONTAINER                       ContainerType;
   typedef OTCell<CONTAINER>               CellType;
   typedef FactoryHeap<CellType>           FactoryType;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   /*! Default constructor. */
   inline AdaptiveGrid ();
   /*! Constructor. Constructs a regular grid with the given bounding box.
       @param box(in)          Bounding box.
       @param voxels(in)       Number of voxels requested in the specified mode.
       @param mode(in)         Mode for voxel creation. 
   */
   inline AdaptiveGrid (const AABox& box, Real voxels, InitMode mode);
   /*! Destructor. */
   virtual inline ~AdaptiveGrid ();
   /*! Initialization. */
   inline void init (const AABox& box, Real voxels, InitMode mode); 
   /*! Clear octree datastructure. Keep defined bounding box. */
   inline void clear (); 
   /*! Depth of octree datastructure. */
   inline u32  getDepth () const;
   /*! Maximum of locational codes. */
   inline u32  getMaxCode () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */
   /*! Returns object array of cell which corresponds to the iterator state. */
   //inline ContainerType& primitives (const AdaptiveGridIter& iter);
   /*! Returns object array of cell which contains point p. */
   inline ContainerType& primitives (PointClass p);
   /*! Read object array of cell with index (x, y, z). */
   inline ContainerType& primitives (u32 x, u32 y, u32 z);
   /*! Read object array of cell with index (x, y, z). */
   inline ContainerType& primitives (const VectorClass3i& index);
   /*! Read object array of smallest cell which contains region. */
   inline ContainerType& primitives (const BoundingVolume<Real>& region);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Lowlevel access.                                             */
   /*! \{                                                                 */
   /*! Get root cell. */
   inline CellType* getRoot ();
   /*! Returns cell with given location codes (xCode, yCode, zCode). */
   inline CellType* getCell (u32 xCode, u32 yCode, u32 zCode);
   /*! Returns cell which contains point p. */
   inline CellType* getLeafCell (PointClass p);
   /*! Read cell with index (x, y, z). */
   inline CellType* getLeafCell (u32 x, u32 y, u32 z);
   /*! Read cell with index (index). */
   inline CellType* getLeafCell (const VectorClass3i& index);
   /*! Read smallest cell which contains region. */
   inline CellType* getLeafCell (const BoundingVolume<Real>& region);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Ray Intersection.                                            */
   /*! \{                                                                 */
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   /*! Octree root node. */
   CellType* m_root;  
   /*! Depth in the octree. */
   u32       m_depth; 
   /*! Conversion of positions to locational codes.  */
   u32       m_maxValue; 
};
typedef AdaptiveGrid<BVolAdapterBase> OSGAdaptiveGrid;


#include "OSGGVAdaptiveGrid.inl"

END_GENVIS_NAMESPACE
#endif
