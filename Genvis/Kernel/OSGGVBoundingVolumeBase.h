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
//   $Revision: 1.2 $
//   $Date: 2004/12/20 15:58:23 $
//                                                                            
//=============================================================================

#ifndef OSGGVBOUNDINGVOLUMEBASE_H
#define OSGGVBOUNDINGVOLUMEBASE_H

#include <iostream>
#include "OSGGVBase.h"
#include "OSGGVReal.h"

BEGIN_GENVIS_NAMESPACE

class OSG_GENVISLIB_DLLMAPPING BoundingVolumeBase
{
public:
   inline BoundingVolumeBase ();

   inline bool isEmpty  () const;
   inline void setEmpty (bool flag);

private:
   bool m_empty;
};

inline BoundingVolumeBase::BoundingVolumeBase ()
  : m_empty(true)
{
}
inline bool BoundingVolumeBase::isEmpty  () const
{
   return m_empty;
}
inline void BoundingVolumeBase::setEmpty (bool flag)
{
   m_empty = flag;
}

END_GENVIS_NAMESPACE
#endif
