//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.3 $
//   $Date: 2003/09/21 15:05:06 $
//                                                                            
//=============================================================================

#include <algorithm>
#include "OSGGVKDopGeometry.h"
#include "OSGGVKDop.h"
#include "OSGGVBVolFunctions.h"
#include "OSGGVGeomFunctions.h"
#include "OSGGL.h"
USING_GENVIS_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING KDopGeometry<Real,3>;
template class OSG_GENVISLIB_DLLMAPPING KDopGeometry<Real,7>;
template class OSG_GENVISLIB_DLLMAPPING KDopGeometry<Real,9>;
template class OSG_GENVISLIB_DLLMAPPING KDopGeometry<Real,13>;
template class OSG_GENVISLIB_DLLMAPPING KDopGeometry<Real,10>;
template class OSG_GENVISLIB_DLLMAPPING KDopGeometry<Real,6>;

/* 
   Half-space intersection
   Attention: BREP-face i corresponds to halfspace i defined by the KDOP
   class!
*/  

template <class REAL, int SIZE>
VectorClass KDopGeometry<REAL,SIZE>::s_x[SIZE+SIZE];
template <class REAL, int SIZE>
VectorClass KDopGeometry<REAL,SIZE>::s_y[SIZE+SIZE];
template <class REAL, int SIZE>
bool KDopGeometry<REAL,SIZE>::s_init = createFrames();

template <class REAL, int SIZE>
bool KDopGeometry<REAL,SIZE>::createFrames()
{
   for (int i=0; i<SIZE; ++i) {
      const VectorClass& normal = BVol::getDirection()[i]; 
      s_x[i].setValue(calcNormal(normal)); 
      s_y[i].setValue(normal.cross(s_x[i])); 
      s_x[i+SIZE].setValue(-s_x[i]);
      s_y[i+SIZE].setValue(-s_y[i]); 
   }
   return true;
}

void KDopGeometry<float,3>::calcGeometry ()
{
   createBox(getBVol(), m_geom);
}

void KDopGeometry<float,10>::calcGeometry ()
{
   createSlab(getBVol(), m_geom);
   unsigned i;
   for (i=1; i<Size; ++i) {
      cutHalfspace(i,      getBVol(), m_geom);
      cutHalfspace(i+Size, getBVol(), m_geom);
   }
   // delete faces 2*Size, 2*Size+1, 2*Size+2, 2*Size+3
   m_geom.faces.back().eraseAll();
   m_geom.faces.pop_back();
   m_geom.faces.back().eraseAll();
   m_geom.faces.pop_back();
   m_geom.faces.back().eraseAll();
   m_geom.faces.pop_back();
   m_geom.faces.back().eraseAll();
   m_geom.faces.pop_back();
}
void KDopGeometry<float,6>::calcGeometry ()
{
   createSlab(getBVol(), m_geom);
   unsigned i;
   for (i=1; i<Size; ++i) {
      cutHalfspace(i,      getBVol(), m_geom);
      cutHalfspace(i+Size, getBVol(), m_geom);
   }
   // delete faces 2*Size, 2*Size+1, 2*Size+2, 2*Size+3
   m_geom.faces.back().eraseAll();
   m_geom.faces.pop_back();
   m_geom.faces.back().eraseAll();
   m_geom.faces.pop_back();
   m_geom.faces.back().eraseAll();
   m_geom.faces.pop_back();
   m_geom.faces.back().eraseAll();
   m_geom.faces.pop_back();
}
template <class REAL, int SIZE>
void KDopGeometry<REAL,SIZE>::calcGeometry ()
{
   createBox(getBVol(), m_geom);
   unsigned i;
   for (i=3; i<SIZE; ++i) {
      cutHalfspace(i,      getBVol(), m_geom);
      cutHalfspace(i+SIZE, getBVol(), m_geom);
   }
}

