/*---------------------------------------------------------------------------*\
 *                       OpenSG - Dynamic Subdivision                        *
 *                                                                           *
 *                                                                           *
 *            contact: v.settgast@cg.cs.tu-bs.de * 
 *                                                                           *
\*---------------------------------------------------------------------------*/

#include <assert.h>
#include "OSGwsdsubdiv.h"
OSG_USING_NAMESPACE

//////////////////////////////////////////////////////////////////////
// mtype == QUAD !!!
//////////////////////////////////////////////////////////////////////

static void homogenize(OSG::Vec4f &v)
{  
   if(v[3]!=0.0) {
      v[0] = v[0]/v[3];
      v[1] = v[1]/v[3];
      v[2] = v[2]/v[3];
      v[3] = 1.0;
   } else {
      v=Vec4f(0,0,0,0);    // error case      
   }
}

////////////////////////////////////////////////////////////////
// new subdivision points

template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::vertexpoint
(VectorType& v, 
 VectorType& v0, VectorType& v1, VectorType& v2, VectorType& v3, VectorType& v4, 
 VectorType& w1, VectorType& w2, VectorType& w3, VectorType& w4)
{
   v= v0*0.5625 + (v1 + v2 + v3 + v4)*0.09375 + (w1 + w2 + w3 + w4)*0.015625;   
}

template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::vertexpointmulti
(VectorType& v, VectorType& v0, VectorType* array, OSG::Int32 val)
{
   OSG::Vec4f v4(0.0,0.0,0.0,0.0);
   OSG::Vec4f w4(0.0,0.0,0.0,0.0);
   OSG::Real32 alpha = 1.0f - (3.0f / (2.0f * (OSG::Real32)val)) - (1.0f / (4.0f * (OSG::Real32)val));
   OSG::Real32 beta  = 3.0f / (2.0f * (OSG::Real32)val * (OSG::Real32)val);
   OSG::Real32 gamma = 1.0f / (4.0f * (OSG::Real32)val * (OSG::Real32)val);
   for (OSG::Int32 i=0; i<(val*2); i=i+2) {
      v4 = v4 + array[i + 1];
      w4 = w4 + array[i];
   }      
   v= v0*alpha + v4*beta + w4*gamma;   
}


template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::edgepoint
(VectorType& v, VectorType& v1, VectorType& v2, 
 VectorType& w1, VectorType& w2,VectorType& w3, VectorType& w4, OSG::Int32 crease)
{
   if (crease == 0) {
      v= (v1 + v2)*0.375 + (w1 + w2 + w3 + w4)*0.0625;
   } else {
      v= (v1 + v2)*0.5;      
   }
}

template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::facepoint
(VectorType& v, VectorType& v1, VectorType& v2, VectorType& v3, VectorType& v4)
{
   v=(v1 + v2 + v3 +v4)*0.25;
}


// crease edges

template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::vertexpointcrease
(VectorType& v, VectorType& v0, VectorType& v1, VectorType& v2)
{
   v= v0*0.75 + (v1 + v2)*0.125;   
}


/////////////////////////////////////


template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::innernormal
(OSG::UInt32& nwi, 
 VectorType& v0, VectorType& v1, VectorType& v2, VectorType& v3, VectorType& v4, 
 VectorType& w1, VectorType& w2, VectorType& w3, VectorType& w4)
{   
   OSG::Vec4f v;
   OSG::Vec4f t1(0.0,0.0,0.0,0.0);
   OSG::Vec4f t2(0.0,0.0,0.0,0.0);
   OSG::Real32 val = 4.0f;
   OSG::Real32 ak = 1.0f + osgcos(2.0f * Pi / val) + osgcos(Pi / val) * osgsqrt(2.0f*(9.0f + osgcos(2.0f * Pi / val)));
   OSG::Real32 b1,b2,c1,c2;

   //i=0
   OSG::Int32 i=0;

   b1 = ak * osgcos(2*Pi*(i+1) / val);
   b2 = ak * osgcos(2*Pi*i / val);
   c1 = osgcos(2*Pi*(i+1) / val) + osgcos(2*Pi*(i+2) / val);
   c2 = osgcos(2*Pi*i / val) + osgcos(2*Pi*(i+1) / val);
   t1+=v1 * b1;
   t2+=v1 * b2;  
   t1+=w1 * c1;
   t2+=w1 * c2;
   i++;    // i=1

   b1 = ak * osgcos(2*Pi*(i+1) / val);
   b2 = ak * osgcos(2*Pi*i / val);
   c1 = osgcos(2*Pi*(i+1) / val) + osgcos(2*Pi*(i+2) / val);
   c2 = osgcos(2*Pi*i / val) + osgcos(2*Pi*(i+1) / val);
   t1+=v2 * b1;
   t2+=v2 * b2;  
   t1+=w2 * c1;
   t2+=w2 * c2;
   i++;    // i=2

   b1 = ak * osgcos(2*Pi*(i+1) / val);
   b2 = ak * osgcos(2*Pi*i / val);
   c1 = osgcos(2*Pi*(i+1) / val) + osgcos(2*Pi*(i+2) / val);
   c2 = osgcos(2*Pi*i / val) + osgcos(2*Pi*(i+1) / val);
   t1+=v3 * b1;
   t2+=v3 * b2;  
   t1+=w3 * c1;
   t2+=w3 * c2;
   i++;    // i=2

   b1 = ak * osgcos(2*Pi*(i+1) / val);
   b2 = ak * osgcos(2*Pi*i / val);
   c1 = osgcos(2*Pi*(i+1) / val) + osgcos(2*Pi*(i+2) / val);
   c2 = osgcos(2*Pi*i / val) + osgcos(2*Pi*(i+1) / val);
   t1+=v4 * b1;
   t2+=v4 * b2;  
   t1+=w4 * c1;
   t2+=w4 * c2;

   v = t1.cross(t2);     
   v.normalize();
   ppatch->mySharedFields->limitnormals->setValue(Vec3f(v.x(),v.y(),v.z()), nwi);     
   nwi+=zweihoch[(wsdmaxdepth - targetdepth)];   
}
template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::innernormalmulti
(OSG::UInt32& nwi, VectorType *array, OSG::Int32 val)
{   
   OSG::Vec4f v;
   OSG::Vec4f t1(0.0,0.0,0.0,0.0);
   OSG::Vec4f t2(0.0,0.0,0.0,0.0);
   OSG::Real32 ak = 1.0f + osgcos(2.0f * Pi / val) + osgcos(Pi / val) * osgsqrt(2.0f*(9.0f + osgcos(2.0f * Pi / val)));
   OSG::Real32 b1,b2,c1,c2;     
   OSG::Int32 j=0;
   for (OSG::Int32 i=0; i<val; i++, j=j+2) {
      b1 = ak * osgcos(2*Pi*(i+1) / val);
      b2 = ak * osgcos(2*Pi*i / val);
      c1 = osgcos(2*Pi*(i+1) / val) + osgcos(2*Pi*(i+2) / val);
      c2 = osgcos(2*Pi*i / val) + osgcos(2*Pi*(i+1) / val);
      t1+=array[j+1] * b1;
      t2+=array[j+1] * b2;  
      t1+=array[j] * c1;
      t2+=array[j] * c2;
   }
   v = t1.cross(t2);     
   v.normalize();
   ppatch->mySharedFields->limitnormals->setValue(Vec3f(v.x(),v.y(),v.z()), nwi);     
   nwi+=zweihoch[(wsdmaxdepth - targetdepth)];   
}


