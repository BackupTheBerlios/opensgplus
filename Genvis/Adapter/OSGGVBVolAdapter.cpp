//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.3 $
//   $Date: 2004/12/20 15:56:34 $
//                                                                            
//=============================================================================

#include "OSGGVBVolAdapter.h"
#include "OSGGeoProperty.h"

USING_GENVIS_NAMESPACE
OSG_USING_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING OpenSGObjectBase<OpenSGTraits>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGObject2BVol<OpenSGTraits,K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGObject2BVol<OpenSGTraits,K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGObject2BVol<OpenSGTraits,K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGObject2BVol<OpenSGTraits,K26Dop>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGObject2BVol<OpenSGTraits,K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGObject2BVol<OpenSGTraits,K20Dop>;

template class OSG_GENVISLIB_DLLMAPPING OpenSGFaceBase  <OpenSGTraits>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGFace2BVol<OpenSGTraits,  K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGFace2BVol<OpenSGTraits,  K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGFace2BVol<OpenSGTraits,  K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGFace2BVol<OpenSGTraits,  K26Dop>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGFace2BVol<OpenSGTraits,  K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGFace2BVol<OpenSGTraits,  K20Dop>;

template class OSG_GENVISLIB_DLLMAPPING OpenSGPrimitiveBase  <OpenSGTraits>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGPrimitive2BVol<OpenSGTraits,  K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGPrimitive2BVol<OpenSGTraits,  K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGPrimitive2BVol<OpenSGTraits,  K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGPrimitive2BVol<OpenSGTraits,  K26Dop>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGPrimitive2BVol<OpenSGTraits,  K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGPrimitive2BVol<OpenSGTraits,  K20Dop>;


template <class BasicTraits>
OpenSGObjectBase<BasicTraits>::OpenSGObjectBase ()
{
}
template <class BasicTraits>
OpenSGObjectBase<BasicTraits>::OpenSGObjectBase (const OpenSGObjectBase<BasicTraits>& src)
  : m_original(src.m_original)
{
}
template <class BasicTraits>
OpenSGObjectBase<BasicTraits>::OpenSGObjectBase (const GeomObjectType& obj)
{
   setOriginal(obj);
}
template <class BasicTraits>
OpenSGObjectBase<BasicTraits>::OpenSGObjectBase (const TransformType&  m2w,
						 const GeomObjectType& obj,
						 u32 id)
{
   setOriginal(obj);
   setTransform(m2w, id);
}
template <class BasicTraits>
OpenSGObjectBase<BasicTraits>::~OpenSGObjectBase ()
{
}

template <class BasicTraits>
GeoPositionsPtr OpenSGObjectBase<BasicTraits>::getPositions (u32 id) const
{
   GeoPositionsPtr orgCoord = GeometryPtr::dcast(getOriginal()->getCore())->getPositions();
   const TransformType& trf = getTransform(id);
   if (trf == Matrix::identity()) {
      return orgCoord;  
   }

   GeoPositions3fPtr coord = GeoPositions3f::create();
   GeoPositions3f::StoredFieldType* c 
     = GeoPositions3fPtr::dcast(coord)->getFieldPtr();

   beginEditCP(coord);
   for (u32 i=0; i<orgCoord->getSize(); ++i) {
      c->addValue(orgCoord->getValue(i));
      trf.mult(c->back());
   }
   endEditCP(coord);
   return coord;
}
template <class BasicTraits>
GeoNormalsPtr   OpenSGObjectBase<BasicTraits>::getNormals (u32 id) const
{
   GeoNormalsPtr orgNormal = GeometryPtr::dcast(getOriginal()->getCore())->getNormals();
   const TransformType& trf = getTransform(id);
   if (trf == Matrix::identity()) {
      return orgNormal;  
   }

   GeoNormals3fPtr normal = GeoNormals3f::create();
   GeoNormals3f::StoredFieldType* n 
     = GeoNormals3fPtr::dcast(normal)->getFieldPtr();

   beginEditCP(normal);
   for (u32 i=0; i<orgNormal->getSize(); ++i) {
      n->addValue(orgNormal->getValue(i));
      trf.mult(n->back());
   }
   endEditCP(normal);
   return normal;
}
template <class BasicTraits>
GeoColorsPtr    OpenSGObjectBase<BasicTraits>::getColors (u32 id) const
{
   GeometryPtr geom = GeometryPtr::dcast(getOriginal()->getCore());
   return geom->getColors();
}
template <class BasicTraits>
GeoTexCoordsPtr OpenSGObjectBase<BasicTraits>::getTexCoords (u32 id) const
{
   GeometryPtr geom = GeometryPtr::dcast(getOriginal()->getCore());
   return geom->getTexCoords();
}


