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

#ifndef OSGGVADAPTERFACTORY_H
#define OSGGVADAPTERFACTORY_H

#include <vector>
#include <list>
#include "OSGGVBase.h"

BEGIN_GENVIS_NAMESPACE

/*! Factory which creates objects on the heap.
 */
template<class AdapterType> 
class OSG_GENVISLIB_DLLMAPPING FactoryHeap
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef FactoryHeap<AdapterType> Self;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   inline FactoryHeap ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Singleton.                                                   */
   /*! \{                                                                 */
   static inline Self& the ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Interface.                                                   */
   /*! \{                                                                 */
   inline AdapterType* newObject    ();
   inline void         deleteObject (AdapterType* obj);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};

template<class AdapterType> 
inline FactoryHeap<AdapterType>::FactoryHeap ()
{
}
template<class AdapterType> 
inline AdapterType* FactoryHeap<AdapterType>::newObject    ()
{
   return new AdapterType();
}
template<class AdapterType> 
inline void         FactoryHeap<AdapterType>::deleteObject (AdapterType* obj)
{
   delete obj;
}
template<class AdapterType> 
inline FactoryHeap<AdapterType>::Self& FactoryHeap<AdapterType>::the ()
{
   static Self instance;
   return instance;
}


/*! Factory which stores a single array of objects and gives them out.
    Deleted objects are collected in an empty list and are given out
    again.
 */
template<class AdapterType> 
class OSG_GENVISLIB_DLLMAPPING FactorySTL
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef FactorySTL<AdapterType> Self;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   inline FactorySTL ();
   inline ~FactorySTL ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Singleton.                                                   */
   /*! \{                                                                 */
   static inline Self& the ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Interface.                                                   */
   /*! \{                                                                 */
   inline AdapterType* newObject    ();
   inline void         deleteObject (AdapterType* obj);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   typedef AdapterType*             Container;
   typedef AdapterType*             ContainerIterator;
   Container                        m_array;
   u32                              m_current;
   u32                              m_size;
   std::list<ContainerIterator>     m_empty;
};

template<class AdapterType> 
inline FactorySTL<AdapterType>::FactorySTL ()
{
   m_array = new AdapterType[GV_MAX_NUM];
   m_current = 0;
   m_size    = GV_MAX_NUM;
}
template<class AdapterType> 
inline FactorySTL<AdapterType>::~FactorySTL ()
{
   delete[] m_array;
}

template<class AdapterType> 
inline AdapterType* FactorySTL<AdapterType>::newObject    ()
{
   if (m_empty.begin() != m_empty.end()) {
      AdapterType* result = m_empty.front();
      m_empty.pop_front();
      return result;
   }
   AdapterType* result = m_array + m_current;
   if (++m_current >= m_size) {
      assert(false);
      return m_array;
   }
   return result;
}
template<class AdapterType> 
inline void         FactorySTL<AdapterType>::deleteObject (AdapterType* obj)
{
   m_empty.push_front((Container::iterator)obj);
}
template<class AdapterType> 
inline FactorySTL<AdapterType>::Self& FactorySTL<AdapterType>::the ()
{
   static Self instance;
   return instance;
}

END_GENVIS_NAMESPACE

#endif
