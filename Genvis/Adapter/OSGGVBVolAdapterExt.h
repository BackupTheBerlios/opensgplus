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

#ifndef OSGGVBVOLADAPTEREXT_H
#define OSGGVBVOLADAPTEREXT_H

#include "OSGGVBase.h"
#include "OSGGVTraits.h"
#include "OSGGVBVolAdapter.h"
#include "OSGGVAdapterFactory.h"

BEGIN_GENVIS_NAMESPACE

/*! Base for adapter classes associated with triangles.
*/
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING OpenSGTriangleBase
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef typename BasicTraits::Cache                 Cache;
   typedef typename BasicTraits::CacheData             CacheData;
   typedef typename BasicTraits::TransformType         TransformType;
   typedef typename BasicTraits::GeomTriangleType      GeomTriangleType;
   typedef CacheData                                   ObjectAdapterType;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   OpenSGTriangleBase ();
   virtual ~OpenSGTriangleBase ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */
   inline GeomTriangleType         getOriginal      () const;
   inline const i32*               getColorIndices  () const;
   inline ObjectAdapterType&       getObjectAdapter () const;
   inline void                     setObjectAdapter (ObjectAdapterType* adapter);
   inline TransformType            getTransform (u32 id) const;
   inline const PointClass*        getPosition  () const; 
   inline const PointClass&        getPosition  (u32 i) const; 
   inline VectorClass              getNormal    (u32 i, 
						 u32 id=BVolAdapterBase::getAdapterId()) const; 
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Internal Members.                                            */
   /*! \{                                                                 */
   /*! Set triangle index.                                                */
   inline void                     setOriginal   (const GeomTriangleType& org);
   inline void                     setOriginalId (i32 id);
   /*! Get triangle index.                                                */
   inline i32                      getOriginalId () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Transformed geometry fields.                                 */
   /*! \{                                                                 */
   OSG::GeoPositionsPtr            getPositions (u32 id=BVolAdapterBase::getAdapterId()) const; 
   OSG::GeoNormalsPtr              getNormals (u32 id=BVolAdapterBase::getAdapterId()) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   ObjectAdapterType*           m_original;
   i32                          m_originalId;
   i32                          m_color[3];
   PointClass                   m_point[3];
#ifdef GV_STOREDCOORDS_TRIANGLE
   OSG::GeoPositionsPtr m_coord; 
   OSG::GeoNormalsPtr   m_normal;
#endif
};
typedef OpenSGTriangleBase<OpenSGTraits> OSGTriangleBase;

template <class BasicTraits>
inline typename OpenSGTriangleBase<BasicTraits>::ObjectAdapterType&
OpenSGTriangleBase<BasicTraits>::getObjectAdapter () const
{
   assert(m_original != NULL);
   return *m_original;
}
template <class BasicTraits>
inline void
OpenSGTriangleBase<BasicTraits>::setObjectAdapter (ObjectAdapterType* adapter)
{
   m_original = adapter;
}
template <class BasicTraits>
inline i32 OpenSGTriangleBase<BasicTraits>::getOriginalId () const
{
   return m_originalId;
}
template <class BasicTraits>
inline typename OpenSGTriangleBase<BasicTraits>::GeomTriangleType 
OpenSGTriangleBase<BasicTraits>::getOriginal () const
{
   // getObjectAdapter().getOriginal() is a NodePtr
   // need the NodeCorePtr here!
   GeomTriangleType it(GeometryPtr((Geometry*)(getObjectAdapter().getOriginal()->getCore().getCPtr()))); 
   it.seek(m_originalId);
   return it;
}
template <class BasicTraits>
inline const i32* OpenSGTriangleBase<BasicTraits>::getColorIndices () const
{
   return m_color;
}
template <class BasicTraits>
inline void OpenSGTriangleBase<BasicTraits>::setOriginal (const GeomTriangleType& org)
{
   m_originalId = org.getIndex();
   m_color[0] = org.getColorIndex(0);
   m_color[1] = org.getColorIndex(1);
   m_color[2] = org.getColorIndex(2);
}
template <class BasicTraits>
inline void OpenSGTriangleBase<BasicTraits>::setOriginalId (i32 id)
{
   m_originalId = id;
   GeomTriangleType tri = getOriginal();
   m_color[0] = tri.getColorIndex(0);
   m_color[1] = tri.getColorIndex(1);
   m_color[2] = tri.getColorIndex(2);
}
template <class BasicTraits>
inline typename OpenSGTriangleBase<BasicTraits>::TransformType 
OpenSGTriangleBase<BasicTraits>::getTransform (u32 id) const
{
   return getObjectAdapter().getAdapterMatrix(id);
}
template <class BasicTraits>
inline const PointClass* 
OpenSGTriangleBase<BasicTraits>::getPosition  () const
{
   return m_point;
}
template <class BasicTraits>
inline const PointClass& 
OpenSGTriangleBase<BasicTraits>::getPosition (u32 i) const
{
   assert (i < 3);
   return m_point[i];
}
template <class BasicTraits>
inline VectorClass 
OpenSGTriangleBase<BasicTraits>::getNormal (u32 i, u32 id) const
{
   assert (i < 3);
   TransformType trf = getTransform(id);
   if (trf == Matrix::identity()) {
      return getOriginal().getNormal(i);
   }
   VectorClass normal;
   trf.mult(getOriginal().getNormal(i), normal);
   return normal;
}


/*! Adapter class for triangles using in a bounding volume hierarchy.
*/
template <class BasicTraits, class BVOL>
class OSG_GENVISLIB_DLLMAPPING OpenSGTriangle2BVol 
: public OpenSGTriangleBase<BasicTraits>, public BVolAdapter<BVOL>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef OpenSGTriangleBase<BasicTraits>           InheritedData;
   typedef BVolAdapter<BVOL>                         Inherited;
   typedef OpenSGTriangle2BVol<BasicTraits,BVOL>     Self;
   typedef FactoryHeap<Self>                         FactoryType;
   typedef typename InheritedData::Cache             Cache;
   typedef typename InheritedData::CacheData         CacheData;
   typedef typename InheritedData::TransformType     TransformType;
   typedef typename InheritedData::GeomTriangleType  GeomTriangleType;
   typedef typename InheritedData::ObjectAdapterType ObjectAdapterType;
   typedef typename Inherited::BVol                  BVol;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   OpenSGTriangle2BVol ();
   OpenSGTriangle2BVol (const GeomTriangleType& obj);
   OpenSGTriangle2BVol (const TransformType& m2w, const GeomTriangleType& obj);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Init.                                                        */
   /*! \{                                                                 */
   void  init (const GeomTriangleType& obj);
   void  init (const TransformType& m2w, const GeomTriangleType& obj);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Interface of BVolAdapterBase.                                */
   /*! \{                                                                 */ 
   virtual void  update ();
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

protected:
   BVOL m_bvol;
};

END_GENVIS_NAMESPACE
#endif