template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::creasenormal
(OSG::UInt32& nwi, VectorType& v0, VectorType *array, OSG::Int32 val, OSG::Int32 a, OSG::Int32 b)
{   
   OSG::Vec4f v;
   OSG::Vec4f t1(0.0,0.0,0.0,0.0);
   OSG::Vec4f t2(0.0,0.0,0.0,0.0);

   if (val < 4) val=4;
   if (b < a) b=b+(val*2);
   t2 = (array[b%(val*2)] - array[a]);
   if (abs(b-a) < 3) {    
      t1 = (v0 * 2.0) + ((array[a] + array[b%(val*2)]) * -1.0);    
      v = t2.cross(t1);
   } else {
      OSG::Real32 k = fabs(((OSG::Real32)b-(OSG::Real32)a)/2);
      OSG::Real32 pidk = Pi / k;
      OSG::Real32 rk = (osgcos(pidk) + 1.0f) / (k * osgsin(pidk) * (3.0f + osgcos(pidk)));
      OSG::Real32 b0 = -1.0f * rk * (1.0f + 2.0f * osgcos(pidk));
      OSG::Real32 c = osgsin(pidk) / ((3.0f + osgcos(pidk)) * k);
      OSG::Vec4f betat1 = array[a] * b0;
      OSG::Vec4f gammat1 = array[(a+1)%(val*2)] * c;
      for (OSG::Int32 ii=1; ii<k; ii++) {
         OSG::Real32 bi = (4.0f * osgsin((OSG::Real32)ii * pidk)) / ((3.0f + osgcos(pidk)) * k);
         betat1+= array[(a+(ii*2))%(val*2)] * bi;
         c = (1.0f * (osgsin((OSG::Real32)ii * pidk) + osgsin(((OSG::Real32)ii + 1.0f) * pidk))) / ((3.0f + osgcos(pidk)) * k);
         gammat1+= array[(a+(ii*2)+1)%(val*2)] * c;
      }
      betat1+= array[b%(val*2)] * b0;
      //assert(((a+(ii*2)+2)%(val*2)) == b);
      OSG::Real32 aa = 4.0f * rk * (osgcos(pidk) - 1.0f);
      t1 = (v0 * aa) + betat1 + gammat1;
      v = t1.cross(t2);
   }
   v.normalize();
   ppatch->mySharedFields->limitnormals->setValue(Vec3f(v.x(),v.y(),v.z()), nwi);
   nwi+=zweihoch[(wsdmaxdepth - targetdepth)];  
}

template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::regcreasenormal
(OSG::UInt32& nwi, 
 VectorType& v0, VectorType& v1, VectorType& w1, VectorType& v2, VectorType& w2, VectorType& v3)
{   
   OSG::Vec4f v;
   OSG::Vec4f t1(0.0,0.0,0.0,0.0);
   OSG::Vec4f t2(0.0,0.0,0.0,0.0);
   t2 = (v1 - v3);
   OSG::Real32 k = 2;
   OSG::Real32 pidk = Pi / k;
   OSG::Real32 rk = (osgcos(pidk) + 1.0f) / (k * osgsin(pidk) * (3.0f + osgcos(pidk)));
   OSG::Real32 b0 = -1.0f * rk * (1.0f + 2.0f * osgcos(pidk));
   OSG::Real32 c = osgsin(pidk) / ((3.0f + osgcos(pidk)) * k);
   OSG::Vec4f betat1 = v1 * b0;
   OSG::Vec4f gammat1 = w1 * c;
   OSG::Int32 ii=1;
   OSG::Real32 bi = (4.0f * osgsin((OSG::Real32)ii * pidk)) / ((3.0f + osgcos(pidk)) * k);

   betat1+= (v2 * bi);
   c = (1.0f * (osgsin((OSG::Real32)ii * pidk) + osgsin(((OSG::Real32)ii + 1.0f) * pidk))) / ((3.0f + osgcos(pidk)) * k);
   gammat1+= (w2 * c);
   betat1+= v3 * b0;
   OSG::Real32 a = 4.0f * rk * (osgcos(pidk) - 1.0f);
   t1 = (v0 * a) + betat1 + gammat1;

   v = t2.cross(t1);
   v.normalize();
   ppatch->mySharedFields->limitnormals->setValue(Vec3f(v.x(),v.y(),v.z()), nwi);
   nwi+=zweihoch[(wsdmaxdepth - targetdepth)];  
}


////////////////////////////////////////////////////////////////
// final points


template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::limitpoint
(OSG::UInt32& lwi, 
 VectorType& v0, VectorType& v1, VectorType& v2, VectorType& v3, VectorType& v4, 
 VectorType& w1, VectorType& w2, VectorType& w3, VectorType& w4)
{
   OSG::Vec4f v;
   v= v0 * 0.4444444444444444444444444f +       
      ( (v1 + v2 + v3 + v4) * 4.0f + w1 + w2 + w3 + w4) * 0.0277777777777777777777777f;   
   homogenize(v);
   ppatch->mySharedFields->limitpoints->setValue(Pnt3f(v.x(),v.y(),v.z()), lwi);   
   lwi+=zweihoch[(wsdmaxdepth - targetdepth)];   
}

template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::limitpointcrease
(OSG::UInt32& lwi, VectorType& v0, VectorType& v1, VectorType& v2)
{   
   OSG::Vec4f v;
   v= v0 * 0.6666666666666666666666666f +       
      ( v1 + v2 ) * 0.1666666666666666666666666f;
   homogenize(v);
   ppatch->mySharedFields->limitpoints->setValue(Pnt3f(v.x(),v.y(),v.z()), lwi);   
   lwi+=zweihoch[(wsdmaxdepth - targetdepth)];   
}