template <class REAL, int SIZE>
void KDopGeometry<REAL,SIZE>::draw ()          const
{
   unsigned f=0;
   for (Polygon3SetIndexed::Container::const_iterator itF=m_geom.faces.begin(); 
	itF != m_geom.faces.end(); 
	++itF) { 
      glBegin(GL_POLYGON);
      glNormal3fv(getBVol().getNormal(f++).getValues());
      for (Polygon3Indexed::Container::const_iterator itE=itF->edges.begin(); 
	   itE != itF->edges.end();
	   ++itE) {
	 glVertex3fv(itE->getPoint().getValues());
      }
      glEnd();
   }
}
template <class REAL, int SIZE>
void KDopGeometry<REAL,SIZE>::drawWireframe ()          const
{
   unsigned f=0;
   for (Polygon3SetIndexed::Container::const_iterator itF=m_geom.faces.begin(); 
	itF != m_geom.faces.end(); 
	++itF) { 
      glBegin(GL_LINE_LOOP);
      glNormal3fv(getBVol().getNormal(f++).getValues());
      for (Polygon3Indexed::Container::const_iterator itE=itF->edges.begin(); 
	   itE != itF->edges.end();
	   ++itE) {
	 glVertex3fv(itE->getPoint().getValues());
      }
      glEnd();
   }
}

