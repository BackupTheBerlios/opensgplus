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
// mtype = QUAD
/////////////////////////////////////////////////////////////////////////////

template<>
void WSDmesh2dat<OSG::Vec3f, MyPolyMesh, QUAD>::collectCorner
(Int32 i, HalfedgeHandle &fhe_h, HalfedgeHandle &op)
{
	Int32 vt = 3;
	Int32 cc = 3;
	OMPoint p;
	HalfedgeHandle test,ende;
	op = mesh->opposite_halfedge_handle(mesh->next_halfedge_handle(op));
	test = mesh->prev_halfedge_handle(fhe_h); 
	if (op == test) {// ecke!
	   ppatch->valenz[i] = 2;
	} else {
	   ende = op;
		
		test = mesh->prev_halfedge_handle(mesh->opposite_halfedge_handle(test)); 
		while (test != ende) {
			test = mesh->prev_halfedge_handle(mesh->opposite_halfedge_handle(test)); 
			vt++; 
			// weil quads: auch den from von prev
			p=mesh->point(mesh->from_vertex_handle(mesh->prev_halfedge_handle(test)));	// gamma
			(ppatch->cornerO[i][(vt-1)*2]).setValues(p[0], p[1], p[2]); 

			p=mesh->point(mesh->from_vertex_handle(test));	// beta
			(ppatch->cornerO[i][1+((vt-1)*2)]).setValues(p[0], p[1], p[2]); 

			// scharfe Kante speichern
			(ppatch->corcrease[i][cc]) = mesh->edge(mesh->edge_handle(test)).isCrease;
			cc++;
		} 
	
		ppatch->valenz[i] = vt;	

		// einen weiter wegen berechnung in cpi!!
		test = mesh->prev_halfedge_handle(mesh->opposite_halfedge_handle(test)); 
		vt++;
		// weil quads: auch den from von prev
		p=mesh->point(mesh->from_vertex_handle(mesh->prev_halfedge_handle(test)));	// gamma
		(ppatch->cornerO[i][(vt-1)*2]).setValues(p[0], p[1], p[2]); 

		p=mesh->point(mesh->from_vertex_handle(test));	// beta
		(ppatch->cornerO[i][1+((vt-1)*2)]).setValues(p[0], p[1], p[2]); 
	}
}

// zum eintragen der creases 2 und 7 (bei 0) usw
template<>
void WSDmesh2dat<OSG::Vec3f, MyPolyMesh, QUAD>::helper1
(Int32 i, Int32 a2, Int32 a7, HalfedgeHandle &fhe_h)			
{
	HalfedgeHandle op = mesh->opposite_halfedge_handle(fhe_h);
	if (mesh->is_boundary(mesh->edge_handle(fhe_h))) {
		if (ppatch->valenz[i] < 4)	
			(ppatch->crease[a2]) = 0;
		else
			(ppatch->crease[a2]) = mesh->edge(mesh->edge_handle(mesh->next_halfedge_handle(op))).isCrease;	
		if (ppatch->valenz[(i+1)%4] < 4)
			(ppatch->crease[a7]) = 0;
		else
			(ppatch->crease[a7]) = mesh->edge(mesh->edge_handle(mesh->prev_halfedge_handle(op))).isCrease;
	} else {
		(ppatch->crease[a2]) = mesh->edge(mesh->edge_handle(mesh->next_halfedge_handle(op))).isCrease;	
		(ppatch->crease[a7]) = mesh->edge(mesh->edge_handle(mesh->prev_halfedge_handle(op))).isCrease;
	}
}

// helper (to make the code clearer)
template<>
void WSDmesh2dat<OSG::Vec3f, MyPolyMesh, QUAD>::p_nach
(const OMPoint& p, Int32 x, Int32 y) 
{
	(ppatch->slateO[x][y]).setValues(p[0], p[1], p[2]);
}

