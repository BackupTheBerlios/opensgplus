/*---------------------------------------------------------------------------*\
 *                       OpenSG - Dynamic Subdivision                        *
 *                                                                           *
 *                                                                           *
 *					  contact: v.settgast@cg.cs.tu-bs.de * 
 *                                                                           *
\*---------------------------------------------------------------------------*/

#ifdef WIN32
#pragma warning (disable : 698 47 373 383 171)
#endif

#include "OSGwsdmain.h"
OSG_USING_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
template<class WSDVector, class Mesh, int mtype>
WSDmain<WSDVector, Mesh, mtype>::WSDmain(Mesh *m)
: pmesh(m), mesh2wsd(m), subdivwsd()
{	
	patchesready = false;	
	isSetViewPort = false;
	silEpsi = 0.01;
	Pmin    = 5;	
	Pmax    = 15;
	myInstances.clear();   
}

template<class WSDVector, class Mesh, int mtype>
WSDmain<WSDVector, Mesh, mtype>::~WSDmain()
{
	clearInstances();
	patches.clear();
    subRefCP(mySharedFields.limitpointsptr);
    subRefCP(mySharedFields.limitnormalsptr);
}

//////////////////////////////////////////////////////////////////////
// public methods
//////////////////////////////////////////////////////////////////////


// initOneGeo: create geometry node
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::initOneGeo(perInstanceData *instance)
{   	
   // maximum lengths and types size:
   Int32 maxsize_tl = (zweihoch[wsdmaxdepth]-2) + (4*zweihoch[wsdmaxdepth]);
	// prepare the geometry node as far as possible
	instance->onlyOneGeoP = Geometry::create();
		
	instance->oneTypesPtr =   GeoPTypesUI8::create();
	instance->oneLengthsPtr = GeoPLengthsUI32::create();
	instance->oneIndicesPtr = GeoIndicesUI32::create();

	instance->oneLengths = 
		instance->oneLengthsPtr->getFieldPtr();
	instance->oneTypes = 
		instance->oneTypesPtr->getFieldPtr();
	instance->oneIndis = 
		instance->oneIndicesPtr->getFieldPtr();
	// allocate required memory:
	instance->oneIndis->resize((wsdmaxvarray*patches.size())*2);
   instance->oneTypes->reserve(maxsize_tl * patches.size());
   instance->oneLengths->reserve(maxsize_tl * patches.size());
	
	beginEditCP(instance->onlyOneGeoP, Geometry::PositionsFieldMask |
                           Geometry::NormalsFieldMask |               
						   Geometry::IndicesFieldMask |
						   Geometry::TypesFieldMask |
						   Geometry::LengthsFieldMask |
						   Geometry::DlistCacheFieldMask);

		instance->onlyOneGeoP->setPositions(mySharedFields.limitpointsptr);
		instance->onlyOneGeoP->setNormals(mySharedFields.limitnormalsptr);		 		
		instance->onlyOneGeoP->setIndices(instance->oneIndicesPtr);	
		instance->onlyOneGeoP->setTypes(instance->oneTypesPtr);
		instance->onlyOneGeoP->setLengths(instance->oneLengthsPtr);
		
		instance->onlyOneGeoP->setDlistCache(false);

	endEditCP(instance->onlyOneGeoP, Geometry::PositionsFieldMask |
                         Geometry::NormalsFieldMask |        
						 Geometry::IndicesFieldMask |
						 Geometry::TypesFieldMask |
						 Geometry::LengthsFieldMask |
						 Geometry::DlistCacheFieldMask);
		
	// set up node cores
	beginEditCP(instance->onlyOneNode);
	 instance->onlyOneNode->setCore(instance->onlyOneGeoP);
	endEditCP(instance->onlyOneNode);	
}

