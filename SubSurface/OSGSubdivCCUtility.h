
#ifndef OSGSUBDIVCCUTILITY_H
#define OSGSUBDIVCCUTILITY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <OpenMesh/Core/Utils/Property.hh>

#include <iostream>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/Types/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/Types/TriMesh_ArrayKernelT.hh>
#include "OSGConfig.h"


OSG_BEGIN_NAMESPACE

template <class Point> 
struct FaceOneRing
{
   Point p;
   Int32 isCrease;
};

template <class Mesh> 
struct NewVertexData
{
   typename Mesh::VertexHandle v_h;
   typename Mesh::Point        neueKoords;
   Int32                       isFinish;
};

template <class Mesh> 
struct NewFaceData
{
   typename Mesh::FaceHandle f_h;
   typename Mesh::Point      facepoint;  
};


template <class Mesh> 
class SubdivCCT
{
public:
   typedef typename Mesh::HalfedgeHandle OMHalfedgeHandle;
   typedef typename Mesh::FaceHandle     OMFaceHandle;

   bool useCreases;

   //! property within openmesh
   OpenMesh::EPropHandleT<OSG::Int32> isCrease;

   // construct with a given mesh
   inline SubdivCCT(Mesh& _mesh) : mesh_(_mesh) {
      useCreases=mesh_.get_property_handle(isCrease,"isCrease");;
   }

   inline ~SubdivCCT () {
      delete[] wara;
   } 

   // destructor: free mem!

   // subdivide mesh _iterations one time
   inline void subdivCC (void) {
      wara = new FaceOneRing<typename Mesh::Point>[30]; // TODO: max valence is set to 30...   
      calcFacePoints();                // new face control points
      calcVertices();                  // new control vertices
      calcEdges();                     // new edge control points
      calcFaces();                     // and now put them all together
      updateVertexkoords();            // finally update vertices
   }  

private:

   // update control vertices
   inline void updateVertexkoords (void) {
      // Iteration über alle alten Vertices
      std::vector< NewVertexData<Mesh> >::iterator iter;
      for (iter=NewVertices.begin(); iter!=NewVertices.end(); iter++) {
         mesh_.set_point((*iter).v_h,(*iter).neueKoords);
      }
   }
  
   // fill workarray for fromV-neighborhood 
   inline Int32 collectWorkarray(typename Mesh::VertexHandle fromV) {
      typename Mesh::HalfedgeHandle hh = mesh_.halfedge_handle(fromV);
      typename Mesh::HalfedgeHandle hhold = hh;
      Int32 i=0;
      Int32 sharp=0;
      // manual iteration over outgoing halfedges     
      do {
         wara[i].p = mesh_.point(mesh_.to_vertex_handle(hh));
         if (useCreases) {
            wara[i].isCrease = mesh_.property(isCrease,mesh_.edge_handle(hh));//mesh_.edge(mesh_.edge_handle(hh)).isCrease;
         } else {
            wara[i].isCrease = 0;
         }
         if (wara[i].isCrease == 1) sharp++;
         i++;
         wara[i].p = mesh_.point(mesh_.to_vertex_handle(mesh_.next_halfedge_handle(hh)));
         // border edges are marked with 8
         if (mesh_.is_boundary(hh)) {
            wara[i].isCrease = 8;
         } else {
            wara[i].isCrease = 4;
         }
         i++;
         hh = mesh_.ccw_rotated_halfedge_handle(hh);
         /*hh = mesh_.prev_halfedge_handle(hh);
         hh = mesh_.opposite_halfedge_handle(hh);*/
         assert(mesh_.from_vertex_handle(hh) == fromV);     
      } while(hh != hhold);
      wara[i].isCrease = sharp;
      return i;
   }

   inline bool isHalfedgeSplitted(OMHalfedgeHandle hh) {
      std::vector<OMHalfedgeHandle>::iterator iter;
      for (iter = splittedEdges.begin(); iter != splittedEdges.end(); iter++) {
         if ((OMHalfedgeHandle)*iter == hh) return true;         
      }
      return false;
   }

