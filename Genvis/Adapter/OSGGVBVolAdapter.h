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
//   $Date: 2004/12/20 15:56:34 $
//                                                                            
//=============================================================================

#ifndef OSGGVBVOLADAPTER_H
#define OSGGVBVOLADAPTER_H

#include "OSGGVBase.h"
#include "OSGGVTraits.h"
#include "OSGGVBVolAdapterBase.h"
#include "OSGGVAdapterFactory.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Base for adapter classes associated with scenegraph objects/nodes.
*/
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING OpenSGObjectBase
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef typename BasicTraits::Cache          Cache;
   typedef typename BasicTraits::CacheData      CacheData;
   typedef typename BasicTraits::TransformType  TransformType;
   typedef typename BasicTraits::GeomObjectType GeomObjectType;
   typedef CacheData                            ObjectAdapterType;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   OpenSGObjectBase ();
   OpenSGObjectBase (const OpenSGObjectBase& src);
   OpenSGObjectBase (const GeomObjectType& obj);
   OpenSGObjectBase (const TransformType&  m2w, 
		     const GeomObjectType& obj,
		     u32 id);
   virtual ~OpenSGObjectBase ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Associated scenegraph object.                                */
   /*! \{                                                                 */
   inline GeomObjectType     getOriginal  () const;
   inline void               setOriginal  (const GeomObjectType& obj);
   inline ObjectAdapterType& getObjectAdapter () const;
   inline void               setObjectAdapter (ObjectAdapterType* adapter);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Adapter transformation.                                      */
   /*! \{                                                                 */
   inline const TransformType& getTransform (u32 id) const;
   inline void                 setTransform (const TransformType& trf, u32 id);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Transformed geometry fields.                                 */
   /*! \{                                                                 */
   OSG::GeoPositionsPtr  getPositions (u32 id=BVolAdapterBase::getAdapterId()) const; 
   OSG::GeoNormalsPtr    getNormals   (u32 id=BVolAdapterBase::getAdapterId()) const;
   OSG::GeoColorsPtr     getColors    (u32 id=BVolAdapterBase::getAdapterId()) const; 
   OSG::GeoTexCoordsPtr  getTexCoords (u32 id=BVolAdapterBase::getAdapterId()) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   ObjectAdapterType* m_original;
};
typedef OpenSGObjectBase<OpenSGTraits> OSGObjectBase;

template <class BasicTraits>
inline typename OpenSGObjectBase<BasicTraits>::ObjectAdapterType& 
OpenSGObjectBase<BasicTraits>::getObjectAdapter () const
{
   assert(m_original != NULL);
   return *m_original;
}
template <class BasicTraits>
inline void OpenSGObjectBase<BasicTraits>::setObjectAdapter (ObjectAdapterType* original)
{
   m_original = original;
}

template <class BasicTraits>
inline typename OpenSGObjectBase<BasicTraits>::GeomObjectType 
OpenSGObjectBase<BasicTraits>::getOriginal () const
{
   return getObjectAdapter().getNode();
}
template <class BasicTraits>
inline void OpenSGObjectBase<BasicTraits>::setOriginal (const GeomObjectType& obj)
{
   // original is derived from object adapter
   CacheData& data = Cache::the()[obj];
   setObjectAdapter(&data);
}
template <class BasicTraits>
inline const typename OpenSGObjectBase<BasicTraits>::TransformType&
OpenSGObjectBase<BasicTraits>::getTransform (u32 id) const
{
   return getObjectAdapter().getAdapterMatrix(id);
}
template <class BasicTraits>
inline void
OpenSGObjectBase<BasicTraits>::setTransform (const TransformType& trf, u32 id)
{
   getObjectAdapter().setAdapterMatrix(id, trf);
}


/*! \brief Adapter class for scenegraph objects/nodes used in a bounding volume hierarchy.
*/
template <class BasicTraits, class BVOL>
class OSG_GENVISLIB_DLLMAPPING OpenSGObject2BVol 
: public OpenSGObjectBase<BasicTraits>, public BVolAdapter<BVOL>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef OpenSGObjectBase<BasicTraits>             InheritedData;
   typedef BVolAdapter<BVOL>                         Inherited;
   typedef OpenSGObject2BVol<BasicTraits,BVOL>       Self;
   typedef FactoryHeap<Self>                         FactoryType;
   typedef typename InheritedData::Cache             Cache;
   typedef typename InheritedData::CacheData         CacheData;
   typedef typename InheritedData::TransformType     TransformType;
   typedef typename InheritedData::GeomObjectType    GeomObjectType;
   typedef typename InheritedData::ObjectAdapterType ObjectAdapterType;
   typedef typename Inherited::BVol                  BVol;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   OpenSGObject2BVol ();
   OpenSGObject2BVol (const GeomObjectType& obj);
   OpenSGObject2BVol (const TransformType& m2w, const GeomObjectType& obj);
   virtual ~OpenSGObject2BVol ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Init.                                                        */
   /*! \{                                                                 */
   void  init (const GeomObjectType& obj);
   void  init (const TransformType& m2w, const GeomObjectType& obj);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Interface of BVolAdapterBase.                                */
   /*! \{                                                                 */
   virtual const BoundingVolume<Real>& getBoundingVolume () const;
   virtual bool  calcIntersect (Intersection& hit);
   virtual void  draw          ();
   virtual void  drawWireframe ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Dump.                                                        */
   /*! \{                                                                 */
   virtual std::ostream& dump (std::ostream& os) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   BVOL m_bvol;
};