template <class BasicTraits, class BVOL>
OpenSGObject2BVol<BasicTraits,BVOL>::OpenSGObject2BVol ()
{
}
template <class BasicTraits, class BVOL>
OpenSGObject2BVol<BasicTraits,BVOL>::OpenSGObject2BVol (const GeomObjectType& obj)
{
   init(obj);
}
template <class BasicTraits, class BVOL>
OpenSGObject2BVol<BasicTraits,BVOL>::OpenSGObject2BVol (const TransformType& m2w,
							const GeomObjectType& obj)
{
   init(m2w, obj);
}

template <class BasicTraits, class BVOL>
OpenSGObject2BVol<BasicTraits,BVOL>::~OpenSGObject2BVol ()
{
}

template <class BasicTraits, class BVOL>
void  OpenSGObject2BVol<BasicTraits,BVOL>::init (const GeomObjectType& obj)
{
   GeometryPtr geom = GeometryPtr::dcast(obj->getCore());
   assert(geom != NullFC);
   setOriginal(obj);
   GeoPositionsPtr coord = geom->getPositions();
   PrimitiveIterator end = geom->endPrimitives(); 
   for (PrimitiveIterator it = geom->beginPrimitives(); 
	it != end;
	++it) {
      for (u32 i=0; i<it.getLength(); ++i) {
	 m_bvol.unify(it.getPosition(i));
      }
   }

   static FloatingComparator<Real> comp;
   m_bvol.unify(comp.getPrecision());
}
template <class BasicTraits, class BVOL>
void  OpenSGObject2BVol<BasicTraits,BVOL>::init (const TransformType& m2w,
						 const GeomObjectType& obj)
{
   GeometryPtr geom = GeometryPtr::dcast(obj->getCore());
   assert(geom != NullFC);
   setOriginal(obj);
   setTransform(m2w, Self::getAdapterId());
   GeoPositionsPtr coord = geom->getPositions();
   PointClass point;
   PrimitiveIterator end = geom->endPrimitives(); 
   for (PrimitiveIterator it = geom->beginPrimitives(); 
	it != end; 
	++it) {
        for (u32 i=0; i<it.getLength(); ++i) {
	   m2w.mult(it.getPosition(i), point);
	   m_bvol.unify(point);
        }
   }

   static FloatingComparator<Real> comp;
   m_bvol.unify(comp.getPrecision());
}

template <class BasicTraits, class BVOL>
const BoundingVolume<Real>& 
OpenSGObject2BVol<BasicTraits,BVOL>::getBoundingVolume () const
{
   return m_bvol;
}

template <class BasicTraits, class BVOL>
void OpenSGObject2BVol<BasicTraits,BVOL>::draw          ()
{
   //glColor3f(0.5f, 0.5f, 0.5f);
   getBoundingVolume().draw();
}
template <class BasicTraits, class BVOL>
void OpenSGObject2BVol<BasicTraits,BVOL>::drawWireframe ()
{
   GLfloat current[4];
   glGetFloatv(GL_CURRENT_COLOR, current);
   glColor3f(0.8f*current[0], 0.8f*current[1], 0.8f*current[2]);
   //glColor3f(0.8f, 0.8f, 0.8f);
   getBoundingVolume().drawWireframe();
   glColor3fv(current);
}

