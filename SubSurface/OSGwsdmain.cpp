/*---------------------------------------------------------------------------*\
 *                       OpenSG - Dynamic Subdivision                        *
 *                                                                           *
 *                                                                           *
 *					  contact: v.settgast@cg.cs.tu-bs.de * 
 *                                                                           *
\*---------------------------------------------------------------------------*/

#include "OSGwsdmain.h"
OSG_USING_NAMESPACE

// explicit instantiations
template class OSG_SUBSURFACELIB_DLLMAPPING WSDmain<OSG::Vec3f, MyTriMesh,   TRIANGLE>;
template class OSG_SUBSURFACELIB_DLLMAPPING WSDmain<OSG::Vec3f, MyPolyMesh,  QUAD>;
template class OSG_SUBSURFACELIB_DLLMAPPING WSDmain<OSG::Vec4f, MyPolyMesh4, QUAD>;

// getFaceIndex: for a given facehandle return the hash value
template<>
Int32 WSDmain<OSG::Vec3f, MyTriMesh, TRIANGLE>::getFaceIndex (FaceHandle fh)
{
	Int32 i=0;
	for (i=0; i<patches.size(); i++)
		if ((OmeshFaceListe[0][i] == fh)
         || (OmeshFaceListe[1][i] == fh)) return i;	
	return -1;
}
#if 0
template<>
Int32 WSDmain<OSG::Vec3f, MyPolyMesh, QUAD>::getFaceIndex (FaceHandle fh)
{
	Int32 i=0;
	for (i=0; i<patches.size(); i++)
		if (OmeshFaceListe[0][i] == fh) return i;	
	return -1;
}
template<>
Int32 WSDmain<OSG::Vec4f, MyPolyMesh4, QUAD>::getFaceIndex (FaceHandle fh)
{
	Int32 i=0;
	for (i=0; i<patches.size(); i++)
		if (OmeshFaceListe[0][i] == fh) return i;	
	return -1;
}
#endif
template<class WSDVector, class Mesh, int mtype>
Int32 WSDmain<WSDVector, Mesh, mtype>::getFaceIndex (FaceHandle fh)
{
   Int32 i=0;
   for (i=0; i<patches.size(); i++) {
      if (OmeshFaceListe[0][i] == fh) {
	 return i; 
      }
   }
   return -1;
}

// helper to do one subdivision step with creases
#if 0
// Catmull-Clark
template<>
void WSDmain<OSG::Vec4f, MyPolyMesh4, QUAD>::simpleCreaseSubdiv
(WorkType* wara_, WorkType* waB_, OMPoint &a, Int32 g, Int32 h, Int32 i)
{
	Int32 gg=i+g;		// i because of possible negative values!
	Int32 val = (i/2);	
	Int32 numface = (h-g)/2;	
	//sharp edge
	waB_[gg%i].p = (a + wara_[gg%i].p) * 0.5;
	gg++;
	//face
	waB_[(gg)%i].p = (a + wara_[(gg)%i].p + 
			wara_[(gg+1)%i].p + wara_[(gg-1)%i].p) * 0.25;
	gg++;
	for (Int32 si=0; si<numface-1; si++) {				
		//edge
		waB_[gg%i].p = (((a + wara_[gg%i].p) * 0.375) + 
			((wara_[(gg+1)%i].p + wara_[(gg-1)%i].p + 
			 wara_[(gg+2)%i].p + wara_[(gg-2)%i].p) * 0.0625));		
		gg++;
		//face
		waB_[(gg)%i].p = (a + wara_[(gg)%i].p + 
				wara_[(gg+1)%i].p + wara_[(gg-1)%i].p) * 0.25;		
		gg++;
	}
	//sharp edge
	waB_[gg%i].p = (a + wara_[gg%i].p) * 0.5;
	// vertex
	Real32 alpha = 0.75; 
	Real32 beta  = 0.125;
	a = (alpha * a) + (beta * (wara_[g].p + wara_[h].p));
}
// Catmull-Clark
template<>
void WSDmain<OSG::Vec3f, MyPolyMesh, QUAD>::simpleCreaseSubdiv
(WorkType* wara_, WorkType* waB_, OMPoint &a, Int32 g, Int32 h, Int32 i)
{
	Int32 gg=i+g;		// i because of possible negative values!
	Int32 val = (i/2);	
	Int32 numface = (h-g)/2;	
	//sharp edge
	waB_[gg%i].p = (a + wara_[gg%i].p) * 0.5;
	gg++;
	//face
	waB_[(gg)%i].p = (a + wara_[(gg)%i].p + 
			wara_[(gg+1)%i].p + wara_[(gg-1)%i].p) * 0.25;
	gg++;
	for (Int32 si=0; si<numface-1; si++) {				
		//edge
		waB_[gg%i].p = (((a + wara_[gg%i].p) * 0.375) + 
			((wara_[(gg+1)%i].p + wara_[(gg-1)%i].p + 
			 wara_[(gg+2)%i].p + wara_[(gg-2)%i].p) * 0.0625));		
		gg++;
		//face
		waB_[(gg)%i].p = (a + wara_[(gg)%i].p + 
				wara_[(gg+1)%i].p + wara_[(gg-1)%i].p) * 0.25;		
		gg++;
	}
	//sharp edge
	waB_[gg%i].p = (a + wara_[gg%i].p) * 0.5;
	// vertex
	Real32 alpha = 0.75; 
	Real32 beta  = 0.125;
	a = (alpha * a) + (beta * (wara_[g].p + wara_[h].p));
}
#endif
// LOOP
template<>
void WSDmain<OSG::Vec3f, MyTriMesh, TRIANGLE>::simpleCreaseSubdiv
(WorkType* wara_, WorkType* waB_, OMPoint &a, Int32 g, Int32 h, Int32 i)
{
	Int32 gg=i+g;		// i because of possible negative values!
	Int32 numface = h-g;	
	//sharp edge
	waB_[gg%i].p = (a + wara_[gg%i].p) * 0.5; // TODO: only regular case for now...
	gg++;
	for (Int32 si=0; si<numface-1; si++) {				
		//edge
		waB_[gg%i].p = (((a + wara_[gg%i].p) * 0.375) + 
			((wara_[(gg+1)%i].p + wara_[(gg-1)%i].p) * 0.125));		
		gg++;	
	}
	//sharp edge
	waB_[gg%i].p = (a + wara_[gg%i].p) * 0.5;
	// vertex
	Real32 alpha = 0.75; 
	Real32 beta  = 0.125;
	a = (alpha * a) + (beta * (wara_[g].p + wara_[h].p));
}
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::simpleCreaseSubdiv
(WorkType* wara_, WorkType* waB_, OMPoint &a, Int32 g, Int32 h, Int32 i)
{
	Int32 gg=i+g;		// i because of possible negative values!
	Int32 val = (i/2);	
	Int32 numface = (h-g)/2;	
	//sharp edge
	waB_[gg%i].p = (a + wara_[gg%i].p) * 0.5;
	gg++;
	//face
	waB_[(gg)%i].p = (a + wara_[(gg)%i].p + 
			wara_[(gg+1)%i].p + wara_[(gg-1)%i].p) * 0.25;
	gg++;
	for (Int32 si=0; si<numface-1; si++) {				
		//edge
		waB_[gg%i].p = (((a + wara_[gg%i].p) * 0.375) + 
			((wara_[(gg+1)%i].p + wara_[(gg-1)%i].p + 
			 wara_[(gg+2)%i].p + wara_[(gg-2)%i].p) * 0.0625));		
		gg++;
		//face
		waB_[(gg)%i].p = (a + wara_[(gg)%i].p + 
				wara_[(gg+1)%i].p + wara_[(gg-1)%i].p) * 0.25;		
		gg++;
	}
	//sharp edge
	waB_[gg%i].p = (a + wara_[gg%i].p) * 0.5;
	// vertex
	Real32 alpha = 0.75; 
	Real32 beta  = 0.125;
	a = (alpha * a) + (beta * (wara_[g].p + wara_[h].p));
}

// helper to do one subdivision step
// LOOP
template<>
void WSDmain<OSG::Vec3f, MyTriMesh, TRIANGLE>::simpleSubdiv
(WorkType* wara_, WorkType* waB_, OMPoint &a, Int32 i)
{
	Int32 gg=i;
	Real32 val = i;
	OMPoint beta(0,0,0);
	for (Int32 si=0; si<i; si++) {				// edges		
		waB_[gg%i].p = (((a + wara_[gg%i].p) * 0.375) + 
			((wara_[(gg+1)%i].p + wara_[(gg-1)%i].p) * 0.125));
		beta+=wara_[gg%i].p;
		gg++;		
	}
	// vertex	
   Real32 ak = (1.0 / val) *
      (0.625 - ((0.375 + 0.25 * cos((2.0 * Pi)/val)) *
                (0.375 + 0.25 * cos((2.0 * Pi)/val))));
	Real32 falpha = 1.0 - (val*ak);	
	a = (falpha * a) + (ak * beta);
}
// CATMULL-CLARK
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::simpleSubdiv
(WorkType* wara_, WorkType* waB_, OMPoint &a, Int32 i)
{
	Int32 gg=i;
	Int32 val = (i/2);
	OMPoint gamma(0,0,0);
	OMPoint beta(0,0,0);
	for (Int32 si=0; si<val; si++) {				
		//edge
		waB_[gg%i].p = (((a + wara_[gg%i].p) * 0.375) + 
			((wara_[(gg+1)%i].p + wara_[(gg-1)%i].p + 
			 wara_[(gg+2)%i].p + wara_[(gg-2)%i].p) * 0.0625));
		beta+=wara_[gg%i].p;
		gg++;
		//face
		waB_[(gg)%i].p = (a + wara_[(gg)%i].p + 
				wara_[(gg+1)%i].p + wara_[(gg-1)%i].p) * 0.25;
		gamma+=wara_[gg%i].p;
		gg++;
	}
	// vertex	
	Real32 falpha = 1.0 - (3.0 / (2.0 * (Real32)val)) - (1.0 / (4.0 * (Real32)val));
	Real32 fbeta  = 3.0 / (2.0 * (Real32)val * (Real32)val);
	Real32 fgamma = 1.0 / (4.0 * (Real32)val * (Real32)val);
	a = (falpha * a) + (fbeta * beta) + (fgamma * gamma);
}

