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

// template specializations
#include "OSGwsdsubdivquad3f.inl"
#include "OSGwsdsubdivquad4f.inl"
#include "OSGwsdsubdivtri.inl"

// explicit template instantiations
template class OSG_SUBSURFACELIB_DLLMAPPING WSDsubdiv<OSG::Vec3f, TRIANGLE>;
template class OSG_SUBSURFACELIB_DLLMAPPING WSDsubdiv<OSG::Vec3f, QUAD>;
template class OSG_SUBSURFACELIB_DLLMAPPING WSDsubdiv<OSG::Vec4f, QUAD>;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

template<class WSDVector, int mtype>
WSDsubdiv<WSDVector, mtype>::WSDsubdiv()
{
   wsdmaxdepth=2;
}


template<class WSDVector, int mtype>
WSDsubdiv<WSDVector, mtype>::~WSDsubdiv()
{

}

////////////////////////////////////////////////////////////////////
// implementation
////////////////////////////////////////////////////////////////////


template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::subdiv(Int32 depth, WSDdat<VectorType, MType> *pp)
{
   ppatch=pp;
   targetdepth=0;
   ppatch->initptabs();
   Int32 ri,wi,erster;
   erster=0;
   Int32 newdepth=depth;
   while(depth>0) {     
      targetdepth++;
      corsupdate();              // for cc only
      // compute new points:
      allcorners();              // for loop only
      firstedgerow();
      firstvertexrow();
      edgerow(1, 2);
      for(ri=2, wi=2; ri<ppatch->tabsize - 2; ++ri) {
         vertexrow(ri, ++wi);
         edgerow(ri, ++wi);
      }
      lastvertexrow(ri, ++wi);
      lastedgerow(ri, ++wi);
      // prepare next subdivision-level
      assert(ppatch->tabsize==ri+2);
      ppatch->tabsize=2*(ppatch->tabsize)-3;
      assert(ppatch->tabsize==wi+1);

      ppatch->rotateptabs(erster);
      erster++;
      --depth;
   }
   corsupdate();                 // for cc only
   limits(); // with normals
   

   // texture coords
   if (useTexture) {
      ppatch->initptex();
      erster=0;
      depth=newdepth;
      targetdepth=0;
      while(depth>0) {
         targetdepth++;        
         vertexrowtex(1,1);
         edgerowtex(1, 2);
         for(ri=2, wi=2; ri<ppatch->tabsize - 2; ++ri) {
               vertexrowtex(ri, ++wi);
               edgerowtex(ri, ++wi);
         }
         vertexrowtex(ri, ++wi);     
         // prepare next subdivision-level     
         ppatch->tabsize=2*(ppatch->tabsize)-3;     
         ppatch->rotateptabs(erster);
         erster++;
         --depth;
      } 
      textureCoords();
   }
   ppatch->currentdepth = newdepth;     
}
  
template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::limits()
{   
   Int32 ri;
   UInt32 lindex=ppatch->varrayOffset;      // offset in varray as startpoint
   firstlimitrow(lindex);   
   for(ri=2; ri<ppatch->tabsize - 2; ++ri) {
      limitrow(ri,lindex);    
   }
   lastlimitrow(lindex);  
}


template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::textureCoords()
{
   UInt32 lindex=ppatch->varrayOffset;
   VectorType* tabA = ppatch->ptabA;
   for(Int32 ri=1; ri<ppatch->tabsize-1; ++ri) {
      for (Int32 i=1; i<ppatch->tabsize-1; i++) {
         VectorType h = tabA[(ri*ppatch->breiteA)+i];
         ppatch->mySharedFields->texcoords->setValue(Vec2f(h.x(),h.y()),lindex);       
         lindex+=zweihoch[(wsdmaxdepth - targetdepth)];  
      }
      lindex-=zweihoch[(wsdmaxdepth - targetdepth)];  
      lindex++;
      lindex+=wsddepthindexarray[wsdmaxdepth]*(zweihoch[(wsdmaxdepth - targetdepth)]-1);
   }   
}

/////////////////////////////////////////////////////////////////////////

