/*---------------------------------------------------------------------------*\
 *                       OpenSG - Dynamic Subdivision                        *
 *                                                                           *
 *                                                                           *
 *            contact: v.settgast@cg.cs.tu-bs.de * 
 *                                                                           *
\*---------------------------------------------------------------------------*/

#include "OSGwsddat.h"
#include "OSGLog.h"

OSG_USING_NAMESPACE

// explicit template instantiations
template class OSG_SUBSURFACELIB_DLLMAPPING WSDdat<OSG::Vec3f, QUAD>;
template class OSG_SUBSURFACELIB_DLLMAPPING WSDdat<OSG::Vec4f, QUAD>;
template class OSG_SUBSURFACELIB_DLLMAPPING WSDdat<OSG::Vec3f, TRIANGLE>;

// static data init
template<class WSDVector, int mtype> 
typename WSDdat<WSDVector, mtype>::VectorType*   WSDdat<WSDVector, mtype>::slateA;
template<class WSDVector, int mtype> 
typename WSDdat<WSDVector, mtype>::VectorType*   WSDdat<WSDVector, mtype>::slateB;
template<class WSDVector, int mtype> 
typename WSDdat<WSDVector, mtype>::VectorType*   WSDdat<WSDVector, mtype>::ptabA;
template<class WSDVector, int mtype> 
typename WSDdat<WSDVector, mtype>::VectorType*   WSDdat<WSDVector, mtype>::ptabB;
template<class WSDVector, int mtype> 
Int32    WSDdat<WSDVector, mtype>::breiteA;
template<class WSDVector, int mtype> 
typename WSDdat<WSDVector, mtype>::VectorCAType  WSDdat<WSDVector, mtype>::cornerA;
template<class WSDVector, int mtype> 
typename WSDdat<WSDVector, mtype>::VectorCAType  WSDdat<WSDVector, mtype>::cornerB;
template<class WSDVector, int mtype> 
typename WSDdat<WSDVector, mtype>::VectorCAType* WSDdat<WSDVector, mtype>::pcorA;
template<class WSDVector, int mtype> 
typename WSDdat<WSDVector, mtype>::VectorCAType* WSDdat<WSDVector, mtype>::pcorB;

#ifdef DEFINE_SHOWROOM
template<class WSDVector, int mtype> UInt32
WSDdat<WSDVector, mtype>::_numtris;
template<class WSDVector, int mtype> UInt32
WSDdat<WSDVector, mtype>::_numquads;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//! DynamicSubdivisionCC description

template<class WSDVector, int mtype>
WSDdat<WSDVector, mtype>::WSDdat()
: tabsize(0)
{  
   ptabA = slateA;
   ptabB = slateB;
   cvindex = 0;
}

template<class WSDVector, int mtype>
WSDdat<WSDVector, mtype>::~WSDdat()
{   
}

template<class WSDVector, int mtype>
void WSDdat<WSDVector, mtype>::initptabs(void)
{   
   ptabA=&slateO[0][0]; breiteA=4;
   ptabB=slateA;
   pcorA=&cornerO;
   pcorB=&cornerA;
   tabsize = 4;   
}

template<class WSDVector, int mtype>
void WSDdat<WSDVector, mtype>::initptex(void)
{   
   ptabA=&TexCoords[0][0]; breiteA=4;     // set the texcoords as starting slate
   ptabB=slateA;
   pcorA=&cornerO;
   pcorB=&cornerA;   
   tabsize = 4;   
}

template<class WSDVector, int mtype>
void WSDdat<WSDVector, mtype>::rotateptabs(Int32 i)
{
  if (i == 0) {     // on first toggle, switch from slateO to slateA
    ptabA=slateA;
    ptabB=slateB;
    pcorA=&cornerA;
    pcorB=&cornerB;
  } else {          // later on just switch between A and B
     VectorType* ptabh=ptabA;
     ptabA=ptabB;
     ptabB=ptabh;
     VectorCAType* pcorh=pcorA;
     pcorA=pcorB;
     pcorB=pcorh;
  }
  breiteA=wsddepthindexarray[wsdmaxdepth]+2;
}