template <class BasicTraits, class BVOL>
bool OpenSGObject2BVol<BasicTraits,BVOL>::calcIntersect (Intersection& hit)
{
   const Ray& ray = hit.getRay();
   Matrix invTrf;
   getTransform(Self::getAdapterId()).inverse(invTrf); 
   PointClass origin;
   invTrf.mult(ray.getOrigin(),    origin);
   VectorClass dir;
   invTrf.mult(ray.getDirection(), dir);
   Ray   localRay(origin, dir);
   Real t;
   VectorClass normal;
   GeometryPtr geom = GeometryPtr::dcast(getOriginal()->getCore());
   bool result = false;
   for (TriangleIterator it = geom->beginTriangles(); 
	it != geom->endTriangles(); 
	++it) {
        if (localRay.calcIntersect(it.getPosition(0), it.getPosition(1), it.getPosition(2), 
				   t, &normal)) {
 	   if (hit.updateDist(t)) { // intersection on this object
	      hit.setTo(this);
	      // additional intersection data
	      IntersectionNormal* data = new IntersectionNormal();
	      data->setSurfaceNormal(normal);
	      hit.setData(data);
	      result = true;
	   }
        }
   }
   return result;
}

template <class BasicTraits, class BVOL>
std::ostream& OpenSGObject2BVol<BasicTraits,BVOL>::dump (std::ostream& os) const
{
   os << "OpenSGObject2BVol<BasicTraits>(";
   BVolAdapter<BVOL>::dump(os) << ")";
   return os;
}


// face
template <class BasicTraits>
GeoPositionsPtr OpenSGFaceBase<BasicTraits>::getPositions (u32 id) const
{
#ifdef GV_STOREDCOORDS_FACE
   if (m_coord == NullFC) {
      GeoPositions3fPtr coord = GeoPositions3f::create();
      Self* nonconstThis = (Self*)this;
      nonconstThis->m_coord = coord;
      GeoPositions3f::StoredFieldType* c 
	= GeoPositions3fPtr::dcast(coord)->getFieldPtr();
      GeomFaceType  it  = getOriginal();
      const TransformType& trf = getTransform(id);

      beginEditCP(coord);
      if (trf == Matrix::identity()) {
 	 for (u32 i=0; i<it.getLength(); ++i) {
	    c->addValue(it.getPosition(i));
	 }
      } else {
	 for (u32 i=0; i<it.getLength(); ++i) {
	    c->addValue(it.getPosition(i));
	    trf.mult(c->back());
	 }
      }
      endEditCP(coord);
      addRefCP(coord);
   }
   return m_coord;
#else
   GeoPositions3fPtr coord = GeoPositions3f::create();
   GeoPositions3f::StoredFieldType* c 
     = GeoPositions3fPtr::dcast(coord)->getFieldPtr();
   GeomFaceType  it  = getOriginal();
   const TransformType& trf = getTransform(id);

   beginEditCP(coord);
   if (trf == Matrix::identity()) {
      for (u32 i=0; i<it.getLength(); ++i) {
	 c->addValue(it.getPosition(i));
      }
   } else {
      for (u32 i=0; i<it.getLength(); ++i) {
	 c->addValue(it.getPosition(i));
	 trf.mult(c->back());
      }
   }
   endEditCP(coord);
   return coord;
#endif
}
template <class BasicTraits>
GeoNormalsPtr   OpenSGFaceBase<BasicTraits>::getNormals (u32 id) const
{
#ifdef GV_STOREDCOORDS_FACE
   if (m_normal == NullFC) {
      GeoNormals3fPtr normal = GeoNormals3f::create();
      Self* nonconstThis = (Self*)this;
      nonconstThis->m_normal = normal;
      GeoNormals3f::StoredFieldType* n 
	= normal->getFieldPtr();
      GeomFaceType  it  = getOriginal();
      const TransformType& trf = getTransform(id);

      beginEditCP(normal);
      if (trf == Matrix::identity()) {
	 for (u32 i=0; i<it.getLength(); ++i) {
	    n->addValue(it.getNormal(i));
	 }
      } else {
	 for (u32 i=0; i<it.getLength(); ++i) {
	    n->addValue(it.getNormal(i));
	    trf.mult(n->back());
	 }
      }
      endEditCP(normal);
      addRefCP(normal);
   }
   return m_normal;
#else
   GeoNormals3fPtr normal = GeoNormals3f::create();
   GeoNormals3f::StoredFieldType* n = normal->getFieldPtr();
   GeomFaceType  it  = getOriginal();
   const TransformType& trf = getTransform(id);

   beginEditCP(normal);
   if (trf == Matrix::identity()) {
      for (u32 i=0; i<it.getLength(); ++i) {
	 n->addValue(it.getNormal(i));
      }
   } else {
      for (u32 i=0; i<it.getLength(); ++i) {
	 n->addValue(it.getNormal(i));
	 trf.mult(n->back());
      }
   }
   endEditCP(normal);
   return normal;
#endif
}

