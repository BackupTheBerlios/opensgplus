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

#ifndef OSGGVNODEADAPTER_H
#define OSGGVNODEADAPTER_H

#include "OSGGVBase.h"
#include "OSGGVTraits.h"
#include "OSGGVBVolAdapter.h"
#include "OSGGVAdapterFactory.h"

BEGIN_GENVIS_NAMESPACE

/** \brief Adapter for collision detection with the bounding volume 
    in OpenSG nodes (without any hierarchy).
 */
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING OpenSGNodeAdapter 
: public OpenSGObjectBase<BasicTraits>, public Adapter
{
public:
   typedef OpenSGObjectBase<BasicTraits>       Inherited;
   typedef OpenSGNodeAdapter<BasicTraits>      SelfType;
   typedef FactoryHeap<SelfType>               FactoryType;

   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   OpenSGNodeAdapter ();
   OpenSGNodeAdapter (const GeomObjectType& obj);
   ~OpenSGNodeAdapter ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Init.                                                        */
   /*! \{                                                                 */
   void  init (const GeomObjectType& obj);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Dump.                                                        */
   /*! \{                                                                 */
   virtual std::ostream& dump (std::ostream& os) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Identifier of adapter type.                                  */
   /*! \{                                                                 */
   static unsigned       getAdapterId ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};
typedef  OpenSGNodeAdapter<OpenSGTraits> OSGNodeAdapter;

END_GENVIS_NAMESPACE
#endif