   inline void setHalfedgeSplit(OMHalfedgeHandle hh) {
     splittedEdges.push_back(hh);
   }

// --------------------------------------------------------------------------------------------

   inline void calcVertices(void) {    
      NewVertexData<Mesh>* tempvertex = NULL;
      // Iteration over all vertices
      for (typename Mesh::VertexIter v_it=mesh_.vertices_begin(); v_it!=mesh_.vertices_end(); ++v_it) {    
         typename Mesh::Point alpha(0.0,0.0,0.0);
         typename Mesh::Point beta(0.0,0.0,0.0);
         typename Mesh::Point gamma(0.0,0.0,0.0);

         alpha = mesh_.point(v_it.handle());
         
         // fill FaceOneRing 
         Int32 val2 = collectWorkarray(v_it.handle());
         // add betas and gammas 
         for (Int32 i=0; i<val2; i++) {
            beta+=wara[i].p;
            i++;
            if (i<val2) gamma+=wara[i].p;
         }
         if (wara[val2].isCrease < 2) {          
            Real32 valenz = (Real32)val2 / 2;
            Real32 b = 3.0 / (2.0 * valenz);
            Real32 c = 1.0 / (4.0 * valenz);
            Real32 a = 1.0 - b - c;
            alpha = alpha * a;
            beta = beta * b / valenz;
            gamma = gamma * c / valenz;
            alpha = alpha + beta + gamma;   
         } else {
            if (wara[val2].isCrease < 3) {            // edge/border
               Real32 b = 0.125;
               Real32 a = 0.75;
               beta = typename Mesh::Point(0,0,0);    // new beta-sum
               for (Int32 j=0; j<val2; j=j+2) {
                  if (wara[j].isCrease != 0) beta+=wara[j].p;          
               }
               alpha = beta * b + alpha * a;
            } else {                                  // corner
               ; // alpha * 1
            }
         }
         NewVertices.push_back(NewVertexData<Mesh>());
         tempvertex = &(NewVertices.back());
         tempvertex->v_h = v_it.handle();
         tempvertex->neueKoords = alpha;
     } // for
   }

