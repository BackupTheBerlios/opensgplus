
#ifndef OSGSUBDIVCCUTILITY_H
#define OSGSUBDIVCCUTILITY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <iostream>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/Types/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/Types/TriMesh_ArrayKernelT.hh>
#include "OSGConfig.h"
#include "OSGSubSurfaceDef.h"

OSG_BEGIN_NAMESPACE

template <class Point> 
struct OSG_SUBSURFACELIB_DLLMAPPING FaceOneRing
{
  Point p;
  Int32 isCrease;
};

template <class Mesh> 
struct OSG_SUBSURFACELIB_DLLMAPPING NewVertexData
{
   typename Mesh::VertexHandle v_h;
   typename Mesh::Point        neueKoords;
   Int32                       isFinish;
};

template <class Mesh> 
struct OSG_SUBSURFACELIB_DLLMAPPING NewFaceData
{
   typename Mesh::FaceHandle f_h;
   typename Mesh::Point      facepoint;	
};


template <class Mesh> 
class OSG_SUBSURFACELIB_DLLMAPPING SubdivCCT
{
public:
  typedef typename Mesh::HalfedgeHandle OMHalfedgeHandle;
  typedef typename Mesh::FaceHandle     OMFaceHandle;

  // construct with a given mesh
  SubdivCCT (Mesh& _mesh) 
    : mesh_(_mesh) {
  }
  ~SubdivCCT () {
     delete[] wara;
  } 

  // destructor: free mem!

  // subdivide mesh _iterations one time
  void subdivCC (void) {
      wara = new FaceOneRing<typename Mesh::Point>[20]; // eigentlich erstmal max valenz feststellen!
   
      calcFacePoints();
	      
      calcVertices();
      calcEdges();
      calcFaces();

      updateVertexkoords();
  }
  

private:

  // Eckpunkte werden aktualisiert
  void updateVertexkoords (void) {
     // Iteration über alle alten Vertices
     std::vector< NewVertexData<Mesh> >::iterator iter;
     for (iter=NewVertices.begin(); iter!=NewVertices.end(); iter++) {
        mesh_.set_point((*iter).v_h,(*iter).neueKoords);
     }
  }
	
  // workarray für fromV-Nachbarschaft füllen
  Int32 collectWorkarray(typename Mesh::VertexHandle fromV) {
     typename Mesh::HalfedgeHandle hh = mesh_.halfedge_handle(fromV);
     typename Mesh::HalfedgeHandle hhold = hh;
     Int32 i=0;
     Int32 sharp=0;
     // manuelle Iteration über alle ausgehenden edges 		
     do {
        wara[i].p = mesh_.point(mesh_.to_vertex_handle(hh));
	wara[i].isCrease = mesh_.edge(mesh_.edge_handle(hh)).isCrease;
	if (mesh_.edge(mesh_.edge_handle(hh)).isCrease == 1) sharp++;
	i++;
	wara[i].p = mesh_.point(mesh_.to_vertex_handle(mesh_.next_halfedge_handle(hh)));
	// Kantengamma mit 8 markieren
	if (mesh_.is_boundary(hh)) wara[i].isCrease = 8;
	else wara[i].isCrease = 4;
	i++;

	hh = mesh_.prev_halfedge_handle(hh);
	hh = mesh_.opposite_halfedge_handle(hh);
	assert(mesh_.from_vertex_handle(hh) == fromV);		 
     } while(hh != hhold);

     wara[i].isCrease = sharp;
     return i;
  }

  bool isHalfedgeSplitted(OMHalfedgeHandle hh) {
     std::vector<OMHalfedgeHandle>::iterator iter;
     for (iter = splittedEdges.begin(); iter != splittedEdges.end(); iter++)
        if ((OMHalfedgeHandle)*iter == hh) return true;         
     return false;
  }

  void setHalfedgeSplit(OMHalfedgeHandle hh) {
     splittedEdges.push_back(hh);
  }

// --------------------------------------------------------------------------------------------

