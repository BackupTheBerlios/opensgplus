/*---------------------------------------------------------------------------*\
 *                       OpenSG - Dynamic Subdivision                        *
 *                                                                           *
 *                                                                           *
 *					  contact: v.settgast@cg.cs.tu-bs.de * 
 *                                                                           *
\*---------------------------------------------------------------------------*/

#include "OSGwsddat.h"
OSG_USING_NAMESPACE

// static data init
template<class WSDVector, int mtype> WSDVector
WSDdat<WSDVector, mtype>::tableA[wsdmaxindex][wsdmaxindex];
template<class WSDVector, int mtype> WSDVector
WSDdat<WSDVector, mtype>::tableB[wsdmaxindex][wsdmaxindex]; 
template<class WSDVector, int mtype> WSDVector*
WSDdat<WSDVector, mtype>::ptabA;
template<class WSDVector, int mtype> WSDVector*
WSDdat<WSDVector, mtype>::ptabB;
template<class WSDVector, int mtype> Int32 WSDdat<WSDVector, mtype>::breiteA;
template<class WSDVector, int mtype> WSDdat<WSDVector, mtype>::WSDVectorVS
WSDdat<WSDVector, mtype>::cornerA;
template<class WSDVector, int mtype> WSDdat<WSDVector, mtype>::WSDVectorVS
WSDdat<WSDVector, mtype>::cornerB;
template<class WSDVector, int mtype> WSDdat<WSDVector, mtype>::WSDVectorVS*
WSDdat<WSDVector, mtype>::pcorA;
template<class WSDVector, int mtype> WSDdat<WSDVector, mtype>::WSDVectorVS*
WSDdat<WSDVector, mtype>::pcorB;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//! DynamicSubdivisionCC description

template<class WSDVector, int mtype>
WSDdat<WSDVector, mtype>::WSDdat()
: tabsize(0)
{	
   ptabA = &tableA[0][0];
   ptabB = &tableB[0][0];
   cvindex = 0;

}

template<class WSDVector, int mtype>
WSDdat<WSDVector, mtype>::~WSDdat()
{   
}

template<class WSDVector, int mtype>
void WSDdat<WSDVector, mtype>::initptabs(void)
{   
   ptabA=&tableO[0][0]; breiteA=4;
   ptabB=&tableA[0][0]; 
   pcorA=&cornerO;
   pcorB=&cornerA;
   tabsize = 4;
}

template<class WSDVector, int mtype>
void WSDdat<WSDVector, mtype>::rotateptabs(Int32 i)
{
	if (i == 0) { // on first toggle switch from tableO to tableA
		ptabA=&tableA[0][0];
		ptabB=&tableB[0][0];   
		pcorA=&cornerA;
		pcorB=&cornerB;
	} else { // later on just switch between A and B
	   WSDVector* ptabh=ptabA;
	   ptabA=ptabB;
	   ptabB=ptabh;
	   WSDVectorVS* pcorh=pcorA;
	   pcorA=pcorB;
	   pcorB=pcorh;
	}
	breiteA=wsdmaxindex;	
}

// helper for getting neighbours
template<class WSDVector, int mtype>
Int32 WSDdat<WSDVector, mtype>::getneighbor(Int32 from, Int32 ad)
// from: index of source patch
// ad: 1 or -1; dependant on the position ni_from+1 or ni_from-1 is the neighbor we want
{
	Int32 ni_from=0;
	while (this->neighbors[ni_from] != from && ni_from<4) ni_from++;
	if (ni_from >=4) {printf("Kritischer Fehler bei Eck-Nachbarschaft!\n"); return 0;}

	ni_from+=4;
	Int32 r_i = ni_from+ad;

	return this->neighbors[r_i%4];
}

