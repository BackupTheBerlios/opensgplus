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
//   $Date: 2004/12/20 15:58:23 $
//                                                                            
//=============================================================================

#ifndef OSGGVFUNCTORS_H
#define OSGGVFUNCTORS_H

#include <assert.h>
#include <vector>
#include "OSGGVBase.h"

BEGIN_GENVIS_NAMESPACE

template<class ResultT, class ArgT, class ObjectT> 
class SingleFunctor
{
public:
   typedef ResultT (ObjectT::*DispatchMethodT) (ArgT* );

   inline SingleFunctor (ObjectT* obj, DispatchMethodT method)
     : m_obj(obj), m_method(method)
   {
   }
   inline SingleFunctor (const SingleFunctor<ResultT,ArgT,ObjectT>& b)
     : m_obj(b.m_obj), m_method(b.m_method)
   {
   }

   inline ResultT call (ArgT* arg) {
      return ((m_obj)->*m_method)(arg);
   }

private:
   ObjectT*        m_obj;
   DispatchMethodT m_method;
};

template<class ResultT, class Arg1T, class Arg2T, class ObjectT> 
class DispatchFunctor
{
public:
   typedef ResultT (ObjectT::*DispatchMethodT) (Arg1T* b0, Arg2T* b1);

   inline DispatchFunctor (ObjectT* obj, DispatchMethodT method)
     : m_obj(obj), m_method(method)
   {
   }
   inline DispatchFunctor (const DispatchFunctor<ResultT,Arg1T,Arg2T,ObjectT>& b)
     : m_obj(b.m_obj), m_method(b.m_method)
   {
   }

   inline ResultT call (Arg1T* a, Arg2T* b) {
      return ((m_obj)->*m_method)(a, b);
   }

private:
   ObjectT*        m_obj;
   DispatchMethodT m_method;
};

template<class ResultT, class ObjectT> 
class InitSingleFunctor
{
public:
   typedef ResultT (ObjectT::*InitMethodT) ();

   inline InitSingleFunctor (ObjectT* obj, InitMethodT method)
     : m_obj(obj), m_method(method)
   {
   }
   inline InitSingleFunctor (const InitSingleFunctor<ResultT,ObjectT>& b)
     : m_obj(b.m_obj), m_method(b.m_method)
   {
   }

   inline ResultT call () {
      return ((m_obj)->*m_method)();
   }

private:
   ObjectT*    m_obj;
   InitMethodT m_method;
};
template<class ResultT, class Arg1T, class Arg2T, class ObjectT> 
class InitSingleFunctor2
{
public:
   typedef ResultT (ObjectT::*InitMethodT) (Arg1T* a, const Arg2T& b);

   inline InitSingleFunctor2 (ObjectT* obj, InitMethodT method)
     : m_obj(obj), m_method(method)
   {
   }
   inline InitSingleFunctor2 (const InitSingleFunctor2<ResultT,Arg1T,Arg2T,ObjectT>& b)
     : m_obj(b.m_obj), m_method(b.m_method)
   {
   }

   inline ResultT call (Arg1T* a, const Arg2T& b) {
      return ((m_obj)->*m_method)(a, b);
   }

private:
   ObjectT*    m_obj;
   InitMethodT m_method;
};

template<class ResultT, class Arg1T, class Arg2T, class Arg3T, class Arg4T, class ObjectT> 
class InitDoubleFunctor
{
public:
   typedef ResultT (ObjectT::*InitMethodT) (Arg1T*, const Arg2T&, const Arg2T&, 
					    Arg3T*, const Arg4T&, const Arg4T&);

   inline InitDoubleFunctor (ObjectT* obj, InitMethodT method)
     : m_obj(obj), m_method(method)
   {
   }
   inline InitDoubleFunctor (const InitDoubleFunctor<ResultT,Arg1T,Arg2T,Arg3T,Arg4T,ObjectT>& b)
     : m_obj(b.m_obj), m_method(b.m_method)
   {
   }

   inline ResultT call (Arg1T* a1, const Arg2T& a2, const Arg2T& a3, 
			Arg3T* b1, const Arg4T& b2, const Arg4T& b3) {
      return ((m_obj)->*m_method)(a1, a2, a3, b1, b2, b3);
   }

private:
   ObjectT*    m_obj;
   InitMethodT m_method;
};

END_GENVIS_NAMESPACE

#endif
