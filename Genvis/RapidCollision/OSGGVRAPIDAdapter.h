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
//   $Date: 2004/12/20 15:59:31 $
//                                                                            
//=============================================================================

#ifndef OSGGVRAPIDADAPTER_H
#define OSGGVRAPIDADAPTER_H

#include "OSGGVBase.h"
#include "OSGGVTraits.h"
#include "OSGGVBVolAdapterExt.h"
#include "OSGGVAdapterFactory.h"
class RAPID_model;

BEGIN_GENVIS_NAMESPACE

/** \brief FaceAdapter for collision detection with RAPID. This class is only
    necessary to implement the common interface for reporting collision pairs.
 */
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING OpenSGRAPIDFaceAdapter 
: public OpenSGTriangleBase<BasicTraits>, public Adapter
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef OpenSGTriangleBase<BasicTraits>           InheritedData;
   typedef Adapter                                   Inherited;
   typedef typename InheritedData::Cache             Cache;
   typedef typename InheritedData::CacheData         CacheData;
   typedef typename InheritedData::TransformType     TransformType;
   typedef typename InheritedData::GeomTriangleType  GeomTriangleType;
   typedef typename InheritedData::ObjectAdapterType ObjectAdapterType;

   typedef OpenSGObjectBase<BasicTraits>             InternalObjectType;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   OpenSGRAPIDFaceAdapter ();
   OpenSGRAPIDFaceAdapter (InternalObjectType* adapter);
   OpenSGRAPIDFaceAdapter (InternalObjectType* adapter,
			   i32                id);
   ~OpenSGRAPIDFaceAdapter ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Dump.                                                        */
   /*! \{                                                                 */
   virtual std::ostream& dump (std::ostream& os) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};
typedef  OpenSGRAPIDFaceAdapter<OpenSGTraits> OSGRAPIDFaceAdapter;


/** \brief Adapter for collision detection with RAPID. This class stores a reference 
    to the RAPID datastructure.
 */
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING OpenSGRAPIDAdapter 
: public OpenSGObjectBase<BasicTraits>, public Adapter
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef OpenSGObjectBase<BasicTraits>             InheritedData;
   typedef Adapter                                   Inherited;
   typedef OpenSGRAPIDAdapter<BasicTraits>           Self;
   typedef typename InheritedData::Cache             Cache;
   typedef typename InheritedData::CacheData         CacheData;
   typedef typename InheritedData::TransformType     TransformType;
   typedef typename InheritedData::GeomObjectType    GeomObjectType;
   typedef typename InheritedData::ObjectAdapterType ObjectAdapterType;

   typedef FactoryHeap<Self>                         FactoryType;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   OpenSGRAPIDAdapter ();
   OpenSGRAPIDAdapter (const GeomObjectType& obj);
   OpenSGRAPIDAdapter (const TransformType&  m2w,
		       const GeomObjectType& obj);
   ~OpenSGRAPIDAdapter ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Init.                                                        */
   /*! \{                                                                 */
   void  init (const GeomObjectType& obj);
   void  init (const TransformType&  m2w,
	       const GeomObjectType& obj);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Class methods.                                               */
   /*! \{                                                                 */
   inline RAPID_model*   getModel     () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Dump.                                                        */
   /*! \{                                                                 */
   virtual std::ostream& dump (std::ostream& os) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Identifier of adapter type.                                  */
   /*! \{                                                                 */
   static u32       getAdapterId ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   RAPID_model* m_model;
};
typedef OpenSGRAPIDAdapter<OpenSGTraits> OSGRAPIDAdapter;

template <class BasicTraits>
inline RAPID_model*   OpenSGRAPIDAdapter<BasicTraits>::getModel    () const
{
   return m_model;
}

END_GENVIS_NAMESPACE
#endif






