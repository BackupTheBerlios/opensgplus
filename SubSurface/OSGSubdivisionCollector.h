
#ifndef OSGSUBDIVISIONCOLLECTOR_H
#define OSGSUBDIVISIONCOLLECTOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "OSGConfig.h"
#include "OSGViewport.h"


OSG_BEGIN_NAMESPACE


template <class Subdiv> 
class OSGSubdivisionCollector
{
public:
  
   typedef std::vector<Subdiv> SubdivContainer;

   // construct with root
   inline OSGSubdivisionCollector(OSG::NodePtr& _root) {
      // traverse the scene and store all Subdiv-Nodes in collection
      OSG::NodePtr  curNode = _root;  
      traverse(curNode);
   }

   // destructor: free mem!
   inline ~OSGSubdivisionCollector () {
      collection.clear();
   } 


   // this is the list for the Collection
   SubdivContainer collection;
   

   // call for prepareFrame
   inline void prepareFrame(const ViewportPtr& port) {
      // for all Subdivs in the collection
      for (UInt32 i=0; i<collection.size(); i++) {
         collection[i]->prepareFrame(port);
      }
   }     


private: 
   //--- private elements ---  

   inline void compCore(OSG::NodePtr& nptr) {
      Subdiv test = Subdiv::dcast(nptr->getCore());
      if (test!=NullFC) {
         collection.push_back(test);
      }
   }

   inline void traverse(OSG::NodePtr& nptr) {
      compCore(nptr);
      if (nptr->getNChildren()>0) {
          for (UInt32 i=0; i<nptr->getNChildren(); i++) {                     
            NodePtr child = nptr->getChild(i);
            traverse(child);                     
          }
      }
   }

   
     
};

OSG_END_NAMESPACE

#endif