// calculate normal for creases / edges
// the sector is from g to h in workarray
// LOOP
template<>
WSDmain<OSG::Vec3f, MyTriMesh, TRIANGLE>::OMPoint 
WSDmain<OSG::Vec3f, MyTriMesh, TRIANGLE>::NormCrease
(OMPoint alpha_, WorkType* wara_, Int32 v, Int32 g, Int32 h)
{
	OMPoint betat1,betat2,t1,t2;
	OMPoint ret(0,0,0);
	Real32 k = (Real32)(h-g+1);				
	betat2 = wara_[g%v].p - wara_[h%v].p;
	if (k < 3.0) {		
		betat1 = (wara_[g%v].p + wara_[h%v].p);
		t1 = (alpha_ * -2.0) + betat1;
		t2 = betat2;
		ret = t1%t2;		
	}
	else 
   if (k < 4.0) {		
		betat1 = wara_[(g+1)%v].p;
		t1 = (alpha_ * -1.0) + betat1;
		t2 = betat2;
		ret = t1%t2;		
   } else
   {		// k > 3	
		Real32 pidk = Pi / (k-1.0f);		
		Real32 b0 = osgsin(pidk)/(2.0f*(osgcos(pidk)-2.0f));
		
		betat1 = wara_[g%v].p * b0;
		for (Int32 i=1; i<k-1; i++)	{
			Real32 bi = osgsin((Real32)i * pidk);
			betat1+= wara_[(g+i)%v].p * bi;			
		}
		betat1+= wara_[h%v].p * b0;
		t1 = betat1;
		t2 = betat2;
		ret = t2%t1;
	}
	ret.normalize();
	return ret;
}
// CATMULL-CLARK
template<class WSDVector, class Mesh, int mtype>
typename WSDmain<WSDVector, Mesh, mtype>::OMPoint 
WSDmain<WSDVector, Mesh, mtype>::NormCrease
(OMPoint alpha_, WorkType* wara_, Int32 v, Int32 g, Int32 h)
{
	OMPoint betat1,gammat1,betat2,gammat2,t1,t2;
	OMPoint ret(0,0,0);
	Real32 k = (Real32)((h-g) / 2);				// k is the number of adjacent polygons
	betat2 = wara_[g%v].p - wara_[h%v].p;
	if (k < 2.0) {		
		betat1 = (wara_[g%v].p + wara_[h%v].p) * -1.0;
		t1 = (alpha_ * 2.0) + betat1;
		t2 = betat2;
		ret = t1%t2;		
	}
	else {		// k >= 2	
		Real32 pidk = Pi / k;
		Real32 rk = (osgcos(pidk) + 1.0) / (k * osgsin(pidk) * (3.0 + osgcos(pidk)));
		Real32 b0 = -1.0 * rk * (1.0 + 2.0 * osgcos(pidk));
		Real32 c = osgsin(pidk) / ((3.0 + osgcos(pidk)) * k);
		betat1 = wara_[g%v].p * b0;
		gammat1 = wara_[(g+1)%v].p * c;
		for (Int32 i=1; i<k; i++)	{
			Real32 bi = (4.0 * osgsin((Real32)i * pidk)) / ((3.0 + osgcos(pidk)) * k);
			betat1+= wara_[(g+(i*2))%v].p * bi;
			c = (1.0 * (osgsin((Real32)i * pidk) + osgsin(((Real32)i + 1.0) * pidk))) / ((3.0 + osgcos(pidk)) * k);
			gammat1+= wara_[(g+(i*2)+1)%v].p * c;
		}
		betat1+= wara_[h%v].p * b0;
		Real32 a = 4.0 * rk * (osgcos(pidk) - 1.0);		
		t1 = (alpha_ * a) + betat1 + gammat1;
		t2 = betat2;
		ret = t2%t1;
	}
	ret.normalize();
	return ret;
}

// calculate normal for inner limitpoints
// LOOP
template<>
WSDmain<OSG::Vec3f, MyTriMesh, TRIANGLE>::OMPoint 
WSDmain<OSG::Vec3f, MyTriMesh, TRIANGLE>::NormInner (WorkType* wara_, Int32 v)
{
	Real32 val = (Real32)v;	
	Real32 b1,b2;
	OMPoint ret; 
	OMPoint betat1(0,0,0);
	OMPoint betat2(0,0,0);	
	for (Int32 j=0; j<v; j++)	{
		b1 = osgcos((2.0*Pi*(Real32)j) / val);
		b2 = osgsin((2.0*Pi*(Real32)j) / val);		
		betat1+=wara_[j].p * b1;
		betat2+=wara_[j].p * b2;		
	}	
	ret = betat1 % betat2; 
	ret = ret.normalize();
	return ret;
}
// CATMULL-CLARK
template<class WSDVector, class Mesh, int mtype>
typename WSDmain<WSDVector, Mesh, mtype>::OMPoint 
WSDmain<WSDVector, Mesh, mtype>::NormInner (WorkType* wara_, Int32 v)
{
	Real32 val = (Real32)v / 2.0;
	Real32 ak = 1.0 + osgcos(2.0 * Pi / val) + osgcos(Pi / val) * osgsqrt(2.0*(9.0 + osgcos(2.0 * Pi / val)));
	Real32 b1,b2,c1,c2;
	OMPoint ret;
	OMPoint betat1(0,0,0); 
	OMPoint gammat1(0,0,0);
	OMPoint betat2(0,0,0); 
	OMPoint gammat2(0,0,0);

	Int32 i=0;
	
	for (Int32 j=0; j<v; j++)	{
		b1 = ak * osgcos(2*Pi*(i+1) / val);
		b2 = ak * osgcos(2*Pi*i / val);
		c1 = osgcos(2*Pi*(i+1) / val) + osgcos(2*Pi*(i+2) / val);
		c2 = osgcos(2*Pi*i / val) + osgcos(2*Pi*(i+1) / val);

		betat1+=wara_[j].p * b1;
		betat2+=wara_[j].p * b2;
		j++;
		gammat1+=wara_[j].p * c1;
		gammat2+=wara_[j].p * c2;

		i++;
	}	
	ret = (betat1 + gammat1) % (betat2 + gammat2); 
	ret = ret.normalize();
	return ret;
}


// ----------------------------------------------------------------------------------------
// --- curvature calculation --------------------------------------------------------------
// ----------------------------------------------------------------------------------------

// getOptiDepth: for a given vertexhandle approximate the curvature