template <class BasicTraits, class BVOL>
OpenSGFace2BVol<BasicTraits,BVOL>::OpenSGFace2BVol ()
  : OpenSGFaceBase<BasicTraits>(), BVolAdapter<BVOL>()
{
}
template <class BasicTraits, class BVOL>
OpenSGFace2BVol<BasicTraits,BVOL>::OpenSGFace2BVol (const GeomFaceType& obj)
  : OpenSGFaceBase<BasicTraits>(), BVolAdapter<BVOL>()
{
   init(obj);
}
template <class BasicTraits, class BVOL>
OpenSGFace2BVol<BasicTraits,BVOL>::OpenSGFace2BVol (const TransformType& m2w,
						    const GeomFaceType&  obj)
  : OpenSGFaceBase<BasicTraits>(), BVolAdapter<BVOL>()
{
   init(m2w, obj);
}
template <class BasicTraits, class BVOL>
OpenSGFace2BVol<BasicTraits,BVOL>::~OpenSGFace2BVol ()
{
#ifdef GV_STOREDCOORDS_FACE
   subRefCP(m_coord);
   subRefCP(m_normal);
#endif
}
template <class BasicTraits, class BVOL>
void  OpenSGFace2BVol<BasicTraits,BVOL>::init (const GeomFaceType& obj)
{
   m_originalId   = obj.getIndex();
   m_originalFace = obj;
#ifdef GV_STOREDBVOL_FACE
   calcBVol (m_bvol);
#endif
}
template <class BasicTraits, class BVOL>
void  OpenSGFace2BVol<BasicTraits,BVOL>::init (const TransformType&,
					       const GeomFaceType&  obj)
{
   m_originalId   = obj.getIndex();
   m_originalFace = obj;
#ifdef GV_STOREDBVOL_FACE
   calcBVol (m_bvol);
#endif
}

template <class BasicTraits, class BVOL>
void OpenSGFace2BVol<BasicTraits,BVOL>::calcBVol (BVol& bvol) const
{
   bvol.setEmpty(true);
   const TransformType& m2w = getTransform(Self::getAdapterId());
   if (m2w == TransformType::identity()) {
      for (u32 i=0; i<getOriginal().getLength(); ++i) {
	 bvol.unify(getOriginal().getPosition(i));
      }
   } else {
      PointClass point; 
      for (u32 i=0; i<getOriginal().getLength(); ++i) {
	 m2w.mult(getOriginal().getPosition(i), point);
	 bvol.unify(point);
      }
   }
   static FloatingComparator<Real> comp;
   bvol.unify(comp.getPrecision());
}