template <class REAL, int SIZE>
void       KDopGeometry<REAL,SIZE>::createSlab (const BVol& bvol,
					        Polygon3SetIndexed&    geom)
{
   geom.attribs.points.resize(8);
   VectorClass xOffset(infinity*getFrameX()[0]);
   VectorClass yOffset(infinity*getFrameY()[0]);
   VectorClass offset(bvol.minVector()[0]*bvol.getDirection()[0]);
   geom.attribs.points[0].setValue(offset - xOffset - yOffset);
   geom.attribs.points[1].setValue(offset - xOffset + yOffset);
   geom.attribs.points[2].setValue(offset + xOffset - yOffset);
   geom.attribs.points[3].setValue(offset + xOffset + yOffset);

   offset = bvol.maxVector()[0]*bvol.getDirection()[0];
   geom.attribs.points[4].setValue(offset - xOffset - yOffset);
   geom.attribs.points[5].setValue(offset - xOffset + yOffset);
   geom.attribs.points[6].setValue(offset + xOffset - yOffset);
   geom.attribs.points[7].setValue(offset + xOffset + yOffset);

   for (unsigned i=0; i<2*SIZE+4; ++i) {
      geom.newFace();
   }
   // CCW orientation
   // xmax
   Polygon3Indexed* face = geom.getFace(0);
   face->newEdge()->point = 4;
   face->newEdge()->point = 6;
   face->newEdge()->point = 7;
   face->newEdge()->point = 5;
   // ymax
   face = geom.getFace(2*SIZE);
   face->newEdge()->point = 2;
   face->newEdge()->point = 3;
   face->newEdge()->point = 7;
   face->newEdge()->point = 6;
   // zmax
   face = geom.getFace(2*SIZE+1);
   face->newEdge()->point = 1;
   face->newEdge()->point = 5;
   face->newEdge()->point = 7;
   face->newEdge()->point = 3;

   // xmin
   face = geom.getFace(SIZE);
   face->newEdge()->point = 0;
   face->newEdge()->point = 1;
   face->newEdge()->point = 3;
   face->newEdge()->point = 2;
   // ymin
   face = geom.getFace(2*SIZE+2);
   face->newEdge()->point = 0;
   face->newEdge()->point = 4;
   face->newEdge()->point = 5;
   face->newEdge()->point = 1;
   // zmin
   face = geom.getFace(2*SIZE+3);
   face->newEdge()->point = 0;
   face->newEdge()->point = 2;
   face->newEdge()->point = 6;
   face->newEdge()->point = 4;

   // neighbourhood xmax
   geom.getFace(0)->getEdge(0)->setOpposit(geom.getFace(2*SIZE+3)->getEdge(2));
   geom.getFace(0)->getEdge(1)->setOpposit(geom.getFace(2*SIZE)->getEdge(2));
   geom.getFace(0)->getEdge(2)->setOpposit(geom.getFace(2*SIZE+1)->getEdge(1));
   geom.getFace(0)->getEdge(3)->setOpposit(geom.getFace(2*SIZE+2)->getEdge(1));
   // neighbourhood xmin
   geom.getFace(SIZE)->getEdge(0)->setOpposit(geom.getFace(2*SIZE+2)->getEdge(3));
   geom.getFace(SIZE)->getEdge(1)->setOpposit(geom.getFace(2*SIZE+1)->getEdge(3));
   geom.getFace(SIZE)->getEdge(2)->setOpposit(geom.getFace(2*SIZE)->getEdge(0));
   geom.getFace(SIZE)->getEdge(3)->setOpposit(geom.getFace(2*SIZE+3)->getEdge(0));
   // neighbourhood ymax
   geom.getFace(2*SIZE)->getEdge(1)->setOpposit(geom.getFace(2*SIZE+1)->getEdge(2));
   geom.getFace(2*SIZE)->getEdge(3)->setOpposit(geom.getFace(2*SIZE+3)->getEdge(1));
   // neighbourhood ymin
   geom.getFace(2*SIZE+2)->getEdge(0)->setOpposit(geom.getFace(2*SIZE+3)->getEdge(3));
   geom.getFace(2*SIZE+2)->getEdge(2)->setOpposit(geom.getFace(2*SIZE+1)->getEdge(0));
}
template <class REAL, int SIZE>
void       KDopGeometry<REAL,SIZE>::createBox   (const BVol& bvol,
						 Polygon3SetIndexed&    geom)
{
   geom.attribs.points.resize(8);
   geom.attribs.points[0].setValues(bvol.minVector()[0], bvol.minVector()[1], bvol.minVector()[2]);
   geom.attribs.points[1].setValues(bvol.minVector()[0], bvol.minVector()[1], bvol.maxVector()[2]);
   geom.attribs.points[2].setValues(bvol.minVector()[0], bvol.maxVector()[1], bvol.minVector()[2]);
   geom.attribs.points[3].setValues(bvol.minVector()[0], bvol.maxVector()[1], bvol.maxVector()[2]);

   geom.attribs.points[4].setValues(bvol.maxVector()[0], bvol.minVector()[1], bvol.minVector()[2]);
   geom.attribs.points[5].setValues(bvol.maxVector()[0], bvol.minVector()[1], bvol.maxVector()[2]);
   geom.attribs.points[6].setValues(bvol.maxVector()[0], bvol.maxVector()[1], bvol.minVector()[2]);
   geom.attribs.points[7].setValues(bvol.maxVector()[0], bvol.maxVector()[1], bvol.maxVector()[2]);

   // CCW orientation
   // xmax
   Polygon3Indexed* face = geom.newFace();
   assert(face == geom.getFace(0));
   face->newEdge()->point = 4;
   face->newEdge()->point = 6;
   face->newEdge()->point = 7;
   face->newEdge()->point = 5;
   // ymax
   face = geom.newFace();
   assert(face == geom.getFace(1));
   face->newEdge()->point = 2;
   face->newEdge()->point = 3;
   face->newEdge()->point = 7;
   face->newEdge()->point = 6;
   // zmax
   face = geom.newFace();
   assert(face == geom.getFace(2));
   face->newEdge()->point = 1;
   face->newEdge()->point = 5;
   face->newEdge()->point = 7;
   face->newEdge()->point = 3;

   unsigned i;
   for (i=3; i<SIZE; ++i) {
      geom.newFace();
   }

   // xmin
   face = geom.newFace();
   assert(face == geom.getFace(SIZE));
   face->newEdge()->point = 0;
   face->newEdge()->point = 1;
   face->newEdge()->point = 3;
   face->newEdge()->point = 2;
   // ymin
   face = geom.newFace();
   assert(face == geom.getFace(SIZE+1));
   face->newEdge()->point = 0;
   face->newEdge()->point = 4;
   face->newEdge()->point = 5;
   face->newEdge()->point = 1;
   // zmin
   face = geom.newFace();
   assert(face == geom.getFace(SIZE+2));
   face->newEdge()->point = 0;
   face->newEdge()->point = 2;
   face->newEdge()->point = 6;
   face->newEdge()->point = 4;

   for (i=3; i<SIZE; ++i) {
      geom.newFace();
   }

   // neighbourhood xmax
   geom.getFace(0)->getEdge(0)->setOpposit(geom.getFace(SIZE+2)->getEdge(2));
   geom.getFace(0)->getEdge(1)->setOpposit(geom.getFace(1)->getEdge(2));
   geom.getFace(0)->getEdge(2)->setOpposit(geom.getFace(2)->getEdge(1));
   geom.getFace(0)->getEdge(3)->setOpposit(geom.getFace(SIZE+1)->getEdge(1));
   // neighbourhood xmin
   geom.getFace(SIZE)->getEdge(0)->setOpposit(geom.getFace(SIZE+1)->getEdge(3));
   geom.getFace(SIZE)->getEdge(1)->setOpposit(geom.getFace(2)->getEdge(3));
   geom.getFace(SIZE)->getEdge(2)->setOpposit(geom.getFace(1)->getEdge(0));
   geom.getFace(SIZE)->getEdge(3)->setOpposit(geom.getFace(SIZE+2)->getEdge(0));
   // neighbourhood ymax
   geom.getFace(1)->getEdge(1)->setOpposit(geom.getFace(2)->getEdge(2));
   geom.getFace(1)->getEdge(3)->setOpposit(geom.getFace(SIZE+2)->getEdge(1));
   // neighbourhood ymin
   geom.getFace(SIZE+1)->getEdge(0)->setOpposit(geom.getFace(SIZE+2)->getEdge(3));
   geom.getFace(SIZE+1)->getEdge(2)->setOpposit(geom.getFace(2)->getEdge(0));
}

