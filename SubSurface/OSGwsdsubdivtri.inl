/*---------------------------------------------------------------------------*\
 *                       OpenSG - Dynamic Subdivision                        *
 *                                                                           *
 *                                                                           *
 *					  contact: v.settgast@cg.cs.tu-bs.de * 
 *                                                                           *
\*---------------------------------------------------------------------------*/

#include <assert.h>
#include "OSGwsdsubdiv.h"
OSG_USING_NAMESPACE

//////////////////////////////////////////////////////////////////////
// mtype == TRIANGLE !!!
//////////////////////////////////////////////////////////////////////

// helper for loop algos
static OSG::Real32 lp_a (OSG::Int32 k)
{
   OSG::Real32 ret = (1.0 / (OSG::Real32)k) *
      (0.625 - ((0.375 + 0.25 * osgcos((2.0 * Pi)/(OSG::Real32)k)) *
                (0.375 + 0.25 * osgcos((2.0 * Pi)/(OSG::Real32)k))));
   return ret;
}

////////////////////////////////////////////////////////////////
// new subdivision points

template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::vertexpoint
(OSG::Vec3f& v, 
 OSG::Vec3f& v0, OSG::Vec3f& v1, OSG::Vec3f& v2, OSG::Vec3f& v3, OSG::Vec3f& v4,
 OSG::Vec3f& w1, OSG::Vec3f& w2, OSG::Vec3f& w3, OSG::Vec3f& w4)
{ // valence = 6!
   v= v0*0.625 + (v1 + v2 + v3 + v4 + w1 + w2) * 0.0625;   
}

template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::vertexpointmulti
(OSG::Vec3f& v, OSG::Vec3f& v0, OSG::Vec3f* array, OSG::Int32 val)
{
	OSG::Real32 alpha = 1.0 - (lp_a(val) * (OSG::Real32)val);
	OSG::Real32 beta  = lp_a(val);
   OSG::Vec3f tmp(0.0,0.0,0.0);	
 
   for (OSG::Int32 i=0; i<val; i++) tmp+= array[i];

   v= (v0*alpha) + (tmp*beta);    
}

//(always from the outside to the inside!)
template<> 
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::edgepoint
(OSG::Vec3f& v, 
 OSG::Vec3f& v1, OSG::Vec3f& v2, 
 OSG::Vec3f& w1, OSG::Vec3f& w2,OSG::Vec3f& w3, OSG::Vec3f& w4, OSG::Int32 crease)
{
	if (crease < 5)               // no crease
		v= (v1 + v2)*0.375 + (w1 + w2)*0.125;
	else 
   if (crease < 10)
	{
		v= (v1 + v2)*0.5;          // regular/regular case!		
	}
   else
   if (crease < 15)
   {
		v= v1*0.375 + v2*0.625;    // irregular/regular case!		
	}
   else
   if (crease < 20)
   {
		v= v1*0.625 + v2*0.375;    // regular/irregular case!		
	}
   else
   if (crease < 25)
   {      
      if (targetdepth == 1)
		v= v1*0.375 + v2*0.625;    // irregular/regular only from depth 0 to 1 		
      else
      v= (v1 + v2)*0.5;
	}
   else
   if (crease < 30)
   {
      if (targetdepth == 1)
		v= (v1 + v2)*0.375 + (w1 + w2)*0.125;   // half/regular for depth 0
      else
      v= (v1 + v2)*0.5;

	}
   else
   if (crease < 35)
   {
      if (targetdepth == 1)
		v= (v1 + v2)*0.375 + (w1 + w2)*0.125;    // half/irregular for depth 0	
      else
      v= v1*0.625 + v2*0.375; 
	}
   else {
      v= (v1 + v2)*0.375 + (w1 + w2)*0.125;
      SLOG << "undefined crease type: " << crease << std::endl;
   }
}

template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::facepoint
(OSG::Vec3f& v, OSG::Vec3f& v1, OSG::Vec3f& v2, OSG::Vec3f& v3, OSG::Vec3f& v4)
{
   // is actually an edge point in the triangle case...
   v= (v1 + v2)*0.375 + (v3 + v4)*0.125;
}


// crease edges

template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::vertexpointcrease
(OSG::Vec3f& v, OSG::Vec3f& v0, OSG::Vec3f& v1, OSG::Vec3f& v2)
{
   v= v0*0.75 + (v1 + v2)*0.125;   
}



//////////// \/ Normals and Limitpoints \/ ///////////////////////


template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::innernormal
(OSG::UInt32& nwi, 
 OSG::Vec3f& v0, OSG::Vec3f& v1, OSG::Vec3f& v2, OSG::Vec3f& v3, OSG::Vec3f& v4, 
 OSG::Vec3f& w1, OSG::Vec3f& w2, OSG::Vec3f& w3, OSG::Vec3f& w4)
{   
	OSG::Vec3f v;
   OSG::Vec3f t1(0.0,0.0,0.0);
   OSG::Vec3f t2(0.0,0.0,0.0);
		
	OSG::Real32 b1,b2,ak;

   ak = 2.0*Pi / 6.0;

	//i=0
	OSG::Int32 i=0;

	b1 = osgcos(ak*(OSG::Real32)i);
	b2 = osgsin(ak*(OSG::Real32)i);	
	t1=v1 * b1;
	t2=v1 * b2;		
	i++;		// i=1
	b1 = osgcos(ak*(OSG::Real32)i);
	b2 = osgsin(ak*(OSG::Real32)i);	
	t1+=v2 * b1;
	t2+=v2 * b2;		
	i++;		// i=2
	b1 = osgcos(ak*(OSG::Real32)i);
	b2 = osgsin(ak*(OSG::Real32)i);	
	t1+=v3 * b1;
	t2+=v3 * b2;		
	i++;		// i=3
	b1 = osgcos(ak*(OSG::Real32)i);
	b2 = osgsin(ak*(OSG::Real32)i);	
	t1+=v4 * b1;
	t2+=v4 * b2;	
   i++;		// i=4
	b1 = osgcos(ak*(OSG::Real32)i);
	b2 = osgsin(ak*(OSG::Real32)i);	
	t1+=w1 * b1;
	t2+=w1 * b2;	
   i++;		// i=5
	b1 = osgcos(ak*(OSG::Real32)i);
	b2 = osgsin(ak*(OSG::Real32)i);	
	t1+=w2 * b1;
	t2+=w2 * b2;	
	
	v = t1.cross(t2);
  
	v.normalize();
	ppatch->mySharedFields->limitnormals->setValue(Vec3f(v.x(),v.y(),v.z()), nwi);
	
	nwi+=zweihoch[(wsdmaxdepth - targetdepth)];   
}
template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::innernormalmulti
(OSG::UInt32& nwi, OSG::Vec3f *array, OSG::Int32 val)
{   
	OSG::Vec3f v;
   OSG::Vec3f t1(0.0,0.0,0.0);
   OSG::Vec3f t2(0.0,0.0,0.0);

	OSG::Real32 ak = 2.0 * Pi / (Real32)val;
	OSG::Real32 b1,b2;
	
	for (OSG::Int32 i=0; i<val; i++)
	{

		b1 = osgcos(ak*i);
		b2 = osgsin(ak*i);
		
		t1+=array[i] * b1;
		t2+=array[i] * b2;	
		
	}
	v = t1.cross(t2);
  
	v.normalize();
	ppatch->mySharedFields->limitnormals->setValue(Vec3f(v.x(),v.y(),v.z()), nwi);
	
	nwi+=zweihoch[(wsdmaxdepth - targetdepth)];   
}

