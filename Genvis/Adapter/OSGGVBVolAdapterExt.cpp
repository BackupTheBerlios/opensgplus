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

#include "OSGGVBVolAdapterExt.h"

#include "OSGVector.h"
#include "OSGFieldContainerPtr.h"
#include "OSGNode.h"
#include "OSGNodeCore.h"
#include "OSGBoxVolume.h"
#include "OSGMFFieldContainerTypes.h"
#include "OSGQuaternion.h"
#include "OSGMatrix.h"
USING_GENVIS_NAMESPACE
OSG_USING_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING OpenSGTriangleBase  <OpenSGTraits>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGTriangle2BVol<OpenSGTraits, K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGTriangle2BVol<OpenSGTraits, K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGTriangle2BVol<OpenSGTraits, K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGTriangle2BVol<OpenSGTraits, K26Dop>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGTriangle2BVol<OpenSGTraits, K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGTriangle2BVol<OpenSGTraits, K20Dop>;


template <class BasicTraits>
OpenSGTriangleBase<BasicTraits>::OpenSGTriangleBase ()
  : m_original(NULL)
{
}
template <class BasicTraits>
OpenSGTriangleBase<BasicTraits>::~OpenSGTriangleBase ()
{
#ifdef GV_STOREDCOORDS_TRIANGLE
   subRefCP(m_coord);
   subRefCP(m_normal);
#endif
}

template <class BasicTraits>
GeoPositionsPtr OpenSGTriangleBase<BasicTraits>::getPositions (u32 id) const
{
#ifdef GV_STOREDCOORDS_TRIANGLE
   if (m_coord == NullFC) {
      GeoPositions3fPtr coord = GeoPositions3f::create();
      Self* nonconstThis = (Self*)this;
      nonconstThis->m_coord = coord;
      addRefCP(coord);
      GeoPositions3f::StoredFieldType* c = coord->getFieldPtr();
      GeomTriangleType it = getOriginal();
      TransformType trf = getTransform(id);
      if (trf == Matrix::identity()) {
	 beginEditCP(coord);
	 c->addValue(it.getPosition(0));
	 c->addValue(it.getPosition(1));
	 c->addValue(it.getPosition(2));
	 endEditCP(coord);
      } else {
	 beginEditCP(coord);
	 c->addValue(it.getPosition(0));
	 trf.mult(c->back());
	 c->addValue(it.getPosition(1));
	 trf.mult(c->back());
	 c->addValue(it.getPosition(2));
	 trf.mult(c->back());
	 endEditCP(coord);
      }
   }
   return m_coord;
#else
   GeoPositions3fPtr coord  = GeoPositions3f::create();
   GeoPositions3f::StoredFieldType* c = coord->getFieldPtr();
   GeomTriangleType it = getOriginal();
   TransformType  trf = getTransform(id);
   if (trf == Matrix::identity()) {
      beginEditCP(coord);
      c->addValue(it.getPosition(0));
      c->addValue(it.getPosition(1));
      c->addValue(it.getPosition(2));
      endEditCP(coord);
   } else {
      beginEditCP(coord);
      c->addValue(it.getPosition(0));
      trf.mult(c->back());
      c->addValue(it.getPosition(1));
      trf.mult(c->back());
      c->addValue(it.getPosition(2));
      trf.mult(c->back());
      endEditCP(coord);
   }
   return coord;
#endif
}

template <class BasicTraits>
GeoNormalsPtr   OpenSGTriangleBase<BasicTraits>::getNormals (u32 id) const
{
#ifdef GV_STOREDCOORDS_TRIANGLE
   if (m_normal == NullFC) {
      GeoNormals3fPtr normal = GeoNormals3f::create();
      Self* nonconstThis = (Self*)this;
      nonconstThis->m_normal = normal;
      addRefCP(normal);
      GeoNormals3f::StoredFieldType* n = normal->getFieldPtr();
      GeomTriangleType it = getOriginal();
      TransformType  trf = getTransform(id);
      if (trf == Matrix::identity()) {
	 beginEditCP(normal);
	 n->addValue(it.getNormal(0));
	 n->addValue(it.getNormal(1));
	 n->addValue(it.getNormal(2));
	 endEditCP(normal);
      } else {
	 beginEditCP(normal);
	 n->addValue(it.getNormal(0));
	 trf.mult(n->back());
	 n->addValue(it.getNormal(1));
	 trf.mult(n->back());
	 n->addValue(it.getNormal(2));
	 trf.mult(n->back());
	 endEditCP(normal);
      }
   }
   return m_normal;
#else
   GeoNormals3fPtr normal = GeoNormals3f::create();
   GeoNormals3f::StoredFieldType* n = normal->getFieldPtr();
   GeomTriangleType it = getOriginal();
   TransformType trf = getTransform(id);
   if (trf == Matrix::identity()) {
      beginEditCP(normal);
      n->addValue(it.getNormal(0));
      n->addValue(it.getNormal(1));
      n->addValue(it.getNormal(2));
      endEditCP(normal);
   } else {
      beginEditCP(normal);
      n->addValue(it.getNormal(0));
      trf.mult(n->back());
      n->addValue(it.getNormal(1));
      trf.mult(n->back());
      n->addValue(it.getNormal(2));
      trf.mult(n->back());
      endEditCP(normal);
   }
   return normal;
#endif
}


