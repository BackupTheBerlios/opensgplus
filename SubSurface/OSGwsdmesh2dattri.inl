/*---------------------------------------------------------------------------*\
 *                       OpenSG - Dynamic Subdivision                        *
 *                                                                           *
 *                                                                           *
 *					  contact: v.settgast@cg.cs.tu-bs.de * 
 *                                                                           *
\*---------------------------------------------------------------------------*/

#include "OSGwsddat.h"
#include "OSGwsdmesh2dat.h"
OSG_USING_NAMESPACE

/////////////////////////////////////////////////////////////////////////////
// mtype = TRIANGLE
/////////////////////////////////////////////////////////////////////////////

// helper to find the leftmost edge of the first triangle
template<>
WSDmesh2dat<OSG::Vec3f, MyTriMesh, TRIANGLE>::HalfedgeHandle 
WSDmesh2dat<OSG::Vec3f, MyTriMesh, TRIANGLE>::getfirsthalfedge
(FaceHandle f_h, FaceHandle tpair) 
{
   HalfedgeHandle ret = mesh->halfedge_handle(f_h);
   int i=0;
   while ((mesh->face_handle(mesh->opposite_halfedge_handle(ret)) != tpair)
      && (i<5) ) {
      ret = mesh->next_halfedge_handle(ret);
      i++;
   }
   if (i<4) return ret;
   else return mesh->halfedge_handle(f_h);
}

// helper to find out if to_vertex of fhe_h is regular
template<>
Int32 WSDmesh2dat<OSG::Vec3f, MyTriMesh, TRIANGLE>::vertexIsRegular 
(HalfedgeHandle fhe_h)
{
   HalfedgeHandle test = mesh->opposite_halfedge_handle(fhe_h);
   HalfedgeHandle ende = test;
   OSG::Int32 count,crease;
   OSG::Int32 crindis[2];
   crindis[0]=0; crindis[1]=0;
   bool boundary_found = false;

   crease = 0;
   count  = 1;
   if (mesh->edge(mesh->edge_handle(test)).isCrease > 0) {
      crindis[crease] = count; crease++;  
   } 
   if (mesh->is_boundary(mesh->edge_handle(test))) {           
            boundary_found = true;             
   }
   test = mesh->cw_rotated_halfedge_handle(test);      
   while (test != ende) {
      count++;
      if (mesh->edge(mesh->edge_handle(test)).isCrease > 0) {
         if (crease < 2) {
            crindis[crease] = count;
            crease++;
         }
         else return 0; // more than 2 creases!
      } 
      if (mesh->is_boundary(mesh->edge_handle(test))) {  
         if (!boundary_found) boundary_found = true;                                       
      }
      test = mesh->cw_rotated_halfedge_handle(test);         
   }
   Int32 div = crindis[1] - crindis[0];
   if (count == 6) {
      if (crease == 2) {
         if (div == 3) return 1;
       else return 0;
      } else
      if (crease == 1) return -1;
      return 0;
   }   
   if ((count == 4) && boundary_found) {
      if (crease == 2) {
         if ((div == 3) || (div == 1)) return 1;
            else return 0;
      }
      else return 0;
   }
   if (crease == 1) return -1;
   return 0;   
}

// CF moved to here