template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::limitpointmulti
(OSG::UInt32& lwi, VectorType& v0, VectorType *array, OSG::Int32 val)
{
   OSG::Real32 alpha = 1.0f - (5.0f / ((OSG::Real32)val + 5.0f));
   OSG::Real32 gamma = (1.0f / ((OSG::Real32)val + 5.0f))/(OSG::Real32)val;
   OSG::Real32 beta  = gamma * 4.0f;
   OSG::Vec4f v4(0.0,0.0,0.0,0.0);
   OSG::Vec4f w4(0.0,0.0,0.0,0.0); //; w4.setValues
   for (OSG::Int32 i=0; i<(val*2); i=i+2) {
      v4 = v4 + array[i + 1];  
      w4 = w4 + array[i];
   }    
   OSG::Vec4f v;
   v= (v0*alpha) + (v4*beta) + (w4*gamma);      
   homogenize(v);
   ppatch->mySharedFields->limitpoints->setValue(Pnt3f(v.x(),v.y(),v.z()), lwi);
   lwi+=zweihoch[(wsdmaxdepth - targetdepth)];   
}

// CF moved to here

template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::initwsdsubdiv(void)
{
}

//////////////////////////////////////////////////////////////////////
// update corners data
template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::corsupdate (void)
{
   OSG::Vec4f* tabA = ppatch->ptabA;
   VectorCAType& corA = *(ppatch->pcorA);
   OSG::Int32 val0 = ppatch->valenz[0];
   OSG::Int32 val1 = ppatch->valenz[1];
   OSG::Int32 val2 = ppatch->valenz[2];
   OSG::Int32 val3 = ppatch->valenz[3];
   OSG::Int32 bA=ppatch->breiteA;
   OSG::Int32 i = ppatch->tabsize-2;
   OSG::Vec4f rep0,rep1,rep2,rep3,rep4,rep5,rep6,rep7;    

   rep0 = tabA[2*bA+0];
   rep1 = tabA[2*bA+1];
   rep2 = tabA[2*bA+2];
   rep3 = tabA[1*bA+2];
   rep4 = tabA[0*bA+2];
   rep5 = tabA[0*bA+1];
   rep7 = tabA[1*bA+0];
   corA[0][0] = rep0;
   corA[0][1] = rep1;
   corA[0][2] = rep2;
   corA[0][3] = rep3;
   corA[0][4] = rep4;
   corA[0][5] = rep5;
   if (val0 == 3) corA[0][7] = rep7;

   rep0 = tabA[(i+1)*bA+2];
   rep1 = tabA[i*bA+2];
   rep2 = tabA[(i-1)*bA+2];
   rep3 = tabA[(i-1)*bA+1];
   rep4 = tabA[(i-1)*bA+0];
   rep5 = tabA[i*bA+0];
   rep7 = tabA[(i+1)*bA+1];
   corA[1][0] = rep0;
   corA[1][1] = rep1;
   corA[1][2] = rep2;
   corA[1][3] = rep3;
   corA[1][4] = rep4;
   corA[1][5] = rep5;
   if (val1 == 3) corA[1][7] = rep7;

   rep0 = tabA[(i-1)*bA+i+1];
   rep1 = tabA[(i-1)*bA+i];
   rep2 = tabA[(i-1)*bA+i-1];
   rep3 = tabA[i*bA+i-1];
   rep4 = tabA[(i+1)*bA+i-1];
   rep5 = tabA[(i+1)*bA+i];
   rep7 = tabA[i*bA+i+1];
   corA[2][0] = rep0;
   corA[2][1] = rep1;
   corA[2][2] = rep2;
   corA[2][3] = rep3;
   corA[2][4] = rep4;
   corA[2][5] = rep5;
   if (val2 == 3) corA[2][7] = rep7;

   rep0 = tabA[0*bA+i-1];
   rep1 = tabA[1*bA+i-1];
   rep2 = tabA[2*bA+i-1];
   rep3 = tabA[2*bA+i];
   rep4 = tabA[2*bA+i+1];
   rep5 = tabA[1*bA+i+1];
   rep7 = tabA[0*bA+i];
   corA[3][0] = rep0;
   corA[3][1] = rep1;
   corA[3][2] = rep2;
   corA[3][3] = rep3;
   corA[3][4] = rep4;
   corA[3][5] = rep5;
   if (val3 == 3) corA[3][7] = rep7;
}


/////////////////////////////////////////////////////////////////////
// new subdiv-rows

template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::inneredgerow 
(OSG::Int32 ri, OSG::Int32 wi, OSG::Int32& i, OSG::Int32& j)
{
   OSG::Vec4f* tabA = ppatch->ptabA;
   OSG::Vec4f* tabB = ppatch->ptabB;
   OSG::Int32 bA=ppatch->breiteA;
   OSG::Int32 bB=wsddepthindexarray[wsdmaxdepth]+2;   
   facepoint(tabB[wi*bB+j],
    tabA[(ri+1)*bA+i], tabA[(ri+1)*bA+i+1],
    tabA[ri*bA+i+1], tabA[ri*bA+i]);
   ++j; ++i;
   assert(i==2);
   assert(j==3);              // j  ist spaltenindex von tabB
   for(; i<ppatch->tabsize-2;++i) {      // i  ist spaltenindex von tabA
      edgepoint(tabB[wi*bB+j],        // ri ist Zeilenindex von tabA
       tabA[(ri+1)*bA+i],tabA[ri*bA+i],      // wi ist Zeilenindex von tabB
       tabA[(ri+1)*bA+i+1],tabA[ri*bA+i+1],
       tabA[ri*bA+i-1],tabA[(ri+1)*bA+i-1],0);
      ++j;
      facepoint(tabB[wi*bB+j],
       tabA[(ri+1)*bA+i], tabA[(ri+1)*bA+i+1],
       tabA[ri*bA+i+1], tabA[ri*bA+i]);
      ++j;
   }
}


