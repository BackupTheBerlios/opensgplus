//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.4 $
//   $Date: 2006/06/08 16:57:46 $
//                                                                            
//=============================================================================

#include "OSGGVRAPIDAdapter.h"
#include "OSGGVBVolHierarchy.h"
#ifdef GV_WITH_RAPID
# include "RAPID.H"
#endif
OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

BEGIN_GENVIS_NAMESPACE

template <class BasicTraits>
OpenSGRAPIDFaceAdapter<BasicTraits>::OpenSGRAPIDFaceAdapter ()
  : InheritedData(), Inherited()
{
}
template <class BasicTraits>
OpenSGRAPIDFaceAdapter<BasicTraits>::OpenSGRAPIDFaceAdapter (InternalObjectType* adapter, i32 id)
  : InheritedData(), Inherited()
{
   init(adapter, id);
}
template <class BasicTraits>
OpenSGRAPIDFaceAdapter<BasicTraits>::OpenSGRAPIDFaceAdapter (ObjectAdapterType* adapter, i32 id)
  : InheritedData(), Inherited()
{
   init(adapter, id);
}
template <class BasicTraits>
OpenSGRAPIDFaceAdapter<BasicTraits>::~OpenSGRAPIDFaceAdapter ()
{
}

template <class BasicTraits>
void OpenSGRAPIDFaceAdapter<BasicTraits>::init (InternalObjectType* adapter, i32 id)
{
   CacheData& data = Cache::the()[adapter->getOriginal()];
   setObjectAdapter(&data);
   setOriginalId(id);
}
template <class BasicTraits>
void OpenSGRAPIDFaceAdapter<BasicTraits>::init (ObjectAdapterType* adapter, i32 id)
{
   setObjectAdapter(adapter);
   setOriginalId(id);
}

template <class BasicTraits>
std::ostream& OpenSGRAPIDFaceAdapter<BasicTraits>::dump (std::ostream& os) const
{
   os << "OpenSGRAPIDFaceAdapter(" << getOriginalId() << ")";
   return os;
}


template <class BasicTraits>
OpenSGRAPIDAdapter<BasicTraits>::OpenSGRAPIDAdapter ()
  : Inherited()
{
   m_model = NULL;
}
template <class BasicTraits>
OpenSGRAPIDAdapter<BasicTraits>::~OpenSGRAPIDAdapter ()
{
#ifdef GV_WITH_RAPID
   delete m_model;
#endif
}

template <class BasicTraits>
const BoundingVolume<Real>& OpenSGRAPIDAdapter<BasicTraits>::getBoundingVolume () const
{
   static K6Dop bvol;

   // get scenegraph bounding volume
   VectorClass minBound, maxBound;
   getObjectAdapter().getNode()->getVolume().getBounds(minBound, maxBound); 
   bvol.minVector()[0] = minBound[0];
   bvol.minVector()[1] = minBound[1];
   bvol.minVector()[2] = minBound[2];
   bvol.maxVector()[0] = maxBound[0];
   bvol.maxVector()[1] = maxBound[1];
   bvol.maxVector()[2] = maxBound[2];

   return bvol;
}

template <class BasicTraits>
unsigned OpenSGRAPIDAdapter<BasicTraits>::getAdapterId ()
{
   static u32 id = Adapter::getNextId();
   return id;
}
template <class BasicTraits>
inline u32 OpenSGRAPIDAdapter<BasicTraits>::getPrivateId()
{
   static u32 id = Adapter::getNextId();
   return id;
}

template <class BasicTraits>
OpenSGRAPIDAdapter<BasicTraits>::OpenSGRAPIDAdapter (const GeomObjectType& obj)
  : Inherited()
{
   m_model = NULL;
   init(obj);
}
template <class BasicTraits>
OpenSGRAPIDAdapter<BasicTraits>::OpenSGRAPIDAdapter (const TransformType&  m2w,
						     const GeomObjectType& obj)
  : Inherited()
{
   m_model = NULL;
   init(m2w, obj);
}

