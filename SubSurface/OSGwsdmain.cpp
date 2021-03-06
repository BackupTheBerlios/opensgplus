/*---------------------------------------------------------------------------*\
 *                       OpenSG - Dynamic Subdivision                        *
 *                                                                           *
 *                                                                           *
 *            contact: v.settgast@cg.cs.tu-bs.de * 
 *                                                                           *
\*---------------------------------------------------------------------------*/

#include "OSGwsdmain.h"

#include "OSGDynamicVolume.h"

OSG_USING_NAMESPACE

// explicit instantiations
template class OSG_SUBSURFACELIB_DLLMAPPING WSDmain<OSG::Vec3f, MyTriMesh,   TRIANGLE>;
template class OSG_SUBSURFACELIB_DLLMAPPING WSDmain<OSG::Vec3f, MyPolyMesh,  QUAD>;
template class OSG_SUBSURFACELIB_DLLMAPPING WSDmain<OSG::Vec4f, MyPolyMesh4, QUAD>;

// getFaceIndex: for a given facehandle return the index
template<class WSDVector, class Mesh, int mtype>
Int32 WSDmain<WSDVector, Mesh, mtype>::getFaceIndex (FaceHandle fh)
{
   return pmesh->property(patchIndex,fh);
}

// helper to do one subdivision step with creases

// LOOP
template<>
void WSDmain<OSG::Vec3f, MyTriMesh, TRIANGLE>::simpleCreaseSubdiv
(WorkType* wara_, WorkType* waB_, OMPoint &a, Int32 g, Int32 h, Int32 i)
{
   Int32 gg=i+g;    // add i because of possible negative values!
   Int32 numface = h-g;  
   //sharp edge
   waB_[gg%i].p = (a + wara_[gg%i].p) * 0.5f; // TODO: only regular case for approx...
   gg++;
   for (Int32 si=0; si<numface-1; si++) {        
      //edge
      waB_[gg%i].p = (((a + wara_[gg%i].p) * 0.375f) + 
      ((wara_[(gg+1)%i].p + wara_[(gg-1)%i].p) * 0.125));    
      gg++;  
   }
   //sharp edge
   waB_[gg%i].p = (a + wara_[gg%i].p) * 0.5f;
   // vertex
   Real32 alpha = 0.75f; 
   Real32 beta  = 0.125f;
   a = (alpha * a) + (beta * (wara_[g].p + wara_[h].p));
}
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::simpleCreaseSubdiv
(WorkType* wara_, WorkType* waB_, OMPoint &a, Int32 g, Int32 h, Int32 i)
{
   Int32 gg=i+g;    // add i because of possible negative values!
   Int32 val = (i/2);  
   Int32 numface = (h-g)/2;  
   //sharp edge
   waB_[gg%i].p = (a + wara_[gg%i].p) * 0.5f;
   gg++;
   //face
   waB_[(gg)%i].p = (a + wara_[(gg)%i].p + 
    wara_[(gg+1)%i].p + wara_[(gg-1)%i].p) * 0.25f;
   gg++;
   for (Int32 si=0; si<numface-1; si++) {        
      //edge
      waB_[gg%i].p = (((a + wara_[gg%i].p) * 0.375f) + 
      ((wara_[(gg+1)%i].p + wara_[(gg-1)%i].p + 
       wara_[(gg+2)%i].p + wara_[(gg-2)%i].p) * 0.0625f));    
      gg++;
      //face
      waB_[(gg)%i].p = (a + wara_[(gg)%i].p + 
       wara_[(gg+1)%i].p + wara_[(gg-1)%i].p) * 0.25f;    
      gg++;
   }
   //sharp edge
   waB_[gg%i].p = (a + wara_[gg%i].p) * 0.5f;
   // vertex
   Real32 alpha = 0.75f; 
   Real32 beta  = 0.125f;
   a = (alpha * a) + (beta * (wara_[g].p + wara_[h].p));
}