// initPatches: initialize data and do preprocess
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::initPatches()
{
	PatchData *temppatch;	
	Int32 i=0;
	// face iteration: how many faces do we have?
	typename Mesh::FaceIter f_it;
	for (f_it=pmesh->faces_begin(); f_it!=pmesh->faces_end(); ++f_it) 
		i++;
	OmeshFaceListe = new FaceHandle[i];		// hash table for neighborhood collection

	// shared data init (limitpoints -normals)
	mySharedFields.limitpointsptr  = GeoPositions3f::create();
	mySharedFields.limitnormalsptr = GeoNormals3f::create(); 
	// shared usage for all geometry nodes with same basis mesh
	addRefCP(mySharedFields.limitpointsptr);
   addRefCP(mySharedFields.limitnormalsptr);

	mySharedFields.limitpoints = mySharedFields.limitpointsptr->getFieldPtr();
   mySharedFields.limitnormals = mySharedFields.limitnormalsptr->getFieldPtr();
	mySharedFields.limitpoints->resize((wsdmaxvarray*i));
   mySharedFields.limitnormals->resize((wsdmaxvarray*i));
	
	i=0;
	// face iteration for collecting patch-data
	for (f_it=pmesh->faces_begin(); f_it!=pmesh->faces_end(); ++f_it) {
		temppatch = new PatchData;				// create new element
		temppatch->mySharedFields = &mySharedFields;
		mesh2wsd.insertface(f_it.handle(),temppatch);
		temppatch->varrayOffset=(i*wsdmaxvarray);
      temppatch->solltiefe = 1;

		subdivwsd.subdiv(1,temppatch);			// subdiv once for corner-limpos		
		OmeshFaceListe[i] = f_it.handle();		// FaceHandle in hash table
		i++;
		patches.push_back(*temppatch);			// patchdata in vector
	}
	
	patchesready=true;	
	//SLOG << i << " patches initialized.\n";

	// collect vertex-depthinfo
	// iteration over vertices
	for (typename Mesh::VertexIter v_it=pmesh->vertices_begin(); v_it!=pmesh->vertices_end(); ++v_it) {
		getOptiDepth(pmesh, v_it.handle());
	}

	// collect neighborhood ( hash table is needed here )
	i=0;
	for (f_it=pmesh->faces_begin(); f_it!=pmesh->faces_end(); ++f_it) {		
		collectNeighbors(f_it.handle(), &patches[i]);
		i++;
	}	
	delete[] OmeshFaceListe;	// hash table has done its job
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
void WSDmain<WSDVector, Mesh, mtype>::initInstance(Int32 n, NodePtr& parent)
{
	// create new instance-element
	perInstanceData *newInst = new perInstanceData;	
	// save parent for later subChild
	newInst->parentNode = parent;
	// create node
	newInst->onlyOneNode = Node::create();
	initOneGeo(newInst);
	// add child to parent-node
	beginEditCP(parent);
	 parent->addChild(newInst->onlyOneNode);
	endEditCP(parent);
	// push back element
	myInstances.push_back(*newInst);
}

// perFrameSetup: is called per frame per instance and does the
// depth assignment and preparation for the geometry node
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::perFrameSetup(OSG::NodePtr& parent, OSG::Vec3f eyepoint)
{   
	if (!patchesready) {SLOG << "patches not initialized!\n"; return;}
   Int32 n = getIndex(parent);   
      
	WSDVector eyep(eyepoint[0], eyepoint[1], eyepoint[2], 1.0);	
	Int32 i;	
	// check position of every vertex ( silhuette, back faces... )
	for (i=0; i<VertexListe.size(); i++)
	{
		WSDVector lp;
		Real32 cw;
		lp = eyep - VertexListe[i].limPo;
		lp.normalize();
		cw = VertexListe[i].limNo.dot(lp);
		if (cw+silEpsi < 0)
			VertexListe[i].ft = BACK;
		else if (cw-silEpsi > 0)
			VertexListe[i].ft = FRONT;
		else
			VertexListe[i].ft = SILUETTE;
	}
	for (i=0; i<patches.size(); i++) {

		patches[i].solltiefe=patches[i].maxdepth;		

		Int32 ist_siluette = 0;
		for (Int32 si=0; si<4; si++)
			ist_siluette+=VertexListe[patches[i].vertexindex[si]].ft;
		if (ist_siluette == 0) {
			patches[i].isFacing = BACK;				// backface;		
		}
		else			
		if (ist_siluette == 4)
			patches[i].isFacing = FRONT;			// all faces are front faces
		else {
			patches[i].isFacing = SILUETTE;				
		}
		// now for the projected patch size:
		if (patches[i].isFacing != BACK) {
			OSG::Vec3f SP; SP.setValues(patches[i].faceloc[0],
							patches[i].faceloc[1], patches[i].faceloc[2]);
			Real32 d = (eyepoint - SP).length();

			Int32 a = patches[i].maxdepth;			
			Int32 l;
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
			if (l < 0) l = 0;
			if (l >wsdmaxdepth-1) l = wsdmaxdepth-1;
			// l is the optimal depth 
			patches[i].solltiefe=l;	         
		}		      
	}

	// beginEditCP for all patches (end after the for-loop)
	beginEditCP(myInstances[n].oneTypesPtr);
	beginEditCP(myInstances[n].oneLengthsPtr);
	beginEditCP(myInstances[n].oneIndicesPtr);

	// delete types, lengths
	myInstances[n].oneTypes->clear();
	myInstances[n].oneLengths->clear();

	UInt32 indisIn=0;

	// iteration over patches
	for (i=0; i<patches.size(); i++) {	
			// only front faces
			if (patches[i].isFacing != BACK) {							
				Int32 neue_tiefe = patches[i].solltiefe;

				// patch depth has to be as high as the highest neighbour
				for (Int32 neibr=0; neibr<4; neibr++)
					if (patches[patches[i].neighbors[neibr]].solltiefe > neue_tiefe) neue_tiefe=patches[patches[i].neighbors[neibr]].solltiefe;

				// do subdivision when the depth has been raised
            if (OSG::osgMax(neue_tiefe,1) > patches[i].currentdepth) {
					beginEditCP(myInstances[n].onlyOneGeoP->getPositions());    
					beginEditCP(myInstances[n].onlyOneGeoP->getNormals());    
					 subdivwsd.subdiv(neue_tiefe,&patches[i]);
					endEditCP(myInstances[n].onlyOneGeoP->getNormals());    
					endEditCP(myInstances[n].onlyOneGeoP->getPositions());
				}

				// prepare everything
				Int32 finer_l=0, finer_r=0;
				if (patches[patches[i].neighbors[0]].solltiefe > patches[i].solltiefe) {
					finer_l = 1;}
				if (patches[patches[i].neighbors[2]].solltiefe > patches[i].solltiefe) {
					finer_r = 1;}
				Int32 finer_o=0, finer_u=0;
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
				patches[i].setOuterGeoP(myInstances[n], finer_l, finer_r, 
										  finer_o, finer_u, indisIn);
				
				if (patches[i].solltiefe > 1)
					patches[i].setInnerGeoP(myInstances[n], indisIn);				

			}						

	}	

   endEditCP(myInstances[n].oneIndicesPtr);
   endEditCP(myInstances[n].oneLengthsPtr);
   endEditCP(myInstances[n].oneTypesPtr);					   
}


// viewport dependant inits
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>
::initViewPortVars(Real32 fovy, Int32 resX, Int32 resY)
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
void WSDmain<WSDVector, Mesh, mtype>
::OpenMeshBoundingBox(OSG::Pnt3f &boundingMin, OSG::Pnt3f &boundingMax)
{
	mesh2wsd.getBoundingBox(boundingMin, boundingMax);
}

// clearInstances: savely delete all instances
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>
::clearInstances(void)
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


// ----------------------------------------------------------------------------------------
// --- curvature calculation --------------------------------------------------------------
// ----------------------------------------------------------------------------------------

// getOptiDepth: for a given vertexhandle approximate the curvature
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::
getOptiDepth(Mesh *pmesh, VertexHandle v_h)
{	
	Real32 epsilon=1.0 - wsdkruemmungseps;
	// build workarray
	tWorkArray WA1, WA2;	
	tWorkArray *wara, *waB, *waT;
	wara = &WA1;
	waB  = &WA2;
	HalfedgeHandle hh = pmesh->halfedge_handle(v_h);
	HalfedgeHandle hhold = hh;
	OMPoint a,n;
	Int32 i=0;
	Int32 sharp=0;
	// manual iteration over all outgoing edges 		
	do {
		(*wara)[i].p = pmesh->point(pmesh->to_vertex_handle(hh));
		(*wara)[i].isCrease = pmesh->edge(pmesh->edge_handle(hh)).isCrease;
		if (pmesh->edge(pmesh->edge_handle(hh)).isCrease == 1) sharp++;
		i++;
		(*wara)[i].p = pmesh->point(pmesh->to_vertex_handle(pmesh->next_halfedge_handle(hh)));
		// mark edge-gamma with 8
		if (pmesh->is_boundary(hh)) (*wara)[i].isCrease = 8;
		else (*wara)[i].isCrease = 4;
		(*wara)[i].fh = pmesh->face_handle(hh);
		i++;
		hh = pmesh->prev_halfedge_handle(hh);
		hh = pmesh->opposite_halfedge_handle(hh);
		assert(pmesh->from_vertex_handle(hh) == v_h);
		 
	} while(hh != hhold);
	//now i is the size of the array and sharp is the number of creases
	(*wara)[i].isCrease = sharp;	
  
	// calc limitnormal
	// if *(*wara)[val2].isCrease < 2 then it is a inner point
	if ((*wara)[i].isCrease < 2) {			
		a = pmesh->point(v_h);
		n = NormCCinner(wara,i);				
		Int32 val = (i/2);
		
		// do subdiv until epsilon is reached
		Int32 l=0;				// start with depth=0
		Real32 test=100;			// in test the min angle is stored
		
		// average normal for all faces
		test = getGreatestAngle(wara,a,n,0,val,i);				
		while ((test < epsilon) && (l < wsdmaxdepth-1))	{						
			l++;								// next depth
			simpleSubdiv(wara,waB,a,i);			// subdiv
			// switch workarrays:
			waT = &(*wara); wara = &(*waB);	waB = &(*waT);		
			// average normal to all adjacent faces
			test = getGreatestAngle(wara,a,n,0,val,i);					
		} 		
		// thats it!
		// now put the new vertex in the vector and connect adj. patches to it
		SVertexData<WSDVector, Mesh> *newelem = new SVertexData<WSDVector, Mesh>;
		newelem->maxdepth = l;
		newelem->vh = v_h;	
		
		a = pmesh->point(v_h);
		// iteration over all adj. faces
		MyMesh::VertexFaceIter vf_it=pmesh->vf_iter(v_h);	
	
		Int32 offset = getLimOffset(&patches[getFaceIndex(vf_it.handle())],a) 
			+ patches[getFaceIndex(vf_it.handle())].varrayOffset;
				
		newelem->limPo.setValue(mySharedFields.limitpoints->getValue(offset));
		newelem->limNo.setValue(mySharedFields.limitnormals->getValue(offset));

		// now the adjacent patches:
		for (vf_it; vf_it; ++vf_it)	{		
			patches[getFaceIndex(vf_it.handle())].vertexindex[
				patches[getFaceIndex(vf_it.handle())].cvindex]=VertexListe.size();
			patches[getFaceIndex(vf_it.handle())].cvindex++;
			if (patches[getFaceIndex(vf_it.handle())].maxdepth < l)
				patches[getFaceIndex(vf_it.handle())].maxdepth = l;

			assert(patches[getFaceIndex(vf_it.handle())].cvindex < 5);
		}
		VertexListe.push_back((*newelem));		
	} 
	else {	// for creases: process for every sector
		Int32 numsect = (*wara)[i].isCrease;		
		Int32 secBegin = 0;
		for (Int32 sect=0; sect<numsect; sect++) {	// sector loop
			while ((*wara)[secBegin%i].isCrease == 0) secBegin+=2;
			if ((*wara)[(secBegin+1)%i].isCrease == 4) {	// no empty sector?
				Int32 secEnd = secBegin+2;
				while ((*wara)[secEnd%i].isCrease == 0) secEnd+=2;
				Int32 numface = (secEnd - secBegin) / 2;
				a = pmesh->point(v_h);
				// copy workarray
				tWorkArray WA3;
				tWorkArray* waA = &WA3;
				for (Int32 co = 0; co < i; co++)
					(*waA)[co].p = (*wara)[co].p;
				// get normal
				n = NormCCcrease(a,wara,i,secBegin,secEnd);
				Int32 l=0;			// start with depth 0
				Real32 test = getGreatestAngle(wara,a,n,secBegin,numface,i);
				while ((test < epsilon) && (l < wsdmaxdepth-1))	{					
					l++;			// next depth					
					simpleCreaseSubdiv(waA,waB,a,secBegin,secEnd,i);
					// switch workarrays:
					waT = &(*waA); waA = &(*waB); waB = &(*waT);
					// get normal
					test = getGreatestAngle(waA,a,n,secBegin,numface,i);
				}
				// thats it for this sector
				SVertexData<WSDVector, Mesh> *newelem = new SVertexData<WSDVector, Mesh>;
				newelem->maxdepth = l;
				newelem->vh = v_h;
				a = pmesh->point(v_h);
				Int32 offset = getLimOffset(&patches[getFaceIndex((*wara)[(secBegin+1)%i].fh)],a)
					+ patches[getFaceIndex((*wara)[(secBegin+1)%i].fh)].varrayOffset;
		
				newelem->limPo.setValue(mySharedFields.limitpoints->getValue(offset));
				newelem->limNo.setValue(mySharedFields.limitnormals->getValue(offset));
	
				// connect adj. patches to vertex
				for (Int32 w=secBegin+1; w<secEnd; w+=2) {
					patches[getFaceIndex((*wara)[w%i].fh)].vertexindex[
						patches[getFaceIndex((*wara)[w%i].fh)].cvindex]=VertexListe.size();
					patches[getFaceIndex((*wara)[w%i].fh)].cvindex++;
					assert(patches[getFaceIndex((*wara)[w%i].fh)].cvindex < 5);					

					if (patches[getFaceIndex((*wara)[w%i].fh)].maxdepth < l)
						patches[getFaceIndex((*wara)[w%i].fh)].maxdepth = l;
				}
				VertexListe.push_back((*newelem));
			}
			secBegin+=2;
		}	// for all sectors
	}	
}

// getFaceIndex: for a given facehandle return the hash value
template<class WSDVector, class Mesh, int mtype>
Int32 WSDmain<WSDVector, Mesh, mtype>
::getFaceIndex(FaceHandle &fh)
{
	Int32 i=0;
	for (i=0; i<patches.size(); i++)
		if (OmeshFaceListe[i] == fh) return i;	
	return -1;
}

// calculate normal with CC for creases / edges
// der Sector geht von g bis h im workarray
template<class WSDVector, class Mesh, int mtype>
WSDmain<WSDVector, Mesh, mtype>::OMPoint WSDmain<WSDVector, Mesh, mtype>::
NormCCcrease(OMPoint alpha_, tWorkArray* wara_, Int32 v, Int32 g, Int32 h)
{
	OMPoint betat1,gammat1,betat2,gammat2,t1,t2;
	OMPoint ret = OMPoint(0,0,0);
	Real32 k = (Real32)((h-g) / 2);				// k is the number of adjacent polygons
	betat2 = (*wara_)[g%v].p - (*wara_)[h%v].p;
	if (k < 2.0) {		
		betat1 = ((*wara_)[g%v].p + (*wara_)[h%v].p) * -1.0;
		t1 = (alpha_ * 2.0) + betat1;
		t2 = betat2;
		ret = t1%t2;		
	}
	else {		// k >= 2	
		Real32 pidk = Pi / k;
		Real32 rk = (osgcos(pidk) + 1.0) / (k * osgsin(pidk) * (3.0 + osgcos(pidk)));
		Real32 b0 = -1.0 * rk * (1.0 + 2.0 * osgcos(pidk));
		Real32 c = osgsin(pidk) / ((3.0 + osgcos(pidk)) * k);
		betat1 = (*wara_)[g%v].p * b0;
		gammat1 = (*wara_)[(g+1)%v].p * c;
		for (Int32 i=1; i<k; i++)	{
			Real32 bi = (4.0 * osgsin((Real32)i * pidk)) / ((3.0 + osgcos(pidk)) * k);
			betat1+= (*wara_)[(g+(i*2))%v].p * bi;
			c = (1.0 * (osgsin((Real32)i * pidk) + osgsin(((Real32)i + 1.0) * pidk))) / ((3.0 + osgcos(pidk)) * k);
			gammat1+= (*wara_)[(g+(i*2)+1)%v].p * c;
		}
		betat1+= (*wara_)[h%v].p * b0;
		Real32 a = 4.0 * rk * (osgcos(pidk) - 1.0);		
		t1 = (alpha_ * a) + betat1 + gammat1;
		t2 = betat2;
		ret = t2%t1;
	}
	ret.normalize();
	return ret;
}

// helper for the vertexarray offset
template<class WSDVector, class Mesh, int mtype>
Int32 WSDmain<WSDVector, Mesh, mtype>::
getLimOffset(WSDdat<WSDVector, mtype> *pp, OMPoint a)
{
	Int32 offset = 0;
	WSDVector t = pp->tableO[1][1];
	if (t[0] == a[0] && t[1] == a[1] && t[2] == a[2]) { 
		offset =0;
	}
	else {
		t = pp->tableO[1][2];
		if (t[0] == a[0] && t[1] == a[1] && t[2] == a[2]) { 
			offset = wsdmaxindex-1;
		} else {
			t = pp->tableO[2][1];
			if (t[0] == a[0] && t[1] == a[1] && t[2] == a[2]) { 
				offset = wsdmaxindex*wsdmaxindex - wsdmaxindex;
			} else {
				offset = wsdmaxindex*wsdmaxindex - 1;
			}
		}
	}
	return offset;
}

// calculate normal with CC for inner limitpoints
template<class WSDVector, class Mesh, int mtype>
WSDmain<WSDVector, Mesh, mtype>::OMPoint WSDmain<WSDVector, Mesh, mtype>::
NormCCinner(tWorkArray* wara_, Int32 v)
{
	Real32 val = (Real32)v / 2.0;
	Real32 ak = 1.0 + osgcos(2.0 * Pi / val) + osgcos(Pi / val) * osgsqrt(2.0*(9.0 + osgcos(2.0 * Pi / val)));
	Real32 b1,b2,c1,c2;
	OMPoint ret, betat1, gammat1, betat2, gammat2;
	gammat1 = betat1 = gammat2 = betat2 = OMPoint(0,0,0);

	Int32 i=0;
	
	for (Int32 j=0; j<v; j++)	{
		b1 = ak * osgcos(2*Pi*(i+1) / val);
		b2 = ak * osgcos(2*Pi*i / val);
		c1 = osgcos(2*Pi*(i+1) / val) + osgcos(2*Pi*(i+2) / val);
		c2 = osgcos(2*Pi*i / val) + osgcos(2*Pi*(i+1) / val);

		betat1+=(*wara_)[j].p * b1;
		betat2+=(*wara_)[j].p * b2;
		j++;
		gammat1+=(*wara_)[j].p * c1;
		gammat2+=(*wara_)[j].p * c2;

		i++;
	}	
	ret = (betat1 + gammat1) % (betat2 + gammat2); 
	ret = ret.normalize();
	return ret;
}

// helper to find the greates angle
template<class WSDVector, class Mesh, int mtype>
Real32 WSDmain<WSDVector, Mesh, mtype>::
getGreatestAngle(tWorkArray* wara_, OMPoint &a,
				 OMPoint &n, Int32 s, Int32 num, Int32 i)
{
	OMPoint fb1,fc,fb2, v1,v2,v3, fn;
	Real32 test=10.0;
	Int32 pp = s;
	for (Int32 fi=0; fi<num; fi++) {					// for all faces (#faces=val)	
		fb1 = (*wara_)[(pp)%i].p;
		fc  = (*wara_)[(pp+1)%i].p;
		fb2 = (*wara_)[(pp+2)%i].p;
		
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

// helper to do one subdivision step
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::
simpleSubdiv(tWorkArray* wara_, tWorkArray* waB_, OMPoint &a, Int32 i)
{
	Int32 gg=i;
	Int32 val = (i/2);
	OMPoint gamma, beta;
	gamma = beta = Mesh::Point(0,0,0);
	for (Int32 si=0; si<val; si++) {				
		//edge
		(*waB_)[gg%i].p = (((a + (*wara_)[gg%i].p) * 0.375) + 
			(((*wara_)[(gg+1)%i].p + (*wara_)[(gg-1)%i].p + 
			 (*wara_)[(gg+2)%i].p + (*wara_)[(gg-2)%i].p) * 0.0625));
		beta+=(*wara_)[gg%i].p;
		gg++;
		//face
		(*waB_)[(gg)%i].p = (a + (*wara_)[(gg)%i].p + 
				(*wara_)[(gg+1)%i].p + (*wara_)[(gg-1)%i].p) * 0.25;
		gamma+=(*wara_)[gg%i].p;
		gg++;
	}
	// vertex	
	Real32 falpha = 1.0 - (3.0 / (2.0 * (Real32)val)) - (1.0 / (4.0 * (Real32)val));
	Real32 fbeta  = 3.0 / (2.0 * (Real32)val * (Real32)val);
	Real32 fgamma = 1.0 / (4.0 * (Real32)val * (Real32)val);
	a = (falpha * a) + (fbeta * beta) + (fgamma * gamma);
}

// helper to do one subdivision step with creases
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::
simpleCreaseSubdiv(tWorkArray* wara_, tWorkArray* waB_, OMPoint &a, 
				   Int32 g, Int32 h, Int32 i)
{
	Int32 gg=i+g;		// i because of possible negative values!
	Int32 val = (i/2);	
	Int32 numface = (h-g)/2;	
	//sharp edge
	(*waB_)[gg%i].p = (a + (*wara_)[gg%i].p) * 0.5;
	gg++;
	//face
	(*waB_)[(gg)%i].p = (a + (*wara_)[(gg)%i].p + 
			(*wara_)[(gg+1)%i].p + (*wara_)[(gg-1)%i].p) * 0.25;
	gg++;
	for (Int32 si=0; si<numface-1; si++) {				
		//edge
		(*waB_)[gg%i].p = (((a + (*wara_)[gg%i].p) * 0.375) + 
			(((*wara_)[(gg+1)%i].p + (*wara_)[(gg-1)%i].p + 
			 (*wara_)[(gg+2)%i].p + (*wara_)[(gg-2)%i].p) * 0.0625));		
		gg++;
		//face
		(*waB_)[(gg)%i].p = (a + (*wara_)[(gg)%i].p + 
				(*wara_)[(gg+1)%i].p + (*wara_)[(gg-1)%i].p) * 0.25;		
		gg++;
	}
	//sharp edge
	(*waB_)[gg%i].p = (a + (*wara_)[gg%i].p) * 0.5;
	// vertex
	Real32 alpha = 0.75; 
	Real32 beta  = 0.125;
	a = (alpha * a) + (beta * ((*wara_)[g].p + (*wara_)[h].p));
}


// collectNeighbors: for a given patch pp get the four neighbors
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>
::collectNeighbors(FaceHandle f_h, WSDdat<WSDVector, mtype> *pp)
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

// helper to find the index in myInstances for parent p
template<class WSDVector, class Mesh, int mtype>
Int32 WSDmain<WSDVector, Mesh, mtype>::getIndex(OSG::NodePtr &p)
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


// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------