template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::creasenormal
(OSG::UInt32& nwi, 
 OSG::Vec3f& v0, OSG::Vec3f *array, OSG::Int32 val, OSG::Int32 a, OSG::Int32 b)
{   
	OSG::Vec3f v;
   OSG::Vec3f t1(0.0,0.0,0.0);
   OSG::Vec3f t2(0.0,0.0,0.0);

   if (val == 2) {
      t2 = (v0 * -2.0f) + (array[b%val] + array[a%val]);
   }
   else
   if (val == 3) {
      t2 = (v0 * -1.0f) + array[(a+1)%val];
   }
   else
   {      
      if (b<a) b=b+val;
      OSG::Int32 k = (b-a)+1;
      OSG::Vec3f beta(0.0,0.0,0.0);
      OSG::Real32 pdk = Pi/((OSG::Real32)(k-1));
      OSG::Real32 b0 = osgsin(pdk) / (2.0f*osgcos(pdk)-2.0f); // -2 in cos?

      beta+=array[a%val]*b0;

      for (Int32 i=1; i<k-1; i++)         
          beta+=array[(a+i)%val]*osgsin(pdk*(OSG::Real32)i);
      
      beta+=array[(a+(k-1))%val]*b0;

      t2 = beta;
   }

   t1 = array[b%val] - array[a%val];
	
	v = t2.cross(t1);	
  
	v.normalize();
	ppatch->mySharedFields->limitnormals->setValue(Vec3f(v.x(),v.y(),v.z()), nwi);
	
	nwi+=zweihoch[(wsdmaxdepth - targetdepth)];  
}

template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::regcreasenormal
(OSG::UInt32& nwi, 
 OSG::Vec3f& v0, OSG::Vec3f& v1, OSG::Vec3f& w1, OSG::Vec3f& v2, OSG::Vec3f& w2, OSG::Vec3f& v3)
{   
	OSG::Vec3f v;
   OSG::Vec3f t1(0.0,0.0,0.0);
   OSG::Vec3f t2(0.0,0.0,0.0);

	t1 = (v3 - v1);
	
	OSG::Vec3f betat2  = (v1 + v3) * (-1.0f);
   OSG::Vec3f gammat2 = (w1 + v2) *  2.0f;
	
	t2 = (v0 * (-2.0f)) + betat2 + gammat2;

	v = t2.cross(t1);
  
	v.normalize();
	
	ppatch->mySharedFields->limitnormals->setValue(Vec3f(v.x(),v.y(),v.z()), nwi);
	
	nwi+=zweihoch[(wsdmaxdepth - targetdepth)];  
}


////////////////////////////////////////////////////////////////
// final points


template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::limitpoint
(OSG::UInt32& lwi, 
 OSG::Vec3f& v0, OSG::Vec3f& v1, OSG::Vec3f& v2, OSG::Vec3f& v3, OSG::Vec3f& v4, 
 OSG::Vec3f& w1, OSG::Vec3f& w2, OSG::Vec3f& w3, OSG::Vec3f& w4)
{
   // as valence is 6, forget about w3 and w4!
   OSG::Vec3f v;   
   v= v0 * 0.5 +       
      ( (v1 + v2 + v3 + v4 + w1 + w2) * 0.08333333333333333333333333 );
   
   ppatch->mySharedFields->limitpoints->setValue(Pnt3f(v.x(),v.y(),v.z()), lwi);
   
   lwi+=zweihoch[(wsdmaxdepth - targetdepth)];   
}

template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::limitpointcrease
(OSG::UInt32& lwi, 
 OSG::Vec3f& v0, OSG::Vec3f& v1, OSG::Vec3f& v2)
{   
   OSG::Vec3f v;
   v= v0 * 0.6666666666666666666666666 +       
      ( v1 + v2 ) * 0.1666666666666666666666666; // regular

      
   ppatch->mySharedFields->limitpoints->setValue(Pnt3f(v.x(),v.y(),v.z()), lwi);
   
   lwi+=zweihoch[(wsdmaxdepth - targetdepth)];   
}

template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::limitpointcreaseirreg
(OSG::UInt32& lwi, OSG::Vec3f& v0, OSG::Vec3f& v1, OSG::Vec3f& v2)
{   
   OSG::Vec3f v;
   
   v= v0 * 0.6 +       
      ( v1 + v2 ) * 0.2;                     // irregular
      
   ppatch->mySharedFields->limitpoints->setValue(Pnt3f(v.x(),v.y(),v.z()), lwi);
   
   lwi+=zweihoch[(wsdmaxdepth - targetdepth)];   
}

template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::limitpointmulti
(OSG::UInt32& lwi, OSG::Vec3f& v0, OSG::Vec3f *array, OSG::Int32 val)
{
	OSG::Real32 alpha = 1.0 - ((OSG::Real32)val/((3.0 / (8.0*lp_a(val)))+(OSG::Real32)val));
	OSG::Real32 beta = 1.0 / (3.0 /(8.0 * lp_a(val)) + (OSG::Real32)val);
	
	OSG::Vec3f v4(0.0,0.0,0.0);	
	for (OSG::Int32 i=0; i<val; i++) {		
		v4 = v4 + array[i];
	}		
	
   OSG::Vec3f v = (v0*alpha) + (v4*beta);
      
   ppatch->mySharedFields->limitpoints->setValue(Pnt3f(v.x(),v.y(),v.z()), lwi);   
   lwi+=zweihoch[(wsdmaxdepth - targetdepth)];   
}


// CF moved to here


template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::initwsdsubdiv(void)
{
}

//////////////////////////////////////////////////////////////////////
// update corners data is for cc only
template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::corsupdate (void)
{
}