template <class REAL, int SIZE>
void KDopGeometry<REAL,SIZE>::cutHalfspace(unsigned i,
					   const BVol& bvol,
					   Polygon3SetIndexed& geom)
{
   static FloatingComparator<REAL> comp(1e-4);

   const VectorClass&  normal = bvol.getNormal(i);
   REAL                scalar = bvol.getScalar(i);

   // find first out-in edge
   Polygon3EdgeIndexed* firstStart = NULL;
   Polygon3EdgeIndexed* start      = NULL;
   REAL in, out;
   for (Polygon3SetIndexed::Container::iterator itF=geom.faces.begin(); 
	itF != geom.faces.end() && firstStart == NULL; 
	++itF) {
      for (Polygon3Indexed::Container::iterator itE=itF->edges.begin(); 
	   itE != itF->edges.end();
	   ++itE) {
	 start = &(*itE);
	 if (comp.greater(out=normal.dot(start->getPoint())-scalar, 0.0)
	     && comp.less(in=normal.dot(start->getOpposit()->getPoint())-scalar, 0.0)) {
	    firstStart = start;
	    break;
	 }
      }
   }
   // new face in plane (normal, scalar)
   Polygon3Indexed* newFace = geom.getFace(i);
   if (firstStart == NULL) {
      return;
   }

   // create new edge
   in = -in;
   REAL sum = in + out;
   geom.attribs.points.push_back((in/sum)*VectorClass(firstStart->getPoint()) 
				 + (out/sum)*VectorClass(firstStart->getOpposit()->getPoint()));
   unsigned firstPoint = geom.attribs.points.size()-1;
   unsigned point      = firstPoint;
   while (true) {
      // find first in-out edge
#ifdef GV_DEBUG
      std::cout << "(start=" << start
		<< ", " << in
		<< ", " << out
		<< ")"
		<< std::endl;
#endif
      Polygon3EdgeIndexed* end = start->getNext();
      while (end != start
	     && end->getOpposit() != firstStart) {
	 if (comp.leq(in=normal.dot(end->getPoint())-scalar, 0.0)
	     && comp.greater(out=normal.dot(end->getOpposit()->getPoint())-scalar, 0.0)) { 
	    break;
	 }
#ifdef GV_DEBUG
	 std::cout << " (" << end 
		   << ", " << in
		   << ", " << out
		   << ")"
		   << std::flush;
#endif
	 end = end->getNext();
      }
#ifdef GV_DEBUG
      std::cout << std::endl;
#endif
      if (start == end) {
	 Polygon3EdgeIndexed* newEdge = newFace->newEdge();
	 Polygon3EdgeIndexed* oppEdge = start->newEdgeBefore();
	 newEdge->point = point;
	 start->point   = point;
	 oppEdge->point = firstPoint;
	 newEdge->setOpposit(oppEdge);
	 if (newFace->size() < 3) {
	    std::cout << "Warning: Face " << i << ": "
		      << newFace->size() << " edges!" 
		      << std::endl;
	 }
	 return;	
      }
      if (end->getOpposit() == firstStart) {
	 // delete out edges
	 Polygon3EdgeIndexed* current = end->getNext();
	 while (current != start) {
 	    Polygon3EdgeIndexed* dummy = current->getNext();
	    current->erase();
	    current = dummy; 
	 }
	 Polygon3EdgeIndexed* newEdge = newFace->newEdge();
	 Polygon3EdgeIndexed* oppEdge = start->newEdgeBefore();
	 newEdge->point = point;
	 start->point   = point;
	 oppEdge->point = firstPoint;
	 newEdge->setOpposit(oppEdge);
	 if (newFace->size() < 3) {
	    std::cout << "Warning: Face " << i << ": "
		      << newFace->size() << " edges!" 
		      << std::endl;
	 }
	 return;
      }
      in  = stdAbs(in);
      sum = in + out;   
      geom.attribs.points.push_back((in/sum)*VectorClass(end->getOpposit()->getPoint()) 
				    + (out/sum)*VectorClass(end->getPoint()));
      // delete out edges
      Polygon3EdgeIndexed* current = end->getNext();
      while (current != start) {
	 Polygon3EdgeIndexed* dummy = current->getNext();
	 current->erase();
	 current = dummy; 
      }
      Polygon3EdgeIndexed* newEdge = newFace->newEdge();
      Polygon3EdgeIndexed* oppEdge = start->newEdgeBefore();
      newEdge->point = point;
      start->point   = point;
      oppEdge->point = point = geom.attribs.points.size()-1;
      newEdge->setOpposit(oppEdge);
      // next face
      start = end->getOpposit();
   }
}