// setup indexarray for the inner patch area
template<class WSDVector, int mtype>
void WSDdat<WSDVector, mtype>::setInnerGeoP(perInstanceData &insta,
					    UInt32 &indisIn)
{	
   Int32 breite = wsddepthindexarray[wsdmaxdepth];
	Int32 tempexpo = (wsdmaxdepth - solltiefe);
	if (tempexpo < 0) tempexpo = 0;
	Int32 step = zweihoch[tempexpo];

	Int32 s,s2,s0;
	s0=varrayOffset+step+(step*breite);
	// zeilenweise quadstrips
	for (Int32 y=0; y<(wsdinnerindexwidth[solltiefe]-1); y++)	{
		insta.oneTypes->addValue(GL_QUAD_STRIP);		
		s=s0;
		s2=s+(step*breite);
		for (Int32 x=0; x<wsdinnerindexwidth[solltiefe]; x++)	{
			insta.oneIndis->setValue(s,indisIn++);
			insta.oneIndis->setValue(s2,indisIn++);
			s+=step;
			s2+=step;
		}
		insta.oneLengths->addValue(wsdinnerindexwidth[solltiefe]*2);      
		s0+=(step*breite);
	}
}

// setup indexarray for the outer patch area
template<class WSDVector, int mtype>
void WSDdat<WSDVector, mtype>::setOuterGeoP(perInstanceData &insta,
					    Int32 finer_l, Int32 finer_r, 
					    Int32 finer_o, Int32 finer_u,
					    UInt32 &indisIn)
{		
   if (solltiefe == 0) {
     setTiefe0(insta.oneLengths,
	       insta.oneTypes,
	       insta.oneIndis, indisIn);
   } else {
		// Kanten
		if (solltiefe > 1) {
			setOuterRightLine(insta.oneLengths,
				insta.oneTypes,
				insta.oneIndis,finer_r,indisIn);
			setOuterLeftLine(insta.oneLengths,
				insta.oneTypes,
				insta.oneIndis,finer_l,indisIn);
			setOuterFirstLine(insta.oneLengths,
				insta.oneTypes,
				insta.oneIndis,finer_o,indisIn);
			setOuterLastLine(insta.oneLengths,
				insta.oneTypes,
				insta.oneIndis,finer_u,indisIn);
		}
		// Ecken
			setOuterC0(insta.oneLengths,
				insta.oneTypes,
				insta.oneIndis,finer_o, finer_l,indisIn);
			setOuterC1(insta.oneLengths,
				insta.oneTypes,
				insta.oneIndis,finer_u, finer_l,indisIn);
			setOuterC2(insta.oneLengths,
				insta.oneTypes,
				insta.oneIndis,finer_u, finer_r,indisIn);
			setOuterC3(insta.oneLengths,
				insta.oneTypes,
				insta.oneIndis,finer_o, finer_r,indisIn);
	}	
}


// ------ init - methods for the edge-areas -------------

template<class WSDVector, int mtype>
void WSDdat<WSDVector, mtype>::setOuterFirstLine(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
						OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
						OSG::GeoIndicesUI32::StoredFieldType*  newindis,
						Int32 finer_o, UInt32 &indisIn)
{	
	Int32 breite = wsddepthindexarray[wsdmaxdepth];
Int32 tempexpo = (wsdmaxdepth - solltiefe);
if (tempexpo < 0) tempexpo = 0;
Int32 step = zweihoch[tempexpo];

	if (finer_o == 0) {
		// not finer, -> quadstrip
		newtypes->addValue(GL_QUAD_STRIP);
		Int32 s=step+varrayOffset;
		Int32 s2=s+(step*breite);
		for (Int32 j=0;j<wsdinnerindexwidth[solltiefe];j++)
		{
			newindis->setValue(s,indisIn++);
			newindis->setValue(s2,indisIn++);
			s+=step;
			s2+=step;
		}
		newlengths->addValue(wsdinnerindexwidth[solltiefe]*2);		      
	}
	else {	// -> finer_o = 1	
		Int32 halfstep = (step/2);
		Int32 s = step+varrayOffset;
		for (Int32 r=0; r<wsddepthindexarray[solltiefe]-3; r++, s=s+step) {
			newtypes->addValue(GL_TRIANGLE_FAN);
			Int32 a=s + halfstep + halfstep*breite;
			Int32 b=s + step*breite;
			Int32 u;
			Int32 in=0;
			newindis->setValue(a,indisIn++); in++;
			newindis->setValue(b+step,indisIn++);in++;
			newindis->setValue(a+halfstep,indisIn++);in++;
			for (u=s+step; u>s; u-=interstep_o){
				newindis->setValue(u,indisIn++);in++;}
			newindis->setValue(s,indisIn++);in++;
			newindis->setValue(a-halfstep,indisIn++);in++;
			newindis->setValue(b,indisIn++);in++;
			newindis->setValue(b+step,indisIn++);in++;
			newlengths->addValue(in);         
		}
	}

}