// determine the Crease-Type for c-edge 3,5,6,8
template<>
OSG::Int32 WSDsubdiv<OSG::Vec3f, TRIANGLE>::getCreaseType
(Int32 crease, Int32 C0, Int32 C1, Int32 ri)
{
   OSG::Int32 CREASE_TYPE = 0;
   if (ppatch->crease[crease]>1) {
      SLOG << "not needed call of getCreaseType!" << std::endl;
      return ppatch->crease[crease];
   }
   if (ppatch->crease[crease]>0) {
      CREASE_TYPE = CREASE_REG_REG;
      // Depth = 0?
      if (targetdepth == 1) {
         if ((ppatch->iscorner[C0] < 2) ||
             (ppatch->iscorner[C1] < 2)) CREASE_TYPE = CREASE_HALF;
         else {
            CREASE_TYPE = CREASE_REG_REG;
            if ((ppatch->isregular[C0] == 1) && (ppatch->isregular[C1] == 0)) 
               CREASE_TYPE = CREASE_REG_IRREG;
            if ((ppatch->isregular[C1] == 1) && (ppatch->isregular[C0] == 0)) 
               CREASE_TYPE = CREASE_IRREG_REG;            
         }
      }
      else { //Depth > 0
         if (ri == 1) {                      // first inner edgerow
            if (ppatch->iscorner[C0] < 2) CREASE_TYPE = CREASE_HALF;
            else
            if (ppatch->isregular[C0]==0) CREASE_TYPE = CREASE_IRREG_REG;
         }
         else if (ri==ppatch->tabsize-3) {   // last inner edgerow
            if (ppatch->iscorner[C1] < 2) CREASE_TYPE = CREASE_HALF;
            else
            if (ppatch->isregular[C1]==0) CREASE_TYPE = CREASE_REG_IRREG;
         }
      }
   }   
   return CREASE_TYPE;
}


//! Helper to calculate all edges around the four corner vertices
template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::allcorners (void)
{
   OSG::Vec3f nodata;
   VectorCAType& corA = *(ppatch->pcorA);
   VectorCAType& corB = *(ppatch->pcorB);

   OSG::Vec3f* tabA = ppatch->ptabA;
   OSG::Int32 bA=ppatch->breiteA;
   OSG::Vec3f cP[4];
 
   cP[0]=tabA[1*bA+1];
   cP[1]=tabA[(ppatch->tabsize-2)*bA+1];
   cP[2]=tabA[(ppatch->tabsize-2)*bA+ppatch->tabsize-2];
   cP[3]=tabA[1*bA+ppatch->tabsize-2];

   Int32 ll;
   Int32 i;
   for (ll=0; ll<4; ll++) {
      for (i=ppatch->valenz[ll]; i<(ppatch->valenz[ll]*2); i++) {         
         edgepoint(corB[ll][i%ppatch->valenz[ll]],
          corA[ll][i%ppatch->valenz[ll]],cP[ll], 
          corA[ll][(i+1)%ppatch->valenz[ll]], corA[ll][(i-1)%ppatch->valenz[ll]],
          nodata, nodata, ppatch->corcrease[ll][i%ppatch->valenz[ll]]);
      }
   }
}

//

/////////////////////////////////////////////////////////////////////
// new subdiv-rows

template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::first_last_inneredgerow
(OSG::Int32 ri, OSG::Int32 wi, OSG::Int32& i, OSG::Int32& j)
{
   OSG::Vec3f nodata;
   OSG::Vec3f* tabA = ppatch->ptabA;
   OSG::Vec3f* tabB = ppatch->ptabB;
   VectorCAType& corA = *(ppatch->pcorA);
   VectorCAType& corB = *(ppatch->pcorB);
   	OSG::Int32 bA=ppatch->breiteA;
	OSG::Int32 bB=wsddepthindexarray[wsdmaxdepth]+2;   


   assert((ri == 0)||(ri == ppatch->tabsize-2));
   

   if (ri == ppatch->tabsize-2)
     tabB[wi*bB+j] = corB[1][ppatch->valenz[1]-1];
   else {
      if ((targetdepth == 1) && (ri==0)) 
        tabB[wi*bB+j] = corB[3][ppatch->valenz[3]-1];
      else
       edgepoint(tabB[wi*bB+j],
         tabA[ri*bA+i], tabA[(ri+1)*bA+i+1],
         tabA[ri*bA+i+1], tabA[(ri+1)*bA+i],
         nodata, nodata, 0);
   }
   ++j; ++i;
   assert(i==2);
   assert(j==3);							      // j  ist spaltenindex von tabB
   for(; i<ppatch->tabsize-2;++i) {			// i  ist spaltenindex von tabA
      edgepoint(tabB[wi*bB+j],				// ri ist Zeilenindex von tabA
         tabA[(ri+1)*bA+i],tabA[ri*bA+i],			// wi ist Zeilenindex von tabB
         tabA[(ri+1)*bA+i+1],tabA[ri*bA+i-1],
         nodata,nodata,0);
      ++j;
      if ((ri == 0) && (i == ppatch->tabsize-3))
         tabB[wi*bB+j] = corB[3][ppatch->valenz[3]-1];
      else
       edgepoint(tabB[wi*bB+j],
         tabA[(ri+1)*bA+i+1], tabA[ri*bA+i],
         tabA[(ri+1)*bA+i], tabA[ri*bA+i+1],
         nodata, nodata, 0);
      ++j;
   }
}

template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::inneredgerow
(OSG::Int32 ri, OSG::Int32 wi, OSG::Int32& i, OSG::Int32& j)
{
   OSG::Vec3f nodata;
   OSG::Vec3f* tabA = ppatch->ptabA;
   OSG::Vec3f* tabB = ppatch->ptabB;
   	OSG::Int32 bA=ppatch->breiteA;
	OSG::Int32 bB=wsddepthindexarray[wsdmaxdepth]+2;   
   edgepoint(tabB[wi*bB+j],
      tabA[ri*bA+i], tabA[(ri+1)*bA+i+1],
      tabA[ri*bA+i+1], tabA[(ri+1)*bA+i],
      nodata, nodata, 0);
   ++j; ++i;
   assert(i==2);
   assert(j==3);							      // j  ist spaltenindex von tabB
   for(; i<ppatch->tabsize-2;++i) {			// i  ist spaltenindex von tabA
      edgepoint(tabB[wi*bB+j],				// ri ist Zeilenindex von tabA
         tabA[(ri+1)*bA+i],tabA[ri*bA+i],			// wi ist Zeilenindex von tabB
         tabA[(ri+1)*bA+i+1],tabA[ri*bA+i-1],
         nodata,nodata,0);
      ++j;
      edgepoint(tabB[wi*bB+j],
         tabA[(ri+1)*bA+i+1], tabA[ri*bA+i],
         tabA[(ri+1)*bA+i], tabA[ri*bA+i+1],
         nodata, nodata, 0);
      ++j;
   }
}