template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::firstedgerow (void)
{
   OSG::Int32 val0 = ppatch->valenz[0];
   OSG::Int32 val3 = ppatch->valenz[3];
   OSG::Int32 wi = 0;
   OSG::Int32 ri = 0;
   OSG::Int32 i  = 0;
   OSG::Int32 j  = 0;
   OSG::Int32 c  = 0;
   OSG::Int32 d  = 0;   
   OSG::Vec4f* tabA = ppatch->ptabA;
   OSG::Vec4f* tabB = ppatch->ptabB;
   VectorCAType& corA = *(ppatch->pcorA);
   VectorCAType& corB = *(ppatch->pcorB);
   OSG::Int32 bA=ppatch->breiteA;
   OSG::Int32 bB=wsddepthindexarray[wsdmaxdepth]+2;   
   ++j; ++i;

   if (val0 < 4) {    
      edgepoint(tabB[wi*bB+j], 
      tabA[(ri+1)*bA+i],tabA[ri*bA+i], 
      tabA[(ri+1)*bA+i+1],tabA[ri*bA+i+1],
      tabA[(ri+2)*bA+0],tabA[(ri+2)*bA+1],ppatch->crease[0]);    
   } else {          // val0 > 3    
      d=6;    
      for (c=6; c<(val0*2); c=c+2) {
         facepoint(corB[0][d],
          corA[0][c-1],   corA[0][c],
          corA[0][c+1], tabA[1*bA+1]);
         d++;
         edgepoint(corB[0][d],
          corA[0][c+1], tabA[1*bA+1],
          corA[0][(c+3)%(val0*2)], corA[0][(c+2)%(val0*2)],
          corA[0][c],   corA[0][c-1],ppatch->corcrease[0][c/2]);
         d++;
      }  
      edgepoint(tabB[wi*bB+j], 
       tabA[(ri+1)*bA+i],tabA[ri*bA+i], 
       tabA[(ri+1)*bA+i+1],tabA[ri*bA+i+1],
       corA[0][6],corA[0][7],ppatch->crease[0]);
   }     
   ++j;
//************************************
   inneredgerow(ri, wi, i, j);
//************************************
   // j=new tabsize-2
   // wi=0
   // ri=0
   // i=tabsize-2
   if (val3 < 4) {
      edgepoint(tabB[wi*bB+j], 
      tabA[(ri+1)*bA+i],tabA[ri*bA+i], 
      tabA[(ri+2)*bA+i],tabA[(ri+2)*bA+i+1],
      tabA[ri*bA+i-1],tabA[(ri+1)*bA+i-1],ppatch->crease[1]);
   } else {    // val3>3        
      edgepoint(tabB[wi*bB+j], 
      tabA[(ri+1)*bA+i],tabA[ri*bA+i], 
      corA[3][((val3-1)*2)],corA[3][((val3-1)*2) - 1],
      tabA[ri*bA+i-1],tabA[(ri+1)*bA+i-1],ppatch->crease[1]);      
      d=6;
      for (c=6; c<(val3*2); c=c+2) {
         facepoint(corB[3][d],
          corA[3][c-1], corA[3][c],
          corA[3][c+1],   tabA[1*bA+i]);
         d++;
         edgepoint(corB[3][d],
          corA[3][c+1], tabA[1*bA+i],
          corA[3][c-1], corA[3][(c+3)%(val3*2)],
          corA[3][c],   corA[3][(c+2)%(val3*2)],ppatch->corcrease[3][c/2]);
         d++;
      }
   }
   //assert(i==ppatch->tabsize-2);
   //assert(j==2*(ppatch->tabsize)-3-2);
}

template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::edgerow
(OSG::Int32 ri, OSG::Int32 wi)
{
   OSG::Int32 i=0;
   OSG::Int32 j=0;
   OSG::Vec4f* tabA = ppatch->ptabA;
   OSG::Vec4f* tabB = ppatch->ptabB;
   OSG::Int32 bA=ppatch->breiteA;
   OSG::Int32 bB=wsddepthindexarray[wsdmaxdepth]+2;   
   facepoint(tabB[wi*bB+j],
    tabA[(ri+1)*bA+i], tabA[(ri+1)*bA+i+1],
    tabA[ri*bA+i+1], tabA[ri*bA+i]);
   ++i; ++j;
   edgepoint(tabB[wi*bB+j], 
    tabA[(ri+1)*bA+i],tabA[ri*bA+i], 
    tabA[(ri+1)*bA+i+1],tabA[ri*bA+i+1],
    tabA[ri*bA+i-1],tabA[(ri+1)*bA+i-1],ppatch->crease[5]);
   ++j;
//*************************
   inneredgerow(ri, wi, i, j);
//*************************
   edgepoint(tabB[wi*bB+j], 
    tabA[(ri+1)*bA+i],tabA[ri*bA+i], 
    tabA[(ri+1)*bA+i+1],tabA[ri*bA+i+1],
    tabA[ri*bA+i-1],tabA[(ri+1)*bA+i-1],ppatch->crease[6]);
   ++j;
   facepoint(tabB[wi*bB+j],
    tabA[(ri+1)*bA+i], tabA[(ri+1)*bA+i+1],
    tabA[ri*bA+i+1], tabA[ri*bA+i]);
//   assert(i==ppatch->tabsize-2);
//   assert(j==2*(ppatch->tabsize)-3-1);
}

template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::lastedgerow
(OSG::Int32 ri, OSG::Int32 wi)
{
  OSG::Int32 i=0;
  OSG::Int32 j=0;
  OSG::Int32 c=0;
  OSG::Int32 d=0;  
  OSG::Int32 val1 = ppatch->valenz[1];
  OSG::Int32 val2 = ppatch->valenz[2];
  OSG::Vec4f* tabA = ppatch->ptabA;
  OSG::Vec4f* tabB = ppatch->ptabB;
  VectorCAType& corA = *(ppatch->pcorA);
  VectorCAType& corB = *(ppatch->pcorB);
  OSG::Int32 bA=ppatch->breiteA;
  OSG::Int32 bB=wsddepthindexarray[wsdmaxdepth]+2;   

  if (val1 < 4) {
    ++i; ++j;
    edgepoint(tabB[wi*bB+j], 
     tabA[(ri+1)*bA+i],tabA[ri*bA+i], 
     tabA[(ri+1)*bA+i+1],tabA[ri*bA+i+1],
     tabA[(ri-1)*bA+i-1],tabA[(ri-1)*bA+i],ppatch->crease[10]);
  } else {
    d=6;
    for (c=6; c<(val1*2); c=c+2) {
         facepoint(corB[1][d],
          corA[1][c-1],   corA[1][c],
          corA[1][c+1], tabA[ri*bA+1]);
         d++;
         edgepoint(corB[1][d],
          corA[1][c+1], tabA[ri*bA+1],
          corA[1][(c+3)%(val1*2)], corA[1][(c+2)%(val1*2)],
          corA[1][c],   corA[1][c-1],ppatch->corcrease[1][c/2]);  
         d++;
    }    
    ++i; ++j;    
    edgepoint(tabB[wi*bB+j], 
     tabA[(ri+1)*bA+i],tabA[ri*bA+i], 
     tabA[(ri+1)*bA+i+1],tabA[ri*bA+i+1],
     corA[1][((val1-1)*2)-1],corA[1][((val1-1)*2)],ppatch->crease[10]);      
   }
   ++j;
//*************************
  inneredgerow(ri, wi, i, j);
//*************************
   if (val2 < 4) {
      edgepoint(tabB[wi*bB+j], 
       tabA[(ri+1)*bA+i],tabA[ri*bA+i], 
       tabA[(ri-1)*bA+i],tabA[(ri-1)*bA+i+1],
       tabA[ri*bA+i-1],tabA[(ri+1)*bA+i-1],ppatch->crease[11]);
   } else {  
      edgepoint(tabB[wi*bB+j], 
       corA[2][5],tabA[ri*bA+i], 
       corA[2][6],corA[2][7],
       tabA[ri*bA+i-1],tabA[(ri+1)*bA+i-1],ppatch->crease[11]);
      ++j;
      d=6;
      for (c=6; c<(val2*2); c=c+2) {
         facepoint(corB[2][d],
          tabA[ri*bA+i],   corA[2][c],
          corA[2][c+1], corA[2][c-1]);
         d++;
         edgepoint(corB[2][d],
          corA[2][c+1], tabA[ri*bA+i],
          corA[2][(c+3)%(val2*2)], corA[2][(c+2)%(val2*2)],
          corA[2][c],   corA[2][c-1],ppatch->corcrease[2][c/2]);
         d++;
      }
   }
}