template<class WSDVector, int mtype>
void WSDdat<WSDVector, mtype>::setOuterLastLine(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
						OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
						OSG::GeoIndicesUI32::StoredFieldType*  newindis,
						Int32 finer_u, UInt32 &indisIn)
{	
	Int32 breite = wsddepthindexarray[wsdmaxdepth];
	Int32 tempexpo = (wsdmaxdepth - solltiefe);
	if (tempexpo < 0) tempexpo = 0;
	Int32 step = zweihoch[tempexpo];

	if (finer_u == 0) {
		newtypes->addValue(GL_QUAD_STRIP);
		Int32 s=(breite*(breite-1-step)) + step + varrayOffset;
		Int32 s2=s+(step*breite);
		for (Int32 j=0;j<wsdinnerindexwidth[solltiefe];j++)
		{
			newindis->setValue(s,indisIn++);
			newindis->setValue(s2,indisIn++);
			s+=step;
			s2+=step;
		}
		newlengths->addValue(wsdinnerindexwidth[solltiefe]*2);		      
	}
	else {	// -> finer_u = 1	
		Int32 halfstep = (step/2);
		Int32 s = (breite*(breite-1-step)) + step + varrayOffset;
		for (Int32 r=0; r<wsddepthindexarray[solltiefe]-3; r++, s=s+step) {
			newtypes->addValue(GL_TRIANGLE_FAN);
			Int32 a=s + halfstep + halfstep*breite;
			Int32 b=s + step*breite;
			Int32 u;
			Int32 in=0;
			newindis->setValue(a,indisIn++); in++;
			newindis->setValue(s,indisIn++); in++;
			newindis->setValue(a-halfstep,indisIn++); in++;
			for (u=b; u<b+step; u+=interstep_u){
				newindis->setValue(u,indisIn++); in++;}
			newindis->setValue(u,indisIn++); in++;
			newindis->setValue(a+halfstep,indisIn++); in++;
			newindis->setValue(s+step,indisIn++); in++;
			newindis->setValue(s,indisIn++); in++;
			newlengths->addValue(in);         
		}
	}

}