template <class BasicTraits>
void  OpenSGRAPIDAdapter<BasicTraits>::init (const GeomObjectType& obj)
{
#ifdef GV_WITH_RAPID
   typedef double RAPIDVector3[3];
   GeometryPtr geom = GeometryPtr::dcast(obj->getCore());

   setOriginal(obj);

   m_model = new RAPID_model;
   m_model->BeginModel();
   i32 i=0;
   TriangleIterator end = geom->endTriangles();
   RAPIDVector3 p0, p1, p2;
   for (OSG::TriangleIterator it = geom->beginTriangles(); 
	it != end; 
	++it, ++i) {
      p0[0] = (double)it.getPosition(0)[0]; p0[1] = (double)it.getPosition(0)[1]; p0[2] = (double)it.getPosition(0)[2]; 
      p1[0] = (double)it.getPosition(1)[0]; p1[1] = (double)it.getPosition(1)[1]; p1[2] = (double)it.getPosition(1)[2]; 
      p2[0] = (double)it.getPosition(2)[0]; p2[1] = (double)it.getPosition(2)[1]; p2[2] = (double)it.getPosition(2)[2]; 
      m_model->AddTri(p0, p1, p2, i);
   }
   m_model->EndModel();
#endif
}
template <class BasicTraits>
void  OpenSGRAPIDAdapter<BasicTraits>::init (const TransformType& m2w, const GeomObjectType& obj)
{
   // build adapters
   OSGObjectHierarchy hier;
   hier.addAdapter(m2w, obj);
}
template <class BasicTraits>
void  OpenSGRAPIDAdapter<BasicTraits>::postInit (const GeomObjectType& obj)
{
#ifdef GV_WITH_RAPID
   typedef double RAPIDVector3[3];
   // find coordinate system
   setOriginal(obj);

   m_index.clear();
   delete m_model; m_model = NULL;

   typename Cache::AdapterVector prim;
   Cache::the().collectAdapter(BVolAdapterBase::getAdapterId(), prim, obj, true);
   if (prim.size() == 0) {
     return;
   }
   m_model = new RAPID_model;
   m_model->BeginModel();
   i32 i=0;
   for (typename Cache::AdapterVector::const_iterator it=prim.begin();
	it != prim.end();
	++it) {
      OSGObjectHierarchy::AdapterType* adapter = (OSGObjectHierarchy::AdapterType*)(*it);
      GeometryPtr geom = GeometryPtr::dcast(adapter->getOriginal()->getCore());

      if (geom != NullFC) {
	GeoPositionsPtr wp = adapter->getPositions (BVolAdapterBase::getAdapterId());
	addRefCP(wp);
	TriangleIterator end = geom->endTriangles();
	RAPIDVector3 p0, p1, p2;
	for (TriangleIterator it = geom->beginTriangles(); 
	     it != end; 
	     ++it, ++i) {
	   const PointClass& wp0 = wp->getValue(it.getPositionIndex(0));
	   p0[0] = (double)wp0[0]; p0[1] = (double)wp0[1]; p0[2] = (double)wp0[2]; 
	   const PointClass& wp1 = wp->getValue(it.getPositionIndex(1));
	   p1[0] = (double)wp1[0]; p1[1] = (double)wp1[1]; p1[2] = (double)wp1[2]; 
 	   const PointClass& wp2 = wp->getValue(it.getPositionIndex(2));
	   p2[0] = (double)wp2[0]; p2[1] = (double)wp2[1]; p2[2] = (double)wp2[2]; 

	   m_model->AddTri(p0, p1, p2, i);
	   m_index.push_back(FaceAdapterType());
	   m_index.back().init(&(adapter->getObjectAdapter()), it.getIndex());
	   m_index.back().setOriginal(it, BVolAdapterBase::getAdapterId());
	}
	subRefCP(wp);
      }
   }
   m_model->EndModel();
#endif
   // clear adapters
   Cache::the().destroyAdapter(BVolAdapterBase::getAdapterId());
}

template <class BasicTraits>
std::ostream& OpenSGRAPIDAdapter<BasicTraits>::dump (std::ostream& os) const
{
   os << "OpenSGRAPIDAdapter(" << getOriginal() << ")";
   return os;
}

template <class BasicTraits>
typename OpenSGRAPIDAdapter<BasicTraits>::FaceAdapterType
OpenSGRAPIDAdapter<BasicTraits>::getFaceAdapter (i32 id)
{
   if (id < m_index.size()) {
      return m_index[id];
   }
   return FaceAdapterType(this, id);
}

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING OpenSGRAPIDFaceAdapter<OpenSGTraits>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGRAPIDAdapter<OpenSGTraits>;

END_GENVIS_NAMESPACE

