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
//   $Revision: 1.5 $
//   $Date: 2004/12/20 15:55:46 $
//                                                                            
//=============================================================================

#ifndef OSGGVREGULARGRID_H
#define OSGGVREGULARGRID_H

#include <vector>
#include "OSGGVBase.h"
#include "OSGGVRegularGridBase.h"
#include "OSGGVRegularGridIter.h"
#include "OSGGVIntersection.h"
#include "OSGGVBVolAdapterBase.h"
#include "OSGGVBVolFunctions.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Regular grid.
 */
template <class ADAPTER,
          class CONTAINER = std::vector<ADAPTER*> >
class RegularGrid : public RegularGridBase
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */   
   typedef RegularGrid<ADAPTER,CONTAINER> Self;
   typedef RegularGridBase                Inherited;
   typedef ADAPTER                        AdapterType;
   typedef ADAPTER*                       PointerType;
   typedef CONTAINER                      ContainerType;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   /*! Default constructor. */
   inline RegularGrid ();
   /*! Constructor. Constructs a regular grid with the given bounding box.
       \see init.
   */
   inline RegularGrid (const AABox& box, Real voxels, InitMode mode);
   /*! Constructor. Constructs a regular grid with the given bounding box and
       the given resolution. \see init.
   */
   inline RegularGrid (const AABox& box, u32 dimx, u32 dimy, u32 dimz); 
   /*! Destructor. */
   virtual inline ~RegularGrid ();
   /*! Initialization. Constructs a regular grid with the given bounding box. mode is
       MaxVoxelsPerDim then voxels gives the number of voxels along maximum axis.
       MinVoxelsPerDim then voxels gives the number of voxels along minimum axis;
                            the number of voxels along maximum axis is computed from that.
       VoxelsPerUnit   then voxels gives the number of voxels per unit of space;
                            the number of voxels along maximum axis is computed from that.
       MaxVoxels       then voxels gives the total number of voxels.
   */
   inline void init (const AABox& box, Real voxels, InitMode mode); 
   /*! Lowlevel Initialization. Constructs a regular grid with the given bounding box and
       the given resolution. 
   */
   inline void init (const AABox& box, u32 dimx, u32 dimy, u32 dimz); 
   /*! Clear voxel array. Keep defined bounding box. */
   inline void clear (); 
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */
   /*! Returns object array of voxel with given index. */
   inline ContainerType& primitives (const u64& index);
   /*! Returns object array of voxel which corresponds to the iterator state. */
   inline ContainerType& primitives (const RegularGridIter& iter);
   /*! Returns object array of voxel which contains point p. */
   inline ContainerType& primitives (PointClass p);
   /*! Read object array of voxel with index (x, y, z). */
   inline ContainerType& primitives (u32 x, u32 y, u32 z);
   /*! Read object array of voxel with index (x, y, z). */
   inline ContainerType& primitives (const VectorClass3u& index);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Lowlevel access.                                             */
   /*! \{                                                                 */
   /*! Get internal data store. */
   inline std::vector<ContainerType>& getStore ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Ray Intersection.                                            */
   /*! \{                                                                 */
   /*! Default implementation always returns false. Specialization for 
       ADAPTER==BVolAdapterBase uses checkIntersect for each voxel entry. */
   virtual bool checkIntersect (const Intersection& in);
   /*! Default implementation always returns false. Specialization for 
       ADAPTER==BVolAdapterBase uses calcIntersect for each voxel entry.  */
   virtual bool calcIntersect  (Intersection& in);

   virtual inline bool calcIntersect  (const PointClass&  origin, 
				       const VectorClass& dir,
				       Real& dist) const;
   virtual inline bool checkIntersect (const PointClass&  origin, 
				       const VectorClass& dir,
				       const Real& dist) const;
   virtual inline bool checkIntersect (const PointClass& point) const;
   virtual inline bool checkIntersect (const BoundingVolume<Real>& box) const;
   virtual inline bool checkIntersect (const PointClass& p1, 
				       const PointClass& p2, 
				       const PointClass& p3) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   /*---------------------------------------------------------------------*/
   /*! \name Internal.                                                    */
   /*! \{                                                                 */
   /*! Compute number of voxel in x, y and z direction based on a given maximum maxVoxels. */
   inline void computeExtends (Real hlx, Real hly, Real hlz,
			       u32& dx, u32& dy, u32& dz, 
			       u64 maxVoxels,
			       u64 maxVoxelsInDim_from, 
			       u64 maxVoxelsInDim_to);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   /*! array of voxels */
   std::vector<ContainerType> m_voxel;  
};
typedef RegularGrid<BVolAdapterBase> OSGRegularGrid;


#include "OSGGVRegularGrid.inl"

END_GENVIS_NAMESPACE
#endif
