//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//                                License                                     
//                                                                            
//   This library is free software; you can redistribute it and/or modify it 
//   under the terms of the GNU Library General Public License as published  
//   by the Free Software Foundation, version 2.                             
//                                                                             
//   This library is distributed in the hope that it will be useful, but       
//   WITHOUT ANY WARRANTY; without even the implied warranty of                
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         
//   Library General Public License for more details.                          
//                                                                            
//   You should have received a copy of the GNU Library General Public         
//   License along with this library; if not, write to the Free Software       
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.3 $
//   $Date: 2003/09/19 21:35:22 $
//                                                                            
//=============================================================================

#ifndef OSGGVCACHE_H
#define OSGGVCACHE_H

#include <vector>
#include "OSGGVBase.h"
#include "OSGGVBVolAdapterBase.h"
#ifdef GV_CACHE_HASHMAP
//# include "OSGGVHashMap.h"
//# include "OSGGVPtrHasher.h"
# ifdef OSG_STL_HAS_HASH_MAP
#  ifdef OSG_HASH_MAP_AS_EXT
#   include <ext/hash_map>
#  else
#   include <hash_map>
#  endif
# else
#  include <map>
# endif
# include "OSGGVCacheData.h"
#else
# include "OSGGenvisCache.h"
#endif

#include "OSGConfig.h"
#include "OSGBaseTypes.h"
#include "OSGNode.h"
#include "OSGMaterial.h"

#ifndef OSG_NOFUNCTORS
#include "OSGTypedFunctors.h"
#endif

BEGIN_GENVIS_NAMESPACE

class OpenSGTraits;
template <class BasicTraits> class StaticInput;
typedef StaticInput<OpenSGTraits> OSGStaticInput;

#ifdef GV_CACHE_HASHMAP

struct NodePtrHasher 
{
   typedef NodePtr                   PtrType;
   typedef PtrType::StoredObjectType StoredObjectType;
   /*! Hash value used by hash_map. */
   size_t operator() (const PtrType& key) const 
   { 
#ifdef WIN32
    union Caster {
      StoredObjectType* pointer;
      size_t            address;
    };
    Caster result;
    result.pointer = key.getCPtr();
    return result.address;
#else
    return reinterpret_cast<size_t>(key.getCPtr()); 
#endif
   }
   /*! Less functor used by map. */
   bool   operator() (const PtrType& a, const PtrType& b) const 
   { 
#ifdef WIN32
    union Caster {
      StoredObjectType* pointer;
      size_t            address;
    };
    Caster ra, rb;
    ra.pointer = a.getCPtr();
    rb.pointer = b.getCPtr();
    return ra.address < rb.address;
#else
    return reinterpret_cast<size_t>(a.getCPtr()) 
      < reinterpret_cast<size_t>(b.getCPtr()); 
#endif
   }
};

class OSG_GENVISLIB_DLLMAPPING OpenSGCache
{
public:
   typedef OpenSGCache                                       Cache;
   typedef OpenSGData                                        CacheData;
   typedef NodePtrHasher                                     HashFunctor;
#ifdef OSG_STL_HAS_HASH_MAP
   typedef OSG_STDEXTENSION_NAMESPACE::hash_map<OSG::NodePtr, 
                                                CacheData, 
                                                HashFunctor> Entry;
#else
   typedef std::map<OSG::NodePtr, CacheData, HashFunctor>    Entry;
#endif
   //typedef my_hash_map<OSG::NodePtr, CacheData, HashFunctor> Entry;
   typedef Entry::iterator                                   EntryIterator;
   typedef Entry::value_type                                 EntryValue;
   typedef std::vector<BVolAdapterBaseP>                     AdapterVector;
   typedef MFNodePtr                                         ChildContainer;

#ifdef OSG_NOFUNCTORS
   typedef void (*FunctorFunc) (OSG::CNodePtr&, OSGStaticInput*);
   struct Functor
   {
   public:
      FunctorFunc _func;
      virtual ResultE call(OSG::CNodePtr& cnode, OSGStaticInput* input) {
	return _func(cnode, input);
      }
   };
   static Functor cacheFunctionFunctor2(FunctorFunc func)
   {
      Functor result;
      result._func = func;
      return result;
    }
#else
    typedef OSG::ArgsCollector<OSGStaticInput*>          FunctorArgs;