// insertface: main method for reading from openmesh
template<>
void WSDmesh2dat<OSG::Vec3f, MyPolyMesh, QUAD>::insertface
(FaceHandle f_h, FaceHandle tpair, WSDdat<VectorType, MType> *pp)
{	
	ppatch = pp;
	OMPoint p;	
	Int32 vt;
	HalfedgeHandle fhe_h;
	HalfedgeHandle op,test,ende;	
	
//*******
	fhe_h=mesh->halfedge_handle(f_h);					// erste Halfedge von face f_h	
	p=mesh->point(mesh->from_vertex_handle(fhe_h));
	p_nach(p, 1,1);	
	(ppatch->crease[5]) = mesh->edge(mesh->edge_handle(fhe_h)).isCrease;

   // tex
   OMTexCoord tc;
   if (useTexture) {
      tc = mesh->texcoord(mesh->from_vertex_handle(fhe_h));
      (ppatch->TexCoords[1][1]).setValues(tc[0],tc[1],0.0);
   }

	//  zwischen rand A
	op = mesh->opposite_halfedge_handle(fhe_h);
	p=mesh->point(mesh->to_vertex_handle(mesh->next_halfedge_handle(op)));
	p_nach(p, 1,0);
	p=mesh->point(mesh->from_vertex_handle(mesh->prev_halfedge_handle(op)));
	p_nach(p, 2,0);
	
	// Ecke 1
	this->collectCorner(0,fhe_h,op);

//*******
	fhe_h=mesh->next_halfedge_handle(fhe_h);
	p=mesh->point(mesh->from_vertex_handle(fhe_h));
	p_nach(p, 2,1);	
	(ppatch->crease[8]) = mesh->edge(mesh->edge_handle(fhe_h)).isCrease;

   // tex
   if (useTexture) {
      tc = mesh->texcoord(mesh->from_vertex_handle(fhe_h));
      (ppatch->TexCoords[2][1]).setValues(tc[0],tc[1],0.0);
   }
	
	// zwischen rand B
	op = mesh->opposite_halfedge_handle(fhe_h);
	p=mesh->point(mesh->to_vertex_handle(mesh->next_halfedge_handle(op)));
	p_nach(p, 3,1);
	p=mesh->point(mesh->from_vertex_handle(mesh->prev_halfedge_handle(op)));
	p_nach(p, 3,2);

	// Ecke 2
	this->collectCorner(1,fhe_h,op);

//*******
	fhe_h=mesh->next_halfedge_handle(fhe_h);
	p=mesh->point(mesh->from_vertex_handle(fhe_h));
	p_nach(p, 2,2);
	(ppatch->crease[6]) = mesh->edge(mesh->edge_handle(fhe_h)).isCrease;

   // tex
   if (useTexture) {
    tc=mesh->texcoord(mesh->from_vertex_handle(fhe_h));
    (ppatch->TexCoords[2][2]).setValues(tc[0],tc[1],0.0);
   }
	
	// zwischen rand C
	op = mesh->opposite_halfedge_handle(fhe_h);
	p=mesh->point(mesh->to_vertex_handle(mesh->next_halfedge_handle(op)));
	p_nach(p, 2,3);
	p=mesh->point(mesh->from_vertex_handle(mesh->prev_halfedge_handle(op)));
	p_nach(p, 1,3);
	
	// Ecke 3
	this->collectCorner(2,fhe_h,op);

//*******
	fhe_h=mesh->next_halfedge_handle(fhe_h);
	p=mesh->point(mesh->from_vertex_handle(fhe_h));
	p_nach(p, 1,2);
	(ppatch->crease[3]) = mesh->edge(mesh->edge_handle(fhe_h)).isCrease;

   // tex
   if (useTexture) {
   tc=mesh->texcoord(mesh->from_vertex_handle(fhe_h));
   (ppatch->TexCoords[1][2]).setValues(tc[0],tc[1],0.0);
   }
	
	// zwischen rand D
	op = mesh->opposite_halfedge_handle(fhe_h);
	p=mesh->point(mesh->to_vertex_handle(mesh->next_halfedge_handle(op)));
	p_nach(p, 0,2);
	p=mesh->point(mesh->from_vertex_handle(mesh->prev_halfedge_handle(op)));
	p_nach(p, 0,1);
	
	// Ecke 4
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
	//alle scharfe Kanten ins array aufnehmen
	
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
	// Ecken (Summe der adjazenten scharfen kanten bilden)

	for (Int32 ll=0; ll<4; ll++) {
		if (ppatch->valenz[ll] == 3){			// valenz 3 bekommt zwei identische Kanten!
			Int32 ahh = osgMax(ppatch->corcrease[ll][3],ppatch->corcrease[ll][2]);
			ppatch->corcrease[ll][3] = ppatch->corcrease[ll][2] = ahh;
		}
		Int32 kk;
		if (ppatch->valenz[ll] > 2)	{
			ppatch->iscorner[ll] = 0;
			for (kk = 0; kk<ppatch->valenz[ll]; kk++) ppatch->iscorner[ll] = ppatch->iscorner[ll] + ppatch->corcrease[ll][kk];
		}
		else ppatch->iscorner[ll] = 3; // wenn valenz = 2 immer Ecke!
	}		

	// crindis bestimmen (bei 2 scharfen kanten und für die normale bei mehr als 2 s k)
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

   if (d1.length() > d2.length()) 
	   ppatch->radiusD = d1.length() / 2.0;
   else 
	   ppatch->radiusD = d2.length() / 2.0;
	
   ppatch->maxdepth=0;
   ppatch->currentdepth=0;

   ppatch->faceloc = (ppatch->slateO[1][1] + ppatch->slateO[2][1] +
					  ppatch->slateO[2][2] + ppatch->slateO[1][2]) * 0.25;

   ppatch->cvindex = 0;
   ppatch->tabsize = 0;
}



