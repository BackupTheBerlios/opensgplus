/*---------------------------------------------------------------------------*\
 *                       OpenSG - Dynamic Subdivision                        *
 *                                                                           *
 *                                                                           *
 *            contact: v.settgast@cg.cs.tu-bs.de * 
 *                                                                           *
\*---------------------------------------------------------------------------*/

#include "OSGwsddat.h"
#include "OSGwsdmesh2dat.h"
OSG_USING_NAMESPACE

// template specializations
#include "OSGwsdmesh2datquad3f.inl"
#include "OSGwsdmesh2datquad4f.inl"
#include "OSGwsdmesh2dattri.inl"

// explicit instantiations
template class OSG_SUBSURFACELIB_DLLMAPPING WSDmesh2dat<OSG::Vec3f, MyTriMesh,   TRIANGLE>;
template class OSG_SUBSURFACELIB_DLLMAPPING WSDmesh2dat<OSG::Vec3f, MyPolyMesh,  QUAD>;
template class OSG_SUBSURFACELIB_DLLMAPPING WSDmesh2dat<OSG::Vec4f, MyPolyMesh4, QUAD>;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

template<class WSDVector, class Mesh, int mtype>
WSDmesh2dat<WSDVector, Mesh, mtype>::WSDmesh2dat(MeshType *m)
{
   mesh = m;
   useCreases = m->get_property_handle(isCrease,"isCrease");
   if (useCreases) {
      SLOG << "counting creases..." << std::endl;
      // property counting creases around a vertex
      mesh->add_property(creasecount);
      for (Mesh::VertexIter v_it=mesh->vertices_begin(); v_it!=mesh->vertices_end(); ++v_it) {
         mesh->property(creasecount,v_it.handle()) = 0;
         for (Mesh::VertexEdgeIter ve_it=mesh->ve_iter(v_it.handle()); ve_it; ++ve_it) {
            if (mesh->property(isCrease,ve_it.handle()) != 0) mesh->property(creasecount,v_it.handle())++;
         }
      }
      //
      SLOG << "done counting creases..." << std::endl;
   }
}

template<class WSDVector, class Mesh, int mtype>
WSDmesh2dat<WSDVector, Mesh, mtype>::~WSDmesh2dat()
{
   // property counting creases around a vertex
   mesh->remove_property(creasecount);
   //
}

template<class WSDVector, class Mesh, int mtype>
void WSDmesh2dat<WSDVector, Mesh, mtype>
::collectCorner(Int32 i, HalfedgeHandle &fhe_h, HalfedgeHandle &op)
{  
}

template<class WSDVector, class Mesh, int mtype>
void WSDmesh2dat<WSDVector, Mesh, mtype>
::helper1(Int32 i, Int32 a2, Int32 a7, HalfedgeHandle &fhe_h)      
// for creases 2 and 7 (at corner 0) and so on
{  
}

template<class WSDVector, class Mesh, int mtype>
void WSDmesh2dat<WSDVector, Mesh, mtype>
::helper2(Int32 i, Int32 a2, Int32 a7, HalfedgeHandle &fhe_h)      
// for creases 10 and 11 (at corner 1) and so on
{  
}


// helper (to make the code clearer)
template<class WSDVector, class Mesh, int mtype>
inline void WSDmesh2dat<WSDVector, Mesh, mtype>
::put_p_to(const OMPoint& p, Int32 x, Int32 y) 
{  
}

// insertface: main method for reading from openmesh
template<class WSDVector, class Mesh, int mtype>
void WSDmesh2dat<WSDVector, Mesh, mtype>
::insertface(FaceHandle f_h, FaceHandle tpair, WSDdat<VectorType, MType> *pp)
{    
}


template<class WSDVector, class Mesh, int mtype>
void WSDmesh2dat<WSDVector, Mesh, mtype>
::getBoundingBox(OSG::Pnt3f& boundingMin, OSG::Pnt3f& boundingMax)
{
   boundingMin.setValues(0,0,0);
   boundingMax.setValues(0,0,0);
   // Iteration über alle Vertices
   for (typename MeshType::VertexIter v_it=mesh->vertices_begin(); 
       v_it!=mesh->vertices_end(); ++v_it) {
      OMPoint p = mesh->point(v_it.handle());
      for (Int32 i=0; i<3; i++) {
         if (boundingMin[i] > p[i]) boundingMin[i] = p[i];
         if (boundingMax[i] < p[i]) boundingMax[i] = p[i];
      }
   }
}


// helper to find the leftmost edge of the first triangle
template<class WSDVector, class Mesh, int mtype>
typename WSDmesh2dat<WSDVector, Mesh, mtype>::HalfedgeHandle 
WSDmesh2dat<WSDVector, Mesh, mtype>::getfirsthalfedge
(FaceHandle, FaceHandle ) 
{  
   return HalfedgeHandle();
}

// helper to find out if to_vertex of fhe_h is regular
template<class WSDVector, class Mesh, int mtype>
Int32 WSDmesh2dat<WSDVector, Mesh, mtype>::vertexIsRegular (HalfedgeHandle )
{
   return 0;
}

//! helper for those Crease-Types.... (just to make it easier to read)
template<class WSDVector, class Mesh, int mtype>
OSG::Int32 WSDmesh2dat<WSDVector, Mesh, mtype>::getCreaseType_Edge
(OSG::Int32 crease, Int32 outer, Int32 inner)
{
   // <0 : only one crease -> half creas
   // ==0: irregular
   // >0 : regular
   if (crease == 0) return 0;   
   if ((outer<0)  && (inner<0))  return CREASE_HALF;
   if ((outer<0)  && (inner==0)) return CREASE_HALF_IRREG_ONCE;
   if ((outer<0)  && (inner>0))  return CREASE_HALF_REG_ONCE;
   if ((outer==0) && (inner<0))  return CREASE_HALF;   
   if ((outer==0) && (inner==0)) return CREASE_IRREG_IRREG;
   if ((outer==0) && (inner>0))  return CREASE_IRREG_REG_ONCE;
   if ((outer>0)  && (inner<0))  return CREASE_HALF;
   if ((outer>0)  && (inner==0)) return CREASE_REG_IRREG;   
   return CREASE_REG_REG;
}