// getOptiDepth: mtype = QUAD
#if 0
template<>
void WSDmain<OSG::Vec3f, MyPolyMesh, QUAD>::getOptiDepth 
(MeshType* pmesh, VertexHandle v_h)
{	
	Real32 epsilon=1.0 - NormalConeAperture;
	// build workarray
	WorkArrayType WA1, WA2;	
	WorkType*    wara = &(WA1[0]);
	WorkType*    waB  = &(WA2[0]);
	WorkType*    waT  = NULL;
	HalfedgeHandle hh = pmesh->halfedge_handle(v_h);
	HalfedgeHandle hhold = hh;
	OMPoint a,n;
	Int32 i=0;
	Int32 sharp=0;
	// manual iteration over all outgoing edges 		
	do {
		wara[i].p = pmesh->point(pmesh->to_vertex_handle(hh));
		wara[i].isCrease = pmesh->edge(pmesh->edge_handle(hh)).isCrease;
		if (pmesh->edge(pmesh->edge_handle(hh)).isCrease == 1) sharp++;
		i++;
		wara[i].p = pmesh->point(pmesh->to_vertex_handle(pmesh->next_halfedge_handle(hh)));
		// mark edge-gamma with 8
		if (pmesh->is_boundary(hh)) wara[i].isCrease = 8;
		else wara[i].isCrease = 4;
		wara[i].fh = pmesh->face_handle(hh);
		i++;
		hh = pmesh->prev_halfedge_handle(hh);
		hh = pmesh->opposite_halfedge_handle(hh);
		assert(pmesh->from_vertex_handle(hh) == v_h);
		 
	} while(hh != hhold);
	//now i is the size of the array and sharp is the number of creases
	wara[i].isCrease = sharp;	
  
	// calc limitnormal
	// if wara[val2].isCrease < 2 then it is a inner point
	if (wara[i].isCrease < 2) {			
		a = pmesh->point(v_h);
		n = NormInner(wara,i);				
		Int32 val = (i/2);
		
		// do subdiv until epsilon is reached
		Int32 l=0;				// start with depth=0
		Real32 test=100;			// in test the min angle is stored
				
      // average normal for all faces
		test = getGreatestAngle(wara,a,n,0,val,i);				
		while ((test < epsilon) && (l < wsdmaxdepth))	{						
			l++;								// next depth
			simpleSubdiv(wara,waB,a,i);			// subdiv
			// switch workarrays:
			waT = wara; wara = waB;	waB = waT;		
			// average normal to all adjacent faces
			test = getGreatestAngle(wara,a,n,0,val,i);					
		} 		
		// thats it!
		// now put the new vertex in the vector and connect adj. patches to it
		SVertexData<VectorType, MeshType> *newelem;
      VertexListe.push_back(SVertexData<VectorType, MeshType>());	
      newelem = &(VertexListe.back());
		newelem->maxdepth = l;
		newelem->vh = v_h;	
		
		a = pmesh->point(v_h);
		// iteration over all adj. faces
		MeshType::VertexFaceIter vf_it=pmesh->vf_iter(v_h);
	
		Int32 offset = getLimOffset(&patches[getFaceIndex(vf_it.handle())],a) 
			+ patches[getFaceIndex(vf_it.handle())].varrayOffset;
				
		newelem->limPo.setValue(mySharedFields.limitpoints->getValue(offset));
		newelem->limNo.setValue(mySharedFields.limitnormals->getValue(offset));

		assert(VertexListe.size()>0);
		// now the adjacent patches:
		for (; vf_it; ++vf_it)	{		
			patches[getFaceIndex(vf_it.handle())].vertexindex[
				patches[getFaceIndex(vf_it.handle())].cvindex]=VertexListe.size()-1;
			patches[getFaceIndex(vf_it.handle())].cvindex++;
			if (patches[getFaceIndex(vf_it.handle())].maxdepth < l)
				patches[getFaceIndex(vf_it.handle())].maxdepth = l;

			assert(patches[getFaceIndex(vf_it.handle())].cvindex < 5);
		}
			
	} 
	else {	// for creases: process for every sector
		Int32 numsect = wara[i].isCrease;		
		Int32 secBegin = 0;
		for (Int32 sect=0; sect<numsect; sect++) {	// sector loop
			while (wara[secBegin%i].isCrease == 0) secBegin+=2;
			if (wara[(secBegin+1)%i].isCrease == 4) {	// no empty sector?
				Int32 secEnd = secBegin+2;
				while (wara[secEnd%i].isCrease == 0) secEnd+=2;
				Int32 numface = (secEnd - secBegin) / 2;
				a = pmesh->point(v_h);
				// copy workarray
				WorkArrayType WA3;
				WorkType*     waA = &(WA3[0]);
				for (Int32 co = 0; co < i; co++)
					waA[co].p = wara[co].p;
				// get normal
				n = NormCrease(a,wara,i,secBegin,secEnd);
				Int32 l=0;			// start with depth 0
				Real32 test = getGreatestAngle(wara,a,n,secBegin,numface,i);
				while ((test < epsilon) && (l < wsdmaxdepth))	{					
					l++;			// next depth					
					simpleCreaseSubdiv(waA,waB,a,secBegin,secEnd,i);
					// switch workarrays:
					waT = waA; waA = waB; waB = waT;
					// get normal
					test = getGreatestAngle(waA,a,n,secBegin,numface,i);
				}
				// thats it for this sector
				SVertexData<VectorType, MeshType> *newelem;
            VertexListe.push_back(SVertexData<VectorType, MeshType>());
            newelem = &VertexListe.back();

				newelem->maxdepth = l;
				newelem->vh = v_h;
				a = pmesh->point(v_h);
				Int32 offset = getLimOffset(&patches[getFaceIndex(wara[(secBegin+1)%i].fh)],a)
					+ patches[getFaceIndex(wara[(secBegin+1)%i].fh)].varrayOffset;
		
				newelem->limPo.setValue(mySharedFields.limitpoints->getValue(offset));
				newelem->limNo.setValue(mySharedFields.limitnormals->getValue(offset));

            assert(VertexListe.size()>0);
	
				// connect adj. patches to vertex
				for (Int32 w=secBegin+1; w<secEnd; w+=2) {
					patches[getFaceIndex(wara[w%i].fh)].vertexindex[
						patches[getFaceIndex(wara[w%i].fh)].cvindex]=VertexListe.size()-1;
					patches[getFaceIndex(wara[w%i].fh)].cvindex++;
					assert(patches[getFaceIndex(wara[w%i].fh)].cvindex < 5);					

					if (patches[getFaceIndex(wara[w%i].fh)].maxdepth < l)
						patches[getFaceIndex(wara[w%i].fh)].maxdepth = l;
				}
				
			}
			secBegin+=2;
		}	// for all sectors
	}	
}

template<>
void WSDmain<OSG::Vec4f, MyPolyMesh4, QUAD>::getOptiDepth
(MeshType *pmesh, VertexHandle v_h)
{	
	Real32 epsilon=1.0 - NormalConeAperture;
	// build workarray
	WorkArrayType WA1, WA2;	
	WorkType*    wara = &(WA1[0]);
	WorkType*    waB  = &(WA2[0]);
	WorkType*    waT  = NULL;
	HalfedgeHandle hh = pmesh->halfedge_handle(v_h);
	HalfedgeHandle hhold = hh;
	OMPoint a,n;
	Int32 i=0;
	Int32 sharp=0;
	// manual iteration over all outgoing edges 		
	do {
		wara[i].p = pmesh->point(pmesh->to_vertex_handle(hh));
		wara[i].isCrease = pmesh->edge(pmesh->edge_handle(hh)).isCrease;
		if (pmesh->edge(pmesh->edge_handle(hh)).isCrease == 1) sharp++;
		i++;
		wara[i].p = pmesh->point(pmesh->to_vertex_handle(pmesh->next_halfedge_handle(hh)));
		// mark edge-gamma with 8
		if (pmesh->is_boundary(hh)) wara[i].isCrease = 8;
		else wara[i].isCrease = 4;
		wara[i].fh = pmesh->face_handle(hh);
		i++;
		hh = pmesh->prev_halfedge_handle(hh);
		hh = pmesh->opposite_halfedge_handle(hh);
		assert(pmesh->from_vertex_handle(hh) == v_h);
		 
	} while(hh != hhold);
	//now i is the size of the array and sharp is the number of creases
	wara[i].isCrease = sharp;	
  
	// calc limitnormal
	// if wara[val2].isCrease < 2 then it is a inner point
	if (wara[i].isCrease < 2) {			
		a = pmesh->point(v_h);
		n = NormInner(wara,i);				
		Int32 val = (i/2);
		
		// do subdiv until epsilon is reached
		Int32 l=0;				// start with depth=0
		Real32 test=100;			// in test the min angle is stored
		
		// average normal for all faces
		test = getGreatestAngle(wara,a,n,0,val,i);				
		while ((test < epsilon) && (l < wsdmaxdepth))	{						
			l++;								// next depth
			simpleSubdiv(wara,waB,a,i);			// subdiv
			// switch workarrays:
			waT = wara; wara = waB;	waB = waT;		
			// average normal to all adjacent faces
			test = getGreatestAngle(wara,a,n,0,val,i);					
		} 		
		// thats it!
		// now put the new vertex in the vector and connect adj. patches to it
		SVertexData<VectorType, MeshType> *newelem;
      VertexListe.push_back(SVertexData<VectorType, MeshType>());
      newelem = &(VertexListe.back());

		newelem->maxdepth = l;
		newelem->vh = v_h;	
		
		a = pmesh->point(v_h);
		// iteration over all adj. faces
		MeshType::VertexFaceIter vf_it=pmesh->vf_iter(v_h);
	
		Int32 offset = getLimOffset(&patches[getFaceIndex(vf_it.handle())],a) 
			+ patches[getFaceIndex(vf_it.handle())].varrayOffset;
				
		newelem->limPo.setValue(mySharedFields.limitpoints->getValue(offset));
		newelem->limNo.setValue(mySharedFields.limitnormals->getValue(offset));

      assert(VertexListe.size()>0);

		// now the adjacent patches:
		for (; vf_it; ++vf_it)	{		
			patches[getFaceIndex(vf_it.handle())].vertexindex[
				patches[getFaceIndex(vf_it.handle())].cvindex]=VertexListe.size()-1;
			patches[getFaceIndex(vf_it.handle())].cvindex++;
			if (patches[getFaceIndex(vf_it.handle())].maxdepth < l)
				patches[getFaceIndex(vf_it.handle())].maxdepth = l;

			assert(patches[getFaceIndex(vf_it.handle())].cvindex < 5);
		}		
	} 
	else {	// for creases: process for every sector
		Int32 numsect = wara[i].isCrease;		
		Int32 secBegin = 0;
		for (Int32 sect=0; sect<numsect; sect++) {	// sector loop
			while (wara[secBegin%i].isCrease == 0) secBegin+=2;
			if (wara[(secBegin+1)%i].isCrease == 4) {	// no empty sector?
				Int32 secEnd = secBegin+2;
				while (wara[secEnd%i].isCrease == 0) secEnd+=2;
				Int32 numface = (secEnd - secBegin) / 2;
				a = pmesh->point(v_h);
				// copy workarray
				WorkArrayType WA3;
				WorkType*     waA = &(WA3[0]);
				for (Int32 co = 0; co < i; co++)
					waA[co].p = wara[co].p;
				// get normal
				n = NormCrease(a,wara,i,secBegin,secEnd);
				Int32 l=0;			// start with depth 0
				Real32 test = getGreatestAngle(wara,a,n,secBegin,numface,i);
				while ((test < epsilon) && (l < wsdmaxdepth))	{					
					l++;			// next depth					
					simpleCreaseSubdiv(waA,waB,a,secBegin,secEnd,i);
					// switch workarrays:
					waT = waA; waA = waB; waB = waT;
					// get normal
					test = getGreatestAngle(waA,a,n,secBegin,numface,i);
				}
				// thats it for this sector
				SVertexData<VectorType, MeshType> *newelem;
            VertexListe.push_back(SVertexData<VectorType, MeshType>());
            newelem = &(VertexListe.back());
				newelem->maxdepth = l;
				newelem->vh = v_h;
				a = pmesh->point(v_h);
				Int32 offset = getLimOffset(&patches[getFaceIndex(wara[(secBegin+1)%i].fh)],a)
					+ patches[getFaceIndex(wara[(secBegin+1)%i].fh)].varrayOffset;
		
				newelem->limPo.setValue(mySharedFields.limitpoints->getValue(offset));
				newelem->limNo.setValue(mySharedFields.limitnormals->getValue(offset));

            assert(VertexListe.size()>0);
	
				// connect adj. patches to vertex
				for (Int32 w=secBegin+1; w<secEnd; w+=2) {
					patches[getFaceIndex(wara[w%i].fh)].vertexindex[
						patches[getFaceIndex(wara[w%i].fh)].cvindex]=VertexListe.size()-1;
					patches[getFaceIndex(wara[w%i].fh)].cvindex++;
					assert(patches[getFaceIndex(wara[w%i].fh)].cvindex < 5);					

					if (patches[getFaceIndex(wara[w%i].fh)].maxdepth < l)
						patches[getFaceIndex(wara[w%i].fh)].maxdepth = l;
				}
				
			}
			secBegin+=2;
		}	// for all sectors
	}	
}
#endif