template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::innervertexrow
(OSG::Int32 ri, OSG::Int32 wi, OSG::Int32& i, OSG::Int32& j)
{
   OSG::Vec4f* tabA = ppatch->ptabA;
   OSG::Vec4f* tabB = ppatch->ptabB;
   OSG::Int32 bA=ppatch->breiteA;
   OSG::Int32 bB=wsddepthindexarray[wsdmaxdepth]+2;   
   edgepoint(tabB[wi*bB+j], 
    tabA[ri*bA+i],tabA[ri*bA+i+1], 
    tabA[(ri+1)*bA+i],tabA[(ri+1)*bA+i+1],
    tabA[(ri-1)*bA+i+1],tabA[(ri-1)*bA+i],0);
   ++j; ++i;
   assert(i==2);
   assert(j==3);
   for(; i<ppatch->tabsize-2;++i) {
      vertexpoint(tabB[wi*bB+j],
       tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[ri*bA+i+1],
       tabA[(ri-1)*bA+i], tabA[ri*bA+i-1],
       tabA[(ri+1)*bA+i-1], tabA[(ri+1)*bA+i+1],
       tabA[(ri-1)*bA+i+1], tabA[(ri-1)*bA+i-1]);
      ++j;
      edgepoint(tabB[wi*bB+j], 
       tabA[ri*bA+i],tabA[ri*bA+i+1], 
       tabA[(ri+1)*bA+i],tabA[(ri+1)*bA+i+1],
       tabA[(ri-1)*bA+i+1],tabA[(ri-1)*bA+i],0);
      ++j;
   }
}

template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::creaseinnervertexrow
(OSG::Int32 ri, OSG::Int32 wi, OSG::Int32& i, OSG::Int32& j, OSG::Int32 ce)
{
   OSG::Vec4f* tabA = ppatch->ptabA;
   OSG::Vec4f* tabB = ppatch->ptabB;
   OSG::Int32 bA=ppatch->breiteA;
   OSG::Int32 bB=wsddepthindexarray[wsdmaxdepth]+2;   
   edgepoint(tabB[wi*bB+j], 
    tabA[ri*bA+i],tabA[ri*bA+i+1], 
    tabA[(ri+1)*bA+i],tabA[(ri+1)*bA+i+1],
    tabA[(ri-1)*bA+i+1],tabA[(ri-1)*bA+i],ce);
   ++j; ++i;
   assert(i==2);
   assert(j==3);
   for(; i<ppatch->tabsize-2;++i) {
      if (ce == 0) {
         vertexpoint(tabB[wi*bB+j],
          tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[ri*bA+i+1],
          tabA[(ri-1)*bA+i], tabA[ri*bA+i-1],
          tabA[(ri+1)*bA+i-1], tabA[(ri+1)*bA+i+1],
          tabA[(ri-1)*bA+i+1], tabA[(ri-1)*bA+i-1]);
      } else {
         vertexpointcrease(tabB[wi*bB+j],
          tabA[ri*bA+i], tabA[ri*bA+i-1], tabA[ri*bA+i+1]);
      }
      ++j;
      edgepoint(tabB[wi*bB+j], 
       tabA[ri*bA+i],tabA[ri*bA+i+1], 
       tabA[(ri+1)*bA+i],tabA[(ri+1)*bA+i+1],
       tabA[(ri-1)*bA+i+1],tabA[(ri-1)*bA+i],ce);
      ++j;
   }
}


template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::firstvertexrow (void)
{
   // regular case:
   //vertexrow(1,1);
   // valenz>=4:
   OSG::Int32 ri=1;
   OSG::Int32 wi=1;
   OSG::Int32 i=0;
   OSG::Int32 j=0;
   OSG::Int32 val0 = ppatch->valenz[0];
   OSG::Int32 val3 = ppatch->valenz[3];
   OSG::Vec4f* tabA = ppatch->ptabA;
   OSG::Vec4f* tabB = ppatch->ptabB;
   VectorCAType& corA = *(ppatch->pcorA);
   VectorCAType& corB = *(ppatch->pcorB);
   OSG::Int32 bA=ppatch->breiteA;
   OSG::Int32 bB=wsddepthindexarray[wsdmaxdepth]+2;   

   if (val0 > 3) {
      edgepoint(tabB[wi*bB+j], 
       tabA[ri*bA+i],tabA[ri*bA+i+1], 
       tabA[(ri+1)*bA+i],tabA[(ri+1)*bA+i+1],
       corA[0][((val0-1)*2)-1],corA[0][(val0-1)*2],ppatch->crease[2]);
   } else {  
      edgepoint(tabB[wi*bB+j], 
       tabA[ri*bA+i],tabA[ri*bA+i+1], 
       tabA[(ri+1)*bA+i],tabA[(ri+1)*bA+i+1],
       tabA[ri*bA+i+2],tabA[(ri-1)*bA+i+2],ppatch->crease[2]);
   }
   ++i; ++j;

   if (ppatch->iscorner[0] > 2) {
      OSG::Vec4f h = tabA[ri*bA+i];
      tabB[wi*bB+j] = h;
   } else {
      if (ppatch->iscorner[0] == 2) {
         vertexpointcrease(tabB[wi*bB+j], tabA[ri*bA+i],
          corA[0][(2*ppatch->crindis[0][0])+1],
          corA[0][(2*ppatch->crindis[0][1])+1]);
      } else {
         vertexpointmulti(tabB[wi*bB+j],
          tabA[ri*bA+i], corA[0], val0);
      }
   }
   ++j;
//*************************
   if (ppatch->crease[3] > 0) {
      creaseinnervertexrow(ri, wi,i,j,1);
   } else {
      innervertexrow(ri, wi,i,j);
   }
//*************************
   if (ppatch->iscorner[3] > 2) {
      OSG::Vec4f h = tabA[ri*bA+i];
      tabB[wi*bB+j] = h;
   } else {
      if (ppatch->iscorner[3] == 2) {
         vertexpointcrease(tabB[wi*bB+j], tabA[ri*bA+i],
          corA[3][2*(ppatch->crindis[3][0])+1],
          corA[3][2*(ppatch->crindis[3][1])+1]);
      } else {
         vertexpointmulti(tabB[wi*bB+j],
          tabA[ri*bA+i], corA[3], val3);
      }
   }
   ++j;
   if (val3 >3) {
      edgepoint(tabB[wi*bB+j], 
      tabA[ri*bA+i],tabA[ri*bA+i+1], 
      tabA[(ri+1)*bA+i],tabA[(ri+1)*bA+i+1],
      corA[3][6],corA[3][7],ppatch->crease[4]);
   } else {
      edgepoint(tabB[wi*bB+j], 
      tabA[ri*bA+i],tabA[ri*bA+i+1], 
      tabA[(ri+1)*bA+i],tabA[(ri+1)*bA+i+1],
      tabA[ri*bA+i-1],tabA[(ri-1)*bA+i-1],ppatch->crease[4]);
   }
   //assert(i==ppatch->tabsize-2);
   //assert(j==2*(ppatch->tabsize)-3-1);
}