template<>
void WSDmesh2dat<OSG::Vec3f, MyTriMesh, TRIANGLE>::collectCorner
(Int32 i, HalfedgeHandle &fhe_h, HalfedgeHandle &op)
{
	Int32 vt = 3;
   Int32 ct = 0;  //crease type
   Int32 otherSideRegular = 0;
	OMPoint p;
	HalfedgeHandle test,ende;

   bool debug_output = false;

   Int32 isRegular = vertexIsRegular(mesh->opposite_halfedge_handle(fhe_h));

   //debug_output = true;//(isRegular>0);

	//op = mesh->opposite_halfedge_handle(mesh->next_halfedge_handle(op));
   if (i == 1 || i == 3) {
	   test = mesh->prev_halfedge_handle(fhe_h); 
      vt = 2;
   }
   else
      test = mesh->prev_halfedge_handle(
             mesh->opposite_halfedge_handle(
             mesh->prev_halfedge_handle(fhe_h))); 

	if (op == test) ppatch->valenz[i] = vt;		// ecke!
	else {
		ende = op;		
		test = mesh->prev_halfedge_handle(mesh->opposite_halfedge_handle(test)); 
		while (test != ende) {			
			vt++; 
         
			p=mesh->point(mesh->from_vertex_handle(test));	// beta
			(ppatch->cornerO[i][vt-1]).setValues(p[0], p[1], p[2]); 

         // store the Crease-Type (or 0 for "no crease")
         otherSideRegular = vertexIsRegular(mesh->opposite_halfedge_handle(test));
			// scharfe Kante speichern
			//old:(ppatch->corcrease[i][vt-1]) = mesh->edge(mesh->edge_handle(test)).isCrease;			         
         (ppatch->corcrease[i][vt-1]) = getCreaseType_Edge(
            mesh->edge(mesh->edge_handle(test)).isCrease,
            otherSideRegular, isRegular);

         if (debug_output) {

            SLOG << "i=" << i <<
               " ct(" << (vt-1) << "):" << ppatch->corcrease[i][vt-1] <<
               "    isRegular=" << isRegular <<
               "    otherside=" << otherSideRegular <<
               std::endl;
         }


         test = mesh->prev_halfedge_handle(mesh->opposite_halfedge_handle(test)); 
		} 	
		ppatch->valenz[i] = vt;
   }

   Int32 stop = 2;
   if (i == 0 || i == 2) stop = 3;
   // go on two/three edges to complete the data!

   for (Int32 ll=0; ll<stop; ll++) {
      otherSideRegular = vertexIsRegular(mesh->opposite_halfedge_handle(test));
      (ppatch->corcrease[i][ll]) = getCreaseType_Edge(
            mesh->edge(mesh->edge_handle(test)).isCrease,
            otherSideRegular, isRegular);
      p=mesh->point(mesh->from_vertex_handle(test));
	   (ppatch->cornerO[i][ll]).setValues(p[0], p[1], p[2]); 
      if (debug_output) {
               SLOG << "i=" << i <<
                  " ct(" << ll << "):" << ppatch->corcrease[i][ll] << 
                  "    isRegular=" << isRegular <<
                  "    otherside=" << otherSideRegular <<
                  std::endl;  }

      test = mesh->prev_halfedge_handle(mesh->opposite_halfedge_handle(test));
   }   

}


// zum eintragen der creases 2 und 7 (bei 0) usw
template<>
void WSDmesh2dat<OSG::Vec3f, MyTriMesh, TRIANGLE>::helper1
(Int32 i, Int32 a2, Int32 a7, HalfedgeHandle &fhe_h)			
{
	HalfedgeHandle op = mesh->opposite_halfedge_handle(fhe_h);	

   Int32 v0_regular = vertexIsRegular(op);
   Int32 v1_regular = vertexIsRegular(fhe_h);
   Int32 n2_regular = vertexIsRegular(mesh->next_halfedge_handle(op));
   Int32 n7_regular = vertexIsRegular(mesh->next_halfedge_handle(
      mesh->opposite_halfedge_handle( 
      mesh->prev_halfedge_handle(op))));
   
	(ppatch->crease[a2]) = getCreaseType_Edge(
      mesh->edge(mesh->edge_handle(mesh->next_halfedge_handle(op))).isCrease,
      n2_regular,v0_regular);
	(ppatch->crease[a7]) = getCreaseType_Edge(
      mesh->edge(
      mesh->edge_handle(
      mesh->prev_halfedge_handle(
      mesh->opposite_halfedge_handle(         
      mesh->prev_halfedge_handle(op))))).isCrease,
      n7_regular,v1_regular);	
}

