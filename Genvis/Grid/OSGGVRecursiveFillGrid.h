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
//   $Date: 2003/09/11 16:20:30 $
//                                                                            
//=============================================================================

#ifndef OSGGVRECURSIVEFILLGRID_H
#define OSGGVRECURSIVEFILLGRID_H

#include "OSGGVBase.h"
#include "OSGGVRegularGrid.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Recursively assigning primitives to grid subregions by testing the primitives 
    bounding volumes.
 */
template <class ADAPTER,
          class CONTAINER = std::vector<ADAPTER*> >
class OSG_GENVISLIB_DLLMAPPING RecursiveFillGrid
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */   
   typedef RecursiveFillGrid<ADAPTER,CONTAINER> Self;
   typedef RegularGrid<ADAPTER,CONTAINER>       GridType;
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
   inline GridType&       getGrid () const;
   inline unsigned        getNumNonEmptyVoxels () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Fill.                                                        */
   /*! \{                                                                 */
   unsigned fillVoxels (const std::vector<Adapter*>&    primitives);
   unsigned fillVoxels (const std::vector<PointerType>& primitives);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   void     fillVoxels (const std::vector<Adapter*>& primitives,
			unsigned from_x, unsigned to_x, 
			unsigned from_y, unsigned to_y, 
			unsigned from_z, unsigned to_z);
   void     fillVoxels (const std::vector<PointerType>& primitives,
			unsigned from_x, unsigned to_x, 
			unsigned from_y, unsigned to_y, 
			unsigned from_z, unsigned to_z);

   unsigned  m_numNonEmpty;
   GridType* m_grid;
};
typedef RecursiveFillGrid<BVolAdapterBase> OSGRecursiveFillGrid;


template <class ADAPTER, class CONTAINER>
inline RecursiveFillGrid<ADAPTER,CONTAINER>::RecursiveFillGrid (GridType& grid)
  : m_grid(&grid)
{
}
template <class ADAPTER, class CONTAINER>
inline unsigned        RecursiveFillGrid<ADAPTER,CONTAINER>::getNumNonEmptyVoxels () const
{
   return m_numNonEmpty;
}
template <class ADAPTER, class CONTAINER>
inline RecursiveFillGrid<ADAPTER,CONTAINER>::GridType& 
RecursiveFillGrid<ADAPTER,CONTAINER>::getGrid () const
{
   assert(m_grid != NULL);
   return *m_grid;
}

END_GENVIS_NAMESPACE

#endif