  void calcVertices(void) {		
     NewVertexData<Mesh>* tempvertex = NULL;
     // Iteration über alle Vertices
     for (typename Mesh::VertexIter v_it=mesh_.vertices_begin(); v_it!=mesh_.vertices_end(); ++v_it) {		
        typename Mesh::Point alpha(0.0,0.0,0.0);
	typename Mesh::Point beta(0.0,0.0,0.0);
	typename Mesh::Point gamma(0.0,0.0,0.0);

	alpha = mesh_.point(v_it.handle());

	//if (v_it->EckenHack != 23)
	{
	   // FaceOneRing füllen
	   Int32 val2 = collectWorkarray(v_it.handle());
	   // betas und gammas aufaddieren
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
	   } else
	     if (wara[val2].isCrease < 3) { // Rand/Kante
	        Real32 b = 0.125;
		Real32 a = 0.75;
		beta = typename Mesh::Point(0,0,0);	// neue Beta-Summe
		for (Int32 j=0; j<val2; j=j+2)
		  if (wara[j].isCrease != 0) beta+=wara[j].p;					
		alpha = beta * b + alpha * a;
	     } else { 	// Ecke
	       ; // alpha * 1
	     }
	}

	NewVertices.push_back(NewVertexData<Mesh>());
	tempvertex = &(NewVertices.back());
	tempvertex->v_h = v_it.handle();
	tempvertex->neueKoords = alpha;
     } // for
  }

  // berechnung der egde-vertices mit einer neuen edge
  void calcEdges(void) {	
     // Iteration über alle Faces
     for (typename Mesh::FaceIter f_it=mesh_.faces_begin(); f_it!=mesh_.faces_end(); ++f_it) {
        //f_it->isFinish = 0;
       
        typename Mesh::HalfedgeHandle hend = mesh_.halfedge_handle(f_it.handle());
	typename Mesh::HalfedgeHandle hh = hend;
	typename Mesh::HalfedgeHandle hnext;

	// Iteration über alle Halfedges
	do {
	   hnext = mesh_.next_halfedge_handle(hh);

           if (!isHalfedgeSplitted(hh)) { // Halfedge ist noch nicht geteilt
				
	      // handle auf Nachbarhalfedge und vorgänger
	     typename Mesh::HalfedgeHandle op = mesh_.opposite_halfedge_handle(hh);
	     typename Mesh::HalfedgeHandle op_prev = mesh_.prev_halfedge_handle(op);
	     
	     typename Mesh::HalfedgeHandle tmp;

	     // neuen Vertex berechnen
	     typename Mesh::Point neuPkt(0.0,0.0,0.0);
	     typename Mesh::Point c1pkt(0.0,0.0,0.0);

	     neuPkt += mesh_.point(mesh_.from_vertex_handle(hh));
	     neuPkt += mesh_.point(mesh_.to_vertex_handle(hh));

	     if (mesh_.edge(mesh_.edge_handle(hh)).isCrease == 0) {					
	        neuPkt = neuPkt * 0.375;	// gewichtung regulär
		
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

		c1pkt = c1pkt * 0.0625;		// gewichtung
		
		neuPkt += c1pkt;			// fertiger Punkt
	     } else {
	        neuPkt = neuPkt * 0.5;	// gewichtung für scharfe Kante						
	     }

	     typename Mesh::VertexHandle vh = mesh_.new_vertex(neuPkt);			
				
	     // --- halfedge update (hneu wird hinter hh eingebunden)
	     typename Mesh::HalfedgeHandle hneu = mesh_.new_edge(vh,mesh_.to_vertex_handle(hh));

	     mesh_.set_halfedge_handle(vh,hneu);

	     mesh_.set_next_halfedge_handle(hneu,hnext);
	     mesh_.set_next_halfedge_handle(hh,hneu);
	     
	     mesh_.set_vertex_handle(hh, vh);
	     
	     mesh_.set_face_handle(hneu,f_it.handle());
	     
	     // --- op halfedge update (hneuop ist natürlich VOR op!)
	     typename Mesh::HalfedgeHandle hneuop = mesh_.opposite_halfedge_handle(hneu);			
	     
	     mesh_.set_next_halfedge_handle(hneuop,op);
	     mesh_.set_next_halfedge_handle(op_prev,hneuop);
	     
	     mesh_.set_halfedge_handle(mesh_.to_vertex_handle(hneu),hneuop);
	     
	     // anmerkung: die prev_halfedge_handles werden automatisch durch set_next_he_h korrigiert!

	     mesh_.set_face_handle(hneuop,mesh_.face_handle(op));

	     if (!mesh_.is_boundary(op))
	       if (mesh_.halfedge_handle(mesh_.face_handle(op)) == op)		// startedge evtl verschieben
		 mesh_.set_halfedge_handle(mesh_.face_handle(op),hneuop);
				
							
	     // Flag setzen um doppeltes Teilen zu vermeiden
	     setHalfedgeSplit(hh);
	     setHalfedgeSplit(hneu);
	     setHalfedgeSplit(hneuop);
	     setHalfedgeSplit(op);

	     mesh_.edge(mesh_.edge_handle(hneu)).isCrease=mesh_.edge(mesh_.edge_handle(hh)).isCrease;
					
	   } // if (isSplit == 0)

			  
	   hh = hnext;
	   
	} while (hh != hend);

     } // for f_it (Iteration über Faces)

  }

  // berechnung des Face vertex 
  void calcFacePoints(void) {
     NewFaceData<Mesh>* nf = NULL;
     // Iteration über alle Faces
     for (typename Mesh::FaceIter f_it=mesh_.faces_begin(); f_it!=mesh_.faces_end(); ++f_it) {
        // facehandles in vector speichern
        allFaces.push_back(NewFaceData<Mesh>());
	nf = &(allFaces.back());
	nf->f_h = f_it.handle();
	int grad = 0;
	typename Mesh::Point FacePoint(0.0,0.0,0.0);
	typename Mesh::ConstFaceVertexIter fvIt = mesh_.cfv_iter(f_it.handle()); 
	for (; fvIt; ++fvIt) { 
	   FacePoint += mesh_.point(fvIt.handle()); grad++; 
	}			
	FacePoint = FacePoint  / (float)grad;
	nf->facepoint = FacePoint;
     }
  }

  // neukomposition der Faces
  void calcFaces(void) {		
     std::vector< NewFaceData<Mesh> >::iterator thisface;
     for (thisface=allFaces.begin(); thisface != allFaces.end(); thisface++) {
        // neuen Vertex ins Mesh einbauen 
        typename Mesh::VertexHandle vh = mesh_.new_vertex(((NewFaceData<Mesh>)*thisface).facepoint);
	
	// neue Edges erzeugen und neue Faces
	typename Mesh::HalfedgeHandle hend = mesh_.halfedge_handle(((NewFaceData<Mesh>)*thisface).f_h);
	typename Mesh::HalfedgeHandle hh = mesh_.next_halfedge_handle(hend);
	hend = mesh_.prev_halfedge_handle(hend);
	typename Mesh::HalfedgeHandle hnext;
	
	typename Mesh::HalfedgeHandle nh1 = mesh_.new_edge(mesh_.from_vertex_handle(hh), vh);
	typename Mesh::HalfedgeHandle nhn = mesh_.opposite_halfedge_handle(nh1);
	
	mesh_.edge(mesh_.edge_handle(nh1)).isCrease=0;
	mesh_.set_halfedge_handle(vh,nhn);
	
	while (hh != hend) {
	   typename Mesh::HalfedgeHandle hold = hh;
	   // neues Face newface reseten und eingliedern
	   typename Mesh::FaceHandle newface = mesh_.new_face();
	   mesh_.set_halfedge_handle(newface, hh);
	   //mesh_.face(newface).isFinish = 1;
	   
	   mesh_.set_prev_halfedge_handle(hh,nhn);
	   
	   hh = mesh_.next_halfedge_handle(hh);
	   
	   hnext = mesh_.next_halfedge_handle(hh);
	   
	   typename Mesh::HalfedgeHandle nh2 = mesh_.new_edge(mesh_.to_vertex_handle(hh), vh);
	   mesh_.edge(mesh_.edge_handle(nh2)).isCrease=0;
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
	
	// das Ausgangsface wird ebenfalls weiterbenutzt...
	hnext = mesh_.next_halfedge_handle(hh);
	
	mesh_.set_prev_halfedge_handle(nh1, hnext);
	mesh_.set_prev_halfedge_handle(hh, nhn);
	mesh_.set_prev_halfedge_handle(nhn, nh1);
	
	mesh_.set_next_halfedge_handle(hnext,nh1);
	mesh_.set_next_halfedge_handle(nh1,nhn);
	mesh_.set_next_halfedge_handle(nhn,hh);
	
	mesh_.set_face_handle(nh1,((NewFaceData<Mesh>)*thisface).f_h);
	mesh_.set_face_handle(nhn,((NewFaceData<Mesh>)*thisface).f_h);		
	
	//} // if (isFinish)
	
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