// helper for getting neighbours
// input: from -  index of source patch
//          ad -  1 or -1; dependant on the position ni_from+1 or ni_from-1 is the neighbor we want
template<class WSDVector, int mtype>
Int32 WSDdat<WSDVector, mtype>::getneighbor (Int32 from, Int32 ad)
{
   if (this->isSingleTriangle) {
      return 0;
   }
   Int32 ni_from=0;
   while (this->neighbors[ni_from] != from && ni_from<4) ni_from++;
   if (ni_from >= 4) {
      //SFATAL << "error finding neighbor" << std::endl; 
      return 0;
   }
   ni_from+=4;
   Int32 r_i = ni_from+ad;  
   return this->neighbors[r_i%4];
}

// setup indexarray for the inner patch area
template<class WSDVector, int mtype>
void WSDdat<WSDVector, mtype>::setInnerGeoP
(perInstanceData &insta, UInt32 &indisIn)
{    
   Int32 breite = wsddepthindexarray[wsdmaxdepth];
   Int32 tempexpo = (wsdmaxdepth - solltiefe);
   if (tempexpo < 0) tempexpo = 0;
   Int32 step = zweihoch[tempexpo];
   Int32 s;
   Int32 s0=varrayOffset+step+(step*breite);
   if (this->isSingleTriangle) {      
      Int32 w=1;
      // linewise quad/triangle strips (only one triangle!)
      for (Int32 y=0; y<(wsdinnerindexwidth[solltiefe]-1); y++)  {    
         s=s0;    
         setupHalfStrip(insta.oneLengths, insta.oneTypes, insta.oneIndis,
          indisIn, s, (s+(step*breite)), step,w);      
         s0+=(step*breite);
         w++;
      }
   } else { // linewise quad/triangle strips
      for (Int32 y=0; y<(wsdinnerindexwidth[solltiefe]-1); y++)  {    
         s=s0;    
         setupStrip(insta.oneLengths, insta.oneTypes, insta.oneIndis,
          indisIn, s, (s+(step*breite)), step);      
         s0+=(step*breite);
      }
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
      // fix the funny looking depth 0 effect
      if (finer_o == 0) {
         interstep_o = interstep_o/2;
      }
      if (finer_u == 0) {
         interstep_u = interstep_u/2;
      }
      if (finer_l == 0) {
         interstep_l = interstep_l/2;
      }
      if (finer_r == 0) {
         interstep_r = interstep_r/2;
      }
      setTiefe0(insta.oneLengths,
       insta.oneTypes,
       insta.oneIndis, indisIn);
   } else {
      // edges
      if (solltiefe > 1) {         
         setOuterLeftLine(insta.oneLengths,
          insta.oneTypes,
          insta.oneIndis,finer_l,indisIn);      
         setOuterLastLine(insta.oneLengths,
          insta.oneTypes,
          insta.oneIndis,finer_u,indisIn);
         if (!this->isSingleTriangle) {
            setOuterFirstLine(insta.oneLengths,
             insta.oneTypes,
             insta.oneIndis,finer_o,indisIn);
            setOuterRightLine(insta.oneLengths,
             insta.oneTypes,
             insta.oneIndis,finer_r,indisIn);
         }
      }  
      // Corners
      setOuterC1(insta.oneLengths,
       insta.oneTypes,
       insta.oneIndis,finer_u, finer_l,indisIn);
      setOuterC0(insta.oneLengths,
       insta.oneTypes,
       insta.oneIndis,finer_o, finer_l,indisIn);
      setOuterC2(insta.oneLengths,
       insta.oneTypes,
       insta.oneIndis,finer_u, finer_r,indisIn);
      if (!this->isSingleTriangle) {
         setOuterC3(insta.oneLengths,
          insta.oneTypes,
          insta.oneIndis,finer_o, finer_r,indisIn);
      }
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
      // not finer, -> trianglestrip
      Int32 s=step+varrayOffset;
      setupStrip(newlengths, newtypes, newindis,
      indisIn, s, (s+(step*breite)), step);    
   } else {  // -> finer_o = 1  
      Int32 halfstep = (step/2);
      Int32 s = step+varrayOffset;
      for (Int32 r=0; r<wsddepthindexarray[solltiefe]-3; r++, s=s+step) {
         newtypes->push_back(GL_TRIANGLE_FAN);
         Int32 a=s + halfstep + halfstep*breite;
         Int32 b=s + step*breite;
         Int32 u;
         Int32 in=0;
         newindis->setValue(a,indisIn++); in++;
         newindis->setValue(b+step,indisIn++);in++;
         newindis->setValue(a+halfstep,indisIn++);in++;
         for (u=s+step; u>s; u-=interstep_o){
            newindis->setValue(u,indisIn++);in++;
         }
         newindis->setValue(s,indisIn++);in++;
         newindis->setValue(a-halfstep,indisIn++);in++;
         newindis->setValue(b,indisIn++);in++;
         newindis->setValue(b+step,indisIn++);in++;
         newlengths->push_back(in);  
#ifdef DEFINE_SHOWROOM
         _numtris+=(in-2);
#endif
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
      Int32 s=((breite*(breite-1-step)) + step + varrayOffset);
      setupStrip(newlengths, newtypes, newindis,
      indisIn, s, (s+(step*breite)), step);    
   } else {  // -> finer_u = 1  
      Int32 halfstep = (step/2);
      Int32 s = (breite*(breite-1-step)) + step + varrayOffset;
      for (Int32 r=0; r<wsddepthindexarray[solltiefe]-3; r++, s=s+step) {
         newtypes->push_back(GL_TRIANGLE_FAN);
         Int32 a=s + halfstep + halfstep*breite;
         Int32 b=s + step*breite;
         Int32 u;
         Int32 in=0;
         newindis->setValue(a,indisIn++); in++;
         newindis->setValue(s,indisIn++); in++;
         newindis->setValue(a-halfstep,indisIn++); in++;
         for (u=b; u<b+step; u+=interstep_u){
            newindis->setValue(u,indisIn++); in++;
         }
         newindis->setValue(u,indisIn++); in++;
         newindis->setValue(a+halfstep,indisIn++); in++;
         newindis->setValue(s+step,indisIn++); in++;
         newindis->setValue(s,indisIn++); in++;
         newlengths->push_back(in); 
#ifdef DEFINE_SHOWROOM
         _numtris+=(in-2);
#endif
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
      Int32 s = ((breite - 1 - step) + (step*breite) + varrayOffset);  
      setupStrip(newlengths, newtypes, newindis,
      indisIn, (s+step), s, (step*breite));    
   } else {  // -> finer_r = 1  
      Int32 halfstep = (step /2);
      Int32 s = (breite - 1 - step) + (step*breite) + varrayOffset;    
      for (Int32 r=0; r<wsddepthindexarray[solltiefe]-3; r++, s+=(step*breite)){    
         newtypes->push_back(GL_TRIANGLE_FAN);         
         Int32 a = s + halfstep + (halfstep * breite);
         Int32 b = s + (step*breite);         
         Int32 in = 0;
         newindis->setValue(a,indisIn++);in++;
         newindis->setValue(s,indisIn++);in++;
         newindis->setValue(b,indisIn++);in++;
         newindis->setValue(b+halfstep,indisIn++);in++;
         for (Int32 u=b+step; u>s; u-=(interstep_r*breite)) {
            newindis->setValue(u,indisIn++); in++;
         }
         newindis->setValue(s+halfstep,indisIn++); in++;
         newindis->setValue(s,indisIn++); in++;      
         newlengths->push_back(in);  
#ifdef DEFINE_SHOWROOM
         _numtris+=(in-2);
#endif
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
      Int32 s = ((breite*step)+varrayOffset);
      setupStrip(newlengths, newtypes, newindis,
      indisIn, (s+step), s, (step*breite));            
   } else {  // -> finer_l = 1  
      Int32 halfstep = (step /2);
      Int32 s = (breite*step)+varrayOffset;    
      for (Int32 r=0; r<wsddepthindexarray[solltiefe]-3; r++, s+=(step*breite)){    
         newtypes->push_back(GL_TRIANGLE_FAN);      
         Int32 a = s + halfstep + (halfstep * breite);
         Int32 b = s + (step*breite);      
         Int32 in = 0;
         newindis->setValue(a,indisIn++);in++;
         newindis->setValue(s+step,indisIn++);in++;
         newindis->setValue(s+halfstep,indisIn++);in++;
         for (Int32 u=s; u<b; u+=(interstep_l*breite)) {
            newindis->setValue(u,indisIn++); in++;
         }      
         newindis->setValue(b,indisIn++);in++;
         newindis->setValue(b+halfstep,indisIn++);in++;
         newindis->setValue(b+step,indisIn++); in++;
         newindis->setValue(s+step,indisIn++); in++;      
         newlengths->push_back(in); 
#ifdef DEFINE_SHOWROOM
         _numtris+=(in-2);
#endif
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
   Int32 lc = 0;    // counter for fan length
   if (finer_l == 0) {
      if (finer_o == 0) {
         if (this->isSingleTriangle) {
            setSpecialTriangle(newlengths, newtypes, newindis, indisIn, s,
             (s+(step*breite)), (s+(step*breite)+step));      
         } else {
            setQuadOrTriangle(newlengths, newtypes, newindis, indisIn, s,
             (s+(step*breite)), (s+(step*breite)+step), (s+step));      
         }
      } else {    // -> finer_o = 1 + finer_l = 0    
         newtypes->push_back(GL_TRIANGLE_FAN);
         Int32 a=s + halfstep + halfstep*breite;
         Int32 b=s + step*breite + step;
         Int32 u;
         Int32 in=0;
         newindis->setValue(a,indisIn++); in++;
         if (!this->isSingleTriangle) {
            newindis->setValue(b,indisIn++); in++;
            newindis->setValue(a+halfstep,indisIn++); in++;          
            for (u=s+step; u>s; u-=interstep_o){
               newindis->setValue(u,indisIn++); in++;
            }        
         } 
         newindis->setValue(s,indisIn++); in++;            
         if (c0_l){newindis->setValue(a-halfstep,indisIn++); in++;}
         newindis->setValue(b-step,indisIn++); in++;
         newindis->setValue(b,indisIn++); in++;
         newlengths->push_back(in); 
#ifdef DEFINE_SHOWROOM
         _numtris+=(in-2);
#endif
    
      }
   } else {  // -> finer_l = 1  
      if (finer_o == 0) {      
      newtypes->push_back(GL_TRIANGLE_FAN);
      Int32 b = s + step*breite;
      Int32 a = b + halfstep - (halfstep * breite);      
      Int32 u;
      Int32 in=0;      
      newindis->setValue(a,indisIn++); in++;
      for (u=s; u<b; u+=(interstep_l*breite)) {
         newindis->setValue(u,indisIn++); in++;
      }       
      newindis->setValue(u,indisIn++); in++; 
      newindis->setValue(u+halfstep,indisIn++); in++; 
      newindis->setValue(u+step,indisIn++); in++; 
      if (!this->isSingleTriangle) {
         newindis->setValue(s+step,indisIn++); in++; 
         if (c0_o){
            newindis->setValue(s+halfstep,indisIn++); in++;
         }
         newindis->setValue(s,indisIn++); in++; 
      }
      newlengths->push_back(in);  
#ifdef DEFINE_SHOWROOM
         _numtris+=(in-2);
#endif
   } else {    // -> finer_o = 1    
      newtypes->push_back(GL_TRIANGLE_FAN);      
      Int32 a=s + halfstep + halfstep*breite;
      Int32 b=s + step*breite + step;
      Int32 u;
      Int32 in=0;
      newindis->setValue(a,indisIn++); in++;
      for (u=s; u<b; u+=(interstep_l*breite)){
         newindis->setValue(u,indisIn++); in++;
      }
      newindis->setValue(b-halfstep,indisIn++); in++;
      newindis->setValue(b,indisIn++); in++;
      if (!this->isSingleTriangle) {
         newindis->setValue(a+halfstep,indisIn++); in++;
         for (u=s+step; u>s; u-=interstep_o){
            newindis->setValue(u,indisIn++); in++;}
            newindis->setValue(s,indisIn++); in++;      
         }
         newlengths->push_back(in);  
#ifdef DEFINE_SHOWROOM
            _numtris+=(in-2);
#endif
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
   Int32 lc = 0;    // counter for fan length
   if (finer_l == 0) {
      if (finer_u == 0) {
         setQuadOrTriangle(newlengths, newtypes, newindis, indisIn, start,
          (start+(step*breite)), (start+(step*breite)+step), (start+step));      
      } else {    // -> finer_u = 1 + finer_l = 0    
         newtypes->push_back(GL_TRIANGLE_FAN);      
         Int32 a=start + halfstep + halfstep*breite;
         Int32 b=start + step*breite;
         Int32 u;
         Int32 in=0;
         newindis->setValue(a,indisIn++); in++;
         newindis->setValue(start,indisIn++); in++;
         if (c1_l){newindis->setValue(a-halfstep,indisIn++); in++;}
         for (u=b; u<b+step; u+=interstep_u){
            newindis->setValue(u,indisIn++); in++;
         }
         newindis->setValue(u,indisIn++); in++;
         newindis->setValue(a+halfstep,indisIn++); in++;
         newindis->setValue(start+step,indisIn++); in++;
         newindis->setValue(start,indisIn++); in++;
         newlengths->push_back(in);  
#ifdef DEFINE_SHOWROOM
         _numtris+=(in-2);
#endif
      }    
   } else {  // -> finer_l = 1  
      if (finer_u == 0) {      
         newtypes->push_back(GL_TRIANGLE_FAN);      
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
         newlengths->push_back(in);  
#ifdef DEFINE_SHOWROOM
         _numtris+=(in-2);
#endif
      } else {    // -> finer_u = 1 + finer_l = 1          
         newtypes->push_back(GL_TRIANGLE_FAN);      
         Int32 a=start + halfstep + halfstep*breite;
         Int32 b=start + step*breite;
         Int32 u;
         Int32 in=0;
         newindis->setValue(a,indisIn++); in++;
         for (u=start; u<b; u+=(interstep_l*breite)){
            newindis->setValue(u,indisIn++); in++;
         }
         for (u=b; u<b+step; u+=interstep_u){
            newindis->setValue(u,indisIn++); in++;
         }
         newindis->setValue(u,indisIn++); in++;
         newindis->setValue(a+halfstep,indisIn++); in++;
         newindis->setValue(start+step,indisIn++); in++;
         newindis->setValue(start+halfstep,indisIn++); in++;
         newindis->setValue(start,indisIn++); in++;
         newlengths->push_back(in);  
#ifdef DEFINE_SHOWROOM
         _numtris+=(in-2);
#endif
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
   Int32 lc = 0;    // counter for fan length
   if (finer_r == 0) {
      if (finer_u == 0) {
         if (this->isSingleTriangle) {
            setSpecialTriangle(newlengths, newtypes, newindis, indisIn, start,
             (start+(step*breite)), (start+(step*breite)+step));      
         } else {
            setQuadOrTriangle(newlengths, newtypes, newindis, indisIn, start,
             (start+(step*breite)), (start+(step*breite)+step), (start+step));      
         }
      } else {    // -> finer_u = 1    
         newtypes->push_back(GL_TRIANGLE_FAN);            
         Int32 a=start + halfstep + halfstep*breite;
         Int32 b=start + step*breite + step;
         Int32 u;
         Int32 in=0;
         newindis->setValue(a,indisIn++); in++;
         for (u=start+(step*breite); u<b; u+=interstep_u){
            newindis->setValue(u,indisIn++); in++;
         }
         newindis->setValue(u,indisIn++); in++;
         if (!this->isSingleTriangle) {
            if (c2_r){newindis->setValue(a+halfstep,indisIn++); in++;}
            newindis->setValue(start+step,indisIn++); in++;
         }
         newindis->setValue(start,indisIn++); in++;
         newindis->setValue(a-halfstep,indisIn++); in++;
         newindis->setValue(start+step*breite,indisIn++); in++;
         newlengths->push_back(in);  
#ifdef DEFINE_SHOWROOM
         _numtris+=(in-2);
#endif
      }    
   } else {  // -> finer_r = 1  
      if (finer_u == 0) {
         newtypes->push_back(GL_TRIANGLE_FAN);            
         Int32 ende = start+(breite*step);         
         Int32 b = start;
         Int32 a = b + halfstep + (halfstep * breite);      
         Int32 u;
         Int32 in=0;
         newindis->setValue(a,indisIn++); in++;
         newindis->setValue(ende,indisIn++); in++;
         if (c2_u){newindis->setValue(ende + halfstep,indisIn++); in++;}
         if (!this->isSingleTriangle) {
            for (u=ende+step; u>b; u-=(interstep_r*breite)){
               newindis->setValue(u,indisIn++); in++;
            }
            newindis->setValue(b+step,indisIn++); in++;
            newindis->setValue(b+halfstep,indisIn++); in++;
         } else {
            newindis->setValue(ende+step,indisIn++); in++;
         }      
         newindis->setValue(b,indisIn++); in++;
         newindis->setValue(ende,indisIn++); in++;
         newlengths->push_back(in);  
#ifdef DEFINE_SHOWROOM
            _numtris+=(in-2);
#endif
      } else {    // -> finer_u = 1          
         newtypes->push_back(GL_TRIANGLE_FAN);            
         Int32 a=start + halfstep + halfstep*breite;
         Int32 b=start + step*breite + step;
         Int32 u;
         Int32 in=0;
         newindis->setValue(a,indisIn++); in++;
         for (u=start+(step*breite); u<b; u+=interstep_u){
            newindis->setValue(u,indisIn++); in++;
         }
         if (!this->isSingleTriangle) {
            for (u=b; u>start+step; u-=(interstep_r*breite)){
               newindis->setValue(u,indisIn++); in++;
            }
            newindis->setValue(u,indisIn++); in++;
            newindis->setValue(start+halfstep,indisIn++);in++;
         } else {
            newindis->setValue(b,indisIn++); in++;
         }
         newindis->setValue(start,indisIn++);in++;
         newindis->setValue(a-halfstep,indisIn++);in++;
         newindis->setValue(start+step*breite,indisIn++);in++;
         newlengths->push_back(in);  
#ifdef DEFINE_SHOWROOM
            _numtris+=(in-2);
#endif
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
   Int32 lc = 0;    // counter for fan length
   if (finer_r == 0) {
      if (finer_o == 0) {
         setQuadOrTriangle(newlengths, newtypes, newindis, indisIn, start,
          (start+(step*breite)), (start+(step*breite)+step), (start+step));      
      } else {    // -> finer_o = 1 + finer_r = 0    
         newtypes->push_back(GL_TRIANGLE_FAN);
         Int32 s = start;//(breite-1-step);      
         Int32 a=s + halfstep + halfstep*breite;
         Int32 b=s + step*breite + step;
         Int32 u;
         Int32 in=0;
         newindis->setValue(a,indisIn++); in++;
         newindis->setValue(b,indisIn++); in++;
         if (c3_r){newindis->setValue(a+halfstep,indisIn++); in++;}
         for (u=s+step; u>=s; u-=interstep_o){
            newindis->setValue(u,indisIn++); in++;
         }
         newindis->setValue(a-halfstep,indisIn++); in++;
         newindis->setValue(s+step*breite,indisIn++); in++;
         newindis->setValue(b,indisIn++); in++;
         newlengths->push_back(in);  
#ifdef DEFINE_SHOWROOM
         _numtris+=(in-2);
#endif
      }    
   } else {  // -> finer_r = 1  
      if (finer_o == 0) {
         newtypes->push_back(GL_TRIANGLE_FAN);
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
            newindis->setValue(u,indisIn++); in++;
         }
         if (c3_o) {newindis->setValue(ende+halfstep,indisIn++); in++;}
         newindis->setValue(ende,indisIn++); in++;
         newlengths->push_back(in);  
#ifdef DEFINE_SHOWROOM
         _numtris+=(in-2);
#endif
      } else {    // -> finer_o = 1 + finer_r = 1            
         newtypes->push_back(GL_TRIANGLE_FAN);
         Int32 s = start;//(breite-1-step);      
         Int32 a=s + halfstep + halfstep*breite;
         Int32 b=s + step*breite + step;
         Int32 u;
         Int32 in=0;
         newindis->setValue(a,indisIn++); in++;
         for (u=b; u>s+step; u-=(interstep_r*breite)){
            newindis->setValue(u,indisIn++); in++;
         }
         for (u=s+step; u>=s; u-=interstep_o){
            newindis->setValue(u,indisIn++); in++;
         }
         newindis->setValue(a-halfstep,indisIn++); in++;
         newindis->setValue(s+step*breite,indisIn++); in++;
         newindis->setValue(b-halfstep,indisIn++); in++;
         newindis->setValue(b,indisIn++); in++;
         newlengths->push_back(in);  
#ifdef DEFINE_SHOWROOM
         _numtris+=(in-2);
#endif
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
   Int32 halfstep = (step/2);     // is 0.5 for wsdmaxdepth=0!
   Int32 s = varrayOffset;  
   Int32 e = s + (step*breite) + step;
   Int32 u,in=0;
   Int32 a=s + halfstep + halfstep*breite;
   if (wsdmaxdepth == 0) {       // use this instead:
      setQuadOrTriangle(newlengths,newtypes,newindis,indisIn,s,s+2,s+3,s+1);
   } else {     
      newtypes->push_back(GL_TRIANGLE_FAN);
      newindis->setValue(a,indisIn++); in++;
      for (u=s; u<s+(step*breite); u+=(interstep_l*breite)) {
         newindis->setValue(u,indisIn++); in++;
      }
      for (u=s+(step*breite); u<e; u+=interstep_u) {
         newindis->setValue(u,indisIn++); in++;
      }
      for (u=e; u>s+step; u-=(interstep_r*breite)) {
         newindis->setValue(u,indisIn++); in++;
      }
      for (u=s+step; u>=s; u-=interstep_o) {
         newindis->setValue(u,indisIn++); in++;
      }
      newlengths->push_back(in);    
#ifdef DEFINE_SHOWROOM
      _numtris+=(in-2);
#endif
   }
}


template<>
void WSDdat<OSG::Vec3f, TRIANGLE>::setupStrip(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
      OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
      OSG::GeoIndicesUI32::StoredFieldType*  newindis,
      UInt32 &indisIn, Int32 s1, Int32 s2, Int32 add)
{
   newtypes->push_back(GL_TRIANGLE_STRIP);  
   for (Int32 j=0;j<wsdinnerindexwidth[solltiefe];j++){
      newindis->setValue(s1,indisIn++);
      newindis->setValue(s2,indisIn++);      // triangle-edge from c0 to c2!
      s1+=add;
      s2+=add;
   }
   newlengths->push_back(wsdinnerindexwidth[solltiefe]*2);
#ifdef DEFINE_SHOWROOM
   _numtris+=((wsdinnerindexwidth[solltiefe]*2)-2);
#endif
}

template<>
void WSDdat<OSG::Vec3f, QUAD>::setupStrip(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
      OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
      OSG::GeoIndicesUI32::StoredFieldType*  newindis,
      UInt32 &indisIn, Int32 s1, Int32 s2, Int32 add)
{
   newtypes->push_back(GL_QUAD_STRIP);  
   for (Int32 j=0;j<wsdinnerindexwidth[solltiefe];j++){    
      newindis->setValue(s1,indisIn++);    
      newindis->setValue(s2,indisIn++);    
      s1+=add;
      s2+=add;
   }
   newlengths->push_back(wsdinnerindexwidth[solltiefe]*2);
#ifdef DEFINE_SHOWROOM
   _numquads+=((wsdinnerindexwidth[solltiefe])-1);
#endif
}

template<>
void WSDdat<OSG::Vec4f, QUAD>::setupStrip(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
      OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
      OSG::GeoIndicesUI32::StoredFieldType*  newindis,
      UInt32 &indisIn, Int32 s1, Int32 s2, Int32 add)
{
   newtypes->push_back(GL_QUAD_STRIP);  
   for (Int32 j=0;j<wsdinnerindexwidth[solltiefe];j++){
      newindis->setValue(s1,indisIn++);
      newindis->setValue(s2,indisIn++);
      s1+=add;
      s2+=add;
   }
   newlengths->push_back(wsdinnerindexwidth[solltiefe]*2);
#ifdef DEFINE_SHOWROOM
   _numquads+=((wsdinnerindexwidth[solltiefe])-1);
#endif
}

template<class WSDVector, int mtype>
void WSDdat<WSDVector, mtype>::setupStrip(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
      OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
      OSG::GeoIndicesUI32::StoredFieldType*  newindis,
      UInt32 &indisIn, Int32 s1, Int32 s2, Int32 add)
{
}

template<class WSDVector, int mtype>
void WSDdat<WSDVector, mtype>::setupHalfStrip(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
      OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
      OSG::GeoIndicesUI32::StoredFieldType*  newindis,
      UInt32 &indisIn, Int32 s1, Int32 s2, Int32 add, Int32 width)
{
   newtypes->push_back(GL_TRIANGLE_STRIP);  
   for (Int32 j=0;j<width;j++){
      newindis->setValue(s1,indisIn++);
      newindis->setValue(s2,indisIn++);
      s1+=add;
      s2+=add;
   }
   newindis->setValue(s2,indisIn++);  
   newlengths->push_back((width*2)+1);
#ifdef DEFINE_SHOWROOM
   _numtris+=(((width*2)+1)-2);
#endif
}


template<>
void WSDdat<OSG::Vec3f, TRIANGLE>::setQuadOrTriangle(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
      OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
      OSG::GeoIndicesUI32::StoredFieldType*  newindis,
      UInt32 &indisIn, Int32 p1, Int32 p2, Int32 p3, Int32 p4)
{
   newtypes->push_back(GL_TRIANGLES);
   newindis->setValue(p1,indisIn++); 
   newindis->setValue(p2,indisIn++);
   newindis->setValue(p3,indisIn++); 
   newindis->setValue(p3,indisIn++);
   newindis->setValue(p4,indisIn++); 
   newindis->setValue(p1,indisIn++); 
   newlengths->push_back(6);
#ifdef DEFINE_SHOWROOM
   _numtris+=2;
#endif
}


template<class WSDVector, int mtype>
void WSDdat<WSDVector, mtype>::setQuadOrTriangle(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
      OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
      OSG::GeoIndicesUI32::StoredFieldType*  newindis,
      UInt32 &indisIn, Int32 p1, Int32 p2, Int32 p3, Int32 p4)
{
   newtypes->push_back(GL_QUADS);
   newindis->setValue(p1,indisIn++); 
   newindis->setValue(p2,indisIn++);
   newindis->setValue(p3,indisIn++); 
   newindis->setValue(p4,indisIn++);   
   newlengths->push_back(4);
#ifdef DEFINE_SHOWROOM
   _numquads++;
#endif
}

template<class WSDVector, int mtype>
void WSDdat<WSDVector, mtype>::setSpecialTriangle(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
      OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
      OSG::GeoIndicesUI32::StoredFieldType*  newindis,
      UInt32 &indisIn, Int32 p1, Int32 p2, Int32 p3)
{
   newtypes->push_back(GL_TRIANGLES);
   newindis->setValue(p1,indisIn++); 
   newindis->setValue(p2,indisIn++);
   newindis->setValue(p3,indisIn++); 
   newlengths->push_back(3);
#ifdef DEFINE_SHOWROOM
   _numtris++;
#endif
}




