template<class WSDVector, int mtype>
void WSDdat<WSDVector, mtype>::setOuterRightLine(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
						OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
						OSG::GeoIndicesUI32::StoredFieldType*  newindis,
						Int32 finer_r, UInt32 &indisIn)
{	
	Int32 breite = wsddepthindexarray[wsdmaxdepth];
Int32 tempexpo = (wsdmaxdepth - solltiefe);
if (tempexpo < 0) tempexpo = 0;
Int32 step = zweihoch[tempexpo];

	if (finer_r == 0) {
		newtypes->addValue(GL_QUAD_STRIP);
		Int32 s = (breite - 1 - step) + (step*breite) + varrayOffset;	
		Int32 s2=s+step;
		for (Int32 j=0;j<wsdinnerindexwidth[solltiefe];j++)
		{
			newindis->setValue(s,indisIn++);
			newindis->setValue(s2,indisIn++);
			s+=(step*breite);
			s2+=(step*breite);
		}
		newlengths->addValue(wsdinnerindexwidth[solltiefe]*2);		      
	}
	else {	// -> finer_r = 1	
		Int32 halfstep = (step /2);
		Int32 s = (breite - 1 - step) + (step*breite) + varrayOffset;		
		for (Int32 r=0; r<wsddepthindexarray[solltiefe]-3; r++, s+=(step*breite)){		
			newtypes->addValue(GL_TRIANGLE_FAN);
			
			Int32 a = s + halfstep + (halfstep * breite);
			Int32 b = s + (step*breite);
			
			Int32 in = 0;
			newindis->setValue(a,indisIn++);in++;
			newindis->setValue(s,indisIn++);in++;
			newindis->setValue(b,indisIn++);in++;
			newindis->setValue(b+halfstep,indisIn++);in++;
			for (Int32 u=b+step; u>s; u-=(interstep_r*breite)) {
				newindis->setValue(u,indisIn++); in++;}
			newindis->setValue(s+halfstep,indisIn++); in++;
			newindis->setValue(s,indisIn++); in++;			
			newlengths->addValue(in);         
		}
	}
}

template<class WSDVector, int mtype>
void WSDdat<WSDVector, mtype>::setOuterLeftLine(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
						OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
						OSG::GeoIndicesUI32::StoredFieldType*  newindis,
						Int32 finer_l, UInt32 &indisIn)
{	
	Int32 breite = wsddepthindexarray[wsdmaxdepth];
	Int32 tempexpo = (wsdmaxdepth - solltiefe);
	if (tempexpo < 0) tempexpo = 0;
	Int32 step = zweihoch[tempexpo];

	if (finer_l == 0) {
		newtypes->addValue(GL_QUAD_STRIP);
		Int32 s = (breite*step)+varrayOffset;
		Int32 s2=s+step;
		for (Int32 j=0;j<wsdinnerindexwidth[solltiefe];j++) {
			newindis->setValue(s,indisIn++);
			newindis->setValue(s2,indisIn++);
			s+=(step*breite);
			s2+=(step*breite);
		}		
		newlengths->addValue(wsdinnerindexwidth[solltiefe]*2);		      
	}
	else {	// -> finer_l = 1	
		Int32 halfstep = (step /2);
		Int32 s = (breite*step)+varrayOffset;		
		for (Int32 r=0; r<wsddepthindexarray[solltiefe]-3; r++, s+=(step*breite)){		
			newtypes->addValue(GL_TRIANGLE_FAN);			
			Int32 a = s + halfstep + (halfstep * breite);
			Int32 b = s + (step*breite);			
			Int32 in = 0;
			newindis->setValue(a,indisIn++);in++;
			newindis->setValue(s+step,indisIn++);in++;
			newindis->setValue(s+halfstep,indisIn++);in++;
			for (Int32 u=s; u<b; u+=(interstep_l*breite)) {
				newindis->setValue(u,indisIn++); in++;}			
			newindis->setValue(b,indisIn++);in++;
			newindis->setValue(b+halfstep,indisIn++);in++;
			newindis->setValue(b+step,indisIn++); in++;
			newindis->setValue(s+step,indisIn++); in++;			
			newlengths->addValue(in);         
		}
	}
}