template <class BasicTraits, class BVOL>
PointClass OpenSGFace2BVol<BasicTraits,BVOL>::getCenter ()
{
   PointClass center;
   const TransformType& m2w = getTransform(Self::getAdapterId());
   if (m2w == TransformType::identity()) {
      center.setValue(getOriginal().getPosition(0));
   } else {
      m2w.mult(getOriginal().getPosition(0), center);
   }
   return center;
}
template <class BasicTraits, class BVOL>
const BoundingVolume<Real>& 
OpenSGFace2BVol<BasicTraits,BVOL>::getBoundingVolume () const
{
#ifdef GV_STOREDBVOL_FACE
   return m_bvol;
#else
   static BVol bvol;
   calcBVol(bvol);
   return bvol;
#endif
}

template <class BasicTraits, class BVOL>
bool OpenSGFace2BVol<BasicTraits,BVOL>::calcIntersect (Intersection& hit)
{
   const Ray& ray = hit.getRay();
   Matrix invTrf; getTransform(Self::getAdapterId()).inverse(invTrf); 
   PointClass origin;
   invTrf.mult(ray.getOrigin(),    origin);
   VectorClass dir;
   invTrf.mult(ray.getDirection(), dir);
   Ray   localRay(origin, dir);
   Real t;
   VectorClass normal;
   GeomFaceType face(getOriginal());
   PointClass p0 = face.getPosition(0);
   bool result = false;
   for (unsigned i=2; i<face.getLength(); ++i) {
       if (localRay.calcIntersect(p0, face.getPosition(i-1), face.getPosition(i), 
				  t, &normal)) {
	  if (hit.updateDist(t)) { // intersection on this face
	     hit.setTo(this);
	     // additional intersection data
	     IntersectionNormal* data = new IntersectionNormal();
	     data->setSurfaceNormal(normal);
	     hit.setData(data);
	     result = true;
	  }
       }
   }
   return result;
}

#ifdef WIN32
static void APIENTRY ConstantColor (const GLfloat* )
{
   glColor3f(1.0f, 0.0f, 0.0f);
}
static void APIENTRY Void (const GLfloat* )
{
}
#else
static void ConstantColor (const GLfloat* )
{
   glColor3f(1.0f, 0.0f, 0.0f);
}
static void Void (const GLfloat* )
{
}
#endif

template <class BasicTraits, class BVOL>
void OpenSGFace2BVol<BasicTraits,BVOL>::draw ()
{
   GeomFaceType it(getOriginal());
#ifdef WIN32
   typedef void (APIENTRY*ColorFunction)  (const GLfloat*);
   typedef void (APIENTRY*NormalFunction) (const GLfloat*);
#else
   typedef void (*ColorFunction)  (const GLfloat*);
   typedef void (*NormalFunction) (const GLfloat*);
#endif
   ColorFunction color;
   if (it.getColorIndex(0) < 0) {
      color = (ColorFunction)&Void;
   } else {
      color = (ColorFunction)&glColor3fv;
   }
   NormalFunction normal;
   if (it.getNormalIndex(0) < 0) {
      normal = (NormalFunction)&Void;
   } else {
      normal = (NormalFunction)&glNormal3fv;
   }

   //glColor3f(1.0f, 0.0f, 0.0f);
   GeoPositions3fPtr coordField = GeoPositions3fPtr::dcast(getPositions());
   addRefCP(coordField);
   GeoPositions3f::StoredFieldType& coord = coordField->getField();
   GeoNormals3fPtr   normField  = GeoNormals3fPtr::dcast(getNormals());
   addRefCP(normField);
   GeoNormals3f::StoredFieldType&   norm  = normField->getField();
   glBegin(GL_TRIANGLE_FAN);
   for (u32 i=0; i<coord.size(); ++i) {
      Color3f col = it.getColor(i);
      color(col.getValuesRGB());
      normal(norm[i].getValues());
      glVertex3fv(coord[i].getValues());
   }
   glEnd();
   subRefCP(coordField);
   subRefCP(normField);
}
template <class BasicTraits, class BVOL>
void OpenSGFace2BVol<BasicTraits,BVOL>::drawWireframe ()
{
   GLfloat current[4];
   GeomFaceType it(getOriginal());
   if (it.getColorIndex(0) < 0) {
      glGetFloatv(GL_CURRENT_COLOR, current);
      glColor3f(0.8f*current[0], 0.8f*current[1], 0.8f*current[2]);
   } else {
      Color3f col = it.getColor(0)*0.8f;
      glColor3fv(col.getValuesRGB());
   }
   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   draw();
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   if (it.getColorIndex(0) < 0) {
      glColor3fv(current);
   }
}