   // calculation of egde-vertices
   inline void calcEdges(void) {  
      // Iteration over all faces
      for (typename Mesh::FaceIter f_it=mesh_.faces_begin(); f_it!=mesh_.faces_end(); ++f_it) {
         //f_it->isFinish = 0;          
         typename Mesh::HalfedgeHandle hend = mesh_.halfedge_handle(f_it.handle());
         typename Mesh::HalfedgeHandle hh = hend;
         typename Mesh::HalfedgeHandle hnext;
         // Iteration over all halfedges
         do {
            hnext = mesh_.next_halfedge_handle(hh);
            if (!isHalfedgeSplitted(hh)) {            // halfedge is not yet splitted           
               // handle to neighbor-halfedge and predecessor
               typename Mesh::HalfedgeHandle op = mesh_.opposite_halfedge_handle(hh);
               typename Mesh::HalfedgeHandle op_prev = mesh_.prev_halfedge_handle(op);       
               typename Mesh::HalfedgeHandle tmp;
               // calc new vertex
               typename Mesh::Point neuPkt(0.0,0.0,0.0);
               typename Mesh::Point c1pkt(0.0,0.0,0.0);

               neuPkt += mesh_.point(mesh_.from_vertex_handle(hh));
               neuPkt += mesh_.point(mesh_.to_vertex_handle(hh));

               Int32 thiscrease = 0;
               if (useCreases) thiscrease = mesh_.property(isCrease,mesh_.edge_handle(hh));

               if (thiscrease == 0) {          
                  neuPkt = neuPkt * 0.375;      // weighting regular
       
                  tmp = mesh_.next_halfedge_handle(hh);
                  if (isHalfedgeSplitted(tmp)) tmp = mesh_.next_halfedge_handle(tmp);
                  c1pkt += mesh_.point(mesh_.to_vertex_handle(tmp));      
                  tmp = mesh_.prev_halfedge_handle(hh);
                  if (isHalfedgeSplitted(tmp)) tmp = mesh_.prev_halfedge_handle(tmp);
                  c1pkt += mesh_.point(mesh_.from_vertex_handle(tmp));                      
                  tmp = mesh_.next_halfedge_handle(op);
                  if (isHalfedgeSplitted(tmp)) tmp = mesh_.next_halfedge_handle(tmp);
                  c1pkt += mesh_.point(mesh_.to_vertex_handle(tmp));
                  tmp = mesh_.prev_halfedge_handle(op);
                  if (isHalfedgeSplitted(tmp)) tmp = mesh_.prev_halfedge_handle(tmp);
                  c1pkt += mesh_.point(mesh_.from_vertex_handle(tmp));
                  c1pkt = c1pkt * 0.0625;       // weights
                  neuPkt += c1pkt;              // finished point
               } else {
                  neuPkt = neuPkt * 0.5;        // weighting for crease
               }

               typename Mesh::VertexHandle vh = mesh_.new_vertex(neuPkt);      
           
               // --- halfedge update (hneu is included after hh)
               typename Mesh::HalfedgeHandle hneu = mesh_.new_edge(vh,mesh_.to_vertex_handle(hh));

               mesh_.set_halfedge_handle(vh,hneu);
               mesh_.set_next_halfedge_handle(hneu,hnext);
               mesh_.set_next_halfedge_handle(hh,hneu);             
               mesh_.set_vertex_handle(hh, vh);             
               mesh_.set_face_handle(hneu,f_it.handle());
                
               // --- op halfedge update (hneuop is in front of op!)
               typename Mesh::HalfedgeHandle hneuop = mesh_.opposite_halfedge_handle(hneu);      
                
               mesh_.set_next_halfedge_handle(hneuop,op);
               mesh_.set_next_halfedge_handle(op_prev,hneuop);                
               mesh_.set_halfedge_handle(mesh_.to_vertex_handle(hneu),hneuop);
                
               // hint: prev_halfedge_handles are automatically corrected with set_next_he_h!

               mesh_.set_face_handle(hneuop,mesh_.face_handle(op));
               if (!mesh_.is_boundary(op)) {
                  if (mesh_.halfedge_handle(mesh_.face_handle(op)) == op)    // startedge has to be moved maybe
                     mesh_.set_halfedge_handle(mesh_.face_handle(op),hneuop);        
               }
                 
               // flag to prevent a double split
               setHalfedgeSplit(hh);
               setHalfedgeSplit(hneu);
               setHalfedgeSplit(hneuop);
               setHalfedgeSplit(op);

               if (useCreases) {
                  mesh_.property(isCrease,mesh_.edge_handle(hneu)) = mesh_.property(isCrease,mesh_.edge_handle(hh));
               }
                   
            } // if (isSplit == 0)        
            hh = hnext;     
         } while (hh != hend);
      } // for f_it (Iteration über Faces)
   }

   // calc of Face vertex 
   inline void calcFacePoints(void) {
      NewFaceData<Mesh>* nf = NULL;
      // iteration over all faces
      for (typename Mesh::FaceIter f_it=mesh_.faces_begin(); f_it!=mesh_.faces_end(); ++f_it) {
         // store facehandles in vector 
         allFaces.push_back(NewFaceData<Mesh>());
         nf = &(allFaces.back());
         nf->f_h = f_it.handle();
 Int32 grad = 0;
         typename Mesh::Point FacePoint(0.0,0.0,0.0);
         typename Mesh::ConstFaceVertexIter fvIt = mesh_.cfv_iter(f_it.handle()); 
         for (; fvIt; ++fvIt) { 
            FacePoint += mesh_.point(fvIt.handle()); grad++; 
         }      
         FacePoint = FacePoint  / (float)grad;
         nf->facepoint = FacePoint;
      }
   }