template<class WSDVector, int mtype>
void WSDdat<WSDVector, mtype>::setOuterC0(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
						OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
						OSG::GeoIndicesUI32::StoredFieldType*  newindis,
						Int32 finer_o, Int32 finer_l, UInt32 &indisIn)
{	
	Int32 breite = wsddepthindexarray[wsdmaxdepth];
	Int32 tempexpo = (wsdmaxdepth - solltiefe);
	if (tempexpo < 0) tempexpo = 0;
	Int32 step = zweihoch[tempexpo];

	Int32 halfstep = (step/2);

	Int32 s = varrayOffset;
	
	Int32 lc = 0;		// counter for fan length
	if (finer_l == 0) {
		if (finer_o == 0) {
			newtypes->addValue(GL_QUADS);
			newindis->setValue(s,indisIn++); 
			newindis->setValue(s+step*breite,indisIn++);
			newindis->setValue(s+(step*breite)+step,indisIn++); 
			newindis->setValue(s+step,indisIn++);
			newlengths->addValue(4);			         
		}
		else {		// -> finer_o = 1 + finer_l = 0		
			newtypes->addValue(GL_TRIANGLE_FAN);
			Int32 a=s + halfstep + halfstep*breite;
			Int32 b=s + step*breite + step;
			Int32 u;
			Int32 in=0;
			newindis->setValue(a,indisIn++); in++;
			for (u=s+step; u>=s; u-=interstep_o){
				newindis->setValue(u,indisIn++); in++;}				
			if (c0_l){newindis->setValue(a-halfstep,indisIn++); in++;}
			newindis->setValue(b-step,indisIn++); in++;
			newindis->setValue(b,indisIn++); in++;
			newindis->setValue(a+halfstep,indisIn++); in++;
			newindis->setValue(s+step,indisIn++); in++;
			newlengths->addValue(in); 			         
		}
	}
	else {	// -> finer_l = 1	
		if (finer_o == 0) {			
			newtypes->addValue(GL_TRIANGLE_FAN);
			Int32 b = s + step*breite;
			Int32 a = b + halfstep - (halfstep * breite);			
			Int32 u;
			Int32 in=0;			
			newindis->setValue(a,indisIn++); in++;
			newindis->setValue(s+step,indisIn++); in++;
			if (c0_o){newindis->setValue(s+(step)-halfstep,indisIn++); in++;}
			for (u=s; u<b; u+=(interstep_l*breite)) {
				newindis->setValue(u,indisIn++); in++;} 			
			newindis->setValue(u,indisIn++); in++; 
			newindis->setValue(u+halfstep,indisIn++); in++; 
			newindis->setValue(u+step,indisIn++); in++; 
			newindis->setValue(s+step,indisIn++); in++; 
			newlengths->addValue(in);			         
		}
		else {		// -> finer_o = 1		
			newtypes->addValue(GL_TRIANGLE_FAN);			
			Int32 a=s + halfstep + halfstep*breite;
			Int32 b=s + step*breite + step;
			Int32 u;
			Int32 in=0;
			newindis->setValue(a,indisIn++); in++;
			for (u=s+step; u>s; u-=interstep_o){
				newindis->setValue(u,indisIn++); in++;}
			for (u=s; u<b; u+=(interstep_l*breite)){
				newindis->setValue(u,indisIn++); in++;}
			newindis->setValue(b-halfstep,indisIn++); in++;
			newindis->setValue(b,indisIn++); in++;
			newindis->setValue(a+halfstep,indisIn++); in++;
			newindis->setValue(s+step,indisIn++); in++;
			newlengths->addValue(in);			         
		}
	}
}