template <class BasicTraits, class BVOL>
std::ostream& OpenSGFace2BVol<BasicTraits,BVOL>::dump (std::ostream& os) const
{
   os << "OpenSGFace2BVol<BasicTraits>(";
   BVolAdapter<BVOL>::dump(os) << ")";
   return os;
}



// primitive
template <class BasicTraits>
GeoPositionsPtr OpenSGPrimitiveBase<BasicTraits>::getPositions (u32 id) const
{
#ifdef GV_STOREDCOORDS_PRIMITIVE
   if (m_coord == NullFC) {
      GeoPositions3fPtr coord = GeoPositions3f::create();
      Self* nonconstThis = (Self*)this;
      nonconstThis->m_coord = coord;
      GeoPositions3f::StoredFieldType* c 
	= GeoPositions3fPtr::dcast(coord)->getFieldPtr();
      GeomPrimitiveType  it  = getOriginal();
      const TransformType& trf = getTransform(id);

      beginEditCP(coord);
      if (trf == Matrix::identity()) {
 	 for (u32 i=0; i<it.getLength(); ++i) {
	    c->addValue(it.getPosition(i));
	 }
      } else {
	 for (u32 i=0; i<it.getLength(); ++i) {
	    c->addValue(it.getPosition(i));
	    trf.mult(c->back());
	 }
      }
      endEditCP(coord);
      addRefCP(coord);
   }
   return m_coord;
#else
   GeoPositions3fPtr coord = GeoPositions3f::create();
   GeoPositions3f::StoredFieldType* c 
     = GeoPositions3fPtr::dcast(coord)->getFieldPtr();
   GeomPrimitiveType  it  = getOriginal();
   const TransformType& trf = getTransform(id);

   beginEditCP(coord);
   if (trf == Matrix::identity()) {
      for (u32 i=0; i<it.getLength(); ++i) {
	 c->addValue(it.getPosition(i));
      }
   } else {
      for (u32 i=0; i<it.getLength(); ++i) {
	 c->addValue(it.getPosition(i));
	 trf.mult(c->back());
      }
   }
   endEditCP(coord);
   return coord;
#endif
}
template <class BasicTraits>
GeoNormalsPtr   OpenSGPrimitiveBase<BasicTraits>::getNormals (u32 id) const
{
#ifdef GV_STOREDCOORDS_PRIMITIVE
   if (m_normal == NullFC) {
      GeoNormals3fPtr normal = GeoNormals3f::create();
      Self* nonconstThis = (Self*)this;
      nonconstThis->m_normal = normal;
      GeoNormals3f::StoredFieldType* n 
	= normal->getFieldPtr();
      GeomPrimitiveType    it  = getOriginal();
      const TransformType& trf = getTransform(id);

      beginEditCP(normal);
      if (trf == Matrix::identity()) {
	 for (u32 i=0; i<it.getLength(); ++i) {
	    n->addValue(it.getNormal(i));
	 }
      } else {
	 for (u32 i=0; i<it.getLength(); ++i) {
	    n->addValue(it.getNormal(i));
	    trf.mult(n->back());
	 }
      }
      endEditCP(normal);
      addRefCP(normal);
   }
   return m_normal;
#else
   GeoNormals3fPtr normal = GeoNormals3f::create();
   GeoNormals3f::StoredFieldType* n = normal->getFieldPtr();
   GeomPrimitiveType    it  = getOriginal();
   const TransformType& trf = getTransform(id);

   beginEditCP(normal);
   if (trf == Matrix::identity()) {
      for (u32 i=0; i<it.getLength(); ++i) {
	 n->addValue(it.getNormal(i));
      }
   } else {
      for (u32 i=0; i<it.getLength(); ++i) {
	 n->addValue(it.getNormal(i));
	 trf.mult(n->back());
      }
   }
   endEditCP(normal);
   return normal;
#endif
}

