//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.3 $
//   $Date: 2004/12/20 15:59:31 $
//                                                                            
//=============================================================================

#include "OSGGVRAPIDAdapter.h"
#ifdef GV_WITH_RAPID
# include "RAPID.H"
#endif
USING_GENVIS_NAMESPACE
OSG_USING_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING OpenSGRAPIDFaceAdapter<OpenSGTraits>;
template class OSG_GENVISLIB_DLLMAPPING OpenSGRAPIDAdapter<OpenSGTraits>;


template <class BasicTraits>
OpenSGRAPIDFaceAdapter<BasicTraits>::OpenSGRAPIDFaceAdapter ()
  : Inherited()
{
}
template <class BasicTraits>
OpenSGRAPIDFaceAdapter<BasicTraits>::OpenSGRAPIDFaceAdapter (InternalObjectType* adapter)
  : Inherited()
{
   CacheData& data = Cache::the()[adapter->getOriginal()];
   setObjectAdapter(&data);
}
template <class BasicTraits>
OpenSGRAPIDFaceAdapter<BasicTraits>::OpenSGRAPIDFaceAdapter (InternalObjectType* adapter,
							     i32 id)
  : Inherited()
{
   CacheData& data = Cache::the()[adapter->getOriginal()];
   setObjectAdapter(&data);
   setOriginalId(id);
}
template <class BasicTraits>
OpenSGRAPIDFaceAdapter<BasicTraits>::~OpenSGRAPIDFaceAdapter ()
{
}

template <class BasicTraits>
std::ostream& OpenSGRAPIDFaceAdapter<BasicTraits>::dump (std::ostream& os) const
{
   os << "OpenSGRAPIDFaceAdapter("
      << ")";
   return os;
}


template <class BasicTraits>
OpenSGRAPIDAdapter<BasicTraits>::OpenSGRAPIDAdapter ()
  : Inherited()
{
}
template <class BasicTraits>
OpenSGRAPIDAdapter<BasicTraits>::~OpenSGRAPIDAdapter ()
{
#ifdef GV_WITH_RAPID
   delete m_model;
#endif
}

template <class BasicTraits>
unsigned OpenSGRAPIDAdapter<BasicTraits>::getAdapterId ()
{
   static unsigned id = Adapter::getNextId();
   return id;
}

template <class BasicTraits>
OpenSGRAPIDAdapter<BasicTraits>::OpenSGRAPIDAdapter (const GeomObjectType& obj)
  : Inherited()
{
   init(obj);
}
template <class BasicTraits>
OpenSGRAPIDAdapter<BasicTraits>::OpenSGRAPIDAdapter (const TransformType& m2w,
						     const GeomObjectType& obj)
  : Inherited()
{
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
   for (OSG::TriangleIterator iter = geom->beginTriangles(); 
	iter != end; 
	++iter, ++i) {
      p0[0] = (double)iter.getPosition(0)[0]; p0[1] = (double)iter.getPosition(0)[1]; p0[2] = (double)iter.getPosition(0)[2]; 
      p1[0] = (double)iter.getPosition(1)[0]; p1[1] = (double)iter.getPosition(1)[1]; p1[2] = (double)iter.getPosition(1)[2]; 
      p2[0] = (double)iter.getPosition(2)[0]; p2[1] = (double)iter.getPosition(2)[1]; p2[2] = (double)iter.getPosition(2)[2]; 
      m_model->AddTri(p0, p1, p2, i);
   }
   m_model->EndModel();
#endif
}
template <class BasicTraits>
void  OpenSGRAPIDAdapter<BasicTraits>::init (const TransformType& m2w, const GeomObjectType& obj)
{
   init(obj);
   //setTransform(m2w);
}

template <class BasicTraits>
std::ostream& OpenSGRAPIDAdapter<BasicTraits>::dump (std::ostream& os) const
{
   os << "OpenSGRAPIDAdapter";
   return os;
}