template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::vertexrow (OSG::Int32 ri, OSG::Int32 wi)
{  
   OSG::Int32 i=0;
   OSG::Int32 j=0;
   OSG::Vec4f* tabA = ppatch->ptabA;
   OSG::Vec4f* tabB = ppatch->ptabB;
   OSG::Int32 bA=ppatch->breiteA;
   OSG::Int32 bB=wsddepthindexarray[wsdmaxdepth]+2;   
   edgepoint(tabB[wi*bB+j], 
    tabA[ri*bA+i],tabA[ri*bA+i+1], 
    tabA[(ri+1)*bA+i],tabA[(ri+1)*bA+i+1],
    tabA[(ri-1)*bA+i+1],tabA[(ri-1)*bA+i],0);
   ++i; ++j;
   if (ppatch->crease[5] == 0) {
      vertexpoint(tabB[wi*bB+j],
       tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[ri*bA+i+1],
       tabA[(ri-1)*bA+i], tabA[ri*bA+i-1],
       tabA[(ri+1)*bA+i-1], tabA[(ri+1)*bA+i+1],
       tabA[(ri-1)*bA+i+1], tabA[(ri-1)*bA+i-1]);
   } else {
      vertexpointcrease(tabB[wi*bB+j],
       tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[(ri-1)*bA+i]);
   }
   ++j;
//*************************
   innervertexrow(ri, wi,i,j);
//************************* 
   if (ppatch->crease[6] == 0) {
      vertexpoint(tabB[wi*bB+j],
       tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[ri*bA+i+1],
       tabA[(ri-1)*bA+i], tabA[ri*bA+i-1],
       tabA[(ri+1)*bA+i-1], tabA[(ri+1)*bA+i+1],
       tabA[(ri-1)*bA+i+1], tabA[(ri-1)*bA+i-1]);
   } else {
      vertexpointcrease(tabB[wi*bB+j],
       tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[(ri-1)*bA+i]);
   }
   ++j;
   edgepoint(tabB[wi*bB+j], 
    tabA[ri*bA+i],tabA[ri*bA+i+1], 
    tabA[(ri+1)*bA+i],tabA[(ri+1)*bA+i+1],
    tabA[(ri-1)*bA+i+1],tabA[(ri-1)*bA+i],0);

   assert(i==ppatch->tabsize-2);
   assert(j==2*(ppatch->tabsize)-3-1);
}

template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::lastvertexrow (OSG::Int32 ri, OSG::Int32 wi)
{   
   OSG::Int32 i=0;
   OSG::Int32 j=0;
   OSG::Int32 val1 = ppatch->valenz[1];
   OSG::Int32 val2 = ppatch->valenz[2];
   OSG::Vec4f* tabA  = ppatch->ptabA;
   OSG::Vec4f* tabB  = ppatch->ptabB;
   VectorCAType& corA = *(ppatch->pcorA);
   VectorCAType& corB = *(ppatch->pcorB);
   OSG::Int32 bA=ppatch->breiteA;
   OSG::Int32 bB=wsddepthindexarray[wsdmaxdepth]+2;   

   if (val1 > 3) {
      edgepoint(tabB[wi*bB+j], 
       corA[1][5],tabA[ri*bA+i+1], 
       corA[1][6], corA[1][7],    
       corA[1][4], corA[1][3],ppatch->crease[7]);
   } else {    // val1 <4  
      edgepoint(tabB[wi*bB+j], 
       tabA[ri*bA+i],tabA[ri*bA+i+1], 
       tabA[(ri+1)*bA+i+2], tabA[ri*bA+i+2],    
       tabA[(ri-1)*bA+i+1],tabA[(ri-1)*bA+i],ppatch->crease[7]);
   }
   ++i; ++j;
   if (ppatch->iscorner[1] > 2) {
      OSG::Vec4f h = tabA[ri*bA+i];
      tabB[wi*bB+j] = h;
   } else {
      if (ppatch->iscorner[1] == 2) {
         vertexpointcrease(tabB[wi*bB+j], tabA[ri*bA+i],
          corA[1][2*(ppatch->crindis[1][0])+1],
          corA[1][2*(ppatch->crindis[1][1])+1]);
      } else {
         vertexpointmulti(tabB[wi*bB+j],
          tabA[ri*bA+i], corA[1], val1);
      }
   }  
   ++j;
//*************************
   if (ppatch->crease[8] > 0) {
      creaseinnervertexrow(ri, wi,i,j,1);
   } else {
      innervertexrow(ri, wi,i,j);  
   }
//*************************
   if (ppatch->iscorner[2] > 2) {
      OSG::Vec4f h = tabA[ri*bA+i];
      tabB[wi*bB+j] = h;
   } else {
      if (ppatch->iscorner[2] == 2) {
         vertexpointcrease(tabB[wi*bB+j], tabA[ri*bA+i],
          corA[2][2*(ppatch->crindis[2][0])+1],
          corA[2][2*(ppatch->crindis[2][1])+1]);
      } else {
         vertexpointmulti(tabB[wi*bB+j],
          tabA[ri*bA+i], corA[2], val2);
      }
   }  
   ++j;
   if (val2 > 3) {
      edgepoint(tabB[wi*bB+j], 
       tabA[ri*bA+i],tabA[ri*bA+i+1], 
       corA[2][((val2-1)*2)-1],corA[2][((val2-1)*2)],
       tabA[(ri-1)*bA+i+1],tabA[(ri-1)*bA+i],ppatch->crease[9]);
   } else {
      edgepoint(tabB[wi*bB+j], 
       tabA[ri*bA+i],tabA[ri*bA+i+1], 
       tabA[(ri+1)*bA+i-1],tabA[ri*bA+i-1],
       tabA[(ri-1)*bA+i+1],tabA[(ri-1)*bA+i],ppatch->crease[9]);
   }
   assert(j==2*(ppatch->tabsize)-3-1);
}

