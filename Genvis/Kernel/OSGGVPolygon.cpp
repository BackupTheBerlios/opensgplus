//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.2 $
//   $Date: 2004/12/20 15:58:23 $
//                                                                            
//=============================================================================

#include "OSGGVPolygon.h"
USING_GENVIS_NAMESPACE

Polygon3::Polygon3 ()
  : MeshFace<MeshAttribsBase,Polygon3Edge>()
{
}
Polygon3::~Polygon3 ()
{
}

PointClass  Polygon3::getPoint  (const MeshEdgeBase& e) const
{
   const Polygon3Edge& ee = (const Polygon3Edge&)e;
   return ee.coord;
}
PointClass  Polygon3::getPoint  (u32 i) const
{
   return edges[i].coord;
}

VectorClass  Polygon3::getNormal (const MeshEdgeBase& e) const
{
   const Polygon3Edge& ee = (const Polygon3Edge&)e;
   return (edges[0].coord - edges[1].coord).cross( 
	  edges[2].coord - edges[1].coord);
}
VectorClass  Polygon3::getNormal (u32 i) const
{
  return (edges[(i-1)%size()].coord - edges[i].coord).cross(
	 edges[(i+1)%size()].coord - edges[i].coord);
}


Polygon3Indexed::Polygon3Indexed ()
  : MeshFaceBase(), externalAttribs(NULL)
{
}
Polygon3Indexed::~Polygon3Indexed ()
{
}

PointClass Polygon3Indexed::getPoint  (const MeshEdgeBase& e) const
{
   const Polygon3EdgeIndexed& ee = (const Polygon3EdgeIndexed&)e;
   assert(ee.point < externalAttribs->points.size());
   return externalAttribs->points[ee.point];
}
PointClass Polygon3Indexed::getPoint  (u32 i) const
{
   u32 point = getEdge(i)->point; 
   assert(point < externalAttribs->points.size());
   return externalAttribs->points[point];
}
VectorClass Polygon3Indexed::getNormal  (const MeshEdgeBase& e) const
{
   const Polygon3EdgeIndexed* ee = (const Polygon3EdgeIndexed*)&e;
   VectorClass p0(ee->getPrev()->getPoint());
   VectorClass p1(ee->getPoint());
   VectorClass p2(ee->getNext()->getPoint());
   return (p0 - p1).cross(p2 - p1);
}
VectorClass Polygon3Indexed::getNormal  (u32 i) const
{
   VectorClass p0(getPoint(i%size()));
   VectorClass p1(getPoint((i+1)%size()));
   VectorClass p2(getPoint((i+2)%size()));
   return (p0-p1).cross(p2-p1);
}

u32 Polygon3EdgeIndexed::size () const
{
   u32 deg = 1;
   const Polygon3EdgeIndexed* iter = this;
   iter = iter->getCW();
   while (iter != this) {
      ++deg;
      iter = iter->getCW();
   }
   return deg;
}
Polygon3EdgeIndexed* Polygon3EdgeIndexed::newEdgeBefore ()
{
   return face->newEdgeBefore(this);
}
Polygon3EdgeIndexed* Polygon3EdgeIndexed::newEdgeAfter ()
{
   return face->newEdgeAfter(this);
}
Polygon3EdgeIndexed* Polygon3Indexed::getEdge (u32 i) const
{
   assert(i < edges.size());
   Container::const_iterator it = edges.begin();
   while (true) {
      if (i==0) {
	 break;
      }
      --i;
      ++it;
   }
   return (Polygon3EdgeIndexed*)(it.operator->());
}

Polygon3EdgeIndexed* Polygon3Indexed::newEdge ()
{
   Container::iterator 
     it = edges.insert(edges.end(), Polygon3EdgeIndexed(this));
   it->index = it;
   return it.operator->();
}
Polygon3EdgeIndexed* Polygon3Indexed::newEdgeBefore (Polygon3EdgeIndexed* e)
{
   Container::iterator 
     it = edges.insert(e->index, Polygon3EdgeIndexed(this));
   it->index = it;
   return it.operator->();
}
Polygon3EdgeIndexed* Polygon3Indexed::newEdgeAfter  (Polygon3EdgeIndexed* e)
{
   Container::iterator 
     it = edges.insert(e->getNext()->index, Polygon3EdgeIndexed(this));
   it->index = it;
   return it.operator->();
}
void Polygon3Indexed::erase  (Polygon3EdgeIndexed* e)
{
   edges.erase(e->index);
}
void Polygon3Indexed::eraseAll ()
{
   // erase all opposites
   Container::iterator it=edges.begin();
   while (it != edges.end()) {
      Container::iterator dummy = it; ++it;
      dummy->erase();
   }
}

Polygon3Indexed* Polygon3SetIndexed::getFace (u32 i) const
{
   assert(i < faces.size());
   Container::const_iterator it = faces.begin();
   while (true) {
      if (i==0) {
	 break;
      }
      --i;
      ++it;
   }
   return (Polygon3Indexed*)(it.operator->());
}
Polygon3Indexed* Polygon3SetIndexed::newFace ()
{
   u32 newIndex = faces.size();
   Container::iterator it = 
     faces.insert(faces.end(), Polygon3Indexed());
   it->externalAttribs = &attribs;
   it->index = newIndex;
   return it.operator->();
}