template<class WSDVector, int mtype>
void WSDdat<WSDVector, mtype>::setOuterC1(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
						OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
						OSG::GeoIndicesUI32::StoredFieldType*  newindis,
						Int32 finer_u, Int32 finer_l, UInt32 &indisIn)
{	
	Int32 breite = wsddepthindexarray[wsdmaxdepth];
	Int32 tempexpo = (wsdmaxdepth - solltiefe);
	if (tempexpo < 0) tempexpo = 0;
	Int32 step = zweihoch[tempexpo];

	Int32 halfstep = (step/2);
	Int32 start = varrayOffset + breite*(breite-1-step);
	
	Int32 lc = 0;		// counter for fan length
	if (finer_l == 0) {
		if (finer_u == 0) {
			newtypes->addValue(GL_QUADS);
			newindis->setValue(start,indisIn++); 
			newindis->setValue(start+(step*breite),indisIn++);
			newindis->setValue(start+(step*breite)+step,indisIn++); 
			newindis->setValue(start+step,indisIn++);
			newlengths->addValue(4);			         
		}
		else {		// -> finer_u = 1 + finer_l = 0		
			newtypes->addValue(GL_TRIANGLE_FAN);			
			Int32 a=start + halfstep + halfstep*breite;
			Int32 b=start + step*breite;
			Int32 u;
			Int32 in=0;
			newindis->setValue(a,indisIn++); in++;
			newindis->setValue(start,indisIn++); in++;
			if (c1_l){newindis->setValue(a-halfstep,indisIn++); in++;}
			for (u=b; u<b+step; u+=interstep_u){
				newindis->setValue(u,indisIn++); in++;}
			newindis->setValue(u,indisIn++); in++;
			newindis->setValue(a+halfstep,indisIn++); in++;
			newindis->setValue(start+step,indisIn++); in++;
			newindis->setValue(start,indisIn++); in++;
			newlengths->addValue(in);					         
		}		
	}
	else {	// -> finer_l = 1	
		if (finer_u == 0) {			
			newtypes->addValue(GL_TRIANGLE_FAN);			
			Int32 b = start + halfstep;
			Int32 a = b + (halfstep * breite);
			Int32 c = b - halfstep;
			Int32 u;
			Int32 in=0;
			newindis->setValue(a,indisIn++); in++;
			newindis->setValue(start + step,indisIn++); in++;
			newindis->setValue(b,indisIn++); in++;
			for (u=c; u<c+(breite*step); u+=(interstep_l*breite)){
				newindis->setValue(u,indisIn++); in++;}
			newindis->setValue(c+(breite*step),indisIn++); in++;
			if (c1_u){newindis->setValue(u+halfstep,indisIn++); in++;}
			newindis->setValue(u + step,indisIn++); in++;
			newindis->setValue(start + step,indisIn++); in++;
			newlengths->addValue(in);			         
		}
		else {		// -> finer_u = 1 + finer_l = 1					
			newtypes->addValue(GL_TRIANGLE_FAN);			
			Int32 a=start + halfstep + halfstep*breite;
			Int32 b=start + step*breite;
			Int32 u;
			Int32 in=0;
			newindis->setValue(a,indisIn++); in++;
			for (u=start; u<b; u+=(interstep_l*breite)){
				newindis->setValue(u,indisIn++); in++;}
			for (u=b; u<b+step; u+=interstep_u){
				newindis->setValue(u,indisIn++); in++;}
			newindis->setValue(u,indisIn++); in++;
			newindis->setValue(a+halfstep,indisIn++); in++;
			newindis->setValue(start+step,indisIn++); in++;
			newindis->setValue(start+halfstep,indisIn++); in++;
			newindis->setValue(start,indisIn++); in++;
			newlengths->addValue(in);						         
		}
	}
}