//////////////////////////////////////////////////////////////////////
// final-rows

template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::innerlimitrow
(OSG::Int32 ri, OSG::UInt32& lwi, OSG::Int32& i, OSG::UInt32& nwi)
{
   OSG::Vec4f* tabA = ppatch->ptabA;
   OSG::Int32 bA=ppatch->breiteA;
   for(; i<ppatch->tabsize-2;++i) {
      limitpoint(lwi,
       tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[ri*bA+i+1],
       tabA[(ri-1)*bA+i], tabA[ri*bA+i-1],
       tabA[(ri+1)*bA+i-1], tabA[(ri+1)*bA+i+1],
       tabA[(ri-1)*bA+i+1], tabA[(ri-1)*bA+i-1]);
      innernormal(nwi,
       tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[ri*bA+i+1],
       tabA[(ri-1)*bA+i], tabA[ri*bA+i-1],
       tabA[(ri+1)*bA+i-1], tabA[(ri+1)*bA+i+1],
       tabA[(ri-1)*bA+i+1], tabA[(ri-1)*bA+i-1]);
   }
}

template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::creaseinnerlimitlastrow
(OSG::Int32 ri, OSG::UInt32& lwi, OSG::Int32& i, OSG::UInt32& nwi)
{
   OSG::Vec4f* tabA = ppatch->ptabA;
   OSG::Int32 bA=ppatch->breiteA;
   for(; i<ppatch->tabsize-2;++i) {
      limitpointcrease(lwi,
       tabA[ri*bA+i], tabA[ri*bA+i-1], tabA[ri*bA+i+1]);
      regcreasenormal(nwi,tabA[ri*bA+i],tabA[ri*bA+i+1],
       tabA[(ri-1)*bA+i+1],tabA[(ri-1)*bA+i],tabA[(ri-1)*bA+i-1],
       tabA[ri*bA+i-1]);    
   }
}

template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::creaseinnerlimitfirstrow
(OSG::Int32 ri, OSG::UInt32& lwi, OSG::Int32& i, OSG::UInt32& nwi)
{
   OSG::Vec4f* tabA = ppatch->ptabA;
   OSG::Int32 bA=ppatch->breiteA;
   for(; i<ppatch->tabsize-2;++i) {
      limitpointcrease(lwi,
       tabA[ri*bA+i], tabA[ri*bA+i-1], tabA[ri*bA+i+1]);
      regcreasenormal(nwi,tabA[ri*bA+i],tabA[ri*bA+i-1],
       tabA[(ri+1)*bA+i-1],tabA[(ri+1)*bA+i],tabA[(ri+1)*bA+i+1],
       tabA[ri*bA+i+1]);    
   }
}

template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::firstlimitrow (OSG::UInt32 &lwi)
{
   //lwi is the starting index into the limit point array
   OSG::Int32 step = zweihoch[(wsdmaxdepth - targetdepth)];   
   OSG::Int32 ri=1;
   OSG::Int32 val0 = ppatch->valenz[0];
   OSG::Int32 val3 = ppatch->valenz[3];
   OSG::Vec4f* tabA = ppatch->ptabA;
   VectorCAType& corA = *(ppatch->pcorA);
   OSG::Int32 bA=ppatch->breiteA;
   OSG::Int32 i=1;  
   OSG::UInt32 nwi=lwi;
   if (ppatch->iscorner[0] > 2) {        // Ecke, also einfach kopieren
      OSG::Vec4f h = tabA[ri*bA+i];    
      homogenize(h);
      ppatch->mySharedFields->limitpoints->setValue(Pnt3f(h.x(),h.y(),h.z()), lwi);     
      lwi+=step;    
      creasenormal(nwi,tabA[ri*bA+i], corA[0], val0, (2*(ppatch->crindis[0][0])+1),
       (2*(ppatch->crindis[0][1])+1));       
   } else {
      if (ppatch->iscorner[0] == 2) {
         limitpointcrease(lwi,tabA[ri*bA+i],corA[0][2*(ppatch->crindis[0][0])+1],
          corA[0][2*(ppatch->crindis[0][1])+1]);
         creasenormal(nwi,tabA[ri*bA+i], corA[0], val0, (2*(ppatch->crindis[0][0])+1),
          (2*(ppatch->crindis[0][1])+1));  
      } else {
         limitpointmulti(lwi,
          tabA[ri*bA+i], corA[0], val0);
         innernormalmulti(nwi,
          corA[0], val0);
      }
   }  
   ++i;
//*****************
   if (ppatch->crease[3] == 0) {
    innerlimitrow(ri, lwi, i, nwi);
   } else {
    creaseinnerlimitfirstrow(ri, lwi, i, nwi);
   }
//*****************
   if (ppatch->iscorner[3] > 2) {                  // Ecke, also einfach kopieren
      OSG::Vec4f h = tabA[ri*bA+i];    
      homogenize(h);
      ppatch->mySharedFields->limitpoints->setValue(Pnt3f(h.x(),h.y(),h.z()), lwi);     
      lwi+=step;    
      creasenormal(nwi,tabA[ri*bA+i], corA[3], val3, (2*(ppatch->crindis[3][0])+1),
       (2*(ppatch->crindis[3][1])+1));  
   } else {
      if (ppatch->iscorner[3] == 2) {
         limitpointcrease(lwi,tabA[ri*bA+i],corA[3][2*(ppatch->crindis[3][0])+1],
          corA[3][2*(ppatch->crindis[3][1])+1]);  
         creasenormal(nwi,tabA[ri*bA+i], corA[3], val3, (2*(ppatch->crindis[3][0])+1),
         (2*(ppatch->crindis[3][1])+1));  
      } else {  
         limitpointmulti(lwi,
          tabA[ri*bA+i], corA[3], val3);
         innernormalmulti(nwi,
          corA[3], val3);  
      }  
   }
}

