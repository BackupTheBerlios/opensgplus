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
//   $Date: 2004/03/12 13:20:55 $
//                                                                            
//=============================================================================

#ifndef OSGGVRECURSIVEFILLADAPTIVEGRID_H
#define OSGGVRECURSIVEFILLADAPTIVEGRID_H

#include <assert.h>
#include "OSGGVBase.h"
#include "OSGGVReal.h"
#include "OSGGVAdaptiveGrid.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Recursively assigning primitives to grid subregions by testing the primitives 
    bounding volumes.
 */
template <class ADAPTER,
          class CONTAINER = std::vector<ADAPTER*> >
class OSG_GENVISLIB_DLLMAPPING RecursiveFillAdaptiveGrid
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */   
   typedef RecursiveFillAdaptiveGrid<ADAPTER,CONTAINER> Self;
   typedef AdaptiveGrid<ADAPTER,CONTAINER>              GridType;
   typedef typename GridType::AdapterType               AdapterType;
   typedef typename GridType::PointerType               PointerType;
   typedef typename GridType::ContainerType             ContainerType;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline RecursiveFillAdaptiveGrid (GridType& grid);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */
   inline GridType&       getGrid () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Fill.                                                        */
   /*! \{                                                                 */
   void fillVoxels (const std::vector<Adapter*>&    primitives);
   void fillVoxels (const std::vector<PointerType>& primitives);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   GridType* m_grid;
};
typedef RecursiveFillAdaptiveGrid<BVolAdapterBase> OSGRecursiveFillAdaptiveGrid;


#include "OSGGVRecursiveFillAdaptiveGrid.inl"

END_GENVIS_NAMESPACE

#endif