template<class WSDVector, int mtype>
void WSDdat<WSDVector, mtype>::setOuterC2(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
						OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
						OSG::GeoIndicesUI32::StoredFieldType*  newindis,
						Int32 finer_u, Int32 finer_r, UInt32 &indisIn)
{	
	Int32 breite = wsddepthindexarray[wsdmaxdepth];
	Int32 tempexpo = (wsdmaxdepth - solltiefe);
	if (tempexpo < 0) tempexpo = 0;
	Int32 step = zweihoch[tempexpo];

	Int32 halfstep = (step/2);
	Int32 start = (breite*(breite-step))-1-step+varrayOffset;
	
	Int32 lc = 0;		// counter for fan length
	if (finer_r == 0) {
		if (finer_u == 0) {
			newtypes->addValue(GL_QUADS);
			newindis->setValue(start,indisIn++); 
			newindis->setValue(start+(step*breite),indisIn++);
			newindis->setValue(start+(step*breite)+step,indisIn++); 
			newindis->setValue(start+step,indisIn++);
			newlengths->addValue(4);			         
		}
		else {		// -> finer_u = 1		
			newtypes->addValue(GL_TRIANGLE_FAN);						
			Int32 a=start + halfstep + halfstep*breite;
			Int32 b=start + step*breite + step;
			Int32 u;
			Int32 in=0;
			newindis->setValue(a,indisIn++); in++;
			for (u=start+(step*breite); u<b; u+=interstep_u){
				newindis->setValue(u,indisIn++); in++;}
			newindis->setValue(u,indisIn++); in++;
			if (c2_r){newindis->setValue(a+halfstep,indisIn++); in++;}
			newindis->setValue(start+step,indisIn++); in++;
			newindis->setValue(start,indisIn++); in++;
			newindis->setValue(a-halfstep,indisIn++); in++;
			newindis->setValue(start+step*breite,indisIn++); in++;
			newlengths->addValue(in);						         
		}		
	}
	else {	// -> finer_r = 1	
		if (finer_u == 0) {
			newtypes->addValue(GL_TRIANGLE_FAN);						
			Int32 ende = start+(breite*step);
		
			Int32 b = start;
			Int32 a = b + halfstep + (halfstep * breite);			
			Int32 u;
			Int32 in=0;

			newindis->setValue(a,indisIn++); in++;
			newindis->setValue(ende,indisIn++); in++;
			if (c2_u){newindis->setValue(ende + halfstep,indisIn++); in++;}
			for (u=ende+step; u>b; u-=(interstep_r*breite)){
				newindis->setValue(u,indisIn++); in++;}
			newindis->setValue(b+step,indisIn++); in++;
			newindis->setValue(b+halfstep,indisIn++); in++;
			newindis->setValue(b,indisIn++); in++;
			newindis->setValue(ende,indisIn++); in++;
			newlengths->addValue(in);						         
		}
		else {		// -> finer_u = 1					
			newtypes->addValue(GL_TRIANGLE_FAN);						
			Int32 a=start + halfstep + halfstep*breite;
			Int32 b=start + step*breite + step;
			Int32 u;
			Int32 in=0;
			newindis->setValue(a,indisIn++); in++;
			for (u=start+(step*breite); u<b; u+=interstep_u){
				newindis->setValue(u,indisIn++); in++;}
			for (u=b; u>start+step; u-=(interstep_r*breite)){
				newindis->setValue(u,indisIn++); in++;}
			newindis->setValue(u,indisIn++); in++;
			newindis->setValue(start+halfstep,indisIn++);in++;
			newindis->setValue(start,indisIn++);in++;
			newindis->setValue(a-halfstep,indisIn++);in++;
			newindis->setValue(start+step*breite,indisIn++);in++;
			newlengths->addValue(in);						         
		}
	}
}