template <class BasicTraits, class BVOL>
OpenSGTriangle2BVol<BasicTraits,BVOL>::OpenSGTriangle2BVol ()
{
   m_originalId = 0;
}
template <class BasicTraits, class BVOL>
OpenSGTriangle2BVol<BasicTraits,BVOL>::OpenSGTriangle2BVol (const GeomTriangleType& obj)
{
   init(obj);
}
template <class BasicTraits, class BVOL>
OpenSGTriangle2BVol<BasicTraits,BVOL>::OpenSGTriangle2BVol (const TransformType& m2w,
							    const GeomTriangleType& obj)
{
   init(m2w, obj);
}

template <class BasicTraits, class BVOL>
void OpenSGTriangle2BVol<BasicTraits,BVOL>::update ()
{
   m_bvol.setEmpty(true);
   init(getTransform(Self::getAdapterId()), getOriginal());
}


template <class BasicTraits, class BVOL>
void  OpenSGTriangle2BVol<BasicTraits,BVOL>::init (const GeomTriangleType& obj)
{
   setOriginal(obj);
   m_point[0] = obj.getPosition(0);
   m_point[1] = obj.getPosition(1);
   m_point[2] = obj.getPosition(2);

   m_bvol.unify(m_point[0]);
   m_bvol.unify(m_point[1]);
   m_bvol.unify(m_point[2]);

   static FloatingComparator<Real> comp;
   m_bvol.unify(comp.getPrecision());
}
template <class BasicTraits, class BVOL>
void  OpenSGTriangle2BVol<BasicTraits,BVOL>::init (const TransformType&    m2w,
						   const GeomTriangleType& obj)
{
   setOriginal(obj);
   m_color[0] = obj.getColorIndex(0);
   m_color[1] = obj.getColorIndex(1);
   m_color[2] = obj.getColorIndex(2);

   m2w.mult(obj.getPosition(0), m_point[0]);
   m_bvol.unify(m_point[0]);
   m2w.mult(obj.getPosition(1), m_point[1]);
   m_bvol.unify(m_point[1]);
   m2w.mult(obj.getPosition(2), m_point[2]);
   m_bvol.unify(m_point[2]);

   static FloatingComparator<Real> comp;
   m_bvol.unify(comp.getPrecision());
}

template <class BasicTraits, class BVOL>
const BoundingVolume<Real>& 
OpenSGTriangle2BVol<BasicTraits,BVOL>::getBoundingVolume () const
{
   return m_bvol;
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
void OpenSGTriangle2BVol<BasicTraits,BVOL>::draw ()
{
   GeomTriangleType it(getOriginal());
#ifdef WIN32
   typedef void (APIENTRY*ColorFunction)  (const GLfloat*);
   typedef void (APIENTRY*NormalFunction) (const GLfloat*);
#else
   typedef void (*ColorFunction)  (const GLfloat*);
   typedef void (*NormalFunction) (const GLfloat*);
#endif
   ColorFunction color;
   if (true || it.getColorIndex(0) < 0) {
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

   //glColor3f(0.5f, 0.5f, 0.5f);
   //glColor3f(1.0f, 0.0f, 0.0f);
   const PointClass* coord = getPosition(); 
   glBegin(GL_TRIANGLES);
   color(it.getColor(0).getValuesRGB());
   glVertex3fv(coord[0].getValues());

   color(it.getColor(1).getValuesRGB());
   glVertex3fv(coord[1].getValues());

   color(it.getColor(2).getValuesRGB());
   glVertex3fv(coord[2].getValues());
   glEnd();
}

template <class BasicTraits, class BVOL>
void OpenSGTriangle2BVol<BasicTraits,BVOL>::drawWireframe ()
{
   GLfloat current[4];
   glGetFloatv(GL_CURRENT_COLOR, current);
   glColor3f(0.8f*current[0], 0.8f*current[1], 0.8f*current[2]);
   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   draw();
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   glColor3fv(current);
}

template <class BasicTraits, class BVOL>
inline bool OpenSGTriangle2BVol<BasicTraits,BVOL>::calcIntersect (Intersection& hit)
{
   Real t;
   VectorClass normal;
   if (hit.getRay().calcIntersect(getPosition(0), getPosition(1), getPosition(2), 
				  t, &normal)) { // intersection case
      if (hit.updateDist(t)) {
	 hit.setTo(this);
	 // additional intersection data
	 IntersectionNormal* data = new IntersectionNormal();
	 data->setSurfaceNormal(normal);
	 hit.setData(data);
	 return true;
      }
   }
   return false;
}

template <class BasicTraits, class BVOL>
std::ostream& OpenSGTriangle2BVol<BasicTraits,BVOL>::dump (std::ostream& os) const
{
   os << "OpenSGTriangle2BVol<BasicTraits>(";
   BVolAdapter<BVOL>::dump(os) << ")";
   return os;
}