// zum eintragen der creases 10 und 11 (bei 1) bzw 0 und 1
template<>
void WSDmesh2dat<OSG::Vec3f, MyTriMesh, TRIANGLE>::helper2
(Int32 i, Int32 a11, Int32 a10, HalfedgeHandle &fhe_h)			
{
	HalfedgeHandle op = mesh->opposite_halfedge_handle(fhe_h);

   Int32 v1_regular = vertexIsRegular(op);
   Int32 v2_regular = vertexIsRegular(fhe_h);
   Int32 n10_regular = vertexIsRegular(mesh->next_halfedge_handle(
      mesh->opposite_halfedge_handle(
      mesh->next_halfedge_handle(op))));
   Int32 n11_regular = vertexIsRegular(
      mesh->opposite_halfedge_handle(
      mesh->prev_halfedge_handle(op)));   
   
	(ppatch->crease[a11]) = getCreaseType_Edge(
      mesh->edge(mesh->edge_handle(mesh->prev_halfedge_handle(op))).isCrease,
      n11_regular,v2_regular);
	(ppatch->crease[a10]) = getCreaseType_Edge(
      mesh->edge(
      mesh->edge_handle(
      mesh->next_halfedge_handle(
      mesh->opposite_halfedge_handle(         
      mesh->next_halfedge_handle(op))))).isCrease,
      n10_regular,v1_regular);	
}

// helper (to make the code clearer)
template<>
void WSDmesh2dat<OSG::Vec3f, MyTriMesh, TRIANGLE>::p_nach
(const OMPoint& p, Int32 x, Int32 y) 
{
	(ppatch->slateO[x][y]).setValues(p[0], p[1], p[2]);
}