template<class WSDVector, int mtype>
void WSDdat<WSDVector, mtype>::setOuterC3(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
						OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
						OSG::GeoIndicesUI32::StoredFieldType*  newindis,
						Int32 finer_o, Int32 finer_r, UInt32 &indisIn)
{	
	Int32 breite = wsddepthindexarray[wsdmaxdepth];
	Int32 tempexpo = (wsdmaxdepth - solltiefe);
	if (tempexpo < 0) tempexpo = 0;
	Int32 step = zweihoch[tempexpo];

	Int32 halfstep = (step/2);
	Int32 start = breite-1-step+varrayOffset;
	
	Int32 lc = 0;		// counter for fan length
	if (finer_r == 0) {
		if (finer_o == 0) {
			newtypes->addValue(GL_QUADS);
			newindis->setValue(start,indisIn++); 
			newindis->setValue(start+(step*breite),indisIn++);
			newindis->setValue(start+(step*breite)+step,indisIn++); 
			newindis->setValue(start+step,indisIn++);
			newlengths->addValue(4);			         
		}
		else {		// -> finer_o = 1 + finer_r = 0		
			newtypes->addValue(GL_TRIANGLE_FAN);
			Int32 s = start;//(breite-1-step);			
			Int32 a=s + halfstep + halfstep*breite;
			Int32 b=s + step*breite + step;
			Int32 u;
			Int32 in=0;
			newindis->setValue(a,indisIn++); in++;
			newindis->setValue(b,indisIn++); in++;
			if (c3_r){newindis->setValue(a+halfstep,indisIn++); in++;}
			for (u=s+step; u>=s; u-=interstep_o){
				newindis->setValue(u,indisIn++); in++;}
			newindis->setValue(a-halfstep,indisIn++); in++;
			newindis->setValue(s+step*breite,indisIn++); in++;
			newindis->setValue(b,indisIn++); in++;
			newlengths->addValue(in);			         
		}		
	}
	else {	// -> finer_r = 1	
		if (finer_o == 0) {
			newtypes->addValue(GL_TRIANGLE_FAN);
			Int32 ende = start;//(breite-step-1);			
			Int32 a = ende + halfstep + (halfstep * breite);
			Int32 b = a + halfstep + (halfstep * breite);						
			Int32 u;
			Int32 in=0;
			newindis->setValue(a,indisIn++); in++;
			newindis->setValue(ende,indisIn++); in++;
			newindis->setValue(b-step,indisIn++); in++;
			newindis->setValue(b-halfstep,indisIn++); in++;
			for (u=b; u>ende; u-=(interstep_r*breite)){
				newindis->setValue(u,indisIn++); in++;}
			if (c3_o) {newindis->setValue(ende+halfstep,indisIn++); in++;}
			newindis->setValue(ende,indisIn++); in++;
			newlengths->addValue(in);			         
		}
		else {		// -> finer_o = 1 + finer_r = 1						
			newtypes->addValue(GL_TRIANGLE_FAN);
			Int32 s = start;//(breite-1-step);			
			Int32 a=s + halfstep + halfstep*breite;
			Int32 b=s + step*breite + step;
			Int32 u;
			Int32 in=0;
			newindis->setValue(a,indisIn++); in++;
			for (u=b; u>s+step; u-=(interstep_r*breite)){
				newindis->setValue(u,indisIn++); in++;}
			for (u=s+step; u>=s; u-=interstep_o){
				newindis->setValue(u,indisIn++); in++;}
			newindis->setValue(a-halfstep,indisIn++); in++;
			newindis->setValue(s+step*breite,indisIn++); in++;
			newindis->setValue(b-halfstep,indisIn++); in++;
			newindis->setValue(b,indisIn++); in++;
			newlengths->addValue(in);			         
		}
	}
}

template<class WSDVector, int mtype>
void WSDdat<WSDVector, mtype>::setTiefe0(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
						OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
						OSG::GeoIndicesUI32::StoredFieldType*  newindis,
						UInt32 &indisIn)
{
	Int32 breite = wsddepthindexarray[wsdmaxdepth];
	Int32 tempexpo = (wsdmaxdepth - solltiefe);
	if (tempexpo < 0) tempexpo = 0;
	Int32 step = zweihoch[tempexpo];

	Int32 halfstep = (step/2);

	newtypes->addValue(GL_TRIANGLE_FAN);
	Int32 s = varrayOffset;	
	Int32 e = s + (step*breite) + step;
	Int32 u,in=0;
	Int32 a=s + halfstep + halfstep*breite;

	newindis->setValue(a,indisIn++); in++;
	for (u=s; u<s+(step*breite); u+=(interstep_l*breite)) {
		newindis->setValue(u,indisIn++); in++;}
	for (u=s+(step*breite); u<e; u+=interstep_u) {
		newindis->setValue(u,indisIn++); in++;}
	for (u=e; u>s+step; u-=(interstep_r*breite)) {
		newindis->setValue(u,indisIn++); in++;}
	for (u=s+step; u>=s; u-=interstep_o) {
		newindis->setValue(u,indisIn++); in++;}	
	newlengths->addValue(in);	   
}


