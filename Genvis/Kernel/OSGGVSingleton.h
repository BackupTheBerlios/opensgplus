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

#ifndef OSGGVSINGLETON_H
#define OSGGVSINGLETON_H

#include "OSGGVBase.h"

BEGIN_GENVIS_NAMESPACE

/** Singleton template.
    Note that the unique instance is created during static initialization.
*/
template <class T>
class Singleton 
{
public:
  /** Singleton access function.
      Use this function to obtain a reference to the instance of the 
      encapsulated class.
  */
  static inline T& the ();

private:
  static T s_instance;

  // Disable constructors/assignment to enforce uniqueness
  Singleton ();
  Singleton (const Singleton&);
  Singleton& operator= (const Singleton&);
};

template <class T>
T Singleton<T>::s_instance;

template <class T>
inline T& Singleton<T>::the ()
{
   return s_instance;
}

END_GENVIS_NAMESPACE
#endif
