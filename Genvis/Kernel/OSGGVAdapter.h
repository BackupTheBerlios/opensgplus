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

#ifndef OSGGVADAPTER_H
#define OSGGVADAPTER_H

#include <iostream>
#include <assert.h>
#include "OSGGVBase.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Base for adapter classes.
 */
class OSG_GENVISLIB_DLLMAPPING  Adapter
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline Adapter ();
   virtual inline ~Adapter ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Dump.                                                        */
   /*! \{                                                                 */   
   virtual inline std::ostream& dump (std::ostream& os) const;
   friend  inline std::ostream& operator << (std::ostream& os, const Adapter& object);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

//protected:
   /*---------------------------------------------------------------------*/
   /*! \name Identifier Generator.                                        */
   /*! \{                                                                 */
   static  inline unsigned getNextId  ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};

inline Adapter::Adapter() 
{
}

inline Adapter::~Adapter() 
{
}

inline std::ostream& operator << (std::ostream& os, const Adapter& object)
{
  return object.dump(os);   
}

inline std::ostream& Adapter::dump (std::ostream& os) const
{
  return os;
}

inline unsigned Adapter::getNextId ()
{
   static unsigned nextId = 0;
   unsigned id = nextId++;
   assert(id < GV_MAX_NUM_ADAPTER);
   return id;
}

END_GENVIS_NAMESPACE
#endif
