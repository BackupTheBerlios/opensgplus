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
//   $Date: 2004/03/12 13:23:23 $
//                                                                            
//=============================================================================

#ifndef OSGGVINDICES_H
#define OSGGVINDICES_H

#include <assert.h>
#include "OSGGVBase.h"

BEGIN_GENVIS_NAMESPACE

template <class C1, class C2, class C3>
class triple
{
public:
  inline triple ();
  inline triple (const C1& v1, const C2& v2, const C3& v3);
  
  C1 first;
  C2 second;
  C3 third;
};

template <class C1, class C2, class C3>
inline triple<C1,C2,C3>::triple () 
{}
template <class C1, class C2, class C3>
inline triple<C1,C2,C3>::triple (const C1& v1, const C2& v2, const C3& v3) 
  : first(v1), second(v2), third(v3)
{
}
template <class C1, class C2, class C3>
inline triple<C1,C2,C3> make_triple (const C1& v1, const C2& v2, const C3& v3)
{
   return triple<C1,C2,C3>(v1, v2, v3);
}

class OSG_GENVISLIB_DLLMAPPING VectorIndices
{
public:
   inline VectorIndices ();
   inline VectorIndices (u32 f, u32 l);
   inline u32 size() const;

   inline bool operator== (const VectorIndices& i) const;

   u32 first;
   u32 last;
};

inline VectorIndices::VectorIndices ()
{
}
inline VectorIndices::VectorIndices (u32 f, u32 l)
  : first(f), last(l)
{
}
inline u32 VectorIndices::size() const
{   
   assert(last >= first);
   return last - first;   
}
inline bool VectorIndices::operator== (const VectorIndices& second) const
{
   return (first == second.first 
	   && last == second.last);
}

END_GENVIS_NAMESPACE
#endif