/*! \brief Base for adapter classes associated with faces.
*/
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING OpenSGFaceBase
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef typename BasicTraits::Cache          Cache;
   typedef typename BasicTraits::CacheData      CacheData;
   typedef typename BasicTraits::TransformType  TransformType;
   typedef typename BasicTraits::GeomFaceType   GeomFaceType;
   typedef CacheData                            ObjectAdapterType;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   inline OpenSGFaceBase ();
   virtual inline ~OpenSGFaceBase ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */
   inline GeomFaceType             getOriginal () const;
   inline const TransformType&     getTransform (u32 id) const;
   inline ObjectAdapterType& getObjectAdapter () const;
   inline void               setObjectAdapter (ObjectAdapterType* adapter);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Transformed geometry fields.                                 */
   /*! \{                                                                 */
   OSG::GeoPositionsPtr getPositions (u32 id=BVolAdapterBase::getAdapterId()) const; 
   OSG::GeoNormalsPtr   getNormals   (u32 id=BVolAdapterBase::getAdapterId()) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   ObjectAdapterType*           m_original;
   i32                          m_originalId;
   GeomFaceType         m_originalFace;
#ifdef GV_STOREDCOORDS_FACE
   OSG::GeoPositionsPtr m_coord; 
   OSG::GeoNormalsPtr   m_normal;
#endif
};
typedef OpenSGFaceBase<OpenSGTraits> OSGFaceBase;

template <class BasicTraits>
inline OpenSGFaceBase<BasicTraits>::OpenSGFaceBase ()
  : m_original(NULL), m_originalId(0)
{
}
template <class BasicTraits>
inline OpenSGFaceBase<BasicTraits>::~OpenSGFaceBase ()
{
}

template <class BasicTraits>
inline typename OpenSGFaceBase<BasicTraits>::ObjectAdapterType&
OpenSGFaceBase<BasicTraits>::getObjectAdapter () const
{
   assert(m_original != NULL);
   return *m_original;
}
template <class BasicTraits>
inline void
OpenSGFaceBase<BasicTraits>::setObjectAdapter (ObjectAdapterType* adapter)
{
   m_original = adapter;
}
template <class BasicTraits>
inline typename OpenSGFaceBase<BasicTraits>::GeomFaceType 
OpenSGFaceBase<BasicTraits>::getOriginal () const
{
   return m_originalFace;
}
template <class BasicTraits>
inline const typename OpenSGFaceBase<BasicTraits>::TransformType&
OpenSGFaceBase<BasicTraits>::getTransform (u32 id) const
{
   return getObjectAdapter().getAdapterMatrix(id);
}


/*! \brief Adapter class for faces used in a bounding volume hierarchy.
*/
template <class BasicTraits, class BVOL>
class OSG_GENVISLIB_DLLMAPPING OpenSGFace2BVol 
: public OpenSGFaceBase<BasicTraits>, public BVolAdapter<BVOL>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef OpenSGFaceBase<BasicTraits>               InheritedData;
   typedef BVolAdapter<BVOL>                         Inherited;
   typedef OpenSGFace2BVol<BasicTraits,BVOL>         Self;
   typedef FactoryHeap<Self>                         FactoryType;
   typedef typename InheritedData::Cache             Cache;
   typedef typename InheritedData::CacheData         CacheData;
   typedef typename InheritedData::TransformType     TransformType;
   typedef typename InheritedData::GeomFaceType      GeomFaceType;
   typedef typename InheritedData::ObjectAdapterType ObjectAdapterType;
   typedef typename Inherited::BVol                  BVol;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   OpenSGFace2BVol ();
   OpenSGFace2BVol (const GeomFaceType& obj);
   OpenSGFace2BVol (const TransformType& m2w, const GeomFaceType&  obj);
   virtual ~OpenSGFace2BVol ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Init.                                                        */
   /*! \{                                                                 */
   void  init (const GeomFaceType& obj);
   void  init (const TransformType& m2w, const GeomFaceType&  obj);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Interface of BVolAdapterBase.                                */
   /*! \{                                                                 */
   virtual PointClass                  getCenter ();
   virtual const BoundingVolume<Real>& getBoundingVolume () const;
   virtual bool  calcIntersect     (Intersection& hit);
   virtual void  draw          ();
   virtual void  drawWireframe ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Dump.                                                        */
   /*! \{                                                                 */
   virtual std::ostream& dump (std::ostream& os) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   void calcBVol (BVol& bvol) const;
