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
//   $Date: 2003/09/11 16:20:29 $
//                                                                            
//=============================================================================

#ifndef OSGGVCOLLISIONDATA_H
#define OSGGVCOLLISIONDATA_H

#include <assert.h>
#include <vector>
#include "OSGGVBase.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Collision pair of two adapter pairs with additional data object.
 */
template <class AdapterType>
class CollisionData
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   CollisionData ();
   CollisionData (AdapterType* first, AdapterType* second);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Member.                                                      */
   /*! \{                                                                 */
   inline void         setFirst  (AdapterType* first);
   inline AdapterType* getFirst  () const;
   inline void         setSecond (AdapterType* second);
   inline AdapterType* getSecond () const;
   inline void         setData (void* data);
   inline void*        getData ();
   inline const void*  getData () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   AdapterType* m_first;
   AdapterType* m_second;
   void*        m_data;
};

template <class AdapterType>
inline    CollisionData<AdapterType>::CollisionData ()
  : m_first(NULL), m_second(NULL), m_data(NULL)
{
}
template <class AdapterType>
inline    CollisionData<AdapterType>::CollisionData (AdapterType* first, AdapterType* second)
  : m_first(first), m_second(second), m_data(NULL)
{
}
template <class AdapterType>
inline void         CollisionData<AdapterType>::setFirst  (AdapterType* first)
{
   m_first = first;
}
template <class AdapterType>
inline AdapterType* CollisionData<AdapterType>::getFirst  () const
{
   return m_first;
}
template <class AdapterType>
inline void         CollisionData<AdapterType>::setSecond (AdapterType* second)
{
   m_second = second;
}
template <class AdapterType>
inline AdapterType* CollisionData<AdapterType>::getSecond () const
{
   return m_second;
}
template <class AdapterType>
inline void         
CollisionData<AdapterType>::setData  (void* data)
{
   m_data = data;
}
template <class AdapterType>
inline void*
CollisionData<AdapterType>::getData ()
{
   return m_data;
}
template <class AdapterType>
inline const void*
CollisionData<AdapterType>::getData  () const
{
   return m_data;
}


/*! \brief Interface class for accessing the additional data in a collision pair object
    in a type-safe way.
 */
template <class AdapterType, class DataType>
class CollisionInterface 
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   CollisionInterface (CollisionData<AdapterType>& data);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Member.                                                      */
   /*! \{                                                                 */
   inline void            setData (const DataType& data);
   inline DataType&       getData ();
   inline const DataType& getData () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Memory management.                                           */
   /*! \{                                                                 */
   inline void            destroy ();
   inline void            create ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   CollisionData<AdapterType>& m_data;
};

template <class AdapterType, class DataType>
inline CollisionInterface<AdapterType,DataType>::CollisionInterface 
(CollisionData<AdapterType>& data)
  : m_data(data)
{
}
template <class AdapterType, class DataType>
inline void CollisionInterface<AdapterType,DataType>::setData (const DataType& data)
{
   assert(m_data.getData() != NULL);
   *((DataType*)m_data.getData()) = data;
}
template <class AdapterType, class DataType>
inline DataType& CollisionInterface<AdapterType,DataType>::getData ()
{
   assert(m_data.getData() != NULL);
   return *((DataType*)m_data.getData());
}
template <class AdapterType, class DataType>
inline const DataType& CollisionInterface<AdapterType,DataType>::getData () const
{
   assert(m_data.getData() != NULL);
   return *((DataType*)m_data.getData());
}
template <class AdapterType, class DataType>
inline void            CollisionInterface<AdapterType,DataType>::destroy ()
{
   delete ((DataType*)m_data.getData());
}
template <class AdapterType, class DataType>
inline void            CollisionInterface<AdapterType,DataType>::create ()
{
   m_data.setData(new DataType());
}

END_GENVIS_NAMESPACE

#endif