template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::tricrease4_inneredgerow
(OSG::Int32 ri, OSG::Int32 wi, OSG::Int32& i, OSG::Int32& j)
{
   OSG::Vec3f nodata;
   OSG::Vec3f* tabA = ppatch->ptabA;
   OSG::Vec3f* tabB = ppatch->ptabB;
   	OSG::Int32 bA=ppatch->breiteA;
	OSG::Int32 bB=wsddepthindexarray[wsdmaxdepth]+2;   
   if (ri == 1)
   edgepoint(tabB[wi*bB+j],
      tabA[ri*bA+i], tabA[(ri+1)*bA+i+1],
      tabA[ri*bA+i+1], tabA[(ri+1)*bA+i],
      nodata, nodata, ppatch->tricrease[4]);
   else
   edgepoint(tabB[wi*bB+j],
      tabA[ri*bA+i], tabA[(ri+1)*bA+i+1],
      tabA[ri*bA+i+1], tabA[(ri+1)*bA+i],
      nodata, nodata, 0);
   ++j; ++i;
   assert(i==2);
   assert(j==3);							      // j  is spaltenindex of tabB
   for(; i<ppatch->tabsize-2;++i) {			// i  is spaltenindex of tabA      
      edgepoint(tabB[wi*bB+j],				// ri is Zeilenindex of tabA
         tabA[(ri+1)*bA+i],tabA[ri*bA+i],			// wi is Zeilenindex of tabB
         tabA[(ri+1)*bA+i+1],tabA[ri*bA+i-1],
         nodata,nodata,0);
      ++j;
      if (ri == i) 
       edgepoint(tabB[wi*bB+j],
         tabA[(ri+1)*bA+i+1], tabA[ri*bA+i],
         tabA[(ri+1)*bA+i], tabA[ri*bA+i+1],
         nodata, nodata, ppatch->tricrease[4]);
      else
       edgepoint(tabB[wi*bB+j],
         tabA[(ri+1)*bA+i+1], tabA[ri*bA+i],
         tabA[(ri+1)*bA+i], tabA[ri*bA+i+1],
         nodata, nodata, 0);
      ++j;
   }
}



template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::firstedgerow (void)
{
   OSG::Vec3f nodata;
	OSG::Int32 val0 = ppatch->valenz[0];
	OSG::Int32 val3 = ppatch->valenz[3];
	OSG::Int32 wi = 0;
	OSG::Int32 ri = 0;
	OSG::Int32 i  = 0;
	OSG::Int32 j  = 0;   
	OSG::Vec3f* tabA = ppatch->ptabA;
	OSG::Vec3f* tabB = ppatch->ptabB;
	VectorCAType& corA = *(ppatch->pcorA);
	VectorCAType& corB = *(ppatch->pcorB);

	OSG::Int32 bA=ppatch->breiteA;
	OSG::Int32 bB=wsddepthindexarray[wsdmaxdepth]+2;   
   

   tabB[wi*bB+j] = corB[0][4%val0];

	++j; ++i;

   tabB[wi*bB+j] = corB[0][3%val0];
   		 
	++j;
	//************************************
	first_last_inneredgerow(ri, wi, i, j);
	//************************************

   if (ppatch->borders[3]==0)
    tabB[wi*bB+j] = corB[3][val3-2];
   else
    tabB[wi*bB+j] = corB[3][val3-1];

}

template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::edgerow (OSG::Int32 ri, OSG::Int32 wi)
{
   OSG::Vec3f nodata;
   OSG::Int32 i=0;
   OSG::Int32 j=0;
   OSG::Int32 CREASE_TYPE=0;
   OSG::Vec3f* tabA = ppatch->ptabA;
   OSG::Vec3f* tabB = ppatch->ptabB;
   VectorCAType& corB = *(ppatch->pcorB);
	OSG::Int32 bA=ppatch->breiteA;
	OSG::Int32 bB=wsddepthindexarray[wsdmaxdepth]+2;   
   if (ri == ppatch->tabsize-3) {     
      tabB[wi*bB+j] = corB[1][2%ppatch->valenz[1]];
   }
   else    
    edgepoint(tabB[wi*bB+j],
      tabA[ri*bA+i], tabA[(ri+1)*bA+i+1],
      tabA[ri*bA+i+1], tabA[(ri+1)*bA+i],
      nodata, nodata ,0);
   ++i; ++j;
   
   CREASE_TYPE = getCreaseType(5,0,1,ri);
      
      edgepoint(tabB[wi*bB+j], 
      tabA[ri*bA+i],tabA[(ri+1)*bA+i], 
      tabA[(ri+1)*bA+i+1],tabA[ri*bA+i-1],
      nodata,nodata,CREASE_TYPE);//ppatch->crease[5]+CREASE_TYPE);
   ++j;
   //*************************
   if (ppatch->tricrease[4] > 0)
    tricrease4_inneredgerow(ri, wi, i, j);
   else
    inneredgerow(ri, wi, i, j);
   //*************************

   CREASE_TYPE = getCreaseType(6,3,2,ri);

   edgepoint(tabB[wi*bB+j], 
      tabA[ri*bA+i],tabA[(ri+1)*bA+i], 
      tabA[(ri+1)*bA+i+1],tabA[ri*bA+i-1],
      nodata,nodata,CREASE_TYPE);//ppatch->crease[6]+CREASE_TYPE);
   ++j;
   if (ri == 1) {      
      tabB[wi*bB+j] = corB[3][2%ppatch->valenz[3]];
   } 
   else 
   edgepoint(tabB[wi*bB+j],
      tabA[(ri+1)*bA+i+1], tabA[ri*bA+i], 
      tabA[(ri+1)*bA+i], tabA[ri*bA+i+1],
      nodata, nodata ,0);
}

template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::lastedgerow (OSG::Int32 ri, OSG::Int32 wi)
{
   OSG::Vec3f nodata;
	OSG::Int32 i=0;
	OSG::Int32 j=0;
	OSG::Int32 c=0;
	OSG::Int32 d=0;
	OSG::Int32 val1 = ppatch->valenz[1];
	OSG::Int32 val2 = ppatch->valenz[2];

	OSG::Vec3f* tabA = ppatch->ptabA;
	OSG::Vec3f* tabB = ppatch->ptabB;
	VectorCAType& corA = *(ppatch->pcorA);
	VectorCAType& corB = *(ppatch->pcorB);
	OSG::Int32 bA=ppatch->breiteA;
	OSG::Int32 bB=wsddepthindexarray[wsdmaxdepth]+2;   

   ++i; ++j;

   if (ppatch->borders[1]==0)
      tabB[wi*bB+j] = corB[1][val1-2];
   else
      tabB[wi*bB+j] = corB[1][val1-1];
   
	++j;
	//*************************
	first_last_inneredgerow(ri, wi, i, j);
	//*************************

   tabB[wi*bB+j] = corB[2][3%val2];
   tabB[wi*bB+j+1] = corB[2][4%val2]; 
}


