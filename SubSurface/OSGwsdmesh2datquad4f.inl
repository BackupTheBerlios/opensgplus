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

/////////////////////////////////////////////////////////////////////////////
// mtype = QUAD
/////////////////////////////////////////////////////////////////////////////

template<>
void WSDmesh2dat<OSG::Vec4f, MyPolyMesh4, QUAD>::collectCorner
(Int32 i, HalfedgeHandle &fhe_h, HalfedgeHandle &op)
{
   Int32 vt = 3;
   Int32 cc = 3;
   OMPoint p;
   HalfedgeHandle test,ende;
   op = mesh->opposite_halfedge_handle(mesh->next_halfedge_handle(op));
   test = mesh->prev_halfedge_handle(fhe_h); 
   if (op == test) {
      ppatch->valenz[i] = 2;    // corner!
   } else {
      ende = op;      
      test = mesh->prev_halfedge_handle(mesh->opposite_halfedge_handle(test)); 
      while (test != ende) {
         test = mesh->prev_halfedge_handle(mesh->opposite_halfedge_handle(test)); 
         vt++; 
         assert(vt<wsdmaxvalenz);
         // quads: also from of prev
         p=mesh->point(mesh->from_vertex_handle(mesh->prev_halfedge_handle(test)));    // gamma
         (ppatch->cornerO[i][(vt-1)*2]).setValues(p[0], p[1], p[2], 1.0); 
         p=mesh->point(mesh->from_vertex_handle(test));                                // beta
         (ppatch->cornerO[i][1+((vt-1)*2)]).setValues(p[0], p[1], p[2], 1.0); 
         // creases
         if (useCreases) {
            ppatch->corcrease[i][cc] = mesh->property(isCrease,mesh->edge_handle(test));
            if (ppatch->corcrease[i][cc]!=0) {
               if (mesh->property(creasecount,mesh->to_vertex_handle(test)) < 2) {
                  ppatch->corcrease[i][cc]=0;         // dart ends on corner
               } else {
                  if (mesh->property(creasecount,mesh->from_vertex_handle(test))<2) {                       
                     // we have a dart ending outside
                     ppatch->corcrease[i][cc] = CREASE_HALF_REG_ONCE;
                     SLOG << "dart found!" << std::endl;
                  }
               }
            }
         } else {
            ppatch->corcrease[i][cc] = 0;
         }
         cc++;
      } 
      ppatch->valenz[i] = vt;  
      // one more for later calc
      test = mesh->prev_halfedge_handle(mesh->opposite_halfedge_handle(test)); 
      vt++;
      // quads: also from of prev
      p=mesh->point(mesh->from_vertex_handle(mesh->prev_halfedge_handle(test)));    // gamma
      (ppatch->cornerO[i][(vt-1)*2]).setValues(p[0], p[1], p[2], 1.0); 
      p=mesh->point(mesh->from_vertex_handle(test));                                // beta
      (ppatch->cornerO[i][1+((vt-1)*2)]).setValues(p[0], p[1], p[2], 1.0); 
   }
}

// for creases 2 and 7 (at 0) etc.
template<>
void WSDmesh2dat<OSG::Vec4f, MyPolyMesh4, QUAD>::helper1
(Int32 i, Int32 a2, Int32 a7, HalfedgeHandle &fhe_h)      
{
   HalfedgeHandle op = mesh->opposite_halfedge_handle(fhe_h);
   if ((mesh->is_boundary(mesh->edge_handle(fhe_h)) && (ppatch->valenz[i] < 4)) 
      || (!useCreases)) {
      ppatch->crease[a2] = 0;
   } else {
      ppatch->crease[a2] = mesh->property(isCrease,mesh->edge_handle(mesh->next_halfedge_handle(op)));//mesh->edge(mesh->edge_handle(mesh->next_halfedge_handle(op))).isCrease;  
      if (ppatch->crease[a2]!=0) {
         if (mesh->property(creasecount,mesh->from_vertex_handle(mesh->next_halfedge_handle(op)))<2) {
            ppatch->crease[a2] = 0; // dart ends on corner
         } else {
            if (mesh->property(creasecount,mesh->to_vertex_handle(mesh->next_halfedge_handle(op)))<2) {
               ppatch->crease[a2] = CREASE_HALF_REG_ONCE;
            }
         }
      }
   }
   if ((mesh->is_boundary(mesh->edge_handle(fhe_h)) && (ppatch->valenz[(i+1)%4] < 4)) 
      || (!useCreases)) {
      ppatch->crease[a7] = 0;  
   } else {
      ppatch->crease[a7] = mesh->property(isCrease,mesh->edge_handle(mesh->prev_halfedge_handle(op)));//mesh->edge(mesh->edge_handle(mesh->prev_halfedge_handle(op))).isCrease;
      if (ppatch->crease[a7]!=0) {
         if (mesh->property(creasecount,mesh->to_vertex_handle(mesh->prev_halfedge_handle(op)))<2) {
            ppatch->crease[a7] = 0;
         } else {
            if (mesh->property(creasecount,mesh->from_vertex_handle(mesh->prev_halfedge_handle(op)))<2) {
               ppatch->crease[a7] = CREASE_HALF_REG_ONCE;
            }
         }
      }
   }
}