template <class BasicTraits, class BVOL>
OpenSGPrimitive2BVol<BasicTraits,BVOL>::OpenSGPrimitive2BVol ()
  : OpenSGPrimitiveBase<BasicTraits>(), BVolAdapter<BVOL>()
{
}
template <class BasicTraits, class BVOL>
OpenSGPrimitive2BVol<BasicTraits,BVOL>::OpenSGPrimitive2BVol (const GeomPrimitiveType& obj)
  : OpenSGPrimitiveBase<BasicTraits>(), BVolAdapter<BVOL>()
{
   init(obj);
}
template <class BasicTraits, class BVOL>
OpenSGPrimitive2BVol<BasicTraits,BVOL>::OpenSGPrimitive2BVol (const TransformType& m2w,
							      const GeomPrimitiveType&  obj)
  : OpenSGPrimitiveBase<BasicTraits>(), BVolAdapter<BVOL>()
{
   init(m2w, obj);
}
template <class BasicTraits, class BVOL>
OpenSGPrimitive2BVol<BasicTraits,BVOL>::~OpenSGPrimitive2BVol ()
{
#ifdef GV_STOREDCOORDS_PRIMITIVE
   subRefCP(m_coord);
   subRefCP(m_normal);
#endif
}
template <class BasicTraits, class BVOL>
void  OpenSGPrimitive2BVol<BasicTraits,BVOL>::init (const GeomPrimitiveType& obj)
{
   m_originalId        = obj.getIndex();
   m_originalPrimitive = obj;
#ifdef GV_STOREDBVOL_PRIMITIVE
   calcBVol (m_bvol);
#endif
}
template <class BasicTraits, class BVOL>
void  OpenSGPrimitive2BVol<BasicTraits,BVOL>::init (const TransformType&,
						    const GeomPrimitiveType&  obj)
{
   m_originalId        = obj.getIndex();
   m_originalPrimitive = obj;
#ifdef GV_STOREDBVOL_PRIMITIVE
   calcBVol (m_bvol);
#endif
}

template <class BasicTraits, class BVOL>
void OpenSGPrimitive2BVol<BasicTraits,BVOL>::calcBVol (BVol& bvol) const
{
   bvol.setEmpty(true);
   const TransformType& m2w = getTransform(Self::getAdapterId());
   if (m2w == TransformType::identity()) {
      for (u32 i=0; i<getOriginal().getLength(); ++i) {
	 bvol.unify(getOriginal().getPosition(i));
      }
   } else {
      PointClass point; 
      for (u32 i=0; i<getOriginal().getLength(); ++i) {
	 m2w.mult(getOriginal().getPosition(i), point);
	 bvol.unify(point);
      }
   }
   static FloatingComparator<Real> comp;
   bvol.unify(comp.getPrecision());
}

template <class BasicTraits, class BVOL>
PointClass OpenSGPrimitive2BVol<BasicTraits,BVOL>::getCenter ()
{
   PointClass center;
   const TransformType& m2w = getTransform(Self::getAdapterId());
   if (m2w == TransformType::identity()) {
      center.setValue(getOriginal().getPosition(0));
   } else {
      m2w.mult(getOriginal().getPosition(0), center);
   }
   return center;
}
template <class BasicTraits, class BVOL>
const BoundingVolume<Real>& 
OpenSGPrimitive2BVol<BasicTraits,BVOL>::getBoundingVolume () const
{
#ifdef GV_STOREDBVOL_PRIMITIVE
   return m_bvol;
#else
   static BVol bvol;
   calcBVol(bvol);
   return bvol;
#endif
}