template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::innervertexrow
(OSG::Int32 ri, OSG::Int32 wi, OSG::Int32& i, OSG::Int32& j)
{
   OSG::Vec3f nodata;
	OSG::Vec3f* tabA = ppatch->ptabA;
	OSG::Vec3f* tabB = ppatch->ptabB;
	OSG::Int32 bA=ppatch->breiteA;
	OSG::Int32 bB=wsddepthindexarray[wsdmaxdepth]+2;   
	edgepoint(tabB[wi*bB+j], 
      tabA[ri*bA+i],tabA[ri*bA+i+1], 
      tabA[(ri+1)*bA+i+1],tabA[(ri-1)*bA+i],
      nodata,nodata,0);
	++j; ++i;
	assert(i==2);
	assert(j==3);
	for(; i<ppatch->tabsize-2;++i) {
		vertexpoint(tabB[wi*bB+j],
		 tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[ri*bA+i+1],
		 tabA[(ri-1)*bA+i], tabA[ri*bA+i-1],
		 tabA[(ri+1)*bA+i+1], tabA[(ri-1)*bA+i-1],
		 nodata, nodata);
		++j;
		edgepoint(tabB[wi*bB+j], 
		 tabA[ri*bA+i],tabA[ri*bA+i+1], 
		 tabA[(ri+1)*bA+i+1],tabA[(ri-1)*bA+i],
		 nodata,nodata,0);
		++j;
	}
}

template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::tricrease4_innervertexrow
(OSG::Int32 ri, OSG::Int32 wi, OSG::Int32& i, OSG::Int32& j)
{
   OSG::Vec3f nodata;
	OSG::Vec3f* tabA = ppatch->ptabA;
	OSG::Vec3f* tabB = ppatch->ptabB;
	OSG::Int32 bA=ppatch->breiteA;
	OSG::Int32 bB=wsddepthindexarray[wsdmaxdepth]+2;   
	edgepoint(tabB[wi*bB+j], 
      tabA[ri*bA+i],tabA[ri*bA+i+1], 
      tabA[(ri+1)*bA+i+1],tabA[(ri-1)*bA+i],
      nodata,nodata,0);
	++j; ++i;
	assert(i==2);
	assert(j==3);
	for(; i<ppatch->tabsize-2;++i) {
      if (ri == i)
      vertexpointcrease(tabB[wi*bB+j],
		  tabA[ri*bA+i], tabA[(ri+1)*bA+i+1], tabA[(ri-1)*bA+i-1]);		
      else
		vertexpoint(tabB[wi*bB+j],
		 tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[ri*bA+i+1],
		 tabA[(ri-1)*bA+i], tabA[ri*bA+i-1],
		 tabA[(ri+1)*bA+i+1], tabA[(ri-1)*bA+i-1],
		 nodata, nodata);
		++j;
		edgepoint(tabB[wi*bB+j], 
		 tabA[ri*bA+i],tabA[ri*bA+i+1], 
		 tabA[(ri+1)*bA+i+1],tabA[(ri-1)*bA+i],
		 nodata,nodata,0);
		++j;
	}
}

template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::creaseinnervertexrow
(OSG::Int32 ri, OSG::Int32 wi, OSG::Int32& i, OSG::Int32& j, OSG::Int32 ce)
{
   OSG::Vec3f* tabA = ppatch->ptabA;
   OSG::Vec3f* tabB = ppatch->ptabB;
   OSG::Int32 bA=ppatch->breiteA;
	OSG::Int32 bB=wsddepthindexarray[wsdmaxdepth]+2;   
   OSG::Int32 CREASE_TYPE = 0;

   if (ri == 1)
   CREASE_TYPE = getCreaseType(3,0,3,i);     // first line crease
   else 
   if (ri == ppatch->tabsize-2)
   CREASE_TYPE = getCreaseType(8,1,2,i);     // last line crease
   else 
      SLOG << "creaseinnervertexrow is not in 1 or tabsize-2" << std::endl;

   edgepoint(tabB[wi*bB+j], 
      tabA[ri*bA+i],tabA[ri*bA+i+1], 
      tabA[(ri+1)*bA+i],tabA[(ri+1)*bA+i+1],
      tabA[(ri-1)*bA+i+1],tabA[(ri-1)*bA+i],CREASE_TYPE);//ce+CREASE_TYPE);
   ++j; ++i;
   assert(i==2);
   assert(j==3);
   for(; i<ppatch->tabsize-2;++i) {
	   if (ce == 0)
      vertexpoint(tabB[wi*bB+j],
         tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[ri*bA+i+1],
         tabA[(ri-1)*bA+i], tabA[ri*bA+i-1],
         tabA[(ri+1)*bA+i-1], tabA[(ri+1)*bA+i+1],
         tabA[(ri-1)*bA+i+1], tabA[(ri-1)*bA+i-1]);
	   else
	  vertexpointcrease(tabB[wi*bB+j],
         tabA[ri*bA+i], tabA[ri*bA+i-1], tabA[ri*bA+i+1]);

      ++j;

      if (ri == 1)
      CREASE_TYPE = getCreaseType(3,0,3,i);
      else if (ri == ppatch->tabsize-2)
      CREASE_TYPE = getCreaseType(8,1,2,i);
      else CREASE_TYPE = 0;

      edgepoint(tabB[wi*bB+j], 
         tabA[ri*bA+i],tabA[ri*bA+i+1], 
         tabA[(ri+1)*bA+i],tabA[(ri+1)*bA+i+1],
         tabA[(ri-1)*bA+i+1],tabA[(ri-1)*bA+i],CREASE_TYPE);//ce+CREASE_TYPE);
      ++j;
   }
}


