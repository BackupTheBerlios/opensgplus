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
//   $Date: 2004/03/12 13:18:34 $
//                                                                            
//=============================================================================

#ifndef OSGGVCOLLISIONDATA_H
#define OSGGVCOLLISIONDATA_H

#include <assert.h>
#include <vector>
#include "OSGGVBase.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Size (in byte) of data array. */
const u32 sizeDataType = 3*sizeof(PointClass);

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
   inline u8*          getData ();
   inline const u8*    getData () const;
   inline bool         getTouched () const;
   inline void         setTouched (bool value=true);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   AdapterType* m_first;
   AdapterType* m_second;
   u8           m_data[sizeDataType];
   bool         m_touched;
};

template <class AdapterType>
inline    CollisionData<AdapterType>::CollisionData ()
  : m_first(NULL), m_second(NULL), m_touched(false)
{
}
template <class AdapterType>
inline    CollisionData<AdapterType>::CollisionData (AdapterType* first, AdapterType* second)
  : m_first(first), m_second(second), m_touched(false)
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
inline bool        
CollisionData<AdapterType>::getTouched () const
{
   return m_touched;
}
template <class AdapterType>
inline void         
CollisionData<AdapterType>::setTouched (bool value)
{
   m_touched = value;
}
template <class AdapterType>
inline u8*
CollisionData<AdapterType>::getData ()
{
   if (!getTouched()) {
      return NULL;
   } 
   return m_data;
}
template <class AdapterType>
inline const u8*
CollisionData<AdapterType>::getData  () const
{
   if (!getTouched()) {
      return NULL;
   } 
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
   inline void            setFirst  (AdapterType* first);
   inline AdapterType*    getFirst  () const;
   inline void            setSecond (AdapterType* second);
   inline AdapterType*    getSecond () const;
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
   assert(sizeof(DataType) <= sizeDataType);
}
template <class AdapterType, class DataType>
inline void CollisionInterface<AdapterType,DataType>::setData (const DataType& data)
{
   m_data.setTouched();
   *((DataType*)m_data.getData()) = data;
}
template <class AdapterType, class DataType>
inline DataType& CollisionInterface<AdapterType,DataType>::getData ()
{
   m_data.setTouched();
   return *((DataType*)m_data.getData());
}
template <class AdapterType, class DataType>
inline const DataType& CollisionInterface<AdapterType,DataType>::getData () const
{
   return *((DataType*)m_data.getData());
}
template <class AdapterType, class DataType>
inline void         CollisionInterface<AdapterType,DataType>::setFirst  (AdapterType* first)
{
   m_data.setFirst(first);
}
template <class AdapterType, class DataType>
inline AdapterType* CollisionInterface<AdapterType,DataType>::getFirst  () const
{
   return m_data.getFirst();
}
template <class AdapterType, class DataType>
inline void         CollisionInterface<AdapterType,DataType>::setSecond (AdapterType* second)
{
   m_data.setSecond(second);
}
template <class AdapterType, class DataType>
inline AdapterType* CollisionInterface<AdapterType,DataType>::getSecond () const
{
   return m_data.getSecond();
}

END_GENVIS_NAMESPACE

#endif