// insertface: main method for reading from openmesh
// data for pp will be read from Mesh->f_h
template<>
void WSDmesh2dat<OSG::Vec3f, MyTriMesh, TRIANGLE>::insertface
(FaceHandle f_h, FaceHandle tpair, WSDdat<VectorType, MType> *pp)
{	
	ppatch = pp;
	OMPoint p;	
	Int32 vt;
	HalfedgeHandle fhe_h;
	HalfedgeHandle op,test,ende;	

   // init tricrease with null (default)
   for (Int32 ll=0; ll<9; ll++) {
      ppatch->tricrease[ll]=0;
   }
   // init crease with null (default)
   for (Int32 ll=0; ll<12; ll++) {
      ppatch->crease[ll]=0;
   }
   // init iscorner with 0 (we have to identify edge-corners while reading the mesh data!)
   for (Int32 ll=0; ll<4; ll++) {
      ppatch->iscorner[ll] = 0;
   }

   
//*******	
   fhe_h = getfirsthalfedge(f_h, tpair);
   fhe_h = mesh->next_halfedge_handle(fhe_h);
   helper1(0,2,7,fhe_h);   // for creases 2,7
   
	p=mesh->point(mesh->from_vertex_handle(fhe_h));
	p_nach(p, 1,1);	
	(ppatch->crease[5]) = mesh->edge(mesh->edge_handle(fhe_h)).isCrease;

   // tex
   OMTexCoord tc;
   if (useTexture) {
     tc = mesh->texcoord(mesh->from_vertex_handle(fhe_h));   
     (ppatch->TexCoords[1][1]).setValues(tc[0],tc[1],0.0);    
   }
   
	//  edge A
	op = mesh->opposite_halfedge_handle(fhe_h);

	p=mesh->point(mesh->to_vertex_handle(mesh->next_halfedge_handle(op)));
	p_nach(p, 1,0);

	p=mesh->point(
      mesh->from_vertex_handle(
      mesh->prev_halfedge_handle(
      mesh->opposite_halfedge_handle(
      mesh->prev_halfedge_handle(op)
      ))));
	p_nach(p, 2,0);
	
	// corner 0
	this->collectCorner(0,fhe_h,op);
   ppatch->isregular[1] = vertexIsRegular(fhe_h);

//*******
	fhe_h=mesh->next_halfedge_handle(fhe_h);
   helper2(1,11,10,fhe_h);

	p=mesh->point(mesh->from_vertex_handle(fhe_h));
	p_nach(p, 2,1);	
	(ppatch->crease[8]) = mesh->edge(mesh->edge_handle(fhe_h)).isCrease;

   // tex
   if (useTexture) {
    tc=mesh->texcoord(mesh->from_vertex_handle(fhe_h));
    (ppatch->TexCoords[2][1]).setValues(tc[0],tc[1],0.0);    
   }
	
	// edge B
	op = mesh->opposite_halfedge_handle(fhe_h);
	p=mesh->point(
      mesh->to_vertex_handle(
      mesh->next_halfedge_handle(
      mesh->opposite_halfedge_handle(
      mesh->next_halfedge_handle(op)
      ))));
	p_nach(p, 3,1);
    p=mesh->point(mesh->to_vertex_handle(mesh->next_halfedge_handle(op)));
	p_nach(p, 3,2);
   p=mesh->point(mesh->from_vertex_handle(
      mesh->prev_halfedge_handle(
      mesh->opposite_halfedge_handle(
      mesh->prev_halfedge_handle(
      op)))));
	p_nach(p, 3,3);

	// corner 1
	this->collectCorner(1,fhe_h,op);
   ppatch->isregular[2] = vertexIsRegular(fhe_h);
   
   // careful: the second triangle may be the first one (single triangle case)

   p=mesh->point(mesh->to_vertex_handle(fhe_h));
	p_nach(p, 2,2);

   // tex
   if (useTexture) {
    tc=mesh->texcoord(mesh->to_vertex_handle(fhe_h));
    (ppatch->TexCoords[2][2]).setValues(tc[0],tc[1],0.0);    
   }

//******* second triangle!
	fhe_h = getfirsthalfedge(tpair, f_h);       // just the other way round to get the next halfedge
   fhe_h = mesh->next_halfedge_handle(fhe_h);
   helper1(2,9,4,fhe_h);   // for creases 9,4

	
	(ppatch->crease[6]) = mesh->edge(mesh->edge_handle(fhe_h)).isCrease;
	
	// edge C
	op = mesh->opposite_halfedge_handle(fhe_h);
	p=mesh->point(mesh->to_vertex_handle(mesh->next_halfedge_handle(op)));
	p_nach(p, 2,3);   
	p=mesh->point(
      mesh->from_vertex_handle(
      mesh->prev_halfedge_handle(
      mesh->opposite_halfedge_handle(      
      mesh->prev_halfedge_handle(op)
      ))));
	p_nach(p, 1,3);
	
	// corner 2
	this->collectCorner(2,fhe_h,op);
   ppatch->isregular[3] = vertexIsRegular(fhe_h);

//*******
	fhe_h=mesh->next_halfedge_handle(fhe_h);
   helper2(3,0,1,fhe_h);

	p=mesh->point(mesh->from_vertex_handle(fhe_h));
	p_nach(p, 1,2);
	(ppatch->crease[3]) = mesh->edge(mesh->edge_handle(fhe_h)).isCrease;

   // tex
   if (useTexture) {
   tc=mesh->texcoord(mesh->from_vertex_handle(fhe_h));   
   (ppatch->TexCoords[1][2]).setValues(tc[0],tc[1],0.0);   
   }
	
	// edge D
	op = mesh->opposite_halfedge_handle(fhe_h);
	p=mesh->point(
      mesh->to_vertex_handle(
      mesh->next_halfedge_handle(
      mesh->opposite_halfedge_handle(
      mesh->next_halfedge_handle(op)
      ))));
	p_nach(p, 0,2);
    p=mesh->point(mesh->to_vertex_handle(mesh->next_halfedge_handle(op)));
	p_nach(p, 0,1);

   p=mesh->point(mesh->from_vertex_handle(
      mesh->prev_halfedge_handle(
      mesh->opposite_halfedge_handle(
      mesh->prev_halfedge_handle(
      op)))));
   p_nach(p, 0,0);

   
	// corner 3
	this->collectCorner(3,fhe_h,op);
   ppatch->isregular[0] = vertexIsRegular(fhe_h);
 

//*************************************************************************************
	// store the crease data in separate arrays

   ppatch->tricrease[0] = ppatch->corcrease[0][4%ppatch->valenz[0]];
   ppatch->tricrease[1] = ppatch->corcrease[3][ppatch->valenz[3]-1];

   ppatch->tricrease[3] = ppatch->corcrease[1][2%ppatch->valenz[1]];
   ppatch->tricrease[4] = ppatch->corcrease[0][1];   
   ppatch->tricrease[5] = ppatch->corcrease[3][2%ppatch->valenz[3]];
   
   ppatch->tricrease[7] = ppatch->corcrease[1][ppatch->valenz[1]-1];   
   ppatch->tricrease[8] = ppatch->corcrease[2][4%ppatch->valenz[2]];   

   assert(ppatch->corcrease[0][1] == ppatch->corcrease[2][1]);


   ppatch->crease[0]  = ppatch->corcrease[0][3%ppatch->valenz[0]];
   ppatch->crease[1]  = ppatch->corcrease[3][4%ppatch->valenz[3]];
   ppatch->crease[2]  = ppatch->corcrease[0][ppatch->valenz[0]-1];
    //ppatch->crease[3]  = ppatch->corcrease[0][2%ppatch->valenz[0]];
   ppatch->crease[4]  = ppatch->corcrease[3][3%ppatch->valenz[3]];
    //ppatch->crease[5]  = ppatch->corcrease[0][0];
    //ppatch->crease[6]  = ppatch->corcrease[3][1];
   ppatch->crease[7]  = ppatch->corcrease[1][ppatch->valenz[1]-1];
    //ppatch->crease[8]  = ppatch->corcrease[1][0];
   ppatch->crease[9]  = ppatch->corcrease[2][ppatch->valenz[2]-1];
   ppatch->crease[10] = ppatch->corcrease[1][4%ppatch->valenz[1]];
   ppatch->crease[11] = ppatch->corcrease[2][3%ppatch->valenz[2]];


   ppatch->slateO[0][0] = ppatch->cornerO[0][4%ppatch->valenz[0]];
   ppatch->slateO[0][1] = ppatch->cornerO[0][3%ppatch->valenz[0]];
   ppatch->slateO[1][0] = ppatch->cornerO[0][ppatch->valenz[0]-1];

   ppatch->slateO[0][2] = ppatch->cornerO[3][4%ppatch->valenz[3]];
   ppatch->slateO[1][3] = ppatch->cornerO[3][3%ppatch->valenz[3]];

   ppatch->slateO[2][0] = ppatch->cornerO[1][3%ppatch->valenz[1]];
   ppatch->slateO[3][1] = ppatch->cornerO[1][4%ppatch->valenz[1]];

   ppatch->slateO[2][3] = ppatch->cornerO[2][ppatch->valenz[2]-1];
   ppatch->slateO[3][2] = ppatch->cornerO[2][3%ppatch->valenz[2]];
   ppatch->slateO[3][3] = ppatch->cornerO[2][4%ppatch->valenz[2]];


   for (Int32 ll=0; ll<4; ll++) {                           // all corners
      // corners (sum up the incident creases)
      for (Int32 kk = 0; kk<ppatch->valenz[ll]; kk++)
         if (ppatch->corcrease[ll][kk]>0) ppatch->iscorner[ll]++;  

      // get crindis (for 2 crease corners and for the normal calculation also more t 2 c)
		if (ppatch->iscorner[ll] >= 2) {         
			Int32 vv=ppatch->valenz[ll];
			Int32 aa=vv;				
			Int32 bb=1;						
			while (ppatch->corcrease[ll][aa%vv] <1) aa--;
			while (ppatch->corcrease[ll][bb%vv] <1) bb++;
			ppatch->crindis[ll][0] = aa%vv;
			ppatch->crindis[ll][1] = bb%vv;         
		}
	}

	// calculate ppatch->radiusD
   VectorType d1 = ppatch->slateO[1][1] - ppatch->slateO[2][2];
   VectorType d2 = ppatch->slateO[1][2] - ppatch->slateO[2][1];

   ppatch->radiusD = osgMax(d1.length(),d2.length()) / 2.0;
  	
   ppatch->maxdepth=0;        
   ppatch->currentdepth=0;

   // faceloc is the avarage of all four corners
   ppatch->faceloc = (ppatch->slateO[1][1] + ppatch->slateO[2][1] +
					  ppatch->slateO[2][2] + ppatch->slateO[1][2]) * 0.25;

   ppatch->cvindex = 0;
   ppatch->tabsize = 0;  
}