#ifdef GV_STOREDBVOL_FACE
   BVOL m_bvol;
#endif
};


/*! \brief Base for adapter classes associated with faces.
*/
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING OpenSGPrimitiveBase
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef typename BasicTraits::Cache             Cache;
   typedef typename BasicTraits::CacheData         CacheData;
   typedef typename BasicTraits::TransformType     TransformType;
   typedef typename BasicTraits::GeomPrimitiveType GeomPrimitiveType;
   typedef CacheData                               ObjectAdapterType;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   inline OpenSGPrimitiveBase ();
   virtual inline ~OpenSGPrimitiveBase ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */
   inline GeomPrimitiveType    getOriginal () const;
   inline const TransformType& getTransform (u32 id) const;
   inline ObjectAdapterType&   getObjectAdapter () const;
   inline void                 setObjectAdapter (ObjectAdapterType* adapter);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Transformed geometry fields.                                 */
   /*! \{                                                                 */
   OSG::GeoPositionsPtr getPositions (u32 id=BVolAdapterBase::getAdapterId()) const; 
   OSG::GeoNormalsPtr   getNormals   (u32 id=BVolAdapterBase::getAdapterId()) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   ObjectAdapterType*           m_original;
   i32                          m_originalId;
   GeomPrimitiveType    m_originalPrimitive;
#ifdef GV_STOREDCOORDS_PRIMITIVE
   OSG::GeoPositionsPtr m_coord; 
   OSG::GeoNormalsPtr   m_normal;
#endif
};
typedef OpenSGPrimitiveBase<OpenSGTraits> OSGPrimitiveBase;

template <class BasicTraits>
inline OpenSGPrimitiveBase<BasicTraits>::OpenSGPrimitiveBase ()
  : m_original(NULL), m_originalId(0)
{
}
template <class BasicTraits>
inline OpenSGPrimitiveBase<BasicTraits>::~OpenSGPrimitiveBase ()
{
}

template <class BasicTraits>
inline typename OpenSGPrimitiveBase<BasicTraits>::ObjectAdapterType&
OpenSGPrimitiveBase<BasicTraits>::getObjectAdapter () const
{
   assert(m_original != NULL);
   return *m_original;
}
template <class BasicTraits>
inline void
OpenSGPrimitiveBase<BasicTraits>::setObjectAdapter (ObjectAdapterType* adapter)
{
   m_original = adapter;
}
template <class BasicTraits>
inline typename OpenSGPrimitiveBase<BasicTraits>::GeomPrimitiveType 
OpenSGPrimitiveBase<BasicTraits>::getOriginal () const
{
   return m_originalPrimitive;
}
template <class BasicTraits>
inline const typename OpenSGPrimitiveBase<BasicTraits>::TransformType&
OpenSGPrimitiveBase<BasicTraits>::getTransform (u32 id) const
{
   return getObjectAdapter().getAdapterMatrix(id);
}


/*! \brief Adapter class for faces used in a bounding volume hierarchy.
*/
template <class BasicTraits, class BVOL>
class OSG_GENVISLIB_DLLMAPPING OpenSGPrimitive2BVol 
: public OpenSGPrimitiveBase<BasicTraits>, public BVolAdapter<BVOL>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef OpenSGPrimitiveBase<BasicTraits>          InheritedData;
   typedef BVolAdapter<BVOL>                         Inherited;
   typedef OpenSGPrimitive2BVol<BasicTraits,BVOL>    Self;
   typedef FactoryHeap<Self>                         FactoryType;
   typedef typename InheritedData::Cache             Cache;
   typedef typename InheritedData::CacheData         CacheData;
   typedef typename InheritedData::TransformType     TransformType;
   typedef typename InheritedData::GeomPrimitiveType GeomPrimitiveType;
   typedef typename InheritedData::ObjectAdapterType ObjectAdapterType;
   typedef typename Inherited::BVol                  BVol;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   OpenSGPrimitive2BVol ();
   OpenSGPrimitive2BVol (const GeomPrimitiveType& obj);
   OpenSGPrimitive2BVol (const TransformType& m2w, const GeomPrimitiveType&  obj);
   virtual ~OpenSGPrimitive2BVol ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Init.                                                        */
   /*! \{                                                                 */   
   void  init (const GeomPrimitiveType& obj);
   void  init (const TransformType& m2w, const GeomPrimitiveType&  obj);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Interface of BVolAdapterBase.                                */
   /*! \{                                                                 */   
   virtual PointClass                   getCenter ();
   virtual const BoundingVolume<Real>& getBoundingVolume () const;
   virtual bool  calcIntersect     (Intersection& hit);
   virtual void  draw          ();
   virtual void  drawWireframe ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Dump.                                                        */
   /*! \{                                                                 */   
   virtual std::ostream& dump (std::ostream& os) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   void calcBVol (BVol& bvol) const;
#ifdef GV_STOREDBVOL_PRIMITIVE
   BVOL m_bvol;
#endif
};

END_GENVIS_NAMESPACE
#endif






