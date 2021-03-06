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
//   $Revision: 1.3 $
//   $Date: 2004/12/20 15:55:46 $
//                                                                            
//=============================================================================

#ifndef OSGGVREGULARGRIDBASE_H
#define OSGGVREGULARGRIDBASE_H

#include "OSGGVBase.h"
#include "OSGGVKDop.h"

BEGIN_GENVIS_NAMESPACE

/*! Class RegularGridBase implements an uniformly sudivided space. */
class OSG_GENVISLIB_DLLMAPPING RegularGridBase : public K6Dop
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   /*! Number of voxel created during setup depends on
       \begin{itemize}
       \item VoxelsPerUnit   Number of voxels per model unit.
       \item MaxVoxels       Total number of voxels; internal algorithm chooses 
                             number of voxels per dimension.
       \item MaxVoxelsPerDim Number of voxels per maximum extent dimension.
       \item MinVoxelsPerDim Number of voxels per minimum extent dimension. 
       \end{itemize} 
   */
   enum InitMode { 
      VoxelsPerUnit, MaxVoxelsPerDim, MinVoxelsPerDim, MaxVoxels
   };
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   /*! Default constructor. */
   RegularGridBase ();
   /*! Constructor.
       @param cen(in) Center point of axes-alignde box.
       @param hlx(in) Half length in x.
       @param hly(in) Half length in y.
       @param hlz(in) Half length in z.
       @param vnx(in) Number of voxels in x.
       @param vny(in) Number of voxels in y.
       @param vnz(in) Number of voxels in z. */
   inline RegularGridBase (const PointClass& cen, 
		      Real hlx, Real hly, Real hlz,
		      u32 vnx, u32 vny, u32 vnz);
   /*! Destructor. */
   virtual ~RegularGridBase ();
   /*! Initialization. */
   void init (const PointClass& center, 
	      Real hlx, Real hly, Real hlz,
              u32 vnx, u32 vny, u32 vnz);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Member.                                                      */
   /*! \{                                                                 */   
   /*! Number of voxels in each dimension. */
   inline const VectorClass3u& getNumVoxelsDim () const;
   /*! Total number of voxels. */ 
   inline u64           getNumVoxels   () const;
   /*! Number of voxels in a z-slice. */
   inline u32           getNumVoxelsXY () const;
   /*! Strides for addressing neighbour voxels. */
   inline const VectorClass3u& getStrides () const;
   /*! Lengths of voxel edges parallel to the axes. */
   inline const VectorClass& getLength() const;
   /*! Inverse lengths of voxel edges parallel to the axes. */
   inline const VectorClass& getInvLength() const;
   /*! Halflength of voxel edges parallel to the axes. */
   inline const VectorClass& getHalfLength() const;
   /*! Center of reference voxel (0, 0, 0). */
   inline const PointClass&  getRefCenter() const;
   /*! Center of voxel (x, y, z). */
   inline PointClass         getRefCenter (u32 x, u32 y, u32 z) const;
   /*! Bounding box of voxel (x, y, z). */
   inline void               getVoxel (K6Dop& box, u32 x, u32 y, u32 z) const;
   /*! Transform euclidean coordinates to voxel coordinates. */
   inline void               toVoxel     (PointClass& p) const;
   /*! Transform voxel coordinates to euclidean coordinates. */
   inline void               toEuclidean (PointClass& p) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   /*! Number of voxels in each dimension. */
   VectorClass3u m_num;
   /*! Total number of voxels. */
   u64    m_numVoxels;
   /*! Stride values. */
   VectorClass3u m_strides;
   // some frequently used constants:
   VectorClass m_vlength;
   VectorClass m_inv_vlength;
   VectorClass m_vh;
   PointClass  m_vcen;
};

inline RegularGridBase::RegularGridBase (const PointClass& cen, 
			       Real hlx, Real hly, Real hlz,
			       u32 vnx, u32 vny, u32 vnz)
  : K6Dop()
{
   init(cen, hlx,hly,hlz, vnx,vny,vnz);
}

inline const VectorClass3u& RegularGridBase::getNumVoxelsDim () const 
{ 
   return m_num; 
}  

inline u64 RegularGridBase::getNumVoxels () const 
{ 
   return m_numVoxels; 
}
inline u32 RegularGridBase::getNumVoxelsXY () const 
{ 
   return m_strides[2]; 
}
inline const VectorClass3u& RegularGridBase::getStrides () const
{
   return m_strides;
}
inline const VectorClass& RegularGridBase::getLength() const 
{ 
   return m_vlength; 
}
inline const VectorClass& RegularGridBase::getInvLength() const 
{ 
   return m_inv_vlength; 
}
inline const VectorClass& RegularGridBase::getHalfLength() const 
{ 
   return m_vh; 
}
inline const PointClass& RegularGridBase::getRefCenter() const 
{ 
   return m_vcen; 
}
inline PointClass RegularGridBase::getRefCenter (u32 x, u32 y, u32 z) const 
{ 
   VectorClass translate(x*getLength()[0], 
			 y*getLength()[1], 
			 z*getLength()[2]);
   return getRefCenter()+translate;
}

inline void RegularGridBase::getVoxel (K6Dop& box, u32 x, u32 y, u32 z) const
{
   box.init(getRefCenter(x, y, z), 
	    getHalfLength()[0], getHalfLength()[1], getHalfLength()[2]);
}

inline void RegularGridBase::toVoxel (PointClass& p) const
{
   p[0] -= minVector()[0];
   p[1] -= minVector()[1];
   p[2] -= minVector()[2];
   p[0] *= getInvLength()[0];
   p[1] *= getInvLength()[1];
   p[2] *= getInvLength()[2];
}
inline void RegularGridBase::toEuclidean (PointClass& p) const
{
   p[0] *= getLength()[0];
   p[1] *= getLength()[1];
   p[2] *= getLength()[2];
   p[0] += minVector()[0];
   p[1] += minVector()[1];
   p[2] += minVector()[2];
}

END_GENVIS_NAMESPACE
#endif