    typedef OSG::TypedVoidFunctor2Base<OSG::CPtrRefCallArg<OSG::CNodePtr>, 
                                       FunctorArgs              > Functor;
#endif

   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   OpenSGCache ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */
   inline const Entry&  getHashTable () const;
   inline OSGStaticInput* getHierarchy () const;
   void                   setHierarchy (OSGStaticInput* hier);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Clearing the cache.                                          */
   /*! \{                                                                 */   
   void       clear ();
   void       clear (const OSG::NodePtr& node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Preparing the cache from the OpenSG scenegraph.              */
   /*! \{                                                                 */
   void   apply      (const OSG::NodePtr& node);
   void   addNode    (const OSG::NodePtr& node);
   void   updateNode (const OSG::NodePtr& node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members valid during preparation.                            */
   /*! \{                                                                 */   
   inline OSG::NodePtr     getCurrentNode() const;
   inline void             setCurrentNode(const OSG::NodePtr& node);
   inline OSG::MaterialPtr getCurrentMaterial () const;
   inline void             setCurrentMaterial (const OSG::MaterialPtr& node);
   inline OSG::Matrix&       getCurrentMatrix ();
   inline const OSG::Matrix& getCurrentMatrix () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Functor management                                           */
   /*! \{                                                                 */   
   void registerEnterFunctor  (const OSG::FieldContainerType& type, 
			       const Functor& func);
   void registerLeaveFunctor  (const OSG::FieldContainerType& type, 
			       const Functor& func);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Functors.                                                    */
   /*! \{                                                                 */   
   static void staticEnterMaterialGroup (OSG::CNodePtr&, OSGStaticInput*);
   static void staticLeaveMaterialGroup (OSG::CNodePtr&, OSGStaticInput*);
   static void staticEnterTransform (OSG::CNodePtr&, OSGStaticInput*);
   static void staticLeaveTransform (OSG::CNodePtr&, OSGStaticInput*);
#ifndef OSG_NOFUNCTORS
   void functorEnterMaterialGroup       (OSG::CNodePtr&, OSGStaticInput*);
   void functorLeaveMaterialGroup       (OSG::CNodePtr&, OSGStaticInput*);
   void functorEnterTransform       (OSG::CNodePtr&, OSGStaticInput*);
   void functorLeaveTransform       (OSG::CNodePtr&, OSGStaticInput*);
#endif
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Accessing the cache data.                                    */
   /*! \{                                                                 */ 
   inline EntryIterator begin ();
   inline EntryIterator end   ();
   CacheData&       getNode  (const EntryValue&   n);
   const CacheData& getNode  (const EntryValue&   n) const;
   CacheData&       getNode  (const OSG::NodePtr& n);
   const CacheData& getNode  (const OSG::NodePtr& n) const;
   CacheData&       operator[]  (const OSG::NodePtr& n);
   const CacheData& operator[]  (const OSG::NodePtr& n) const;
   inline CacheData&       operator[]  (EntryValue& n);
   inline const CacheData& operator[]  (const EntryValue& n) const;
   CacheData&       getParent  (const OSG::NodePtr& n, int i=0);
   const CacheData& getParent  (const OSG::NodePtr& n, int i=0) const;

   void             destroyAdapter (unsigned id, unsigned len=0);
   void             clearAdapter   (unsigned id, unsigned len=0);
   void             collectAdapter (unsigned id, AdapterVector& all);
   void             collectAdapter (unsigned id, AdapterVector& all, NodePtr node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Class methods.                                               */
   /*! \{                                                                 */   
   static OpenSGCache& the ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Debugging output                                             */
   /*! \{                                                                 */   
   std::ostream&       dump (std::ostream& os) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   /*! \name Calling functors.                                            */
   /*! \{                                                                 */   
   void callEnterFunctor (OSG::NodePtr node);
   void callLeaveFunctor (OSG::NodePtr node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Functors.                                                    */
   /*! \{                                                                 */   
   void registerFunctors (void);
   static void defaultFunction  (OSG::CNodePtr&, OSGStaticInput*);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   void applyIntern (const OSG::NodePtr& node);

   std::vector<Functor>  m_enterFunctors;
   std::vector<Functor>  m_leaveFunctors;

   OSG::NodePtr     m_currentNode;
   OSG::MaterialPtr m_currentMaterial;
   OSG::Matrix      m_currentMatrix;
   OSGStaticInput*  m_hierarchy;

   //static HashFunctor s_hasher;
   Entry              m_data;
};


inline genvis::OSGStaticInput* OpenSGCache::getHierarchy () const
{
   return m_hierarchy;
}

inline OSG::NodePtr OpenSGCache::getCurrentNode() const
{
   return m_currentNode;
}
inline void         OpenSGCache::setCurrentNode(const OSG::NodePtr& node)
{
   m_currentNode = node;
}
inline OSG::MaterialPtr OpenSGCache::getCurrentMaterial () const
{
   return m_currentMaterial;
}
inline void OpenSGCache::setCurrentMaterial (const OSG::MaterialPtr& mat)
{
   m_currentMaterial = mat;
}
inline Matrix& OpenSGCache::getCurrentMatrix ()
{
   return m_currentMatrix;
}
inline const Matrix& OpenSGCache::getCurrentMatrix () const
{
   return m_currentMatrix;
}

inline OpenSGCache::EntryIterator OpenSGCache::begin ()
{
   return m_data.begin();
}
inline OpenSGCache::EntryIterator OpenSGCache::end   ()
{
   return m_data.end();
}
inline const OpenSGCache::Entry& OpenSGCache::getHashTable () const
{
   return m_data;
}

inline OpenSGCache::CacheData&       
OpenSGCache::operator[] (EntryValue& n)
{
   return n.second;
}
inline const OpenSGCache::CacheData& 
OpenSGCache::operator[] (const EntryValue& n) const
{
   return n.second;
}

inline std::ostream& operator<< (std::ostream& os, const OpenSGCache& cache)
{
   return os;
}


#else


/*! \brief Cache for storing secondary structures with OpenSG. This singleton class
    is used in the following way: 
    \begin{itemize}
    \item Register secondary structure at the cache: OSGCache::the().setHierarchy(&inst);
    \item Create secondary structure for an OpenSG subgraph with OSGCache::the().apply(subgraph);
    \item Use secondary structure
    \item Memory management is done by the secondary structure: inst.clear()/inst.destroy(). Do not
    use OSGCache::the().clearAdapter(..)/OSGCache::the().destroyAdapter(..) directly!
    \end{itemize} 
 */
class OSG_GENVISLIB_DLLMAPPING OpenSGCache
{
public:
   struct EntryLess {
      static unsigned value (const OSG::NodePtr& key) {
#ifdef WIN32
	 union Caster {
	   OSG::Node* pointer;
	   unsigned   address;
	 };
	 Caster result;
	 result.pointer = key.getCPtr();
	 return result.address;
#else
	 return reinterpret_cast<unsigned>(key.getCPtr()); 
#endif
      }
      bool operator() (const OSG::NodePtr& a, const NodePtr& b) const {
	 return value(a) < value(b);
      } 
   };
   typedef OpenSGCache                                    Cache;
   typedef GenvisCache                                    CacheData;
   typedef std::map<OSG::NodePtr,OSG::NodePtr,EntryLess>  Entry;
   typedef Entry::iterator                                EntryIterator;
   typedef std::vector<BVolAdapterBaseP>                  AdapterVector;
   typedef MFNodePtr::StorageType                         ChildContainer;

#ifdef OSG_NOFUNCTORS
   typedef void (*FunctorFunc) (OSG::CNodePtr&, OSGStaticInput*);
   struct Functor
   {
   public:
      FunctorFunc _func;
      virtual ResultE call(OSG::CNodePtr& cnode, OSGStaticInput* input) {
	return _func(cnode, input);
      }
   };
   static Functor cacheFunctionFunctor2(FunctorFunc func)
   {
      Functor result;
      result._func = func;
      return result;
   }
#else
    typedef OSG::ArgsCollector<OSGStaticInput*>          FunctorArgs;

    typedef OSG::TypedVoidFunctor2Base<OSG::CPtrRefCallArg<OSG::CNodePtr>, 
                                       FunctorArgs              > Functor;
#endif

   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   OpenSGCache  ();
   ~OpenSGCache ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */
   inline OSGStaticInput*  getHierarchy () const;
   void                    setHierarchy (OSGStaticInput* hier);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Clearing the cache.                                          */
   /*! \{                                                                 */   
   inline void             clear ();
   void                    clear (const OSG::NodePtr& node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Preparing the cache from the OpenSG scenegraph.              */
   /*! \{                                                                 */
   void                    apply (const OSG::NodePtr& node);
   void                    addNode    (const OSG::NodePtr& node);
   void                    updateNode (const OSG::NodePtr& node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members valid during preparation.                            */
   /*! \{                                                                 */   
   inline OSG::NodePtr     getCurrentNode () const;
   inline void             setCurrentNode (const OSG::NodePtr& node);
   inline OSG::MaterialPtr getCurrentMaterial () const;
   inline void             setCurrentMaterial (const OSG::MaterialPtr& node);
   inline OSG::Matrix&       getCurrentMatrix ();
   inline const OSG::Matrix& getCurrentMatrix () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Functor management.                                          */
   /*! \{                                                                 */   
   void registerEnterFunctor  (const OSG::FieldContainerType& type, 
			       const Functor& func);
   void registerLeaveFunctor  (const OSG::FieldContainerType& type, 
			       const Functor& func);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Accessing the cache data.                                    */
   /*! \{                                                                 */
   inline EntryIterator    begin ();
   inline EntryIterator    end   ();
   inline CacheData&       getNode    (const OSG::NodePtr& node) const;
   inline CacheData&       getNode    (const Entry::value_type& node) const;
   inline CacheData&       operator[] (const OSG::NodePtr& node) const;
   inline CacheData&       operator[] (const Entry::value_type& node) const;
   inline CacheData&       getParent  (const OSG::NodePtr& node, int i=0) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Internal methods for memory management.                      */
   /*! \{                                                                 */
   void                    destroyAdapter (unsigned id, unsigned len=0);
   void                    clearAdapter   (unsigned id, unsigned len=0);
   void                    collectAdapter (unsigned id, AdapterVector& all);
   void                    collectAdapter (unsigned id, AdapterVector& all, NodePtr node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Functors.                                                    */
   /*! \{                                                                 */   
   static void staticEnterMaterialGroup (OSG::CNodePtr&, OSGStaticInput*);
   static void staticLeaveMaterialGroup (OSG::CNodePtr&, OSGStaticInput*);
   static void staticEnterTransform (OSG::CNodePtr&, OSGStaticInput*);
   static void staticLeaveTransform (OSG::CNodePtr&, OSGStaticInput*);
#ifndef OSG_NOFUNCTORS
   void functorEnterMaterialGroup       (OSG::CNodePtr&, OSGStaticInput*);
   void functorLeaveMaterialGroup       (OSG::CNodePtr&, OSGStaticInput*);
   void functorEnterTransform       (OSG::CNodePtr&, OSGStaticInput*);
   void functorLeaveTransform       (OSG::CNodePtr&, OSGStaticInput*);
#endif
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Singleton.                                                   */
   /*! \{                                                                 */   
   static OpenSGCache&     the ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Dump.                                                        */
   /*! \{                                                                 */   
   std::ostream&    dump (std::ostream& os) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   /*---------------------------------------------------------------------*/
   /*! \name Accessing the cache data.                                    */
   /*! \{                                                                 */   
   inline OSG::GenvisCachePtr findNode (const OSG::NodePtr& node) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Calling functors.                                            */
   /*! \{                                                                 */   
   void callEnterFunctor (const OSG::NodePtr& node);
   void callLeaveFunctor (const OSG::NodePtr& node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Functors.                                                    */
   /*! \{                                                                 */   
   void registerFunctors (void);
   static void defaultFunction  (OSG::CNodePtr&, OSGStaticInput*);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   void  applyIntern (const OSG::NodePtr& node);

   std::vector<Functor>  m_enterFunctors;
   std::vector<Functor>  m_leaveFunctors;

   OSG::NodePtr     m_root;
   OSG::NodePtr     m_currentNode;
   OSG::MaterialPtr m_currentMaterial;
   OSG::Matrix      m_currentMatrix;
   OSGStaticInput*  m_hierarchy;
   Entry            m_all;
   static GenvisCachePtr s_dummy;
};


inline genvis::OSGStaticInput* OpenSGCache::getHierarchy () const
{
   return m_hierarchy;
}

inline OSG::NodePtr OpenSGCache::getCurrentNode() const
{
   return m_currentNode;
}
inline void OpenSGCache::setCurrentNode(const OSG::NodePtr& node)
{
   m_currentNode = node;
}

inline OSG::MaterialPtr OpenSGCache::getCurrentMaterial () const
{
   return m_currentMaterial;
}
inline void OpenSGCache::setCurrentMaterial (const OSG::MaterialPtr& mat)
{
   m_currentMaterial = mat;
}
inline Matrix& OpenSGCache::getCurrentMatrix ()
{
   return m_currentMatrix;
}
inline const Matrix& OpenSGCache::getCurrentMatrix () const
{
   return m_currentMatrix;
}

inline OpenSGCache::EntryIterator OpenSGCache::begin ()
{
   return m_all.begin();
}
inline OpenSGCache::EntryIterator OpenSGCache::end   ()
{
   return m_all.end();
}


inline void OpenSGCache::clear ()
{
   if (m_root != NullFC) {
      clear(m_root);
   }
}

inline GenvisCachePtr OpenSGCache::findNode (const NodePtr& node) const
{
   assert(node != NullFC);
   GenvisCachePtr nc =
     GenvisCachePtr::dcast(node->findAttachment(GenvisCache::getClassType(), 0));
   if (nc == NullFC) {
      return s_dummy;
   }
   return nc;
}
inline OpenSGCache::CacheData& OpenSGCache::getNode    (const NodePtr& n) const
{
   GenvisCachePtr nc = findNode(n);
   return *nc;
}
inline OpenSGCache::CacheData& OpenSGCache::getNode    (const Entry::value_type& n) const
{
   return getNode(n.first);
}
inline OpenSGCache::CacheData& OpenSGCache::operator[] (const osg::NodePtr& n) const
{
   return getNode(n);
}
inline OpenSGCache::CacheData& OpenSGCache::operator[] (const Entry::value_type& n) const
{
   return getNode(n.first);
}
inline OpenSGCache::CacheData& OpenSGCache::getParent (const osg::NodePtr& node, int i) const
{
   NodePtr p = node;
   while (i > 0) {
      p = node->getParent();
      --i;
   }
   return getNode(p);
}


inline std::ostream& operator<< (std::ostream& os, const OpenSGCache& )
{
   return os;
}


#endif



/** \brief OSGCache.
 */
typedef OpenSGCache OSGCache;

END_GENVIS_NAMESPACE
#endif