template <class REAL, int SIZE>
REAL KDopGeometry<REAL,SIZE>::getSurfaceArea () const
{
   REAL area = 0.0;
   unsigned f = 0;
   for (Polygon3SetIndexed::Container::const_iterator itF=m_geom.faces.begin(); 
	itF != m_geom.faces.end(); 
	++itF, ++f) { 
      if (itF->size() < 3) { 
	 continue; 
      }
      Polygon3Indexed::Container::const_iterator itE = itF->edges.begin();
      PointClass point = itE->getPoint();
      ++itE;
      REAL lastX = 0.0;
      REAL lastY = 0.0;
      REAL current = 0.0;
      while (itE != itF->edges.end()) {
	 VectorClass diff = itE->getPoint()-point;
	 REAL x = diff.dot(s_x[f]);
	 REAL y = diff.dot(s_y[f]);
	 // area of trapezoid ((lastX, 0), (x, 0), (x, y), (lastX, lastY))
	 area -= (y+lastY)*(x-lastX);
	 lastX = x;
	 lastY = y;	 
	 ++itE;
      }
      area += lastY*lastX;
   }
   assert(area >= 0.0);
   return 0.5*area;
}

template <class REAL, int SIZE>
REAL KDopGeometry<REAL,SIZE>::getVolume      () const
{
   static REAL factor = 1.0/6.0;
   PointClass  center = getBVol().getCenter();
   REAL vol = 0.0;
   for (Polygon3SetIndexed::Container::const_iterator itF=m_geom.faces.begin(); 
	itF != m_geom.faces.end(); 
	++itF) {
      if (itF->size() < 3) { 
	 continue; 
      }
      Polygon3Indexed::Container::const_iterator itE = itF->edges.begin();
      PointClass  p0 = itE->getPoint();
      VectorClass d2 = p0 - center;
      ++itE;
      VectorClass d0 = p0 - itE->getPoint();
      for (++itE; 
	   itE != itF->edges.end();
	   ++itE) {
 	 VectorClass d1 = p0 - itE->getPoint();
	 // volume of tetrahedron (p_0, p_i-1, p_i, center)
	 vol += (+d0[0]*d1[1]*d2[2] - d0[0]*d1[2]*d2[1]
		 +d0[1]*d1[2]*d2[0] - d0[1]*d1[0]*d2[2]
		 +d0[2]*d1[0]*d2[1] - d0[2]*d1[1]*d2[0]);
	 d0 = d1;
      }
   }
   assert(vol >= 0.0);
   return factor*vol;
}