template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::firstvertexrow (void)
{
   OSG::Vec3f nodata;
	OSG::Int32 ri=1;
	OSG::Int32 wi=1;
	OSG::Int32 i=0;
	OSG::Int32 j=0;
	OSG::Int32 val0 = ppatch->valenz[0];
	OSG::Int32 val3 = ppatch->valenz[3];
	
	OSG::Vec3f* tabA = ppatch->ptabA;
	OSG::Vec3f* tabB = ppatch->ptabB;
	VectorCAType& corA = *(ppatch->pcorA);
	VectorCAType& corB = *(ppatch->pcorB);
	OSG::Int32 bA=ppatch->breiteA;
	OSG::Int32 bB=wsddepthindexarray[wsdmaxdepth]+2;   

   tabB[wi*bB+j] = corB[0][val0-1];
   
	++i; ++j;

	if (ppatch->iscorner[0] > 2) {            // more than 2 creases -> copy point
		OSG::Vec3f h = tabA[ri*bA+i];
		tabB[wi*bB+j] = h;
	}
	else
	if (ppatch->iscorner[0] == 2)
	   vertexpointcrease(tabB[wi*bB+j], tabA[ri*bA+i],					
                  corA[0][ppatch->crindis[0][0]],
						corA[0][ppatch->crindis[0][1]]);
	else {
      vertexpointmulti(tabB[wi*bB+j],
		 tabA[ri*bA+i], corA[0], val0);
   }
	++j;

	//*************************
	if (ppatch->crease[3] > 0)
	 creaseinnervertexrow(ri, wi,i,j,1);
	else
	 innervertexrow(ri, wi,i,j);
	//*************************
   

	if (ppatch->iscorner[3] > 2) {            // more than 2 creases -> copy point
		OSG::Vec3f h = tabA[ri*bA+i];
		tabB[wi*bB+j] = h;
	}
	else
	if (ppatch->iscorner[3] == 2)
		vertexpointcrease(tabB[wi*bB+j], tabA[ri*bA+i],
                     corA[3][ppatch->crindis[3][0]],
							corA[3][ppatch->crindis[3][1]]);
	else			
       vertexpointmulti(tabB[wi*bB+j],
		 tabA[ri*bA+i], corA[3], val3);	
	++j;

   if (ppatch->borders[2]==0)
      tabB[wi*bB+j] = corB[3][3%val3];
   else
      tabB[wi*bB+j] = corB[3][2%val3];

}

template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::vertexrow (OSG::Int32 ri, OSG::Int32 wi)
{  
   OSG::Vec3f nodata;
	OSG::Int32 i=0;
	OSG::Int32 j=0;
	OSG::Vec3f* tabA = ppatch->ptabA;
	OSG::Vec3f* tabB = ppatch->ptabB;
	OSG::Int32 bA=ppatch->breiteA;
	OSG::Int32 bB=wsddepthindexarray[wsdmaxdepth]+2;   
	edgepoint(tabB[wi*bB+j], 
	  tabA[ri*bA+i],tabA[ri*bA+i+1], 
	  tabA[(ri+1)*bA+i+1],tabA[(ri-1)*bA+i],
	  nodata,nodata,0);
	++i; ++j;
	if (ppatch->crease[5] > 0)        
		vertexpointcrease(tabB[wi*bB+j],
		  tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[(ri-1)*bA+i]);
	else
      vertexpoint(tabB[wi*bB+j],
		  tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[ri*bA+i+1],
		  tabA[(ri-1)*bA+i], tabA[ri*bA+i-1],
		  tabA[(ri+1)*bA+i+1], tabA[(ri-1)*bA+i-1],
		  nodata, nodata);		

	++j;
	//*************************
   if (ppatch->tricrease[4] > 0)
   tricrease4_innervertexrow(ri, wi,i,j);
   else
	innervertexrow(ri, wi,i,j);
	//************************* 
	if (ppatch->crease[6] > 0)         
      vertexpointcrease(tabB[wi*bB+j],
		  tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[(ri-1)*bA+i]);		
	else
      vertexpoint(tabB[wi*bB+j],
		  tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[ri*bA+i+1],
		  tabA[(ri-1)*bA+i], tabA[ri*bA+i-1],
		  tabA[(ri+1)*bA+i+1], tabA[(ri-1)*bA+i-1],
		  nodata, nodata);
		

	++j;
	edgepoint(tabB[wi*bB+j], 
	  tabA[ri*bA+i+1],tabA[ri*bA+i], 
	  tabA[(ri+1)*bA+i+1],tabA[(ri-1)*bA+i],
	  nodata,nodata,0);

	assert(i==ppatch->tabsize-2);
	assert(j==2*(ppatch->tabsize)-3-1);
}

template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::lastvertexrow (OSG::Int32 ri, OSG::Int32 wi)
{  
   OSG::Vec3f nodata;
	OSG::Int32 i=0;
	OSG::Int32 j=0;
   	OSG::Int32 val1 = ppatch->valenz[1];
	OSG::Int32 val2 = ppatch->valenz[2];

	OSG::Vec3f* tabA  = ppatch->ptabA;
	OSG::Vec3f* tabB  = ppatch->ptabB;
	VectorCAType& corA = *(ppatch->pcorA);
	VectorCAType& corB = *(ppatch->pcorB);
	OSG::Int32 bA=ppatch->breiteA;
	OSG::Int32 bB=wsddepthindexarray[wsdmaxdepth]+2;   

   if (ppatch->borders[0]==0)
      tabB[wi*bB+j] = corB[1][3%val1];
   else
      tabB[wi*bB+j] = corB[1][2%val1];


	++i; ++j;

	if (ppatch->iscorner[1] > 2) {
		OSG::Vec3f h = tabA[ri*bA+i];
		tabB[wi*bB+j] = h;      
	}
	else
   if (ppatch->iscorner[1] == 2) {      
		vertexpointcrease(tabB[wi*bB+j], tabA[ri*bA+i],
							corA[1][ppatch->crindis[1][0]],
							corA[1][ppatch->crindis[1][1]]);      
   }
   else {
      vertexpointmulti(tabB[wi*bB+j],
		    tabA[ri*bA+i], corA[1], val1);
   }	
	++j;

	//*************************
	if (ppatch->crease[8] > 0)
		creaseinnervertexrow(ri, wi,i,j,1);
	else
		innervertexrow(ri, wi,i,j);	
	//*************************

	if (ppatch->iscorner[2] > 2) {
		OSG::Vec3f h = tabA[ri*bA+i];
		tabB[wi*bB+j] = h;
	}
	else
   if (ppatch->iscorner[2] == 2) {
	   vertexpointcrease(tabB[wi*bB+j], tabA[ri*bA+i],
          corA[2][ppatch->crindis[2][0]],
		    corA[2][ppatch->crindis[2][1]]);
      assert(ppatch->corcrease[2][ppatch->crindis[2][1]] != 0);
   }
   else {	
      vertexpointmulti(tabB[wi*bB+j],
		    tabA[ri*bA+i], corA[2], val2);	
   }
   
	++j;

   tabB[wi*bB+j] = corB[2][val2-1];
}

//////////////////////////////////////////////////////////////////////
// final-rows