// texture coords calculation

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::edgerowtex(OSG::Int32 ri, OSG::Int32 wi)
{
   OSG::Int32 i=0;
   OSG::Int32 j=0;
   VectorType* tabA = ppatch->ptabA;
   VectorType* tabB = ppatch->ptabB;
   OSG::Int32 bA=ppatch->breiteA;
   OSG::Int32 bB=wsddepthindexarray[wsdmaxdepth]+2;    
   ++i; ++j;
   tabB[wi*bB+j] = (tabA[(ri+1)*bA+i]+tabA[ri*bA+i])*0.5;     
   ++j;
//*************************
   tabB[wi*bB+j] = 
      (tabA[(ri+1)*bA+i] + tabA[(ri+1)*bA+i+1] +
         tabA[ri*bA+i+1] + tabA[ri*bA+i]) * 0.25;
   ++j; ++i;
   for(; i<ppatch->tabsize-2;++i) {
      tabB[wi*bB+j] = (tabA[(ri+1)*bA+i]+tabA[ri*bA+i])*0.5;     
      ++j;
      tabB[wi*bB+j] = (tabA[(ri+1)*bA+i] + tabA[(ri+1)*bA+i+1] +
                       tabA[ri*bA+i+1] + tabA[ri*bA+i]) * 0.25;
      ++j;
   }
//*************************
   tabB[wi*bB+j] = (tabA[(ri+1)*bA+i]+tabA[ri*bA+i])*0.5;        
}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::vertexrowtex(OSG::Int32 ri, OSG::Int32 wi)
{  
   OSG::Int32 i=0;
   OSG::Int32 j=0;
   VectorType* tabA = ppatch->ptabA;
   VectorType* tabB = ppatch->ptabB;
   OSG::Int32 bA=ppatch->breiteA;
   OSG::Int32 bB=wsddepthindexarray[wsdmaxdepth]+2;        
   ++i; ++j;  
   tabB[wi*bB+j] = tabA[ri*bA+i];  
   ++j;
//*************************
   tabB[wi*bB+j] = (tabA[ri*bA+i]+tabA[ri*bA+i+1])*0.5;  
   ++j; ++i;  
   for(; i<ppatch->tabsize-2;++i) {
      tabB[wi*bB+j] = tabA[ri*bA+i];
      ++j;
      tabB[wi*bB+j] = (tabA[ri*bA+i]+tabA[ri*bA+i+1])*0.5;      
      ++j;
   }
//************************* 
   tabB[wi*bB+j] = tabA[ri*bA+i];    
}

/////////////////////////////////////////////////////////////////////////
// default
/////////////////////////////////////////////////////////////////////////

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::initwsdsubdiv(void)
{
}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::inneredgerow(Int32 ri, Int32 wi, Int32& i, Int32& j)
{
}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::allcorners(void)
{
}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::firstedgerow(void)
{
}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::edgerow(Int32 ri, Int32 wi)
{
}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::lastedgerow(Int32 ri, Int32 wi)
{
}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::innervertexrow(Int32 ri, Int32 wi, Int32& i, Int32& j)
{
}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::firstvertexrow(void)
{
}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::vertexrow(Int32 ri, Int32 wi)
{
}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::lastvertexrow(Int32 ri, Int32 wi)
{
}


template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::firstlimitrow(UInt32 &lwi)
{
}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::limitrow(Int32 ri, UInt32 &lwi)
{
}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::lastlimitrow(UInt32 &lwi)
{
}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::innerlimitrow(Int32 ri, UInt32& lwi, Int32& i, UInt32& nwi)
{
}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::tricrease4_innerlimitrow(Int32 ri, UInt32& lwi, Int32& i, UInt32& nwi)
{
}


template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::vertexpoint(VectorType& v, VectorType& v0, VectorType& v1, 
     VectorType& v2, VectorType& v3, VectorType& v4, 
     VectorType& w1, VectorType& w2,VectorType& w3, VectorType& w4)
{
}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::edgepoint(VectorType& v, VectorType& v1, VectorType& v2, 
     VectorType& w1, VectorType& w2,VectorType& w3, VectorType& w4, Int32 crease)
{
}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::facepoint(VectorType& v, VectorType& v1, VectorType& v2, 
                                            VectorType& v3, VectorType& v4)
{
}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::limitpoint(UInt32& lwi, VectorType& v0, VectorType& v1, 
     VectorType& v2, VectorType& v3, VectorType& v4, VectorType& w1, VectorType& w2, VectorType& w3, 
     VectorType& w4)
{
}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::limitpointcs
(OSG::UInt32& lwi, VectorType& v0, VectorType& v1, VectorType& v2,
 OSG::Int32 type0,  OSG::Int32 type1, OSG::Int32 type2 )
{
}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::innernormal(UInt32& nwi, VectorType& v0, VectorType& v1, 
     VectorType& v2, VectorType& v3, VectorType& v4, VectorType& w1, VectorType& w2, VectorType& w3, 
     VectorType& w4)
{
}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::limitnormalcs(OSG::UInt32& nwi, 
   VectorType& v0, VectorType *array, OSG::Int32 val, OSG::Int32 a, OSG::Int32 b,
   OSG::Int32 type0,  OSG::Int32 type1, OSG::Int32 type2 )
{
}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::corsupdate(void)
{
}


template<class WSDVector, int mtype>
Int32 WSDsubdiv<WSDVector, mtype>::getCreaseType(Int32 crease, Int32 C0, Int32 C1, Int32 ri)
{ 
   return 0; 
}

template<class WSDVector, int mtype>
Int32 WSDsubdiv<WSDVector, mtype>::getIsRegular(Int32 crease)
{ 
   return 0; 
}
