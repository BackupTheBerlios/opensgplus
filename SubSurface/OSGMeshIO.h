
#ifndef OSGMESHIO_H
#define OSGMESHIO_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <OpenMesh/Core/Utils/Property.hh>

#include <iostream>
#include <OpenMesh/Core/IO/OMFormat.hh>
#include <OpenMesh/Core/IO/writer/OMWriter.hh>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/Types/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/Types/TriMesh_ArrayKernelT.hh>
#include "OSGConfig.h"


OSG_BEGIN_NAMESPACE


template <class Mesh> 
class OSGMeshIO
{
public:
  

   // construct with a given mesh
   inline OSGMeshIO(Mesh& _mesh) : mesh_(_mesh) {
      hasCreases = false;
      hasCreases = mesh_.get_property_handle(isCrease,"isCrease");      
   }

   // destructor: free mem!
   inline ~OSGMeshIO () {
   } 
   

   // OpenMesh Output
   inline void writeOpenMesh(BinaryDataHandler &pMem) {

      OpenMesh::IO::ExporterT<Mesh> _be(mesh_);
        
  
      size_t bytes = 0;  

      UInt32 i, nV, nF;
      Vec3f v;
      Vec2f t;
      std::vector<OpenMesh::VertexHandle> vhandles;

      bool isPolyMesh = !_be.is_triangle_mesh();

      // write header
      pMem.putValue((UInt8)'O');
      pMem.putValue((UInt8)'M');
      if ( isPolyMesh ) {
         pMem.putValue((UInt8)'P');
      } else {
         pMem.putValue((UInt8)'T');
      }
      pMem.putValue((UInt32)_be.n_vertices());
      pMem.putValue((UInt32)_be.n_faces());
      pMem.putValue((UInt32)_be.n_edges());

        
      // -------------------- write vertex data
        
      // ---------- write vertex position
      if (_be.n_vertices()) {
          
         
         for (i=0, nV=_be.n_vertices(); i<nV; ++i) {
            //bytes += vector_store( _os, _be.point(VertexHandle(i)), swap );
            OpenMesh::Vec3f p = _be.point(OpenMesh::VertexHandle(i));
            pMem.putValue((Real32)p[0]);
            pMem.putValue((Real32)p[1]);
            pMem.putValue((Real32)p[2]);
         }

      }
        


      // -------------------- write face data

      // ---------- write topology
      {

         for (i=0, nF=_be.n_faces(); i<nF; ++i)  {
            nV = _be.get_vhandles(OpenMesh::FaceHandle(i), vhandles);
            if ( isPolyMesh ) {
               pMem.putValue((UInt16)vhandles.size());
            }
            
            for (size_t j=0; j < vhandles.size(); ++j) {
               pMem.putValue((UInt32)vhandles[j].idx());
            }
         }
      }

      // -------------------- write crease property

      if (hasCreases) {
         pMem.putValue((UInt8)1);
         typename Mesh::EdgeIter eIt(mesh_.edges_begin());
         typename Mesh::EdgeIter eEnd(mesh_.edges_end());
         Int32 g=0;
         for (; eIt!=eEnd; ++eIt) {
            pMem.putValue((Int32)mesh_.property(isCrease,eIt.handle())); 
         }
      } else {
         pMem.putValue((UInt8)0);
      }

  }


     // OpenMesh Input
   inline void readOpenMesh(BinaryDataHandler &pMem) {
   
      mesh_.clear();
      OpenMesh::IO::ImporterT<Mesh> _bi(mesh_);
    

      UInt32 i, nV, nF;
      Vec3f v;
      Vec2f t;
      std::vector<OpenMesh::VertexHandle> vhandles;

      bool isPolyMesh = !_bi.is_triangle_mesh();

        

      // read header
      UInt8 O,M,Mtype;
      pMem.getValue(O);
      pMem.getValue(M);
      pMem.getValue(Mtype);
        
      UInt32 n_vertices,n_faces,n_edges;
      pMem.getValue(n_vertices);
      pMem.getValue(n_faces);
      pMem.getValue(n_edges);
        
        
      // ---------- read vertex position
      if (n_vertices) {   
          
         for (i=0, nV=n_vertices; i<nV; ++i) {  
            OpenMesh::Vec3f p;
             
            pMem.getValue(p[0]);
            pMem.getValue(p[1]);
            pMem.getValue(p[2]);

            _bi.add_vertex(p);
             
         }

      }
        
      // -------------------- read face data

      // ---------- read topology
      {
         for (i=0, nF=n_faces; i<nF; ++i)  {
             
            UInt16 vh_size=3;

            if ( isPolyMesh ) {	         
               pMem.getValue(vh_size);
            }

            UInt32* v_idx;
            v_idx = new UInt32[vh_size];
            
            for (size_t j=0; j < vh_size; ++j) {
               pMem.getValue(v_idx[j]);
            }
           
            OpenMesh::IO::BaseImporter::VHandles vhandles;

            for(UInt32 k=0;k<vh_size;++k) {            
                  vhandles.push_back(OpenMesh::VertexHandle(v_idx[k]));            
            }
            OpenMesh::FaceHandle fh = _bi.add_face(vhandles);

            delete[] v_idx;


         }
      }

      // -------------------- read crease property

      UInt8 fileHasCreases;
      pMem.getValue(fileHasCreases);

      if (hasCreases) {
         typename Mesh::EdgeIter eIt(mesh_.edges_begin());
         typename Mesh::EdgeIter eEnd(mesh_.edges_end());
         Int32 g=0;
         for (; eIt!=eEnd; ++eIt) {
            Int32 get=0;
            if (fileHasCreases == 1) pMem.getValue(get);             
            mesh_.property(isCrease,eIt.handle()) = get;
         }
      }


  }


private: 
   //--- private elements ---  

   Mesh& mesh_;
   OpenMesh::EPropHandleT<Int32> isCrease;
   bool hasCreases;

   
     
};

OSG_END_NAMESPACE

#endif