// getOptiDepth: mtype = TRIANGLE
template<>
void WSDmain<OSG::Vec3f, MyTriMesh, TRIANGLE>::getOptiDepth
(MeshType *pmesh, VertexHandle v_h)
{	
	Real32 epsilon=1.0 - NormalConeAperture;
	// build workarray
	WorkArrayType WA1, WA2;	
	WorkType*    wara = &(WA1[0]);
	WorkType*    waB  = &(WA2[0]);
	WorkType*    waT  = NULL;
	HalfedgeHandle hh = pmesh->halfedge_handle(v_h);
	HalfedgeHandle hhold = hh;
	OMPoint a,n;
	Int32 i=0;
	Int32 sharp=0;
	// manual iteration over all outgoing edges 		
	do {
		wara[i].p = pmesh->point(pmesh->to_vertex_handle(hh));
		wara[i].isCrease = pmesh->edge(pmesh->edge_handle(hh)).isCrease;
		if (pmesh->edge(pmesh->edge_handle(hh)).isCrease > 0) sharp++;
		wara[i].fh = pmesh->face_handle(hh);      
      if (pmesh->is_boundary(hh)) {
          wara[i].isCrease = 99;          
      }
		i++;
		hh = pmesh->prev_halfedge_handle(hh);
		hh = pmesh->opposite_halfedge_handle(hh);
		assert(pmesh->from_vertex_handle(hh) == v_h);		 
	} while(hh != hhold);
	// now i is the size of the array (the valence in loop case) 
   // and sharp is the number of creases
	wara[i].isCrease = sharp;	
  
	// calc limitnormal
	// if wara[val2].isCrease < 2 then it is a inner point
	if (wara[i].isCrease < 2) {			
		a = pmesh->point(v_h);
		n = NormInner(wara,i);				
		Int32 val = i;       
		
		// do subdiv until epsilon is reached
		Int32 l=0;				// start with depth=0
		Real32 test=100;			// in test the min angle is stored
		
		// average normal for all faces
		test = getGreatestAngle(wara,a,n,0,val,i);				
		while ((test < epsilon) && (l < wsdmaxdepth))	{						
			l++;								// next depth
			simpleSubdiv(wara,waB,a,i);			// subdiv with loop
			// switch workarrays:
			waT = wara; wara = waB;	waB = waT;		
			// average normal to all adjacent faces
			test = getGreatestAngle(wara,a,n,0,val,i);					
		} 		
		// thats it!
		// now put the new vertex in the vector and connect adj. patches to it
		SVertexData<VectorType, MeshType> *newelem;
      VertexListe.push_back(SVertexData<VectorType, MeshType>());	
      newelem = &(VertexListe.back());
		newelem->maxdepth = l;
		newelem->vh = v_h;	
		
		a = pmesh->point(v_h);
		// iteration over all adj. faces
		MeshType::VertexFaceIter vf_it=pmesh->vf_iter(v_h);
	
		Int32 offset = getLimOffset(&patches[getFaceIndex(vf_it.handle())],a) 
			+ patches[getFaceIndex(vf_it.handle())].varrayOffset;
				
		newelem->limPo.setValue(mySharedFields.limitpoints->getValue(offset));
		newelem->limNo.setValue(mySharedFields.limitnormals->getValue(offset));

      Int32 merk_index, old_index, first_index;
      old_index = -1;
      first_index = -21;

      assert(VertexListe.size()>0);
		// now the adjacent patches:
		for (; vf_it; ++vf_it)	{		
         merk_index = getFaceIndex(vf_it.handle());         
         if ((merk_index != old_index) &&
             (merk_index != first_index)){
			   patches[merk_index].vertexindex[
				   patches[merk_index].cvindex]=VertexListe.size()-1;
			   patches[merk_index].cvindex++;
			   if (patches[merk_index].maxdepth < l)
				   patches[merk_index].maxdepth = l;
         }
         if (first_index == -21) first_index = merk_index;
         old_index = merk_index;

			assert(patches[getFaceIndex(vf_it.handle())].cvindex < 5);
		}
			
	} 
	else {	// for creases: process for every sector
		Int32 numsect = wara[i].isCrease;		      
		Int32 secBegin = 0;
		for (Int32 sect=0; sect<numsect; sect++) {	// sector loop
			while (wara[secBegin%i].isCrease < 1) secBegin++;
			if (wara[(secBegin)%i].isCrease < 99) {	// no empty sector?
				Int32 secEnd = secBegin+1;
				while (wara[secEnd%i].isCrease < 1) secEnd++;
				Int32 numface = (secEnd - secBegin);
				a = pmesh->point(v_h);
				// copy workarray
				WorkArrayType WA3;
				WorkType*     waA = &(WA3[0]);
				for (Int32 co = 0; co < i; co++)
					waA[co].p = wara[co].p;
				// get normal
				n = NormCrease(a,wara,i,secBegin,secEnd);
				Int32 l=0;			// start with depth 0
				Real32 test = getGreatestAngle(wara,a,n,secBegin,numface,i);
				while ((test < epsilon) && (l < wsdmaxdepth))	{					
					l++;			// next depth					
					simpleCreaseSubdiv(waA,waB,a,secBegin,secEnd,i);
					// switch workarrays:
					waT = waA; waA = waB; waB = waT;
					// get normal
					test = getGreatestAngle(waA,a,n,secBegin,numface,i);
				}
				// thats it for this sector
				SVertexData<VectorType, MeshType> *newelem;
            VertexListe.push_back(SVertexData<VectorType, MeshType>());
            newelem = &VertexListe.back();

				newelem->maxdepth = l;
				newelem->vh = v_h;

				a = pmesh->point(v_h);
            Int32 faceindex = getFaceIndex(wara[secBegin%i].fh);
            assert(faceindex>=0);
				Int32 offset = getLimOffset(&patches[faceindex],a)
					+ patches[faceindex].varrayOffset;
		
				newelem->limPo.setValue(mySharedFields.limitpoints->getValue(offset));
				newelem->limNo.setValue(mySharedFields.limitnormals->getValue(offset));

            assert(VertexListe.size()>0);
	
            Int32 oldindex = -1;             // because we have two faces per patch double indices can occur!
				// connect adj. patches to vertex
				for (Int32 w=secBegin; w<secEnd; w++) {
               faceindex = getFaceIndex(wara[w%i].fh);
               if (faceindex != oldindex) {                 
                  assert(wara[w%i].isCrease < 99);
					   patches[faceindex].vertexindex[
						   patches[faceindex].cvindex]=VertexListe.size()-1;
					   patches[faceindex].cvindex++;
					   assert(patches[faceindex].cvindex < 5);					
					   if (patches[faceindex].maxdepth < l)
						   patches[faceindex].maxdepth = l;
               }
               oldindex = faceindex;
				}
				
			}
			secBegin++;
		}	// for all sectors
	}	
}