template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::limitrow (OSG::Int32 ri, OSG::UInt32 &lwi)
{
   OSG::Vec4f* tabA = ppatch->ptabA;
   OSG::Int32 bA=ppatch->breiteA;
   OSG::Int32 i=1;
   //vertical step in the vertex/normal array:
   OSG::Int32 step = zweihoch[(wsdmaxdepth - targetdepth)];
   lwi-=step;   
   OSG::Int32 breite = wsddepthindexarray[wsdmaxdepth];
   lwi+=(((step-1)*breite)+1);
   OSG::UInt32 nwi=lwi;
   if (ppatch->crease[5] == 0) {
      limitpoint(lwi,
       tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[ri*bA+i+1],
       tabA[(ri-1)*bA+i], tabA[ri*bA+i-1],
       tabA[(ri+1)*bA+i-1], tabA[(ri+1)*bA+i+1],
       tabA[(ri-1)*bA+i+1], tabA[(ri-1)*bA+i-1]);
      innernormal(nwi,
       tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[ri*bA+i+1],
       tabA[(ri-1)*bA+i], tabA[ri*bA+i-1],
       tabA[(ri+1)*bA+i-1], tabA[(ri+1)*bA+i+1],
       tabA[(ri-1)*bA+i+1], tabA[(ri-1)*bA+i-1]);   
   } else {
      limitpointcrease(lwi,
       tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[(ri-1)*bA+i]);   
      regcreasenormal(nwi, tabA[ri*bA+i], tabA[(ri+1)*bA+i], 
       tabA[(ri+1)*bA+i+1], tabA[ri*bA+i+1], tabA[(ri-1)*bA+i+1], tabA[(ri-1)*bA+i]);
   }
   ++i;
//************************
   innerlimitrow(ri, lwi, i, nwi);
//************************
   if (ppatch->crease[6] == 0) {
      limitpoint(lwi,
       tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[ri*bA+i+1],
       tabA[(ri-1)*bA+i], tabA[ri*bA+i-1],
       tabA[(ri+1)*bA+i-1], tabA[(ri+1)*bA+i+1],
       tabA[(ri-1)*bA+i+1], tabA[(ri-1)*bA+i-1]);
      innernormal(nwi,
       tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[ri*bA+i+1],
       tabA[(ri-1)*bA+i], tabA[ri*bA+i-1],
       tabA[(ri+1)*bA+i-1], tabA[(ri+1)*bA+i+1],
       tabA[(ri-1)*bA+i+1], tabA[(ri-1)*bA+i-1]);   
   } else {
      limitpointcrease(lwi,
       tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[(ri-1)*bA+i]);
      regcreasenormal(nwi, tabA[ri*bA+i], tabA[(ri-1)*bA+i], 
       tabA[(ri-1)*bA+i-1], tabA[ri*bA+i-1], tabA[(ri+1)*bA+i-1], tabA[(ri+1)*bA+i]);   
   }   
   assert(i==ppatch->tabsize-2);
}

template<>
void WSDsubdiv<OSG::Vec4f, QUAD>::lastlimitrow (OSG::UInt32 &lwi)
{     
   //vertical step in the vertex/normal array:
   OSG::Int32 step = zweihoch[(wsdmaxdepth - targetdepth)];
   lwi-=step;   
   OSG::Int32 breite = wsddepthindexarray[wsdmaxdepth];
   lwi+=(((step-1)*breite)+1);
   //limitrow(ppatch->tabsize-2);
   OSG::Int32 ri=ppatch->tabsize-2;
   OSG::Int32 val1 = ppatch->valenz[1];
   OSG::Int32 val2 = ppatch->valenz[2];
   OSG::Vec4f* tabA = ppatch->ptabA;
   VectorCAType& corA = *(ppatch->pcorA);
   OSG::Int32 bA=ppatch->breiteA;
   OSG::Int32 i=1;
   OSG::UInt32 nwi=lwi;
   if (ppatch->iscorner[1] > 2) {      // Ecke, also einfach kopieren
      OSG::Vec4f h = tabA[ri*bA+i];    
      homogenize(h);
      ppatch->mySharedFields->limitpoints->setValue(Pnt3f(h.x(),h.y(),h.z()), lwi);     
      lwi+=step;    
      creasenormal(nwi,tabA[ri*bA+i], corA[1], val1, (2*(ppatch->crindis[1][0])+1),
       (2*(ppatch->crindis[1][1])+1));
   } else {
      if (ppatch->iscorner[1] == 2) {
         limitpointcrease(lwi,tabA[ri*bA+i],corA[1][2*(ppatch->crindis[1][0])+1],
          corA[1][2*(ppatch->crindis[1][1])+1]);
         creasenormal(nwi,tabA[ri*bA+i], corA[1], val1, (2*(ppatch->crindis[1][0])+1),
          (2*(ppatch->crindis[1][1])+1));
      } else {  
         limitpointmulti(lwi,
          tabA[ri*bA+i], corA[1], val1);
         innernormalmulti(nwi,
          corA[1], val1);  
      }
   }
  ++i;
//**************************
   if (ppatch->crease[8] == 0) {
      innerlimitrow(ri, lwi, i, nwi);
   } else {
      creaseinnerlimitlastrow(ri, lwi, i, nwi); 
   }
//**************************
   if (ppatch->iscorner[2] > 2) {                  // corner, just copy
      OSG::Vec4f h = tabA[ri*bA+i];    
      homogenize(h);
      ppatch->mySharedFields->limitpoints->setValue(Pnt3f(h.x(),h.y(),h.z()), lwi);     
      lwi+=step;    
      creasenormal(nwi,tabA[ri*bA+i], corA[2], val2, (2*(ppatch->crindis[2][0])+1),
       (2*(ppatch->crindis[2][1])+1));  
   } else {
      if (ppatch->iscorner[2] == 2) {
         limitpointcrease(lwi,tabA[ri*bA+i],corA[2][2*(ppatch->crindis[2][0])+1],
          corA[2][2*(ppatch->crindis[2][1])+1]);
         creasenormal(nwi,tabA[ri*bA+i], corA[2], val2, (2*(ppatch->crindis[2][0])+1),
          (2*(ppatch->crindis[2][1])+1));        
      } else {  
         limitpointmulti(lwi,
          tabA[ri*bA+i], corA[2], val2);
         innernormalmulti(nwi,    
          corA[2], val2);
      }
   }  
}
