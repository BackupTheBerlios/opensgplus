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
//   $Revision: 1.4 $
//   $Date: 2004/12/20 15:55:46 $
//                                                                            
//=============================================================================

#ifndef OSGGVRECURSIVEFILLGRID_H
#define OSGGVRECURSIVEFILLGRID_H

#include <assert.h>
#include "OSGGVBase.h"
#include "OSGGVReal.h"
#include "OSGGVRegularGrid.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Default functor for computing the bounding volume. Calls the method
    void const BoundingVolume<Real>& getBoundingVolume() const;
 */
template <class ADAPTER>
struct ComputeBoundingVolume
{
   inline const BoundingVolume<Real>& operator() (const ADAPTER* adapter) const {
      return adapter->getBoundingVolume();
   }
};

/*! \brief Recursively assigning primitives to grid subregions by testing the primitives 
    bounding volumes.
 */
template <class ADAPTER,
          class FUNCTOR   = ComputeBoundingVolume<ADAPTER>,
          class CONTAINER = std::vector<ADAPTER*> >
class RecursiveFillGrid
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */   
   typedef RecursiveFillGrid<ADAPTER,CONTAINER> Self;
   typedef RegularGrid<ADAPTER,CONTAINER>       GridType;
   typedef FUNCTOR                              FunctorType;
   typedef typename GridType::AdapterType       AdapterType;
   typedef typename GridType::PointerType       PointerType;
   typedef typename GridType::ContainerType     ContainerType;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline RecursiveFillGrid (GridType& grid);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */
   inline GridType&  getGrid () const;
   inline u32        getNumNonEmptyVoxels () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Fill.                                                        */
   /*! \{                                                                 */
   u32 fillVoxels (const std::vector<Adapter*>&    primitives);
   u32 fillVoxels (const std::vector<PointerType>& primitives);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   void     fillVoxels (const std::vector<Adapter*>& primitives,
			u32 from_x, u32 to_x, 
			u32 from_y, u32 to_y, 
			u32 from_z, u32 to_z);
   void     fillVoxels (const std::vector<PointerType>& primitives,
			u32 from_x, u32 to_x, 
			u32 from_y, u32 to_y, 
			u32 from_z, u32 to_z);

   u32  m_numNonEmpty;
   GridType*   m_grid;
   FunctorType m_functor;
};
typedef RecursiveFillGrid<BVolAdapterBase> OSGRecursiveFillGrid;


#include "OSGGVRecursiveFillGrid.inl"

END_GENVIS_NAMESPACE

#endif