// helper (to make the code clearer)
template<>
void WSDmesh2dat<OSG::Vec4f, MyPolyMesh4, QUAD>::put_p_to
(const OMPoint& p, Int32 x, Int32 y) 
{
   (ppatch->slateO[x][y]).setValues(p[0], p[1], p[2], 1.0);
}

// insertface: main method for reading from openmesh
template<>
void WSDmesh2dat<OSG::Vec4f, MyPolyMesh4, QUAD>::insertface
(FaceHandle f_h, FaceHandle tpair, WSDdat<VectorType, MType> *pp)
{     
   ppatch = pp;
   OMPoint p;  
   Int32 vt;
   HalfedgeHandle fhe_h;
   HalfedgeHandle op,test,ende;    
//*******
   fhe_h=mesh->halfedge_handle(f_h);          // first halfedge of face f_h  
   p=mesh->point(mesh->from_vertex_handle(fhe_h));
   put_p_to(p, 1,1);  
   
   // crease
   if (useCreases) {
      ppatch->crease[5] = mesh->property(isCrease,mesh->edge_handle(fhe_h));
   } else {
      ppatch->crease[5] = 0;
   }
   // tex
   OMTexCoord tc;
   if (useTexture) {
      tc = mesh->texcoord(mesh->from_vertex_handle(fhe_h));
      (ppatch->TexCoords[1][1]).setValues(tc[0],tc[1],0.0,0.0);
   }
   //  between edge 0
   op = mesh->opposite_halfedge_handle(fhe_h);
   p=mesh->point(mesh->to_vertex_handle(mesh->next_halfedge_handle(op)));
   put_p_to(p, 1,0);
   p=mesh->point(mesh->from_vertex_handle(mesh->prev_halfedge_handle(op)));
   put_p_to(p, 2,0);
     
   // corner 0
   this->collectCorner(0,fhe_h,op);

//*******
   fhe_h=mesh->next_halfedge_handle(fhe_h);
   p=mesh->point(mesh->from_vertex_handle(fhe_h));
   put_p_to(p, 2,1);  
   
   // crease
   if (useCreases) {
      ppatch->crease[8] = mesh->property(isCrease,mesh->edge_handle(fhe_h));
   } else {
      ppatch->crease[8] = 0;
   }
   // tex
   if (useTexture) {
      tc=mesh->texcoord(mesh->from_vertex_handle(fhe_h));
      (ppatch->TexCoords[2][1]).setValues(tc[0],tc[1],0.0,0.0);
   }  
   // between edge 1
   op = mesh->opposite_halfedge_handle(fhe_h);
   p=mesh->point(mesh->to_vertex_handle(mesh->next_halfedge_handle(op)));
   put_p_to(p, 3,1);
   p=mesh->point(mesh->from_vertex_handle(mesh->prev_halfedge_handle(op)));
   put_p_to(p, 3,2);

   // corner 1
   this->collectCorner(1,fhe_h,op);
//*******
   fhe_h=mesh->next_halfedge_handle(fhe_h);
   p=mesh->point(mesh->from_vertex_handle(fhe_h));
   put_p_to(p, 2,2);

   // crease
   if (useCreases) {
      ppatch->crease[6] = mesh->property(isCrease,mesh->edge_handle(fhe_h));
   } else {
      ppatch->crease[6] = 0;
   }
   // tex
   if (useTexture) {
      tc=mesh->texcoord(mesh->from_vertex_handle(fhe_h));
      (ppatch->TexCoords[2][2]).setValues(tc[0],tc[1],0.0,0.0);
   }  
   // between edge 2
   op = mesh->opposite_halfedge_handle(fhe_h);
   p=mesh->point(mesh->to_vertex_handle(mesh->next_halfedge_handle(op)));
   put_p_to(p, 2,3);
   p=mesh->point(mesh->from_vertex_handle(mesh->prev_halfedge_handle(op)));
   put_p_to(p, 1,3);
     
   // corner 2
   this->collectCorner(2,fhe_h,op);

//*******
   fhe_h=mesh->next_halfedge_handle(fhe_h);
   p=mesh->point(mesh->from_vertex_handle(fhe_h));
   put_p_to(p, 1,2);

   // crease
   if (useCreases) {
      ppatch->crease[3] = mesh->property(isCrease,mesh->edge_handle(fhe_h));
   } else {
      ppatch->crease[3] = 0;
   }
   // tex
   if (useTexture) {
      tc=mesh->texcoord(mesh->from_vertex_handle(fhe_h));
      (ppatch->TexCoords[1][2]).setValues(tc[0],tc[1],0.0,0.0);
   }  
   // zwischen rand D
   op = mesh->opposite_halfedge_handle(fhe_h);
   p=mesh->point(mesh->to_vertex_handle(mesh->next_halfedge_handle(op)));
   put_p_to(p, 0,2);
   p=mesh->point(mesh->from_vertex_handle(mesh->prev_halfedge_handle(op)));
   put_p_to(p, 0,1);  
   // corner 3
   this->collectCorner(3,fhe_h,op);

   // helper1
   fhe_h=mesh->next_halfedge_handle(fhe_h);
   helper1(0,2,7,fhe_h);
   fhe_h=mesh->next_halfedge_handle(fhe_h);
   helper1(1,10,11,fhe_h);
   fhe_h=mesh->next_halfedge_handle(fhe_h);
   helper1(2,9,4,fhe_h);
   fhe_h=mesh->next_halfedge_handle(fhe_h);
   helper1(3,1,0,fhe_h);

//*************************************************************************************
   //all creases into the array     
   ppatch->corcrease[0][0] = ppatch->crease[5];
   ppatch->corcrease[0][1] = ppatch->crease[3];
   if (ppatch->valenz[0] > 2) {
      ppatch->corcrease[0][2] = ppatch->crease[0];
      ppatch->corcrease[0][(osgMax(ppatch->valenz[0],4))-1] = ppatch->crease[2];
   }     
   ppatch->corcrease[1][0] = ppatch->crease[8];
   ppatch->corcrease[1][1] = ppatch->crease[5];
   if (ppatch->valenz[1] > 2) {  
      ppatch->corcrease[1][2] = ppatch->crease[7];
      ppatch->corcrease[1][(osgMax(ppatch->valenz[1],4))-1] = ppatch->crease[10];
   }       
   ppatch->corcrease[2][0] = ppatch->crease[6];
   ppatch->corcrease[2][1] = ppatch->crease[8];
   if (ppatch->valenz[2] > 2) {  
      ppatch->corcrease[2][2] = ppatch->crease[11];
      ppatch->corcrease[2][(osgMax(ppatch->valenz[2],4))-1] = ppatch->crease[9];
   }       
   ppatch->corcrease[3][0] = ppatch->crease[3];
   ppatch->corcrease[3][1] = ppatch->crease[6];
   if (ppatch->valenz[3] > 2) {  
      ppatch->corcrease[3][2] = ppatch->crease[4];
      ppatch->corcrease[3][(osgMax(ppatch->valenz[3],4))-1] = ppatch->crease[1];
   }

   // corners (Sum of adjacent creases)
   for (Int32 ll=0; ll<4; ll++) {
      if (ppatch->valenz[ll] == 3){      // valence 3 gets 2 identical edges!
         Int32 ahh = osgMax(ppatch->corcrease[ll][3],ppatch->corcrease[ll][2]);
         ppatch->corcrease[ll][3] = ppatch->corcrease[ll][2] = ahh;
      }
      Int32 kk;
      if (ppatch->valenz[ll] > 2)  {
         ppatch->iscorner[ll] = 0;
         for (kk = 0; kk<ppatch->valenz[ll]; kk++) 
            ppatch->iscorner[ll] = ppatch->iscorner[ll] + ppatch->corcrease[ll][kk];
      } else {
         ppatch->iscorner[ll] = 3; // if valence = 2 allways a corner!
      }
   }    
   // crindis (for 2 creases-case and for normal calculation)
   for (Int32 yy=0; yy<4; yy++) {
      if (ppatch->iscorner[yy] >= 2) {
         Int32 vv=osgMax(ppatch->valenz[yy],4);
         Int32 aa=vv;        
         Int32 bb=1;            
         while (ppatch->corcrease[yy][aa%vv] == 0) aa--;
         while (ppatch->corcrease[yy][bb%vv] == 0) bb++;
         ppatch->crindis[yy][0] = aa%vv;
         ppatch->crindis[yy][1] = bb%vv;
      }
   }
   //ppatch->radiusD
   VectorType d1 = ppatch->slateO[1][1] - ppatch->slateO[2][2];
   VectorType d2 = ppatch->slateO[1][2] - ppatch->slateO[2][1];

   if (d1.length() > d2.length()) {
      ppatch->radiusD = d1.length() / 2.0;
   } else {
      ppatch->radiusD = d2.length() / 2.0;
   }
   ppatch->maxdepth=0;
   ppatch->currentdepth=0;
   ppatch->faceloc = (ppatch->slateO[1][1] + ppatch->slateO[2][1] +
   ppatch->slateO[2][2] + ppatch->slateO[1][2]) * 0.25;
   ppatch->cvindex = 0;
   ppatch->tabsize = 0;
}




