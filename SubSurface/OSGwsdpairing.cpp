/*---------------------------------------------------------------------------*\
 *                       OpenSG - Dynamic Subdivision                        *
 *                                                                           *
 *                                                                           *
 *            contact: v.settgast@cg.cs.tu-bs.de * 
 *                                                                           *
\*---------------------------------------------------------------------------*/

#include "OSGwsdpairing.h"
#include "OSGwsddat.h"

OSG_USING_NAMESPACE

// explicit instantiations
template class OSG_SUBSURFACELIB_DLLMAPPING WSDpairing<OSG::Vec3f, MyTriMesh>;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

template<class WSDVector, class Mesh>
WSDpairing<WSDVector, Mesh>::WSDpairing( MeshType *_mesh )
{
   mesh = _mesh;
}

template<class WSDVector, class Mesh>
WSDpairing<WSDVector, Mesh>::~WSDpairing()
{
}

//////////////////////////////////////////////////////////////////////

//! main method for pairing
template<class WSDVector, class Mesh>
void 
WSDpairing<WSDVector, Mesh>::buildPairs( void )
{
   SLOG << "Entering pairing algorithm" << std::endl;
   // clear the pairing list
   prepairs.clear();
   // init priority lists
   IndexContainer s[4];
   UInt32 i=0;
   // build vector of faces
   PairFaceData *tempData;   
   typename MeshType::FaceIter f_it;
   for (f_it=mesh->faces_begin(); f_it!=mesh->faces_end(); ++f_it) {    
      prepairs.push_back(PairFaceData());
      tempData = &(prepairs.back());
      tempData->fh = f_it.handle();
      tempData->freeneighbors = 3;
      tempData->partner = -1;
      tempData->prio = 3;
      //prepairs.push_back(*tempData);
      //s[3].push_back(i); // not yet, because of border faces have less free neighbors!
      i++;
   }   
   // get neighbors
   for (i=0; i<prepairs.size(); i++) {
      typename MeshType::HalfedgeHandle h=mesh->halfedge_handle(prepairs[i].fh);
      for (Int32 z=0; z<3; z++) {
       Int32 getIndex = getIndexOfFace(mesh->face_handle(mesh->opposite_halfedge_handle(h)));
       //assert(getIndex != -1); -1 means no neighbor (border)
       if (getIndex == -1) {
          // border -> one less free neighbour
          prepairs[i].freeneighbors--;
          prepairs[i].n[z] = i;
       } else {
          prepairs[i].n[z] = getIndex;
       }
       h = mesh->next_halfedge_handle(h);
      }      
   }
   for (i=0; i<prepairs.size(); i++) {         
      if (prepairs[i].freeneighbors < 2) {
         prepairs[i].prio = 0;
      }
      else
      if (prepairs[i].freeneighbors < 3) {
         prepairs[i].prio = 0;
         Int32 su=0;
         for (Int32 j=0; j<3; j++) su+=prepairs[prepairs[i].n[j]].prio;
         if (su == 6) {
            prepairs[i].prio = 2;
         } else {
            prepairs[i].prio = 1;
         }
      }
      s[prepairs[i].prio].push_back(i);
   }
   
   // lets go now!
   bool finished = false;   
   while (!finished) {   
      Int32 workindex = -1; 
      Int32 p=0;
      // 1. get triangle f from nonempty highest prio set
      while (workindex == -1) {
         if (s[p].size() > 0) {
            workindex = s[p].back();   // get last element
         } else {
            p++;
         }
         if (p > 3) { 
            workindex = 0;
            finished = true;            
         }
      }
      if (finished) continue;    // ignore the rest, we are finish!

      // 2. look for a free partner for f (workindex)
      bool neighborFound = false;
      if (prepairs[workindex].freeneighbors > 0) {
         //
         Int32 l=0;         
         while ((prepairs[workindex].n[l] == workindex) ||
            ((prepairs[prepairs[workindex].n[l]].prio > 3) 
            && (l<3))) l++;         
         Int32 newpartner = prepairs[workindex].n[l];
         for (Int32 gg=1; gg<3; gg++) {
            if ((prepairs[prepairs[workindex].n[gg]].prio < prepairs[newpartner].prio) 
             && (prepairs[prepairs[workindex].n[gg]].prio < 4)
             && (prepairs[workindex].n[gg] != workindex)) {
               newpartner = prepairs[workindex].n[gg];
            }
         }
         prepairs[workindex].partner = newpartner;
         prepairs[newpartner].partner = workindex;
         neighborFound = true;
         // delete f,g from set
         assert(prepairs[workindex].prio >= 0);
         assert(prepairs[workindex].prio < 4);
         assert(prepairs[newpartner].prio >= 0);
         assert(prepairs[newpartner].prio < 4);

         IndexContainer::iterator toDel;
         toDel = getIterator(s[prepairs[workindex].prio],workindex);
         if (toDel != s[prepairs[workindex].prio].end()) {
            s[prepairs[workindex].prio].erase(toDel);
         } else {
            SLOG << "end()" << std::endl;
         }
         toDel = getIterator(s[prepairs[newpartner].prio],newpartner);
         if (toDel != s[prepairs[newpartner].prio].end()) {
            s[prepairs[newpartner].prio].erase(toDel);
         } else {
            SLOG << "end() np=" << newpartner << std::endl;
         }
         prepairs[workindex].prio = 5;
         prepairs[newpartner].prio = 5;
         // put the finished pair into the pairinglist "pair"
         pairs.push_back(PairFaceData());
         tempData = &(pairs.back());
         tempData->fh  = prepairs[workindex].fh;
         tempData->fh2 = prepairs[newpartner].fh;
         tempData->freeneighbors = 0;
         tempData->partner = 1;
         tempData->prio = 0;         
      } else {   // no free neighbors left
         s[p].erase(s[p].end() - 1); // remove last element (which is workindex!)
         prepairs[workindex].prio = 5;
         pairs.push_back(PairFaceData());
         tempData = &(pairs.back());
         tempData->fh  = prepairs[workindex].fh;
         tempData->fh2 = prepairs[prepairs[workindex].n[0]].fh;
         tempData->freeneighbors = 0;
         tempData->partner = 0;
         tempData->prio = 0;         
      }      
      // 3. increase neighbor priorities for f and g
      bool movedFromS3 = false;
      while (workindex > -1) {
         for (Int32 nn=0; nn<3; nn++) {
            Int32 ni = prepairs[workindex].n[nn];
            prepairs[ni].freeneighbors--;
            Int32 si = prepairs[ni].prio;
            if (si > 3) continue;   // this neighbor is not in the sets
            IndexContainer::iterator toMove = getIterator(s[si],ni);
            if (toMove != s[si].end()) {
               s[si].erase(toMove);
               if (si == 3) {
                  prepairs[ni].prio--; si--;
                  movedFromS3 = true;
               } else {
                  prepairs[ni].prio=0; si=0;
               }
               s[si].push_back(ni);
            } else {
               SLOG << ni << " in S" << si << " not found!" << std::endl;
            }
         }
         if (neighborFound) {
            workindex = prepairs[workindex].partner;
            neighborFound = false;
         } else {
            workindex = -1;
         }
      }      
      if (movedFromS3) {
         IndexContainer::iterator toMove = s[2].begin();
         for (;toMove != s[2].end(); toMove++) {
            bool toS1 = false;
            for (Int32 z=0; z<3; z++) {
               if (prepairs[prepairs[*toMove].n[z]].prio < 3) toS1 = true;
            }
            if (toS1) {
               prepairs[*toMove].prio = 1;
               s[1].push_back(*toMove);
               s[2].erase(toMove);  //?
               if (s[2].size() < 1) break;
               toMove = s[2].begin();               
            }
         }
      }      
   } 
}

//! returns the index within vector "prepairs" of face "fh"
template<class WSDVector, class Mesh>
Int32 
WSDpairing<WSDVector, Mesh>::getIndexOfFace(FaceHandle fh)
{
   for (UInt32 i=0; i<prepairs.size(); i++) {
      if (prepairs[i].fh == fh) return i;
   }
   return -1;
}


//! returns the iterator of i in vector v
template<class WSDVector, class Mesh>
WSDpairing<WSDVector, Mesh>::IndexContainer::iterator 
WSDpairing<WSDVector, Mesh>::getIterator(IndexContainer &v, Int32 i)
{
   IndexContainer::iterator iter = v.begin();
   for (; iter !=v.end(); iter++) {
      if (*iter == i) return iter;
   }
   return v.end();
}