template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::getOptiDepth
(MeshType* pmesh, VertexHandle v_h)
{
	Real32 epsilon=1.0 - NormalConeAperture;
	// build workarray
	WorkArrayType WA1, WA2;	
	WorkType*    wara = &(WA1[0]);
	WorkType*    waB  = &(WA2[0]);
	WorkType*    waT  = NULL;
	HalfedgeHandle hh = pmesh->halfedge_handle(v_h);
	HalfedgeHandle hhold = hh;
	OMPoint a,n;
	Int32 i=0;
	Int32 sharp=0;
	// manual iteration over all outgoing edges 		
	do {
		wara[i].p = pmesh->point(pmesh->to_vertex_handle(hh));
		wara[i].isCrease = pmesh->edge(pmesh->edge_handle(hh)).isCrease;
		if (pmesh->edge(pmesh->edge_handle(hh)).isCrease == 1) sharp++;
		i++;
		wara[i].p = pmesh->point(pmesh->to_vertex_handle(pmesh->next_halfedge_handle(hh)));
		// mark edge-gamma with 8
		if (pmesh->is_boundary(hh)) wara[i].isCrease = 8;
		else wara[i].isCrease = 4;
		wara[i].fh = pmesh->face_handle(hh);
		i++;
		hh = pmesh->prev_halfedge_handle(hh);
		hh = pmesh->opposite_halfedge_handle(hh);
		assert(pmesh->from_vertex_handle(hh) == v_h);
		 
	} while(hh != hhold);
	//now i is the size of the array and sharp is the number of creases
	wara[i].isCrease = sharp;	
  
	// calc limitnormal
	// if wara[val2].isCrease < 2 then it is a inner point
	if (wara[i].isCrease < 2) {			
		a = pmesh->point(v_h);
		n = NormInner(wara,i);				
		Int32 val = (i/2);
		
		// do subdiv until epsilon is reached
		Int32 l=0;				// start with depth=0
		Real32 test=100;			// in test the min angle is stored
		
		// average normal for all faces
		test = getGreatestAngle(wara,a,n,0,val,i);				
		while ((test < epsilon) && (l < wsdmaxdepth))	{						
			l++;								// next depth
			simpleSubdiv(wara,waB,a,i);			// subdiv
			// switch workarrays:
			waT = wara; wara = waB;	waB = waT;		
			// average normal to all adjacent faces
			test = getGreatestAngle(wara,a,n,0,val,i);					
		} 		
		// thats it!
		// now put the new vertex in the vector and connect adj. patches to it
		SVertexData<VectorType, MeshType> *newelem;
      VertexListe.push_back(SVertexData<VectorType, MeshType>());	
      newelem = &(VertexListe.back());
		newelem->maxdepth = l;
		newelem->vh = v_h;	
		
		a = pmesh->point(v_h);
		// iteration over all adj. faces
		typename MeshType::VertexFaceIter vf_it=pmesh->vf_iter(v_h);
	
		Int32 offset = getLimOffset(&patches[getFaceIndex(vf_it.handle())],a) 
			+ patches[getFaceIndex(vf_it.handle())].varrayOffset;
				
		newelem->limPo.setValue(mySharedFields.limitpoints->getValue(offset));
		newelem->limNo.setValue(mySharedFields.limitnormals->getValue(offset));

		assert(VertexListe.size()>0);
		// now the adjacent patches:
		for (; vf_it; ++vf_it)	{		
			patches[getFaceIndex(vf_it.handle())].vertexindex[
				patches[getFaceIndex(vf_it.handle())].cvindex]=VertexListe.size()-1;
			patches[getFaceIndex(vf_it.handle())].cvindex++;
			if (patches[getFaceIndex(vf_it.handle())].maxdepth < l)
				patches[getFaceIndex(vf_it.handle())].maxdepth = l;

			assert(patches[getFaceIndex(vf_it.handle())].cvindex < 5);
		}
			
	} 
	else {	// for creases: process for every sector
		Int32 numsect = wara[i].isCrease;		
		Int32 secBegin = 0;
		for (Int32 sect=0; sect<numsect; sect++) {	// sector loop
			while (wara[secBegin%i].isCrease == 0) secBegin+=2;
			if (wara[(secBegin+1)%i].isCrease == 4) {	// no empty sector?
				Int32 secEnd = secBegin+2;
				while (wara[secEnd%i].isCrease == 0) secEnd+=2;
				Int32 numface = (secEnd - secBegin) / 2;
				a = pmesh->point(v_h);
				// copy workarray
				WorkArrayType WA3;
				WorkType*     waA = &(WA3[0]);
				for (Int32 co = 0; co < i; co++)
					waA[co].p = wara[co].p;
				// get normal
				n = NormCrease(a,wara,i,secBegin,secEnd);
				Int32 l=0;			// start with depth 0
				Real32 test = getGreatestAngle(wara,a,n,secBegin,numface,i);
				while ((test < epsilon) && (l < wsdmaxdepth))	{					
					l++;			// next depth					
					simpleCreaseSubdiv(waA,waB,a,secBegin,secEnd,i);
					// switch workarrays:
					waT = waA; waA = waB; waB = waT;
					// get normal
					test = getGreatestAngle(waA,a,n,secBegin,numface,i);
				}
				// thats it for this sector
				SVertexData<VectorType, MeshType> *newelem;
            VertexListe.push_back(SVertexData<VectorType, MeshType>());
            newelem = &VertexListe.back();

				newelem->maxdepth = l;
				newelem->vh = v_h;
				a = pmesh->point(v_h);
				Int32 offset = getLimOffset(&patches[getFaceIndex(wara[(secBegin+1)%i].fh)],a)
					+ patches[getFaceIndex(wara[(secBegin+1)%i].fh)].varrayOffset;
		
				newelem->limPo.setValue(mySharedFields.limitpoints->getValue(offset));
				newelem->limNo.setValue(mySharedFields.limitnormals->getValue(offset));

            assert(VertexListe.size()>0);
	
				// connect adj. patches to vertex
				for (Int32 w=secBegin+1; w<secEnd; w+=2) {
					patches[getFaceIndex(wara[w%i].fh)].vertexindex[
						patches[getFaceIndex(wara[w%i].fh)].cvindex]=VertexListe.size()-1;
					patches[getFaceIndex(wara[w%i].fh)].cvindex++;
					assert(patches[getFaceIndex(wara[w%i].fh)].cvindex < 5);					

					if (patches[getFaceIndex(wara[w%i].fh)].maxdepth < l)
						patches[getFaceIndex(wara[w%i].fh)].maxdepth = l;
				}
				
			}
			secBegin+=2;
		}	// for all sectors
	}	
}

// CF moved to here

// helper for the vertexarray offset
template<class WSDVector, class Mesh, int mtype>
Int32 WSDmain<WSDVector, Mesh, mtype>::getLimOffset
(WSDdat<VectorType, MType> *pp, OMPoint a)
{
	Int32 offset = 0;
	VectorType t = pp->slateO[1][1];
	if (t[0] == a[0] && t[1] == a[1] && t[2] == a[2]) { 
		offset =0;
	}
	else {
		t = pp->slateO[1][2];
		if (t[0] == a[0] && t[1] == a[1] && t[2] == a[2]) { 
			offset = wsddepthindexarray[wsdmaxdepth]-1;
		} else {
			t = pp->slateO[2][1];
			if (t[0] == a[0] && t[1] == a[1] && t[2] == a[2]) { 
				offset = wsddepthindexarray[wsdmaxdepth]*wsddepthindexarray[wsdmaxdepth] - wsddepthindexarray[wsdmaxdepth];
			} else {
				offset = wsddepthindexarray[wsdmaxdepth]*wsddepthindexarray[wsdmaxdepth] - 1;
			}
		}
	}
	return offset;
}

// helper to find the greates angle
template<class WSDVector, class Mesh, int mtype>
Real32 WSDmain<WSDVector, Mesh, mtype>::getGreatestAngle
(WorkType* wara_, OMPoint &a, OMPoint &n, Int32 s, Int32 num, Int32 i)
{
	OMPoint fb1,fc,fb2, v1,v2,v3, fn;
	Real32 test=10.0;
	Int32 pp = s;
	for (Int32 fi=0; fi<num; fi++) {					// for all faces (#faces=val)	
		fb1 = wara_[(pp)%i].p;
		fc  = wara_[(pp+1)%i].p;
		fb2 = wara_[(pp+2)%i].p;
		
		v1 = fb1 - a;
		v2 = fc - a;
		v3 = fb2 - a;
		fn = (v1-v3) % v2;
		fn = fn.normalize();
		Real32 coswinkel = (fn | n);				
		if (coswinkel < test) test = coswinkel;		// minimum				
		pp+=2;
	}
	return test;
}