   // rebuild faces
   inline void calcFaces(void) {    
      std::vector< NewFaceData<Mesh> >::iterator thisface;
      for (thisface=allFaces.begin(); thisface != allFaces.end(); thisface++) {
         // put new vertex into mesh 
         typename Mesh::VertexHandle vh = mesh_.new_vertex(((NewFaceData<Mesh>)*thisface).facepoint);
  
         // biuld new edges and faces
         typename Mesh::HalfedgeHandle hend = mesh_.halfedge_handle(((NewFaceData<Mesh>)*thisface).f_h);
         typename Mesh::HalfedgeHandle hh = mesh_.next_halfedge_handle(hend);
         hend = mesh_.prev_halfedge_handle(hend);
         typename Mesh::HalfedgeHandle hnext;           
         typename Mesh::HalfedgeHandle nh1 = mesh_.new_edge(mesh_.from_vertex_handle(hh), vh);
         typename Mesh::HalfedgeHandle nhn = mesh_.opposite_halfedge_handle(nh1);  
         
         if (useCreases) {
            mesh_.property(isCrease,mesh_.edge_handle(nh1))=0;
         }

         mesh_.set_halfedge_handle(vh,nhn);  
         while (hh != hend) {
            typename Mesh::HalfedgeHandle hold = hh;
            // reset and insert new face "newface" 
            typename Mesh::FaceHandle newface = mesh_.new_face();
            mesh_.set_halfedge_handle(newface, hh);            
              
            mesh_.set_prev_halfedge_handle(hh,nhn);              
            hh = mesh_.next_halfedge_handle(hh);              
            hnext = mesh_.next_halfedge_handle(hh);
              
            typename Mesh::HalfedgeHandle nh2 = mesh_.new_edge(mesh_.to_vertex_handle(hh), vh);

            if (useCreases) {
               mesh_.property(isCrease,mesh_.edge_handle(nh2)) = 0;
            }
               
            mesh_.set_prev_halfedge_handle(nhn, nh2);
            mesh_.set_prev_halfedge_handle(nh2, hh);              
            mesh_.set_next_halfedge_handle(hh,nh2);
            mesh_.set_next_halfedge_handle(nh2,nhn);
            mesh_.set_next_halfedge_handle(nhn,hold);              
            mesh_.set_face_handle(hold,newface);
            mesh_.set_face_handle(hh,newface);
            mesh_.set_face_handle(nh2,newface);
            mesh_.set_face_handle(nhn,newface);              
            nhn = mesh_.opposite_halfedge_handle(nh2);              
            hh = hnext;
         }  
         // the old face will be reused
         hnext = mesh_.next_halfedge_handle(hh);
           
         mesh_.set_prev_halfedge_handle(nh1, hnext);
         mesh_.set_prev_halfedge_handle(hh, nhn);
         mesh_.set_prev_halfedge_handle(nhn, nh1);           
         mesh_.set_next_halfedge_handle(hnext,nh1);
         mesh_.set_next_halfedge_handle(nh1,nhn);
         mesh_.set_next_halfedge_handle(nhn,hh);
           
         mesh_.set_face_handle(nh1,((NewFaceData<Mesh>)*thisface).f_h);
         mesh_.set_face_handle(nhn,((NewFaceData<Mesh>)*thisface).f_h);                        
      } // for thisface iterator
   }

   //--- private elements ---
   FaceOneRing<typename Mesh::Point>* wara;

   Mesh& mesh_;

   std::vector<NewVertexData<Mesh> > NewVertices;
   std::vector<OMHalfedgeHandle>     splittedEdges;
   std::vector<NewFaceData<Mesh> >   allFaces;   
     
};

OSG_END_NAMESPACE

#endif