template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::innerlimitrow
(OSG::Int32 ri, OSG::UInt32& lwi, OSG::Int32& i, OSG::UInt32& nwi)
{
   OSG::Vec3f nodata;
   OSG::Vec3f* tabA = ppatch->ptabA;
   OSG::Int32 bA=ppatch->breiteA;
	
   for(; i<ppatch->tabsize-2;++i) {
      limitpoint(lwi,
      tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[ri*bA+i+1],
      tabA[(ri-1)*bA+i], tabA[ri*bA+i-1],
      tabA[(ri+1)*bA+i+1], tabA[(ri-1)*bA+i-1],
      nodata, nodata);
	   innernormal(nwi,
	   tabA[ri*bA+i], tabA[(ri+1)*bA+i], 
      tabA[(ri+1)*bA+i+1], tabA[ri*bA+i+1],
	   tabA[(ri-1)*bA+i], tabA[(ri-1)*bA+i-1], tabA[ri*bA+i-1],	   
	   nodata, nodata);     
   }
}


template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::creaseinnerlimitfirstrow
(OSG::Int32 ri, OSG::UInt32& lwi, OSG::Int32& i, OSG::UInt32& nwi)
{
   OSG::Vec3f nodata;
   OSG::Vec3f* tabA = ppatch->ptabA;
   OSG::Int32 bA=ppatch->breiteA;
   for(; i<ppatch->tabsize-2;++i) {
     limitpointcrease(lwi,
      tabA[ri*bA+i], tabA[ri*bA+i-1], tabA[ri*bA+i+1]);
	  regcreasenormal(nwi,tabA[ri*bA+i],tabA[ri*bA+i-1],
		  tabA[(ri+1)*bA+i],tabA[(ri+1)*bA+i+1],nodata,
		  tabA[ri*bA+i+1]);	  
   }
}

template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::creaseinnerlimitlastrow
(OSG::Int32 ri, OSG::UInt32& lwi, OSG::Int32& i, OSG::UInt32& nwi)
{
   OSG::Vec3f nodata;
   OSG::Vec3f* tabA = ppatch->ptabA;
   OSG::Int32 bA=ppatch->breiteA;
   for(; i<ppatch->tabsize-2;++i) {
     limitpointcrease(lwi,
      tabA[ri*bA+i], tabA[ri*bA+i-1], tabA[ri*bA+i+1]);
	  regcreasenormal(nwi,tabA[ri*bA+i],tabA[ri*bA+i+1],
		  tabA[(ri-1)*bA+i],tabA[(ri-1)*bA+i-1],nodata,
		  tabA[ri*bA+i-1]);	  
   }
}

template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::firstlimitrow (OSG::UInt32 &lwi)
{
   //lwi is the starting index into the limit point array
   OSG::Int32 step = zweihoch[(wsdmaxdepth - targetdepth)];
   OSG::Vec3f nodata;
   
	OSG::Int32 ri=1;
	OSG::Int32 val0 = ppatch->valenz[0];
	OSG::Int32 val3 = ppatch->valenz[3];
	OSG::Vec3f* tabA = ppatch->ptabA;
	VectorCAType& corA = *(ppatch->pcorA);
	OSG::Int32 bA=ppatch->breiteA;
	OSG::Int32 i=1;
	
	OSG::UInt32 nwi=lwi;
      
	if (ppatch->iscorner[0] > 2) {				// Ecke, also einfach kopieren
		OSG::Vec3f h = tabA[ri*bA+i];		      
		ppatch->mySharedFields->limitpoints->setValue(Pnt3f(h.x(),h.y(),h.z()), lwi);	   
	   lwi+=step;		
	   creasenormal(nwi,tabA[ri*bA+i], corA[0], val0, 
         (ppatch->crindis[0][0]),
			(ppatch->crindis[0][1]));		   
	}
	else
	if (ppatch->iscorner[0] == 2) {        // crease
      if (ppatch->isregular[0] > 0) {

       Int32 crind0 = ppatch->crindis[0][0];

	    limitpointcrease(lwi,tabA[ri*bA+i],corA[0][crind0],
									    corA[0][ppatch->crindis[0][1]]);
       
       regcreasenormal(nwi,tabA[ri*bA+i], corA[0][crind0],
          corA[0][(crind0+1)%val0],
          corA[0][(crind0+2)%val0], nodata,
			 corA[0][ppatch->crindis[0][1]]);       
      }
      else { // bug in here ??
       limitpointcreaseirreg(lwi,tabA[ri*bA+i],corA[0][ppatch->crindis[0][0]],
									    corA[0][ppatch->crindis[0][1]]);
       creasenormal(nwi,tabA[ri*bA+i], corA[0], val0, (ppatch->crindis[0][0]),
												    (ppatch->crindis[0][1]));	
      }	   
	}
	else {   
		limitpointmulti(lwi,
		  tabA[ri*bA+i], corA[0], val0);
      innernormalmulti(nwi,corA[0], val0);
	}
	
	++i;

	//*****************

	if (ppatch->crease[3] == 0)
		innerlimitrow(ri, lwi, i, nwi);
	else 
		creaseinnerlimitfirstrow(ri, lwi, i, nwi);

	//*****************
   
	if (ppatch->iscorner[3] > 2) {									// corner, just copy
		OSG::Vec3f h = tabA[ri*bA+i];		
		ppatch->mySharedFields->limitpoints->setValue(Pnt3f(h.x(),h.y(),h.z()), lwi);
	   
	   lwi+=step;		
		creasenormal(nwi,tabA[ri*bA+i], corA[3], val3, (ppatch->crindis[3][0]),
													 (ppatch->crindis[3][1]));	
	}
	else
	if (ppatch->iscorner[3] == 2) {     // crease
      if (ppatch->isregular[3] > 0) {
       Int32 crind0 = ppatch->crindis[3][0];

		 limitpointcrease(lwi,tabA[ri*bA+i],corA[3][crind0],
										 corA[3][ppatch->crindis[3][1]]);	
       regcreasenormal(nwi,tabA[ri*bA+i], corA[3][crind0],
          corA[3][(crind0+1)%val3],
          corA[3][(crind0+2)%val3], nodata,
			 corA[3][ppatch->crindis[3][1]]);
      }
      else {
       limitpointcreaseirreg(lwi,tabA[ri*bA+i],corA[3][ppatch->crindis[3][0]],
										 corA[3][ppatch->crindis[3][1]]);	
		 creasenormal(nwi,tabA[ri*bA+i], corA[3], val3, (ppatch->crindis[3][0]),
													 (ppatch->crindis[3][1]));	
      }
	}
	else {            
      limitpointmulti(lwi, tabA[ri*bA+i], corA[3], val3);

      innernormalmulti(nwi,corA[3], val3);
	}	
	
}