// collectNeighbors: for a given patch pp get the four neighbors
#if 0
template<>
void WSDmain<OSG::Vec4f, MyPolyMesh4, QUAD>::collectNeighbors
(FaceHandle f_h, WSDdat<VectorType, MType> *pp)
{		
	HalfedgeHandle fhe_h = pmesh->halfedge_handle(f_h);	
	for (Int32 i=0; i<4; i++)	{
		if (pmesh->is_boundary(pmesh->edge_handle(fhe_h)))	// bei Kanten auf sich selbst zeigen
			pp->neighbors[i] = getFaceIndex(f_h);
		else
			pp->neighbors[i] = 
			getFaceIndex(pmesh->face_handle(pmesh->opposite_halfedge_handle(fhe_h)));
		fhe_h = pmesh->next_halfedge_handle(fhe_h);		
	}
}
template<>
void WSDmain<OSG::Vec3f, MyPolyMesh, QUAD>::collectNeighbors
(FaceHandle f_h, WSDdat<VectorType, MType> *pp)
{		
	HalfedgeHandle fhe_h = pmesh->halfedge_handle(f_h);	
	for (Int32 i=0; i<4; i++)	{
		if (pmesh->is_boundary(pmesh->edge_handle(fhe_h)))	// bei Kanten auf sich selbst zeigen
			pp->neighbors[i] = getFaceIndex(f_h);
		else
			pp->neighbors[i] = 
			getFaceIndex(pmesh->face_handle(pmesh->opposite_halfedge_handle(fhe_h)));
		fhe_h = pmesh->next_halfedge_handle(fhe_h);		
	}
}
#endif
template<>
void WSDmain<OSG::Vec3f, MyTriMesh, TRIANGLE>::collectNeighbors
(FaceHandle f_h, WSDdat<VectorType, MType> *pp)
{		
   Int32 i=0;
	HalfedgeHandle fhe_h = pmesh->halfedge_handle(f_h);	
   OSG::Vec3f tempvertex(0,0,0);
   OMPoint p = pmesh->point(pmesh->from_vertex_handle(fhe_h));
   tempvertex.setValues(p[0], p[1], p[2]);
   while (tempvertex != pp->slateO[1][1] ) {
      fhe_h = pmesh->next_halfedge_handle(fhe_h);
      p = pmesh->point(pmesh->from_vertex_handle(fhe_h));
      tempvertex.setValues(p[0], p[1], p[2]);
      i++; assert(i<5); //dbg
   }
	for (i=0; i<4; i++)	{  
      pp->borders[i] = 0;
      if (pmesh->is_boundary(pmesh->edge_handle(fhe_h))) {	// bei Kanten auf sich selbst zeigen
			pp->neighbors[i] = getFaceIndex(f_h);
         pp->borders[i] = 1;
         //SLOG << "Kante" << std::endl;
      }
		else
			pp->neighbors[i] = 
			getFaceIndex(pmesh->face_handle(pmesh->opposite_halfedge_handle(fhe_h)));
		fhe_h = pmesh->next_halfedge_handle(fhe_h);		
      i++;
      pp->borders[i] = 0;
      if (pmesh->is_boundary(pmesh->edge_handle(fhe_h)))	{// bei Kanten auf sich selbst zeigen
			pp->neighbors[i] = getFaceIndex(f_h);
         pp->borders[i] = 1;
         //SLOG << "Kante" << std::endl;
      }
		else
			pp->neighbors[i] = 
			getFaceIndex(pmesh->face_handle(pmesh->opposite_halfedge_handle(fhe_h)));
		fhe_h = pmesh->next_halfedge_handle(
         pmesh->opposite_halfedge_handle(
         pmesh->next_halfedge_handle(
         fhe_h)));		
	}   
}
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::collectNeighbors
(FaceHandle f_h, WSDdat<VectorType, MType>* pp)
{		
	HalfedgeHandle fhe_h = pmesh->halfedge_handle(f_h);	
	for (Int32 i=0; i<4; i++)	{
		if (pmesh->is_boundary(pmesh->edge_handle(fhe_h)))	// bei Kanten auf sich selbst zeigen
			pp->neighbors[i] = getFaceIndex(f_h);
		else
			pp->neighbors[i] = 
			getFaceIndex(pmesh->face_handle(pmesh->opposite_halfedge_handle(fhe_h)));
		fhe_h = pmesh->next_halfedge_handle(fhe_h);		
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
template<class WSDVector, class Mesh, int mtype>
WSDmain<WSDVector, Mesh, mtype>::WSDmain(MeshType *m)
: pmesh(m), mesh2wsd(m), subdivwsd()
{	
	patchesready = false;	
	isSetViewPort = false;
   isSetBFCull = true;
   useProjSize = true;
   useSilhouette = true;
   useCurvature = true;
   useTexture = false;  
}

template<class WSDVector, class Mesh, int mtype>
WSDmain<WSDVector, Mesh, mtype>::~WSDmain()
{   
   delete[] PatchData::slateA; PatchData::slateA=NULL;
   delete[] PatchData::slateB; PatchData::slateB=NULL;
	clearInstances();
	patches.clear();
    subRefCP(mySharedFields.limitpointsptr);
    subRefCP(mySharedFields.limitnormalsptr);
    if (useTexture)
     subRefCP(mySharedFields.texcoordsptr);
}

//////////////////////////////////////////////////////////////////////
// public methods
//////////////////////////////////////////////////////////////////////

template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::setMaxDepth (Int32 setdepth)
{
   if (patchesready) 
      SLOG << "Setting depth after patch init is not possible at the moment!" << std::endl;
   else 
      wsdmaxdepth = setdepth;
}

// initOneGeo: create geometry node
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::initOneGeo (perInstanceData *instance)
{   	
   // maximum lengths and types size:
   Int32 maxsize_tl = 8+zweihoch[wsdmaxdepth];//(zweihoch[wsdmaxdepth]-2) + (4*zweihoch[wsdmaxdepth]);

   // prepare the geometry node as far as possible
   instance->onlyOneGeoP = Geometry::create();
		
   instance->oneTypesPtr =   GeoPTypesUI8::create();
   instance->oneLengthsPtr = GeoPLengthsUI32::create();
   instance->oneIndicesPtr = GeoIndicesUI32::create();

   instance->oneLengths = instance->oneLengthsPtr->getFieldPtr();
   instance->oneTypes   = instance->oneTypesPtr->getFieldPtr();
   instance->oneIndis   = instance->oneIndicesPtr->getFieldPtr();

   // allocate required memory:
   beginEditCP(instance->oneIndicesPtr);
   beginEditCP(instance->oneLengthsPtr);
   beginEditCP(instance->oneTypesPtr);
   instance->oneIndis->resize((wsddepthindexarray[wsdmaxdepth+1]*wsddepthindexarray[wsdmaxdepth+1]*patches.size())*2); //??
   instance->oneTypes->reserve(maxsize_tl * patches.size());
   instance->oneLengths->reserve(maxsize_tl * patches.size());
   endEditCP(instance->oneTypesPtr);
   endEditCP(instance->oneLengthsPtr);
   endEditCP(instance->oneIndicesPtr);

   beginEditCP(instance->onlyOneGeoP, Geometry::PositionsFieldMask |
	       Geometry::NormalsFieldMask | 
	       Geometry::TexCoordsFieldMask |
	       Geometry::IndicesFieldMask |
	       Geometry::TypesFieldMask |
	       Geometry::LengthsFieldMask |
	       Geometry::DlistCacheFieldMask);

   instance->onlyOneGeoP->setPositions(mySharedFields.limitpointsptr);
   instance->onlyOneGeoP->setNormals(mySharedFields.limitnormalsptr);		 		       
   if (useTexture) 
     instance->onlyOneGeoP->setTexCoords(mySharedFields.texcoordsptr);		 		
   instance->onlyOneGeoP->setIndices(instance->oneIndicesPtr);	
   instance->onlyOneGeoP->setTypes(instance->oneTypesPtr);
   instance->onlyOneGeoP->setLengths(instance->oneLengthsPtr);
   
   instance->onlyOneGeoP->setDlistCache(false);
   
   endEditCP(instance->onlyOneGeoP, Geometry::PositionsFieldMask |
	     Geometry::NormalsFieldMask | 
	     Geometry::TexCoordsFieldMask |
	     Geometry::IndicesFieldMask |
	     Geometry::TypesFieldMask |
	     Geometry::LengthsFieldMask |
	     Geometry::DlistCacheFieldMask);
		
   // set up node cores
   beginEditCP(instance->onlyOneNode);
   instance->onlyOneNode->setCore(instance->onlyOneGeoP);
   endEditCP(instance->onlyOneNode);	
}

// initOSGStuff sets up mySharedFields
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::initOSGStuff (Int32 fsize)
{
   Int32 wsdmaxvarray = wsddepthindexarray[wsdmaxdepth]*wsddepthindexarray[wsdmaxdepth];
   Int32 slatesize = (wsddepthindexarray[wsdmaxdepth]+2)*(wsddepthindexarray[wsdmaxdepth]+2);
   // shared data init (limitpoints -normals)
   mySharedFields.limitpointsptr  = GeoPositions3f::create();
   mySharedFields.limitnormalsptr = GeoNormals3f::create();   
   // shared usage for all geometry nodes with same basis mesh
   addRefCP(mySharedFields.limitpointsptr);
   addRefCP(mySharedFields.limitnormalsptr);
   mySharedFields.limitpoints  = mySharedFields.limitpointsptr->getFieldPtr();
   mySharedFields.limitnormals = mySharedFields.limitnormalsptr->getFieldPtr();    
 
   assert(wsdmaxvarray > 0);

   SLOG << "wsdmaxvarray=" << wsdmaxvarray << std::endl;
   SLOG << "fsize=" << fsize << std::endl;

   beginEditCP(mySharedFields.limitpointsptr);
   beginEditCP(mySharedFields.limitnormalsptr);
   mySharedFields.limitpoints->resize((wsdmaxvarray*fsize));
   mySharedFields.limitnormals->resize((wsdmaxvarray*fsize));   
   endEditCP(mySharedFields.limitnormalsptr);
   endEditCP(mySharedFields.limitpointsptr);

   // texture coords are shared too
   if (useTexture) {
      mySharedFields.texcoordsptr = GeoTexCoords2f::create(); 
      addRefCP(mySharedFields.texcoordsptr);
      mySharedFields.texcoords = mySharedFields.texcoordsptr->getFieldPtr();

      beginEditCP(mySharedFields.texcoordsptr);
      mySharedFields.texcoords->resize((wsdmaxvarray*fsize));
      endEditCP(mySharedFields.texcoordsptr);
   }

   // get memory for slates
   PatchData::slateA = new VectorType[slatesize];
   PatchData::slateB = new VectorType[slatesize];
}


// initPatches: mtype = TRIANGLE
template<>
void WSDmain<OSG::Vec3f, MyTriMesh, TRIANGLE>::initPatches ()
{
   Int32 i=0;
   Int32 wsdmaxvarray = wsddepthindexarray[wsdmaxdepth]*wsddepthindexarray[wsdmaxdepth];
   subdivwsd.wsdmaxdepth = this->wsdmaxdepth;
   // check for texture coords in pmesh
   useTexture = pmesh->has_vertex_texcoords();     
   subdivwsd.useTexture = this->useTexture;
   mesh2wsd.useTexture = this->useTexture;

   MeshType::FaceIter f_it;
	for (f_it=pmesh->faces_begin(); f_it!=pmesh->faces_end(); ++f_it) 
		i++;
   if (i==0) {
      SLOG << "no faces at all!" << std::endl;
      patchesready=false;
      return;
   }
	OmeshFaceListe[0] = new FaceHandle[i];		// hash table for neighborhood collection
   OmeshFaceListe[1] = new FaceHandle[i];		// no more than i elements are needed

   // step one: pairing list
   WSDpairing<OSG::Vec3f, MyTriMesh> pairinglist(pmesh);
   pairinglist.buildPairs();
   
   PatchData *temppatch;	
	i=pairinglist.pairs.size();
   assert(i>0);
   
   initOSGStuff(i);  
   patches.reserve(i);
   i=0;
	// face iteration for collecting patch-data
   WSDpairing<OSG::Vec3f, MyTriMesh>::PairFaceDataContainer::iterator p_iter;
	for (p_iter = pairinglist.pairs.begin(); p_iter!=pairinglist.pairs.end(); ++p_iter) {
      patches.push_back(PatchData());
		temppatch = &(patches.back());
		temppatch->mySharedFields = &mySharedFields;
      temppatch->wsdmaxdepth = this->wsdmaxdepth;
		mesh2wsd.insertface(p_iter->fh, p_iter->fh2, temppatch);
		temppatch->varrayOffset=(i*wsdmaxvarray);      
      temppatch->neighbors[0] = 0;
      temppatch->neighbors[1] = 0;
      temppatch->neighbors[2] = 0;
      temppatch->neighbors[3] = 0;
      temppatch->isSingleTriangle=(p_iter->partner < 1);
      if (temppatch->isSingleTriangle) SLOG << "Single found" << std::endl;
      temppatch->cvindex = 0;
      for (int vindex=0; vindex<4; vindex++)
         temppatch->vertexindex[vindex]=-2;

      OmeshFaceListe[0][i] = p_iter->fh;
      if (p_iter->partner > 0)
      OmeshFaceListe[1][i] = p_iter->fh2;
      else
      OmeshFaceListe[1][i] = p_iter->fh;     // single triangle

      if (wsdmaxdepth > 0) {
         temppatch->solltiefe = 1;
		   subdivwsd.subdiv(1,temppatch);			// subdiv once for corner-limpos				
      }
      else {
         temppatch->solltiefe = 0;
         subdivwsd.subdiv(0,temppatch);			// subdiv once for corner-limpos				
      }

		i++;      
	}	
	patchesready=true;	
	SLOG << i << " patches initialized.\n";
   
   // collect vertex-depthinfo
	// iteration over vertices
   VertexListe.reserve(pmesh->n_vertices());
	for (MeshType::VertexIter v_it=pmesh->vertices_begin(); v_it!=pmesh->vertices_end(); ++v_it) {
		getOptiDepth(pmesh, v_it.handle());
	}

   // now collect neighbors
   i=0;
	for (p_iter = pairinglist.pairs.begin(); p_iter!=pairinglist.pairs.end(); ++p_iter) {      
		collectNeighbors(p_iter->fh, &patches[i]);
      if (patches[i].isSingleTriangle) {
       patches[i].neighbors[2] = getFaceIndex(p_iter->fh2);
       patches[i].neighbors[3] = getFaceIndex(p_iter->fh2);
      }
		i++;
	}	
   delete[] OmeshFaceListe[0];	// hash table has done its job   
   delete[] OmeshFaceListe[1];	  

   // now for the neighbor-neighbors
	for (i=0; i<patches.size(); i++) {
      //if (!patches[i].isSingleTriangle) {
		patches[i].neighbor1in0 = patches[patches[i].neighbors[0]].getneighbor(i,-1);
		patches[i].neighbor3in0 = patches[patches[i].neighbors[0]].getneighbor(i,1);
		patches[i].neighbor1in2 = patches[patches[i].neighbors[2]].getneighbor(i,1);
		patches[i].neighbor3in2 = patches[patches[i].neighbors[2]].getneighbor(i,-1);
		patches[i].neighbor0in1 = patches[patches[i].neighbors[1]].getneighbor(i,1);
		patches[i].neighbor2in1 = patches[patches[i].neighbors[1]].getneighbor(i,-1);
		patches[i].neighbor0in3 = patches[patches[i].neighbors[3]].getneighbor(i,-1);
		patches[i].neighbor2in3 = patches[patches[i].neighbors[3]].getneighbor(i,1);
      //}
	}
}

template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::initPatches ()
{  
	PatchData *temppatch;	
	Int32 i=0;
   Int32 wsdmaxvarray = wsddepthindexarray[wsdmaxdepth]*wsddepthindexarray[wsdmaxdepth];
   subdivwsd.wsdmaxdepth = this->wsdmaxdepth;
   // check for texture coords in pmesh
   useTexture = pmesh->has_vertex_texcoords();     
   subdivwsd.useTexture = this->useTexture;
   mesh2wsd.useTexture = this->useTexture;

	// face iteration: how many faces do we have?
	typename MeshType::FaceIter f_it;
	for (f_it=pmesh->faces_begin(); f_it!=pmesh->faces_end(); ++f_it) 
		i++;
   if (i==0) {
      SLOG << "no faces at all!" << std::endl;
      patchesready=false;
      return;
   }
	OmeshFaceListe[0] = new FaceHandle[i];		// hash table for neighborhood collection
	initOSGStuff(i);
	patches.reserve(i);
	i=0;
	// face iteration for collecting patch-data
	for (f_it=pmesh->faces_begin(); f_it!=pmesh->faces_end(); ++f_it) {
      if (!isQuad(f_it.handle())) {
         SLOG << "Face of degree <> 4 detected!" << std::endl;
       	patchesready=false;
         return;
      }
		patches.push_back(PatchData());			// patchdata in vector
		temppatch = &(patches.back());				// create new element
		temppatch->mySharedFields = &mySharedFields;
      temppatch->wsdmaxdepth = this->wsdmaxdepth;
		mesh2wsd.insertface(f_it.handle(),f_it.handle(), temppatch);
		temppatch->varrayOffset=(i*wsdmaxvarray);      
      temppatch->isSingleTriangle = false;

      if (wsdmaxdepth > 0) {
         temppatch->solltiefe = 1;
		   subdivwsd.subdiv(1,temppatch);			// subdiv once for corner-limpos		
      }
      else {
         temppatch->solltiefe = 0;
         subdivwsd.subdiv(0,temppatch);			// subdiv once for corner-limpos		
      }

		OmeshFaceListe[0][i] = f_it.handle();		// FaceHandle in hash table      
		i++;
	}
	
	patchesready=true;	
	SLOG << i << " patches initialized.\n";

	// collect vertex-depthinfo
	// iteration over vertices
   VertexListe.reserve(pmesh->n_vertices());
	for (typename MeshType::VertexIter v_it=pmesh->vertices_begin(); 
	     v_it != pmesh->vertices_end(); 
	     ++v_it) {
		getOptiDepth(pmesh, v_it.handle());     
	}

	// collect neighborhood ( hash table is needed here )
	i=0;
	for (f_it=pmesh->faces_begin(); f_it!=pmesh->faces_end(); ++f_it) {		
		collectNeighbors(f_it.handle(), &patches[i]);
		i++;
	}	
	delete[] OmeshFaceListe[0];	// hash table has done its job
	// now for the neighbor-neighbors
	for (i=0; i<patches.size(); i++) {
		patches[i].neighbor1in0 = patches[patches[i].neighbors[0]].getneighbor(i,-1);
		patches[i].neighbor3in0 = patches[patches[i].neighbors[0]].getneighbor(i,1);
		patches[i].neighbor1in2 = patches[patches[i].neighbors[2]].getneighbor(i,1);
		patches[i].neighbor3in2 = patches[patches[i].neighbors[2]].getneighbor(i,-1);
		patches[i].neighbor0in1 = patches[patches[i].neighbors[1]].getneighbor(i,1);
		patches[i].neighbor2in1 = patches[patches[i].neighbors[1]].getneighbor(i,-1);
		patches[i].neighbor0in3 = patches[patches[i].neighbors[3]].getneighbor(i,-1);
		patches[i].neighbor2in3 = patches[patches[i].neighbors[3]].getneighbor(i,1);
	}
}


// initInstance: called per Instance, creates geometry node for every instance
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::initInstance (Int32 n, NodePtr& parent)
{
   // create new instance-element
   myInstances.push_back(perInstanceData());
   perInstanceData* newInst = &(myInstances.back());
   // save parent for later subChild
   // CF not necessary
   newInst->parentNode = parent;
   // create node
   newInst->onlyOneNode = Node::create();
   initOneGeo(newInst);
   // add child to parent-node
   beginEditCP(parent);
   parent->addChild(newInst->onlyOneNode);
   endEditCP(parent);
}

// perFrameSetup: is called per frame per instance and does the
// depth assignment and preparation for the geometry node
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::perFrameSetup 
(OSG::NodePtr& parent, OSG::Vec3f eyepoint)
{  
   if (!patchesready) {
      SLOG << "patches not initialized!" << std::endl; 
      return;
   }
   Int32 n = getIndex(parent);   
      
   VectorType eyep(eyepoint[0], eyepoint[1], eyepoint[2], 1.0);	
   Int32 i;	
   
   // check position of every vertex ( silhuette, back faces... )
   for (i=0; i<VertexListe.size(); i++) {
      VectorType lp;
      Real32 cw;
      lp = eyep - VertexListe[i].limPo;
      lp.normalize();
      cw = VertexListe[i].limNo.dot(lp);
      if (cw+VertexClassifier < 0)
	      VertexListe[i].ft = BACK;
      else if (cw-VertexClassifier > 0)
	      VertexListe[i].ft = FRONT;
      else
	      VertexListe[i].ft = SILUETTE;
   }

   // find the depth
   for (i=0; i<patches.size(); i++) {

      if (patches[i].isSingleTriangle) {
         patches[i].solltiefe = patches[patches[i].neighbors[2]].solltiefe;
         //SLOG << "single" << std::endl;            
      }  else {

	patches[i].solltiefe = patches[i].maxdepth;	      
      
	      Int32 ist_siluette = 0;
	      for (Int32 si=0; si<4; si++) {
	        assert(patches[i].vertexindex[si]>=0);
	        ist_siluette+=VertexListe[patches[i].vertexindex[si]].ft;
	      }
	      if (ist_siluette == 0) {
	        if (isSetBFCull)
	          patches[i].isFacing = BACK;				// backface;		
	        else
	          patches[i].isFacing = FRONT;				// backface culling is deactivated;		
	      }
	      else			
	        if (ist_siluette == 4)
	          patches[i].isFacing = FRONT;			// all faces are front faces
	        else {
	          patches[i].isFacing = SILUETTE;				
	        }
         Int32 l=patches[i].solltiefe;
	      // now for the projected patch size:
	      if ((patches[i].isFacing != BACK) && (useProjSize)) {
	        OSG::Vec3f SP; SP.setValues(patches[i].faceloc[0],
				            patches[i].faceloc[1], patches[i].faceloc[2]);
	        Real32 d = (eyepoint - SP).length();
	        
	        Int32 a = patches[i].maxdepth;

	        Real32 Rmin;
	        if (patches[i].isFacing == SILUETTE) {
	          l = (a + wsdmaxdepth) / 2;
	          Rmin = d*hminSil;
	        }
	        else {
	          l = a;
	          Rmin = d*hmin;
	        }
	        Real32 Rl = patches[i].radiusD;
	        if (Rl > 0) {
	          while (a>0) { Rl = Rl * 0.5; a--; }						
	          
	          if (Rl < Rmin) {
	            do {
		      l--;
		      Rl = Rl *2;
	            } while (Rl < Rmin);
	          }
	          else {
	            Real32 Rmax = d*hmax;
	            if (Rl > Rmax) {
		      do {
		        l++;
		        Rl = Rl * 0.5;
		      } while (Rl > Rmax);
	            }				   
	          }
	        } else {
	          SLOG << "ndoo! radius is corrupt!" << std::endl;
	        }
	        
	        if (l < 0) l = 0;
	        if (l >wsdmaxdepth) l = wsdmaxdepth;
	        // l is the optimal depth 
	        patches[i].solltiefe=l;	         
	        // transfer to single triangle
	        if (patches[patches[i].neighbors[0]].isSingleTriangle)
                  patches[patches[i].neighbors[0]].solltiefe=l;                  
	      }	         
      } //not single      
   }  // for

   // beginEditCP for all patches (end after the for-loop)
   beginEditCP(myInstances[n].oneTypesPtr);
   beginEditCP(myInstances[n].oneLengthsPtr);
   beginEditCP(myInstances[n].oneIndicesPtr);
   
   // delete types, lengths
   myInstances[n].oneTypes->clear();
   myInstances[n].oneLengths->clear();
   
   UInt32 indisIn=0;


   //debug
   //bool ongoing,allfaces;
   // iteration over patches
   for (i=0; i<patches.size(); i++)    {
      
     if (patches[i].isFacing != BACK) {							
       
       Int32 neue_tiefe = patches[i].solltiefe;
       //patches[i].solltiefe = 3;
       //Int32 neue_tiefe = 3;
       
       // patch depth has to be as high as the highest neighbour
       for (Int32 neibr=0; neibr<patches[i].cvindex; neibr++)
	      if (patches[patches[i].neighbors[neibr]].solltiefe > neue_tiefe) 
            neue_tiefe=patches[patches[i].neighbors[neibr]].solltiefe;
       
       // do subdivision when the depth has been raised
       if (neue_tiefe > patches[i].currentdepth) {
	       beginEditCP(myInstances[n].onlyOneGeoP->getPositions());    
	       beginEditCP(myInstances[n].onlyOneGeoP->getNormals());  
          beginEditCP(myInstances[n].onlyOneGeoP->getColors());
          if (useTexture) 
	         beginEditCP(myInstances[n].onlyOneGeoP->getTexCoords());
          subdivwsd.subdiv(neue_tiefe,&patches[i]);               
          if (useTexture) 
            endEditCP(myInstances[n].onlyOneGeoP->getTexCoords());
          endEditCP(myInstances[n].onlyOneGeoP->getColors());
	       endEditCP(myInstances[n].onlyOneGeoP->getNormals());    
	       endEditCP(myInstances[n].onlyOneGeoP->getPositions());
       }
       
       // prepare everything
       Int32 finer_l=0, finer_r=0;
       Int32 finer_o=0, finer_u=0;
       
       if (patches[patches[i].neighbors[0]].solltiefe > patches[i].solltiefe) {
	 finer_l = 1;}
       if (patches[patches[i].neighbors[2]].solltiefe > patches[i].solltiefe) {
	 finer_r = 1;}
       
       if (patches[patches[i].neighbors[1]].solltiefe > patches[i].solltiefe) {
	 finer_u = 1;}
       if (patches[patches[i].neighbors[3]].solltiefe > patches[i].solltiefe) {
	 finer_o = 1;}
       
       // calculate the required steps for gap filling
       patches[i].interstep_l = zweihoch[(wsdmaxdepth - patches[patches[i].neighbors[0]].solltiefe)];
       patches[i].interstep_u = zweihoch[(wsdmaxdepth - patches[patches[i].neighbors[1]].solltiefe)];
       patches[i].interstep_r = zweihoch[(wsdmaxdepth - patches[patches[i].neighbors[2]].solltiefe)];
       patches[i].interstep_o = zweihoch[(wsdmaxdepth - patches[patches[i].neighbors[3]].solltiefe)];            
       
       if (patches[i].solltiefe > 0) {
	 // repair-pices in corners
	 patches[i].c0_l = 
	   ((patches[i].solltiefe == patches[patches[i].neighbors[0]].solltiefe)
	    && (patches[patches[i].neighbor3in0].solltiefe 
		- patches[patches[i].neighbors[0]].solltiefe > 0));
	 patches[i].c0_o =
	   ((patches[i].solltiefe == patches[patches[i].neighbors[3]].solltiefe)
	    && (patches[patches[i].neighbor0in3].solltiefe 
		- patches[patches[i].neighbors[3]].solltiefe > 0));
	 patches[i].c1_l =
	   ((patches[i].solltiefe == patches[patches[i].neighbors[0]].solltiefe)
	    && (patches[patches[i].neighbor1in0].solltiefe 
		- patches[patches[i].neighbors[0]].solltiefe > 0));
	 patches[i].c1_u =
	   ((patches[i].solltiefe == patches[patches[i].neighbors[1]].solltiefe)
	    && (patches[patches[i].neighbor0in1].solltiefe 
		- patches[patches[i].neighbors[1]].solltiefe > 0));
	 patches[i].c2_r =
	   ((patches[i].solltiefe == patches[patches[i].neighbors[2]].solltiefe)
	    && (patches[patches[i].neighbor1in2].solltiefe 
		- patches[patches[i].neighbors[2]].solltiefe > 0));
	 patches[i].c2_u =
	   ((patches[i].solltiefe == patches[patches[i].neighbors[1]].solltiefe)
	    && (patches[patches[i].neighbor2in1].solltiefe 
		- patches[patches[i].neighbors[1]].solltiefe > 0));
	 patches[i].c3_r =
	   ((patches[i].solltiefe == patches[patches[i].neighbors[2]].solltiefe)
	    && (patches[patches[i].neighbor3in2].solltiefe 
		- patches[patches[i].neighbors[2]].solltiefe > 0));
	 patches[i].c3_o = 
	   ((patches[i].solltiefe == patches[patches[i].neighbors[3]].solltiefe)
	    && (patches[patches[i].neighbor2in3].solltiefe 
		- patches[patches[i].neighbors[3]].solltiefe > 0));
       }
       
       // update the patch edges
       patches[i].setOuterGeoP(myInstances[n], finer_l, finer_r, finer_o, finer_u, indisIn);		
       if (patches[i].solltiefe > 1) {
	  patches[i].setInnerGeoP(myInstances[n], indisIn);
       }
     }					
   }	
   endEditCP(myInstances[n].oneIndicesPtr);
   endEditCP(myInstances[n].oneLengthsPtr);
   endEditCP(myInstances[n].oneTypesPtr);	
}


// viewport dependant inits
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::initViewPortVars
(Real32 fovy, Int32 resX, Int32 resY)
{	
	PminSil = 0.75 * Pmin;	               // take 3/4 of Pmin for the shiluette
	gamma_winkel = (fovy*2)/resY;
	hmin    = tan((gamma_winkel/2)*Pmin);
	hminSil = tan((gamma_winkel/2)*PminSil);
	hmax    = tan((gamma_winkel/2)*Pmax);	
	isSetViewPort=true;
}

// bounding box calculation
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::OpenMeshBoundingBox
(OSG::Pnt3f &boundingMin, OSG::Pnt3f &boundingMax)
{
	mesh2wsd.getBoundingBox(boundingMin, boundingMax);
}

// clearInstances: savely delete all instances
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::clearInstances (void)
{	
	if (myInstances.size() > 0) {
		perInstanceDataContainer::iterator theIterator;
		for (theIterator = myInstances.begin(); theIterator != myInstances.end();
			 theIterator++) {
 			beginEditCP(theIterator->parentNode);
			 theIterator->parentNode->subChild(theIterator->onlyOneNode);
			endEditCP(theIterator->parentNode);                                            
		}
		myInstances.clear();		
	}	
}

// helper to find the index in myInstances for parent p
template<class WSDVector, class Mesh, int mtype>
Int32 WSDmain<WSDVector, Mesh, mtype>::getIndex (OSG::NodePtr &p)
{
   Int32 ret = 0;
   perInstanceDataContainer::iterator i = myInstances.begin();
   while (i != myInstances.end()) {
      if (i->parentNode == p) return ret;
      i++;
      ret++;
   }
   return 0;
}

// helper to find out if f_h is a quad (returns true then)
template<class WSDVector, class Mesh, int mtype>
bool WSDmain<WSDVector, Mesh, mtype>::isQuad (FaceHandle f_h)
{
   HalfedgeHandle hh = pmesh->halfedge_handle(f_h);
   HalfedgeHandle hend = hh;
   hh = pmesh->next_halfedge_handle(hh);
   Int32 degree=1;
   while (hh != hend) {
      hh = pmesh->next_halfedge_handle(hh);
      degree++;
   }
   if (degree == 4) return true;
   return false;
}


// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------

