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
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

template<class WSDVector, int mtype>
WSDsubdiv<WSDVector, mtype>::WSDsubdiv()
{

}


template<class WSDVector, int mtype>
WSDsubdiv<WSDVector, mtype>::~WSDsubdiv()
{

}

////////////////////////////////////////////////////////////////////
// implementation
////////////////////////////////////////////////////////////////////


template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::subdiv(Int32 depth, WSDdat<WSDVector, mtype> *pp)
{
   ppatch=pp;
   targetdepth=depth;
   ppatch->initptabs();
   Int32 ri,wi,erster;
   erster=0;
   //Int32 depth=3;
   //=ppatch->maxdepth;
   //assert(ppatch->tabsize==4);   
   Int32 newdepth=depth;
   while(depth>0) {	   
	  corsupdate();
      // compute new points:
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
      //if(depth!=1)
      //   ppatch->drawwiretab(1.0f, 0.0f, 0.0f);
      --depth;
   }
   corsupdate();	// die letzen änderungen für limitpoints kopieren!

   limits(); // mit normals

   ppatch->currentdepth = newdepth;
}
  

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::limits()
{   
   Int32 ri;
   UInt32 lindex=ppatch->varrayOffset;			// offset im varray als startpunkt
   firstlimitrow(lindex);   
   for(ri=2; ri<ppatch->tabsize - 2; ++ri) {
      limitrow(ri,lindex);	  
   }
   lastlimitrow(lindex);   
}

/////////////////////////////////////////////////////////////////////////
// default
/////////////////////////////////////////////////////////////////////////

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::initwsdsubdiv(void)
{}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::inneredgerow(Int32 ri, Int32 wi, Int32& i, Int32& j)
{}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::firstedgerow(void)
{}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::edgerow(Int32 ri, Int32 wi)
{}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::lastedgerow(Int32 ri, Int32 wi)
{}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::innervertexrow(Int32 ri, Int32 wi, Int32& i, Int32& j)
{}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::firstvertexrow(void)
{}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::vertexrow(Int32 ri, Int32 wi)
{}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::lastvertexrow(Int32 ri, Int32 wi)
{}


template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::firstlimitrow(UInt32 &lwi)
{}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::limitrow(Int32 ri, UInt32 &lwi)
{}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::lastlimitrow(UInt32 &lwi)
{}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::innerlimitrow(Int32 ri, UInt32& lwi, Int32& i, UInt32& nwi)
{}


template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::vertexpoint(WSDVector& v, WSDVector& v0, WSDVector& v1, 
     WSDVector& v2, WSDVector& v3, WSDVector& v4, 
     WSDVector& w1, WSDVector& w2,WSDVector& w3, WSDVector& w4)
{}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::edgepoint(WSDVector& v, WSDVector& v1, WSDVector& v2, 
     WSDVector& w1, WSDVector& w2,WSDVector& w3, WSDVector& w4, Int32 crease)
{}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::facepoint(WSDVector& v, WSDVector& v1, WSDVector& v2, 
                                            WSDVector& v3, WSDVector& v4)
{}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::limitpoint(UInt32& lwi, WSDVector& v0, WSDVector& v1, 
     WSDVector& v2, WSDVector& v3, WSDVector& v4, WSDVector& w1, WSDVector& w2, WSDVector& w3, 
     WSDVector& w4)
{}

template<class WSDVector, int mtype>
void WSDsubdiv<WSDVector, mtype>::innernormal(UInt32& nwi, WSDVector& v0, WSDVector& v1, 
     WSDVector& v2, WSDVector& v3, WSDVector& v4, WSDVector& w1, WSDVector& w2, WSDVector& w3, 
     WSDVector& w4)
{}