template <class BasicTraits, class BVOL>
bool OpenSGPrimitive2BVol<BasicTraits,BVOL>::calcIntersect (Intersection& hit)
{
   const Ray& ray = hit.getRay();
   Matrix invTrf; getTransform(Self::getAdapterId()).inverse(invTrf); 
   PointClass origin;
   invTrf.mult(ray.getOrigin(),    origin);
   VectorClass dir;
   invTrf.mult(ray.getDirection(), dir);
   Ray   localRay(origin, dir);
   Real t;
   VectorClass normal;
   GeomPrimitiveType primitive(getOriginal());
   PointClass p0 = primitive.getPosition(0);
   bool result = false;
   for (unsigned i=2; i<primitive.getLength(); ++i) {
       if (localRay.calcIntersect(p0, primitive.getPosition(i-1), primitive.getPosition(i), 
				  t, &normal)) {
	  if (hit.updateDist(t)) { // intersection on this primitive
	     hit.setTo(this);
	     // additional intersection data
	     IntersectionNormal* data = new IntersectionNormal();
	     data->setSurfaceNormal(normal);
	     hit.setData(data);
	     result = true;
	  }
       }
   }
   return result;
}

template <class BasicTraits, class BVOL>
void OpenSGPrimitive2BVol<BasicTraits,BVOL>::draw ()
{
   GeomPrimitiveType it(getOriginal());
#ifdef WIN32
   typedef void (APIENTRY*ColorFunction)  (const GLfloat*);
   typedef void (APIENTRY*NormalFunction) (const GLfloat*);
#else
   typedef void (*ColorFunction)  (const GLfloat*);
   typedef void (*NormalFunction) (const GLfloat*);
#endif
   ColorFunction color;
   if (it.getColorIndex(0) < 0) {
      color = (ColorFunction)&Void;
   } else {
      color = (ColorFunction)&glColor3fv;
   }
   NormalFunction normal;
   if (it.getNormalIndex(0) < 0) {
      normal = (NormalFunction)&Void;
   } else {
      normal = (NormalFunction)&glNormal3fv;
   }

   //glColor3f(1.0f, 0.0f, 0.0f);
   GeoPositions3fPtr coordField = GeoPositions3fPtr::dcast(getPositions());
   addRefCP(coordField);
   GeoPositions3f::StoredFieldType& coord = coordField->getField();
   GeoNormals3fPtr   normField  = GeoNormals3fPtr::dcast(getNormals());
   addRefCP(normField);
   GeoNormals3f::StoredFieldType&   norm  = normField->getField();
   glBegin(GL_TRIANGLE_FAN);
   for (u32 i=0; i<coord.size(); ++i) {
      Color3f col = it.getColor(i);
      color(col.getValuesRGB());
      normal(norm[i].getValues());
      glVertex3fv(coord[i].getValues());
   }
   glEnd();
   subRefCP(coordField);
   subRefCP(normField);
}
template <class BasicTraits, class BVOL>
void OpenSGPrimitive2BVol<BasicTraits,BVOL>::drawWireframe ()
{
   GLfloat current[4];
   GeomPrimitiveType it(getOriginal());
   if (it.getColorIndex(0) < 0) {
      glGetFloatv(GL_CURRENT_COLOR, current);
      glColor3f(0.8f*current[0], 0.8f*current[1], 0.8f*current[2]);
   } else {
      Color3f col = it.getColor(0)*0.8f;
      glColor3fv(col.getValuesRGB());
   }
   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   draw();
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   if (it.getColorIndex(0) < 0) {
      glColor3fv(current);
   }
}

template <class BasicTraits, class BVOL>
std::ostream& OpenSGPrimitive2BVol<BasicTraits,BVOL>::dump (std::ostream& os) const
{
   os << "OpenSGPrimitive2BVol<BasicTraits>(";
   BVolAdapter<BVOL>::dump(os) << ")";
   return os;
}