template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::limitrow 
(OSG::Int32 ri, OSG::UInt32 &lwi)
{
   OSG::Vec3f nodata;
   OSG::Vec3f* tabA = ppatch->ptabA;
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
		  tabA[(ri+1)*bA+i+1], tabA[(ri-1)*bA+i-1],
		  nodata, nodata);
	   innernormal(nwi,
		  tabA[ri*bA+i], 
        tabA[(ri+1)*bA+i], tabA[(ri+1)*bA+i+1],
		  tabA[ri*bA+i+1], tabA[(ri-1)*bA+i], 
		  tabA[(ri-1)*bA+i-1], tabA[ri*bA+i-1],
		  nodata, nodata);   
   }
   else {      
	   limitpointcrease(lwi,
		  tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[(ri-1)*bA+i]);
   
	   regcreasenormal(nwi, tabA[ri*bA+i], tabA[(ri+1)*bA+i], 
			tabA[(ri+1)*bA+i+1], tabA[ri*bA+i+1], nodata, tabA[(ri-1)*bA+i]);      
   }
   ++i;
   //************************
   innerlimitrow(ri, lwi, i, nwi);
   //************************
   if (ppatch->crease[6] == 0) {      
	   limitpoint(lwi,
		  tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[ri*bA+i+1],
		  tabA[(ri-1)*bA+i], tabA[ri*bA+i-1],
		  tabA[(ri+1)*bA+i+1], tabA[(ri-1)*bA+i-1],
		  nodata, nodata);
	   innernormal(nwi,
		  tabA[ri*bA+i], 
        tabA[(ri+1)*bA+i], tabA[(ri+1)*bA+i+1], tabA[ri*bA+i+1],
		  tabA[(ri-1)*bA+i], tabA[(ri-1)*bA+i-1], tabA[ri*bA+i-1],		   
		  nodata, nodata);   
   }
   else {      
	   limitpointcrease(lwi,
		  tabA[ri*bA+i], tabA[(ri+1)*bA+i], tabA[(ri-1)*bA+i]);
	   regcreasenormal(nwi, tabA[ri*bA+i], tabA[(ri-1)*bA+i], 
			tabA[(ri-1)*bA+i-1], tabA[ri*bA+i-1], nodata, tabA[(ri+1)*bA+i]);   
   }   
   assert(i==ppatch->tabsize-2);
}

template<>
void WSDsubdiv<OSG::Vec3f, TRIANGLE>::lastlimitrow (OSG::UInt32 &lwi)
{  
   OSG::Vec3f nodata;
   //vertical step in the vertex/normal array:
   OSG::Int32 step = zweihoch[(wsdmaxdepth - targetdepth)];
   lwi-=step;   
   OSG::Int32 breite = wsddepthindexarray[wsdmaxdepth];
   lwi+=(((step-1)*breite)+1);

   //limitrow(ppatch->tabsize-2);
	OSG::Int32 ri=ppatch->tabsize-2;
	OSG::Int32 val1 = ppatch->valenz[1];
	OSG::Int32 val2 = ppatch->valenz[2];
	OSG::Vec3f* tabA = ppatch->ptabA;
	VectorCAType& corA = *(ppatch->pcorA);
	OSG::Int32 bA=ppatch->breiteA;
	OSG::Int32 i=1;

	OSG::UInt32 nwi=lwi;

	if (ppatch->iscorner[1] > 2) {			// corner, so just copy
		OSG::Vec3f h = tabA[ri*bA+i];		
		ppatch->mySharedFields->limitpoints->setValue(Pnt3f(h.x(),h.y(),h.z()), lwi);
	   
	   lwi+=step;		
		creasenormal(nwi,tabA[ri*bA+i], corA[1], val1, (ppatch->crindis[1][0]),
													 (ppatch->crindis[1][1]));
	}
	else
	if (ppatch->iscorner[1] == 2) {      // crease
      if (ppatch->isregular[1] >0) {
         Int32 crind0 = ppatch->crindis[1][0];
         limitpointcrease(lwi,tabA[ri*bA+i],corA[1][crind0],
									 corA[1][ppatch->crindis[1][1]]);
         regcreasenormal(nwi,tabA[ri*bA+i], corA[1][crind0],
          corA[1][(crind0+1)%val1],
          corA[1][(crind0+2)%val1], nodata,
			 corA[1][ppatch->crindis[1][1]]);
         assert((val1 == 4) || (val1 == 6));
      }
      else {
         limitpointcreaseirreg(lwi,tabA[ri*bA+i],corA[1][ppatch->crindis[1][0]],
									 corA[1][ppatch->crindis[1][1]]);
	      creasenormal(nwi,tabA[ri*bA+i], corA[1], val1, (ppatch->crindis[1][0]),
									 (ppatch->crindis[1][1]));
      }
	}
	else {	     
     limitpointmulti(lwi, tabA[ri*bA+i], corA[1], val1);
		
     innernormalmulti(nwi,corA[1], val1);
	}
	
	++i;

	//**************************
	if (ppatch->crease[8] == 0)
	innerlimitrow(ri, lwi, i, nwi);
	else creaseinnerlimitlastrow(ri, lwi, i, nwi); 
	//**************************
  

	if (ppatch->iscorner[2] > 2) {				// Ecke, also einfach kopieren
		OSG::Vec3f h = tabA[ri*bA+i];		
		ppatch->mySharedFields->limitpoints->setValue(Pnt3f(h.x(),h.y(),h.z()), lwi);
	   
	   lwi+=step;		
	   creasenormal(nwi,tabA[ri*bA+i], corA[2], val2, (ppatch->crindis[2][0]),
												 (ppatch->crindis[2][1]));	
	}
	else
	if (ppatch->iscorner[2] == 2)	{
      if (ppatch->isregular[2] >0) {
         Int32 crind0 = ppatch->crindis[2][0];

         limitpointcrease(lwi,tabA[ri*bA+i],corA[2][crind0],
									 corA[2][ppatch->crindis[2][1]]);

         regcreasenormal(nwi,tabA[ri*bA+i], corA[2][crind0],
          corA[2][(crind0+1)%val2],
          corA[2][(crind0+2)%val2], nodata,
			 corA[2][ppatch->crindis[2][1]]);
      }
      else {
       limitpointcreaseirreg(lwi,tabA[ri*bA+i],corA[2][ppatch->crindis[2][0]],
									 corA[2][ppatch->crindis[2][1]]);
	    creasenormal(nwi,tabA[ri*bA+i], corA[2], val2, (ppatch->crindis[2][0]),
												 (ppatch->crindis[2][1]));
      }
	}
	else {	          
     limitpointmulti(lwi, tabA[ri*bA+i], corA[2], val2);	
     innernormalmulti(nwi,corA[2], val2);
	}
}