// helper to do one subdivision step
// LOOP
template<>
void WSDmain<OSG::Vec3f, MyTriMesh, TRIANGLE>::simpleSubdiv
(WorkType* wara_, WorkType* waB_, OMPoint &a, Int32 i)
{
   Int32 gg=i;
   Real32 val = (Real32)i;
   OMPoint beta(0,0,0);
   for (Int32 si=0; si<i; si++) {        // edges    
      waB_[gg%i].p = (((a + wara_[gg%i].p) * 0.375f) + 
       ((wara_[(gg+1)%i].p + wara_[(gg-1)%i].p) * 0.125f));
      beta+=wara_[gg%i].p;
      gg++;    
   }
   // vertex  
   Real32 ak = (1.0f / val) *
    (0.625f - ((0.375f + 0.25f * cos((2.0f * Pi)/val)) *
    (0.375f + 0.25f * cos((2.0f * Pi)/val))));
   Real32 falpha = 1.0f - (val*ak);  
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
      waB_[gg%i].p = (((a + wara_[gg%i].p) * 0.375f) + 
       ((wara_[(gg+1)%i].p + wara_[(gg-1)%i].p + 
         wara_[(gg+2)%i].p + wara_[(gg-2)%i].p) * 0.0625f));
      beta+=wara_[gg%i].p;
      gg++;
      //face
      waB_[(gg)%i].p = (a + wara_[(gg)%i].p + 
       wara_[(gg+1)%i].p + wara_[(gg-1)%i].p) * 0.25f;
      gamma+=wara_[gg%i].p;
      gg++;
   }
   // vertex  
   Real32 falpha = 1.0f - (3.0f / (2.0f * (Real32)val)) - (1.0f / (4.0f * (Real32)val));
   Real32 fbeta  = 3.0f / (2.0f * (Real32)val * (Real32)val);
   Real32 fgamma = 1.0f / (4.0f * (Real32)val * (Real32)val);
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
   if (k < 3.0f) {    
      betat1 = (wara_[g%v].p + wara_[h%v].p);
      t1 = (alpha_ * -2.0f) + betat1;
      t2 = betat2;
      ret = t1%t2;    
   } else {
      if (k < 4.0f) {    
         betat1 = wara_[(g+1)%v].p;
         t1 = (alpha_ * -1.0) + betat1;
         t2 = betat2;
         ret = t1%t2;    
      } else {    // k > 3  
         Real32 pidk = Pi / (k-1.0f);    
         Real32 b0 = osgsin(pidk)/(2.0f*(osgcos(pidk)-2.0f));
         
         betat1 = wara_[g%v].p * b0;
         for (Int32 i=1; i<k-1; i++)  {
         Real32 bi = osgsin((Real32)i * pidk);
         betat1+= wara_[(g+i)%v].p * bi;      
         }
         betat1+= wara_[h%v].p * b0;
         t1 = betat1;
         t2 = betat2;
         ret = t2%t1;
      }
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
   Real32 k = (Real32)((h-g) / 2);        // k is the number of adjacent polygons
   betat2 = wara_[g%v].p - wara_[h%v].p;
   if (k < 2.0) {    
      betat1 = (wara_[g%v].p + wara_[h%v].p) * -1.0;
      t1 = (alpha_ * 2.0f) + betat1;
      t2 = betat2;
      ret = t1%t2;    
   } else {    // k >= 2  
      Real32 pidk = Pi / k;
      Real32 rk = (osgcos(pidk) + 1.0f) / (k * osgsin(pidk) * (3.0f + osgcos(pidk)));
      Real32 b0 = -1.0f * rk * (1.0f + 2.0f * osgcos(pidk));
      Real32 c = osgsin(pidk) / ((3.0f + osgcos(pidk)) * k);
      betat1 = wara_[g%v].p * b0;
      gammat1 = wara_[(g+1)%v].p * c;
      for (Int32 i=1; i<k; i++)  {
         Real32 bi = (4.0f * osgsin((Real32)i * pidk)) / ((3.0f + osgcos(pidk)) * k);
         betat1+= wara_[(g+(i*2))%v].p * bi;
         c = (1.0f * (osgsin((Real32)i * pidk) + osgsin(((Real32)i + 1.0f) * pidk))) / ((3.0f + osgcos(pidk)) * k);
         gammat1+= wara_[(g+(i*2)+1)%v].p * c;
      }
      betat1+= wara_[h%v].p * b0;
      Real32 a = 4.0f * rk * (osgcos(pidk) - 1.0f);    
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
   for (Int32 j=0; j<v; j++)  {
      b1 = osgcos((2.0f*Pi*(Real32)j) / val);
      b2 = osgsin((2.0f*Pi*(Real32)j) / val);    
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
   Real32 val = (Real32)v / 2.0f;
   Real32 ak = 1.0f + osgcos(2.0f * Pi / val) + osgcos(Pi / val) * osgsqrt(2.0f*(9.0f + osgcos(2.0f * Pi / val)));
   Real32 b1,b2,c1,c2;
   OMPoint ret;
   OMPoint betat1(0,0,0); 
   OMPoint gammat1(0,0,0);
   OMPoint betat2(0,0,0); 
   OMPoint gammat2(0,0,0);
   Int32 i=0;
   for (Int32 j=0; j<v; j++)  {
      b1 = ak * osgcos(2.0f*Pi*((Real32)i+1.0f) / val);
      b2 = ak * osgcos(2.0f*Pi*(Real32)i / val);
      c1 = osgcos(2.0f*Pi*((Real32)i+1) / val) + osgcos(2.0f*Pi*((Real32)i+2.0f) / val);
      c2 = osgcos(2.0f*Pi*(Real32)i / val) + osgcos(2.0f*Pi*((Real32)i+1.0f) / val);
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

// getOptiDepth: mtype = TRIANGLE
template<>
void WSDmain<OSG::Vec3f, MyTriMesh, TRIANGLE>::getOptiDepth
(MeshType *pmesh, VertexHandle v_h)
{  
   Real32 epsilon=1.0f - NormalConeAperture;
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
      if (useCreases) {
         wara[i].isCrease = pmesh->property(isCrease,pmesh->edge_handle(hh));//pmesh->edge(pmesh->edge_handle(hh)).isCrease;
      } else {
         wara[i].isCrease = 0;
      }
      if (wara[i].isCrease > 0) {
         sharp++;
      }
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
      Int32 l=0;           // start with depth=0
      Real32 test=100;     // in test the min angle is stored      
      // average normal for all faces
      test = getGreatestAngle(wara,a,n,0,val,i);        
      while ((test < epsilon) && (l < wsdmaxdepth))  {            
         l++;                // next depth
         simpleSubdiv(wara,waB,a,i);      // subdiv with loop
         // switch workarrays:
         waT = wara; wara = waB;  waB = waT;    
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

      Int32 offset = getLimOffset(&patches[getFaceIndex(vf_it.handle())],a);       
         
      newelem->limPo.setValue(mySharedFields.limitpoints->getValue(offset));
      newelem->limNo.setValue(mySharedFields.limitnormals->getValue(offset));

      Int32 merk_index, old_index, first_index;
      old_index = -1;
      first_index = -21;

      assert(VertexListe.size()>0);
      // now the adjacent patches:
      for (; vf_it; ++vf_it)  {    
         merk_index = getFaceIndex(vf_it.handle());         
         if ((merk_index != old_index) &&
               (merk_index != first_index)){
            patches[merk_index].vertexindex[
            patches[merk_index].cvindex]=VertexListe.size()-1;
            patches[merk_index].cvindex++;
            if (patches[merk_index].maxdepth < l) {
               patches[merk_index].maxdepth = l;
            }
         }
         if (first_index == -21) first_index = merk_index;
         old_index = merk_index;
         //assert(patches[getFaceIndex(vf_it.handle())].cvindex < 5);
      }      
   } else {  // for creases: process for every sector
      Int32 numsect = wara[i].isCrease;          
      Int32 secBegin = 0;
      for (Int32 sect=0; sect<numsect; sect++) {  // sector loop
         while (wara[secBegin%i].isCrease < 1) secBegin++;
         if (wara[(secBegin)%i].isCrease < 99) {  // no empty sector?
            Int32 secEnd = secBegin+1;
            while (wara[secEnd%i].isCrease < 1) {
               secEnd++;
            }
            Int32 numface = (secEnd - secBegin);
            a = pmesh->point(v_h);
            // copy workarray
            WorkArrayType WA3;
            WorkType*     waA = &(WA3[0]);
            for (Int32 co = 0; co < i; co++) {
               waA[co].p = wara[co].p;
            }
            // get normal
            n = NormCrease(a,wara,i,secBegin,secEnd);
            Int32 l=0;      // start with depth 0
            Real32 test = getGreatestAngle(wara,a,n,secBegin,numface,i);
            while ((test < epsilon) && (l < wsdmaxdepth))  {          
               l++;         // next depth          
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
            Int32 offset = getLimOffset(&patches[faceindex],a);             
         
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
      }  // for all sectors
   }  
}

template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::getOptiDepth
(MeshType* pmesh, VertexHandle v_h)
{
   Real32 epsilon=1.0f - NormalConeAperture;
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
      if (useCreases) {
         wara[i].isCrease = pmesh->property(isCrease,pmesh->edge_handle(hh));
      } else {
         wara[i].isCrease = 0;
      }
      if (wara[i].isCrease > 0) {
         sharp++;
      }
      i++;
      wara[i].p = pmesh->point(pmesh->to_vertex_handle(pmesh->next_halfedge_handle(hh)));
      // mark edge-gamma with 8
      if (pmesh->is_boundary(hh)) {
         wara[i].isCrease = 8;
      } else {
         wara[i].isCrease = 4;
      }
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
      Int32 l=0;        // start with depth=0
      Real32 test=100;      // in test the min angle is stored      
      // average normal for all faces
      test = getGreatestAngle(wara,a,n,0,i,i);        
      while ((test < epsilon) && (l < wsdmaxdepth))  {            
         l++;                // next depth
         simpleSubdiv(wara,waB,a,i);      // subdiv
         // switch workarrays:
         waT = wara; wara = waB;  waB = waT;    
         // average normal to all adjacent faces
      test = getGreatestAngle(wara,a,n,0,i,i);          
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

      Int32 offset = getLimOffset(&patches[getFaceIndex(vf_it.handle())],a);       
         
      newelem->limPo.setValue(mySharedFields.limitpoints->getValue(offset));
      newelem->limNo.setValue(mySharedFields.limitnormals->getValue(offset));
      assert(VertexListe.size()>0);
      // now the adjacent patches:
      for (; vf_it; ++vf_it)  {    
         patches[getFaceIndex(vf_it.handle())].vertexindex[
         patches[getFaceIndex(vf_it.handle())].cvindex]=VertexListe.size()-1;
         patches[getFaceIndex(vf_it.handle())].cvindex++;
         if (patches[getFaceIndex(vf_it.handle())].maxdepth < l)
            patches[getFaceIndex(vf_it.handle())].maxdepth = l;
         assert(patches[getFaceIndex(vf_it.handle())].cvindex < 5);
      }      
   } else {  // for creases: process for every sector
      Int32 numsect = wara[i].isCrease;    
      Int32 secBegin = 0;
      for (Int32 sect=0; sect<numsect; sect++) {  // sector loop
         while (wara[secBegin%i].isCrease == 0) secBegin+=2;
         if (wara[(secBegin+1)%i].isCrease == 4) {  // no empty sector?
            Int32 secEnd = secBegin+2;
            while (wara[secEnd%i].isCrease == 0) {
               secEnd+=2;
            }
            Int32 numface = (secEnd - secBegin) / 2;
            a = pmesh->point(v_h);
            // copy workarray
            WorkArrayType WA3;
            WorkType*     waA = &(WA3[0]);
            for (Int32 co = 0; co < i; co++) {
               waA[co].p = wara[co].p;
            }
            // get normal
            n = NormCrease(a,wara,i,secBegin,secEnd);
            Int32 l=0;      // start with depth 0
        Real32 test = getGreatestAngle(wara,a,n,secBegin,numface*2,i);
            while ((test < epsilon) && (l < wsdmaxdepth))  {          
               l++;      // next depth          
               simpleCreaseSubdiv(waA,waB,a,secBegin,secEnd,i);
               // switch workarrays:
               waT = waA; waA = waB; waB = waT;
               // get normal
          test = getGreatestAngle(waA,a,n,secBegin,numface*2,i);
            }
            // thats it for this sector
            SVertexData<VectorType, MeshType> *newelem;
            VertexListe.push_back(SVertexData<VectorType, MeshType>());
            newelem = &VertexListe.back();
            newelem->maxdepth = l;
            newelem->vh = v_h;
            a = pmesh->point(v_h);
            Int32 offset = getLimOffset(&patches[getFaceIndex(wara[(secBegin+1)%i].fh)],a);             
         
            newelem->limPo.setValue(mySharedFields.limitpoints->getValue(offset));
            newelem->limNo.setValue(mySharedFields.limitnormals->getValue(offset));
            assert(VertexListe.size()>0);
            // connect adj. patches to vertex
            for (Int32 w=secBegin+1; w<secEnd; w+=2) {
               patches[getFaceIndex(wara[w%i].fh)].vertexindex[
               patches[getFaceIndex(wara[w%i].fh)].cvindex]=VertexListe.size()-1;
               patches[getFaceIndex(wara[w%i].fh)].cvindex++;
               assert(patches[getFaceIndex(wara[w%i].fh)].cvindex < 5);          
               if (patches[getFaceIndex(wara[w%i].fh)].maxdepth < l) {
                  patches[getFaceIndex(wara[w%i].fh)].maxdepth = l;
               }
            }            
         }
         secBegin+=2;
      }  // for all sectors
   }  
}

// CF moved to here

// helper for the vertexarray offset
// input: pp   -  pointer to patch
//         a   -  OpenMesh Point to find in patch pp
// output: the offset to point a within the limitpoint cache
template<class WSDVector, class Mesh, int mtype>
Int32 WSDmain<WSDVector, Mesh, mtype>::getLimOffset
(WSDdat<VectorType, MType> *pp, OMPoint a)
{
   Int32 offset=0;
   VectorType t = pp->slateO[1][1];
   if (t[0] == a[0] && t[1] == a[1] && t[2] == a[2]) { 
      offset =pp->varrayOffset;
   } else {
      t = pp->slateO[1][2];
      if (t[0] == a[0] && t[1] == a[1] && t[2] == a[2]) { 
         offset = (wsddepthindexarray[wsdmaxdepth]-1) + pp->varrayOffset;
      } else {
         t = pp->slateO[2][1];
         if (t[0] == a[0] && t[1] == a[1] && t[2] == a[2]) { 
            offset = (wsddepthindexarray[wsdmaxdepth]*wsddepthindexarray[wsdmaxdepth] - wsddepthindexarray[wsdmaxdepth]) + pp->varrayOffset;
         } else {
            offset = (wsddepthindexarray[wsdmaxdepth]*wsddepthindexarray[wsdmaxdepth] - 1) + pp->varrayOffset;
         }
      }
   }
   return offset;
}

// helper to find the greatest angle
// input: wara_   -  Array of 1-neighborhood of
//            a   -  vector of point to calculate
//            s   -  startindex of sector (a sector is defined by creases)
//          num   -  secotrsize (in cc case we have two values per face!)
//         size   -  size of wara_
// output: scalar product of greatest angle
template<class WSDVector, class Mesh, int mtype>
Real32 WSDmain<WSDVector, Mesh, mtype>::getGreatestAngle
(WorkType* wara_, OMPoint &a, OMPoint &n, Int32 s, Int32 num, Int32 size)
{
  OMPoint fb1,fb2, v1,v2, fn;
  Real32 test=10.0;  
  for (Int32 pp=s; pp<(num+s); pp++) {         // for all faces in sector
    fb1 = wara_[(pp)%size].p;    
    fb2 = wara_[(pp+1)%size].p;    
    v1 = fb1 - a;
    v2 = fb2 - a;    
    fn = v1 % v2;                              // crossproduct to get the normal fn
    fn = fn.normalize();
    Real32 coswinkel = (fn | n);        
    if (coswinkel < test) test = coswinkel;    // minimum            
  }
  return test;
}

// collectNeighbors: for a given patch pp and facehandle f_h get the four neighbors
template<>
void WSDmain<OSG::Vec3f, MyTriMesh, TRIANGLE>::collectNeighbors
(FaceHandle f_h, WSDdat<VectorType, MType> *pp)
{    
   Int32 i=0;
   HalfedgeHandle fhe_h = pmesh->halfedge_handle(f_h);  
   OSG::Vec3f tempvertex(0,0,0);
   OMPoint p = pmesh->point(pmesh->from_vertex_handle(fhe_h));
   tempvertex.setValues(p[0], p[1], p[2]);
   while (tempvertex != pp->slateO[1][1] ) {             // navigate to first halfedge
      fhe_h = pmesh->next_halfedge_handle(fhe_h);
      p = pmesh->point(pmesh->from_vertex_handle(fhe_h));
      tempvertex.setValues(p[0], p[1], p[2]);
      i++; assert(i<5); //dbg
   }
   for (i=0; i<4; i++)  {  
      pp->borders[i] = 0;
      if (pmesh->is_boundary(pmesh->edge_handle(fhe_h))) {  // boundary edge: point to own face
         pp->neighbors[i] = getFaceIndex(f_h);
         pp->borders[i] = 1;         
      } else {
         pp->neighbors[i] = 
          getFaceIndex(pmesh->face_handle(pmesh->opposite_halfedge_handle(fhe_h)));
      }
      if ((i == 1) && (!pp->isSingleTriangle)) {   // next face
         fhe_h = pmesh->next_halfedge_handle(
         pmesh->opposite_halfedge_handle(
         pmesh->next_halfedge_handle(
         fhe_h)));
      } else {
         fhe_h = pmesh->next_halfedge_handle(fhe_h);    
      }
   }      
}
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::collectNeighbors
(FaceHandle f_h, WSDdat<VectorType, MType>* pp)
{    
   HalfedgeHandle fhe_h = pmesh->halfedge_handle(f_h);  
   for (Int32 i=0; i<4; i++)  {
      if (pmesh->is_boundary(pmesh->edge_handle(fhe_h))) {  // point to itself on boundary
         pp->neighbors[i] = getFaceIndex(f_h);
      } else {
         pp->neighbors[i] = 
          getFaceIndex(pmesh->face_handle(pmesh->opposite_halfedge_handle(fhe_h)));

      }
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
   errorcase = false;
   isSetViewPort = false;
   isSetBFCull = true;
   useTexture = false;     
   oldGeop = NullFC;
#ifdef DEFINE_SHOWROOM
   useProjSize = true;
   useSilhouette = true;
   useCurvature = true;
   uniDepth = 3;  
#endif
   useCreases = m->get_property_handle(isCrease,"isCrease");
   singles.clear();
}

template<class WSDVector, class Mesh, int mtype>
WSDmain<WSDVector, Mesh, mtype>::~WSDmain()
{   
   delete[] PatchData::slateA; PatchData::slateA=NULL;
   delete[] PatchData::slateB; PatchData::slateB=NULL;
   clearInstances();
   patches.clear();
   singles.clear();
   subRefCP(mySharedFields.limitpointsptr);
   subRefCP(mySharedFields.limitnormalsptr);
   if (useTexture)
      subRefCP(mySharedFields.texcoordsptr);
}

//////////////////////////////////////////////////////////////////////
// public methods
//////////////////////////////////////////////////////////////////////

template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::setMaxDepth (UInt16 setdepth)
{
   if (patchesready) {
      SWARNING << "Setting depth after patch init is not possible!" << std::endl;
   } else {
      wsdmaxdepth = setdepth;
   }
}

template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::setMinDepth (UInt16 setdepth)
{
   wsdmindepth = setdepth;
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
   if (useTexture) {
      instance->onlyOneGeoP->setTexCoords(mySharedFields.texcoordsptr);         
   }
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
   bool recycleGeop = (oldGeop != NullFC);

   Int32 wsdmaxvarray = wsddepthindexarray[wsdmaxdepth]*wsddepthindexarray[wsdmaxdepth];
   Int32 slatesize = (wsddepthindexarray[wsdmaxdepth]+2)*(wsddepthindexarray[wsdmaxdepth]+2);
   // shared data init (limitpoints -normals)
   if (recycleGeop) {
      mySharedFields.limitpointsptr  = GeoPositions3fPtr::dcast(oldGeop->getPositions());
      mySharedFields.limitnormalsptr = GeoNormals3fPtr::dcast(oldGeop->getNormals());   
   } else {
      mySharedFields.limitpointsptr  = GeoPositions3f::create();
      mySharedFields.limitnormalsptr = GeoNormals3f::create();   
   }
   // shared usage for all geometry nodes with same basis mesh
   addRefCP(mySharedFields.limitpointsptr);
   addRefCP(mySharedFields.limitnormalsptr);
   mySharedFields.limitpoints  = mySharedFields.limitpointsptr->getFieldPtr();
   mySharedFields.limitnormals = mySharedFields.limitnormalsptr->getFieldPtr();    
 
   assert(wsdmaxvarray > 0);

   if (!recycleGeop) {
      beginEditCP(mySharedFields.limitpointsptr);
      beginEditCP(mySharedFields.limitnormalsptr);
      mySharedFields.limitpoints->resize((wsdmaxvarray*fsize));
      mySharedFields.limitnormals->resize((wsdmaxvarray*fsize));   
      endEditCP(mySharedFields.limitnormalsptr);
      endEditCP(mySharedFields.limitpointsptr);
   }

   // texture coords are shared too
   if (useTexture) {
      if (recycleGeop) {
         mySharedFields.texcoordsptr = GeoTexCoords2fPtr::dcast(oldGeop->getTexCoords()); 
      } else {
         mySharedFields.texcoordsptr = GeoTexCoords2f::create(); 
      }
      addRefCP(mySharedFields.texcoordsptr);
      mySharedFields.texcoords = mySharedFields.texcoordsptr->getFieldPtr();

      if (!recycleGeop) {
         beginEditCP(mySharedFields.texcoordsptr);
         mySharedFields.texcoords->resize((wsdmaxvarray*fsize));
         endEditCP(mySharedFields.texcoordsptr);
      }
   }
   // get memory for slates
   PatchData::slateA = new VectorType[slatesize];
   PatchData::slateB = new VectorType[slatesize];
}


// initPatches: mtype = TRIANGLE
template<>
void WSDmain<OSG::Vec3f, MyTriMesh, TRIANGLE>::initPatches (const OSG::GeometryPtr& geop)
{
   SINFO << "initPatches start" << std::endl;
   bool recycleGeop = (geop != NullFC);
   oldGeop = geop;
   if (recycleGeop) {
      SINFO << "geometry found" << std::endl;
   } else {
      SINFO << "creating new geometry" << std::endl;
   }

   Int32 i=0;
   Int32 wsdmaxvarray = wsddepthindexarray[wsdmaxdepth]*wsddepthindexarray[wsdmaxdepth];
   subdivwsd.wsdmaxdepth = this->wsdmaxdepth;
   // check for texture coords in pmesh
   useTexture = pmesh->has_vertex_texcoords();     
   subdivwsd.useTexture = this->useTexture;
   mesh2wsd.useTexture = this->useTexture;

   MeshType::FaceIter f_it;
   i=pmesh->n_faces();

   if (i==0) {
      SWARNING << "no faces at all!" << std::endl;
      patchesready=false;
      errorcase=true;
      return;
   }

   // add temp property patchIndex to identify the patch for a given facehandle
   pmesh->add_property(patchIndex,"patchIndex");   

   // step one: pairing list
   WSDpairing<OSG::Vec3f, MyTriMesh> pairinglist(pmesh);
   pairinglist.buildPairs();
   
   PatchData *temppatch;  
   i=pairinglist.pairs.size();
   assert(i>0);
   
   // now we check for creases in the middle
   WSDpairing<OSG::Vec3f, MyTriMesh>::PairFaceDataContainer::iterator p_iter;
   UInt16 doubledPatches = 0;
   temppatch = new PatchData;
   for (p_iter = pairinglist.pairs.begin(); p_iter!=pairinglist.pairs.end(); ++p_iter) {       
      mesh2wsd.insertface(p_iter->fh, p_iter->fh2, temppatch);   
      // maybe better to do that in pairing...
      if (temppatch->tricrease[4] != 0) {
         temppatch->isSingleTriangle=true;
         SINFO << "crease in the middle!" << std::endl;
         doubledPatches++;
      }
   }
   delete temppatch;
   //   
   initOSGStuff(i+doubledPatches);  
   patches.reserve(i+doubledPatches);
   
   i=0;
   // face iteration for collecting patch-data
   for (p_iter = pairinglist.pairs.begin(); p_iter!=pairinglist.pairs.end(); ++p_iter) {
      bool makeItDouble = false;
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
      if (temppatch->isSingleTriangle) {         
         SINFO << "single found" << std::endl;
      }
      // if there is a crease right through the patch we have to make it two!
      if (temppatch->tricrease[4] != 0) {
         temppatch->isSingleTriangle=true;
         makeItDouble = true;
      }                  
      temppatch->cvindex = 0;
      for (Int32 vindex=0; vindex<4; vindex++) {
         temppatch->vertexindex[vindex]=-2;
      }      
      pmesh->property(patchIndex,p_iter->fh)=i;
      if (!temppatch->isSingleTriangle) {      
         pmesh->property(patchIndex,p_iter->fh2)=i;
      } else {
         singles.push_back(i);
      }
      if (!recycleGeop) {
         if (wsdmaxdepth > 0) {
            temppatch->solltiefe = 1;
            subdivwsd.subdiv(1,temppatch);      // subdiv once for corner-limpos        
         } else {
            temppatch->solltiefe = 0;
            subdivwsd.subdiv(0,temppatch);      // subdiv once for corner-limpos        
         }
      }
      i++;      
      if (makeItDouble) {
         patches.push_back(PatchData());
         temppatch = &(patches.back());
         temppatch->mySharedFields = &mySharedFields;
         temppatch->wsdmaxdepth = this->wsdmaxdepth;
         mesh2wsd.insertface(p_iter->fh2, p_iter->fh, temppatch);     // switch faces now!
         temppatch->varrayOffset=(i*wsdmaxvarray);      
         temppatch->neighbors[0] = 0;
         temppatch->neighbors[1] = 0;
         temppatch->neighbors[2] = 0;
         temppatch->neighbors[3] = 0;
         temppatch->isSingleTriangle=true;
         temppatch->cvindex = 0;
         for (int vindex=0; vindex<4; vindex++) {
            temppatch->vertexindex[vindex]=-2;
         }
         pmesh->property(patchIndex,p_iter->fh2)=i;
  
         if (!recycleGeop) {
            if (wsdmaxdepth > 0) {
               temppatch->solltiefe = 1;
               subdivwsd.subdiv(1,temppatch);      // subdiv once for corner-limpos        
            } else {
               temppatch->solltiefe = 0;
               subdivwsd.subdiv(0,temppatch);      // subdiv once for corner-limpos        
            }
         }
         i++;      
      }
   }  
   patchesready=true;  
   SINFO << i << " patches initialized.\n";    
#ifdef DEFINE_SHOWROOM  
   numPatches = i;
#endif
   
   // collect vertex-depthinfo
   // iteration over vertices
   VertexListe.reserve(pmesh->n_vertices());
   for (MeshType::VertexIter v_it=pmesh->vertices_begin(); v_it!=pmesh->vertices_end(); ++v_it) {
      getOptiDepth(pmesh, v_it.handle());
   }  

   // iteration over pairinglist for collecting neighborhood information
   for (p_iter = pairinglist.pairs.begin(); p_iter!=pairinglist.pairs.end(); ++p_iter) {
      collectNeighbors(p_iter->fh, &patches[getFaceIndex(p_iter->fh)]);      
   }

   // now for the neighbor-neighbors
   // iteration over patches
  for (i=0; i<(Int32)patches.size(); i++) {
    patches[i].neighbor1in0 = patches[patches[i].neighbors[0]].getneighbor(i,-1);
    patches[i].neighbor3in0 = patches[patches[i].neighbors[0]].getneighbor(i,1);
    patches[i].neighbor1in2 = patches[patches[i].neighbors[2]].getneighbor(i,1);
    patches[i].neighbor3in2 = patches[patches[i].neighbors[2]].getneighbor(i,-1);
    patches[i].neighbor0in1 = patches[patches[i].neighbors[1]].getneighbor(i,1);
    patches[i].neighbor2in1 = patches[patches[i].neighbors[1]].getneighbor(i,-1);
    patches[i].neighbor0in3 = patches[patches[i].neighbors[3]].getneighbor(i,-1);
    patches[i].neighbor2in3 = patches[patches[i].neighbors[3]].getneighbor(i,1);      
    if (recycleGeop) {
       patches[i].currentdepth = wsdmaxdepth;
       patches[i].solltiefe = wsdmaxdepth/2;
    }
  }
  pmesh->remove_property(patchIndex);
  SINFO << "initPatches done" << std::endl;
}

template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::initPatches (const OSG::GeometryPtr& geop)
{     
   SINFO << "initPatches start" << std::endl;
   bool recycleGeop = (geop != NullFC);
   oldGeop = geop;
   if (recycleGeop) {
     SINFO << "geometry found" << std::endl;
   } else {
     SINFO << "creating new geometry" << std::endl;
   }

   PatchData *temppatch;  
   UInt32 i=0;
   Int32 wsdmaxvarray = wsddepthindexarray[wsdmaxdepth]*wsddepthindexarray[wsdmaxdepth];
   subdivwsd.wsdmaxdepth = this->wsdmaxdepth;
   // check for texture coords in pmesh
   useTexture = pmesh->has_vertex_texcoords();     
   subdivwsd.useTexture = this->useTexture;
   mesh2wsd.useTexture = this->useTexture;

   // face iteration: how many faces do we have?
   typename MeshType::FaceIter f_it;
   i = pmesh->n_faces();
  
   if (i==0) {
      SWARNING << "no faces at all!" << std::endl;
      patchesready=false;
      errorcase=true;
      return;
   }   
   initOSGStuff(i);
   patches.reserve(i);

   // temp property for index to patch
   pmesh->add_property(patchIndex,"patchIndex");

   i=0;
   // face iteration for collecting patch-data
   for (f_it=pmesh->faces_begin(); f_it!=pmesh->faces_end(); ++f_it) {
      if (!isQuad(f_it.handle())) {
         SWARNING << "Face of degree <> 4 detected!" << std::endl;
         patchesready=false;
         errorcase=true;
         return;
      }
      patches.push_back(PatchData());      // patchdata in vector
      temppatch = &(patches.back());        // create new element
      temppatch->mySharedFields = &mySharedFields;
      temppatch->wsdmaxdepth = this->wsdmaxdepth;
      mesh2wsd.insertface(f_it.handle(),f_it.handle(), temppatch);
      temppatch->varrayOffset=(i*wsdmaxvarray);      
      temppatch->isSingleTriangle = false;
      if (!recycleGeop) {
         if (wsdmaxdepth > 0) {
            temppatch->solltiefe = 1;
            subdivwsd.subdiv(1,temppatch);      // subdiv once for corner-limpos    
         } else {
            temppatch->solltiefe = 0;
            subdivwsd.subdiv(0,temppatch);      // subdiv once for corner-limpos    
         }
      }
      pmesh->property(patchIndex,f_it.handle())=i;
      i++;
   }  
   patchesready=true;  
   SINFO << i << " patches initialized.\n";
#ifdef DEFINE_SHOWROOM
   numPatches = i;
#endif

   // collect vertex-depthinfo
   // iteration over vertices
   VertexListe.reserve(pmesh->n_vertices());
   for (typename MeshType::VertexIter v_it=pmesh->vertices_begin(); 
      v_it != pmesh->vertices_end(); 
      ++v_it) {
      getOptiDepth(pmesh, v_it.handle());     
   }
   // collect neighborhood ( hash table is needed here )
   // iteration over patches
   i=0;
   for (f_it=pmesh->faces_begin(); f_it!=pmesh->faces_end(); ++f_it) {    
      collectNeighbors(f_it.handle(), &patches[i]);
      i++;
   }
   // now for the neighbor-neighbors
   // iteration over patches
   for (i=0; i<patches.size(); i++) {
      patches[i].neighbor1in0 = patches[patches[i].neighbors[0]].getneighbor(i,-1);
      patches[i].neighbor3in0 = patches[patches[i].neighbors[0]].getneighbor(i,1);
      patches[i].neighbor1in2 = patches[patches[i].neighbors[2]].getneighbor(i,1);
      patches[i].neighbor3in2 = patches[patches[i].neighbors[2]].getneighbor(i,-1);
      patches[i].neighbor0in1 = patches[patches[i].neighbors[1]].getneighbor(i,1);
      patches[i].neighbor2in1 = patches[patches[i].neighbors[1]].getneighbor(i,-1);
      patches[i].neighbor0in3 = patches[patches[i].neighbors[3]].getneighbor(i,-1);
      patches[i].neighbor2in3 = patches[patches[i].neighbors[3]].getneighbor(i,1);
      if (recycleGeop) {
         patches[i].currentdepth = wsdmaxdepth;
         patches[i].solltiefe = wsdmaxdepth/2;
      }
   }   
   pmesh->remove_property(patchIndex);

}


// initInstance: called per Instance, creates geometry node for every instance
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::initInstance (Int32 n, const NodePtr& parent)
{
   // create new instance-element
   myInstances.push_back(perInstanceData());
   perInstanceData* newInst = &(myInstances.back());
   // save parent for later subChild
   // CF not necessary
   newInst->parentNode = parent;
   // create node
   newInst->onlyOneNode = Node::create();
   setName(newInst->onlyOneNode,"DynSubdivNode");

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
(const OSG::NodePtr& parent, OSG::Vec3f eyepoint)
{  
   if ((!patchesready)) {
      SWARNING << "patches not initialized  - skipping perFrameSetup" << std::endl; 
      return;
   }
   if (errorcase) {
      SWARNING << "error occured - skipping perFrameSetup" << std::endl; 
      return;
   }
   if (!isSetViewPort) {
      SWARNING << "viewport variables are not set!" << std::endl;
   }
   Int32 n = getIndex(parent);         
      
#ifdef DEFINE_SHOWROOM
   WSDdat<WSDVector, mtype>::_numtris = 0;
   WSDdat<WSDVector, mtype>::_numquads = 0;
   for (int ij=0; ij<wsdmaxdepth+1; ij++)
      _depthsInUse[ij] = 0;
#endif

   VectorType eyep(eyepoint[0], eyepoint[1], eyepoint[2], 1.0);  
   UInt32 i;     
   // check position of every vertex ( silhuette, back faces... )
   for (i=0; i<VertexListe.size(); i++) {
      VectorType lp;
      Real32 cw;
      lp = eyep - VertexListe[i].limPo;
      lp.normalize();
      cw = VertexListe[i].limNo.dot(lp);     // cw: 1... 0 -> front
      if (cw+VertexClassifier < 0) {          //     0...-1 -> back
        VertexListe[i].ft = BACK;
      } else { 
         if (cw-VertexClassifier > 0) {
            VertexListe[i].ft = FRONT;
         } else {
            VertexListe[i].ft = SILUETTE;
         }
      }
   }
   // find the depth
   for (i=0; i<patches.size(); i++) {        
#ifdef DEFINE_SHOWROOM
         if (useCurvature)
#endif
      patches[i].solltiefe = patches[i].maxdepth;     
#ifdef DEFINE_SHOWROOM         
          else  patches[i].solltiefe = uniDepth;  
#endif
      
      Int32 ist_siluette = 0;
      for (Int32 si=0; si<patches[i].cvindex; si++) {
         if (patches[i].vertexindex[si]>=0) {
            ist_siluette+=VertexListe[patches[i].vertexindex[si]].ft;
         }
      }
      if (ist_siluette == 0) {
         if (isSetBFCull) {
            patches[i].isFacing = BACK;        // backface;    
         } else {
            patches[i].isFacing = FRONT;        // backface culling is deactivated;    
         }
      } else {
         if (ist_siluette == patches[i].cvindex) {
            patches[i].isFacing = FRONT;      // all faces are front faces
         } else {
#ifdef DEFINE_SHOWROOM
            if (useSilhouette)
#endif
            patches[i].isFacing = SILUETTE;        
#ifdef DEFINE_SHOWROOM
            else patches[i].isFacing = FRONT;
#endif
         }
      }
      Int32 l=patches[i].solltiefe;
      // now for the projected patch size:
#ifdef DEFINE_SHOWROOM
      if ((patches[i].isFacing != BACK) && (useProjSize)) {
#else
      if (patches[i].isFacing != BACK) {
#endif
         OSG::Vec3f SP; SP.setValues(patches[i].faceloc[0],
            patches[i].faceloc[1], patches[i].faceloc[2]);
         Real32 d = (eyepoint - SP).length();          
         Int32 a = patches[i].maxdepth;
#ifdef DEFINE_SHOWROOM
           if (!useCurvature) a=uniDepth;
#endif

         Real32 Rmin;
         if (patches[i].isFacing == SILUETTE) {
            l = (a + wsdmaxdepth) / 2;
            Rmin = d*hminSil;
         } else {
            l = a;
            Rmin = d*hmin;
         }
         Real32 Rl = patches[i].radiusD;
         if (Rl > 0) {
            while (a>0) { Rl = Rl * 0.5f; a--; }                        
            if (Rl < Rmin) {
               do {
                  l--;
                  Rl = Rl *2;
               } while (Rl < Rmin);// && (l>0));               

            } else {
               Real32 Rmax = d*hmax;
               if (Rl > Rmax) {
                  do {
                     l++;
                     Rl = Rl * 0.5f;
                  } while ((Rl > Rmax) && (l<wsdmaxdepth+1));
               }           
            }
         } else {
            SFATAL << "radius is corrupt!" << std::endl;
         }          
      }
#ifdef DEFINE_SHOWROOM 
         else {
            if ((patches[i].isFacing != BACK) && (useSilhouette)) {
               Int32 a = patches[i].maxdepth;
               if (!useCurvature) a=uniDepth;          
               if (patches[i].isFacing == SILUETTE) 
                  l = (a + wsdmaxdepth) / 2;
            }
         }
#endif
         if (l < wsdmindepth) {
            l = l - wsdmindepth;
            if ((Pmin / zweihoch[-l]) < 0.1f ) {            // very small patches are marked as back faces
               patches[i].isFacing = BACK;
            } else {
               if ((Pmin / zweihoch[-l]) < 0.5f ) {         // small patches are drawn as points
                  l=-2;
               } else {
                  if ((Pmin / zweihoch[-l]) < 3.5f ) {      // relative small patches are drawn as quads (without checking for neighbors)
                     l=-1;
                  } else {
                     l=wsdmindepth;
                  }
               }
            }
         }
         if (l > wsdmaxdepth) l = wsdmaxdepth;
         // l is the optimal depth 
         patches[i].solltiefe=l;           
      
   }  // for

   // transfer to single triangle
   for (i=0; i<(Int32)singles.size(); i++) {
      patches[singles[i]].solltiefe = patches[patches[singles[i]].neighbors[2]].solltiefe;
   }

   // beginEditCP for all patches (end after the for-loop)
   beginEditCP(myInstances[n].oneTypesPtr);
   beginEditCP(myInstances[n].oneLengthsPtr);
   beginEditCP(myInstances[n].oneIndicesPtr);   
   // delete types, lengths
   myInstances[n].oneTypes->clear();
   myInstances[n].oneLengths->clear();
   
   UInt32 indisIn=0;

#ifdef DEFINE_SHOWROOM
   UInt32 _numPatches = 0;    // count drawn patches
#endif

   //debug
   //bool ongoing,allfaces;
   // iteration over patches
   for (i=0; i<patches.size(); i++)    {      
      if (patches[i].isFacing != BACK) { 
      
#ifdef DEFINE_SHOWROOM
        _numPatches++;
#endif

        if (patches[i].solltiefe < -1) {
           patches[i].setFastPoint(myInstances[n], indisIn);
        } else {
            if (patches[i].solltiefe < 0) {
               patches[i].setFastQuad(myInstances[n], indisIn);
            } else {

       
         Int32 neue_tiefe = patches[i].solltiefe;         
         // patch depth has to be as high as the highest neighbour
         for (Int32 neibr=0; neibr<patches[i].cvindex; neibr++) {
            if (patches[patches[i].neighbors[neibr]].solltiefe > neue_tiefe) {
               neue_tiefe=patches[patches[i].neighbors[neibr]].solltiefe;
            }
         }       
         // do subdivision when the depth has been raised
         if (neue_tiefe > patches[i].currentdepth) {
            beginEditCP(myInstances[n].onlyOneGeoP->getPositions());    
            beginEditCP(myInstances[n].onlyOneGeoP->getNormals());              
            if (useTexture) 
               beginEditCP(myInstances[n].onlyOneGeoP->getTexCoords());
            subdivwsd.subdiv(neue_tiefe,&patches[i]);               
            if (useTexture) 
               endEditCP(myInstances[n].onlyOneGeoP->getTexCoords());            
            endEditCP(myInstances[n].onlyOneGeoP->getNormals());    
            endEditCP(myInstances[n].onlyOneGeoP->getPositions());
         }       
         // prepare everything
         Int32 finer_l=0, finer_r=0;
         Int32 finer_o=0, finer_u=0;

         if (patches[patches[i].neighbors[0]].solltiefe > patches[i].solltiefe) {
            finer_l = 1;
         }
         if (patches[patches[i].neighbors[2]].solltiefe > patches[i].solltiefe) {
            finer_r = 1;
         }       
         if (patches[patches[i].neighbors[1]].solltiefe > patches[i].solltiefe) {
            finer_u = 1;
         }
         if (patches[patches[i].neighbors[3]].solltiefe > patches[i].solltiefe) {
            finer_o = 1;
         }       
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
#ifdef DEFINE_SHOWROOM
       _depthsInUse[patches[i].solltiefe]++;
#endif
      }         
      }
      }
   }  
   endEditCP(myInstances[n].oneIndicesPtr);
   endEditCP(myInstances[n].oneLengthsPtr);
   endEditCP(myInstances[n].oneTypesPtr);  
   
#ifdef DEFINE_SHOWROOM
   // transfer data to output-vars
   numtris = WSDdat<WSDVector, mtype>::_numtris;
   numquads = WSDdat<WSDVector, mtype>::_numquads;
   for (int ij=0; ij<wsdmaxdepth+1; ij++)
      depthsInUse[ij] = _depthsInUse[ij];
   numPatches = _numPatches;
#endif
}



// uniformSetup: is called after bin load to have at least uniform
// tesselation (a depth of maxdepth/2 is used)
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::uniformSetup(void)
{  
   if ((!patchesready)) {
      SWARNING << "patches not initialized  - skipping uniformSetup" << std::endl; 
      return;
   }
   if (errorcase) {
      SWARNING << "an error occured - skipping uniformSetup" << std::endl; 
      return;
   }


   // set the depth
   for (UInt32 i=0; i<patches.size(); i++) {        
      patches[i].solltiefe = wsdmaxdepth/2;     
   }  

   for (UInt32 n=0; n<myInstances.size(); n++) {      
      
      // beginEditCP for all patches (end after the for-loop)
      beginEditCP(myInstances[n].oneTypesPtr);
      beginEditCP(myInstances[n].oneLengthsPtr);
      beginEditCP(myInstances[n].oneIndicesPtr);   
      // delete types, lengths
      myInstances[n].oneTypes->clear();
      myInstances[n].oneLengths->clear();
      
      UInt32 indisIn=0;
      
      //bool ongoing,allfaces;
      // iteration over patches
      for (UInt32 i=0; i<patches.size(); i++)    {                
          
         Int32 neue_tiefe = patches[i].solltiefe;         
         
         // do subdivision when the depth has been raised (only for manual calls)
         if (neue_tiefe > patches[i].currentdepth) {
            beginEditCP(myInstances[n].onlyOneGeoP->getPositions());    
            beginEditCP(myInstances[n].onlyOneGeoP->getNormals());              
            if (useTexture) 
               beginEditCP(myInstances[n].onlyOneGeoP->getTexCoords());
            subdivwsd.subdiv(neue_tiefe,&patches[i]);               
            if (useTexture) 
               endEditCP(myInstances[n].onlyOneGeoP->getTexCoords());            
            endEditCP(myInstances[n].onlyOneGeoP->getNormals());    
            endEditCP(myInstances[n].onlyOneGeoP->getPositions());
         }       
         // prepare everything
         Int32 finer_l=0, finer_r=0;
         Int32 finer_o=0, finer_u=0;
         
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

      }  // for i    
        
      endEditCP(myInstances[n].oneIndicesPtr);
      endEditCP(myInstances[n].oneLengthsPtr);
      endEditCP(myInstances[n].oneTypesPtr);  

   } // for n

}


// viewport dependant inits
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::initViewPortVars
(Real32 fovy, Int32 resX, Int32 resY)
{  
   PminSil = 0.75f * Pmin;                 // take 3/4 of Pmin for the shiluette
   gamma_winkel = (fovy*2.0f)/resY;
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

// clearInstances: savely delete all instances
template<class WSDVector, class Mesh, int mtype>
void WSDmain<WSDVector, Mesh, mtype>::preprocessInstances (void)
{  
   perInstanceDataContainer::iterator theIterator;
   if (myInstances.size() > 0) {      
      for (theIterator = myInstances.begin(); theIterator != myInstances.end();
         theIterator++) {
         beginEditCP(theIterator->onlyOneGeoP->getPositions());    
         beginEditCP(theIterator->onlyOneGeoP->getNormals());              
         if (useTexture) {
            beginEditCP(theIterator->onlyOneGeoP->getTexCoords());
         }
      }
   }
   for (UInt32 i=0; i<patches.size(); i++)    { 
      patches[i].solltiefe = wsdmaxdepth;      
      if (wsdmaxdepth > patches[i].currentdepth) {         
         subdivwsd.subdiv(wsdmaxdepth,&patches[i]);                        
      }     
            
   }
   
   if (myInstances.size() > 0) {
      for (theIterator = myInstances.begin(); theIterator != myInstances.end();
         theIterator++) {
            if (useTexture) {
             endEditCP(theIterator->onlyOneGeoP->getTexCoords());            
            }
            endEditCP(theIterator->onlyOneGeoP->getNormals());    
            endEditCP(theIterator->onlyOneGeoP->getPositions());

         beginEditCP(theIterator->oneTypesPtr);
         beginEditCP(theIterator->oneLengthsPtr);         
         theIterator->oneTypes->clear();
         theIterator->oneLengths->clear();         
         endEditCP(theIterator->oneTypesPtr);
         endEditCP(theIterator->oneLengthsPtr);
      }      
   }  
   
   
}

// helper to find the index in myInstances for parent p
template<class WSDVector, class Mesh, int mtype>
Int32 WSDmain<WSDVector, Mesh, mtype>::getIndex (const OSG::NodePtr &p)
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

















