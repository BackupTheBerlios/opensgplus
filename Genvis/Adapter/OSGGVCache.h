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
//   $Revision: 1.5 $
//   $Date: 2006/06/08 17:08:46 $
//                                                                            
//=============================================================================

#ifndef OSGGVCACHE_H
#define OSGGVCACHE_H

#include "OSGGVBase.h"
#include "OSGGVBVolAdapterBase.h"
#include <stack>
#include <vector>
#ifdef GV_CACHE_HASHMAP
//# include "OSGGVHashMap.h"
//# include "OSGGVPtrHasher.h"
# ifdef OSG_STL_HAS_HASH_MAP
#  ifdef OSG_HASH_MAP_AS_EXT
#   include "ext/hash_map"
#  else
#   include "hash_map"
#  endif
# else
#  include "map"
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

template <class FCPTR>
struct FCPtrLess {
   typedef FCPTR                              PtrType;
   typedef typename PtrType::StoredObjectType StoredObjectType;
   static size_t value (const PtrType& key) {
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
   bool operator() (const PtrType& a, const PtrType& b) const {
      return value(a) < value(b);
   } 
};

#ifdef GV_CACHE_HASHMAP

class OSG_GENVISLIB_DLLMAPPING OpenSGCache
{
public:
   struct NodePtrHasher 
   {
     typedef NodePtr                   PtrType;
     typedef PtrType::StoredObjectType StoredObjectType;
     static size_t value (const PtrType& key) {
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
     /*! Hash functor used by hash_map. */
     size_t operator() (const PtrType& key) const 
     { 
       return value(key); 
     }
     /*! Less functor used by map. */
     bool   operator() (const PtrType& a, const PtrType& b) const 
     { 
       return value(a) < value(b); 
     }
   };

   typedef OpenSGCache                                       Cache;
   typedef OpenSGData                                        CacheData;
   typedef NodePtrHasher                                     HashFunctor;
   typedef std::stack<OSG::MaterialPtr>                      MaterialStack;
   typedef std::stack<OSG::Matrix>                           MatrixStack;
   typedef std::map<OSG::NodeCorePtr,CacheData*,FCPtrLess<OSG::NodeCorePtr> >
           SharesContainer;
#ifdef OSG_STL_HAS_HASH_MAP
   typedef OSG_STDEXTENSION_NAMESPACE::hash_map<OSG::NodePtr, 
                                                CacheData, 
                                                HashFunctor> Entry;
#else
   typedef std::map<OSG::NodePtr, CacheData, HashFunctor>    Entry;
#endif
   typedef Entry::iterator                                   EntryIterator;
   typedef Entry::value_type                                 EntryValue;
   typedef std::vector<BVolAdapterBaseP>                     AdapterVector;
   typedef MFNodePtr                                         ChildContainer;

#ifdef OSG_NOFUNCTORS
   typedef bool (*FunctorFunc) (OSG::CNodePtr&, OSGStaticInput*);
   struct Functor
   {
   public:
      FunctorFunc _func;
      virtual bool call(OSG::CNodePtr& cnode, OSGStaticInput* input) {
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
   typedef OSG::ArgsCollector<OSGStaticInput*>                                           FunctorArgs;
   typedef OSG::TypedFunctor2Base<bool, OSG::CPtrRefCallArg<OSG::CNodePtr>, FunctorArgs> Functor;
#endif

   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   OpenSGCache ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */
   Entry&          getHashTable ();
   const Entry&    getHashTable () const;
   OSGStaticInput* getHierarchy () const;
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
   void   apply         (const OSG::NodePtr& node);
   void   addNode       (const OSG::NodePtr& node);
   void   collectLeaves (std::vector<CacheData*>& all, const NodePtr& node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members valid during preparation.                            */
   /*! \{                                                                 */   
   OSG::NodePtr     getCurrentNode() const;
   void             setCurrentNode(const OSG::NodePtr& node);
   OSG::MaterialPtr getCurrentMaterial () const;
   void             pushMaterial (const OSG::MaterialPtr& node);
   void             popMaterial  ();
   OSG::Matrix&       getCurrentMatrix ();
   const OSG::Matrix& getCurrentMatrix () const;
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
   static bool staticEnterMaterialGroup (OSG::CNodePtr&, OSGStaticInput*);
   static bool staticLeaveMaterialGroup (OSG::CNodePtr&, OSGStaticInput*);
   static bool staticEnterTransform (OSG::CNodePtr&, OSGStaticInput*);
   static bool staticLeaveTransform (OSG::CNodePtr&, OSGStaticInput*);
   static bool staticEnterGeometry  (OSG::CNodePtr&, OSGStaticInput*);
   static bool staticEnterSwitch    (OSG::CNodePtr&, OSGStaticInput*);
#ifndef OSG_NOFUNCTORS
   bool functorEnterMaterialGroup   (OSG::CNodePtr&, OSGStaticInput*);
   bool functorLeaveMaterialGroup   (OSG::CNodePtr&, OSGStaticInput*);
   bool functorEnterTransform       (OSG::CNodePtr&, OSGStaticInput*);
   bool functorLeaveTransform       (OSG::CNodePtr&, OSGStaticInput*);
   bool functorEnterGeometry        (OSG::CNodePtr&, OSGStaticInput*);
   bool functorEnterSwitch          (OSG::CNodePtr&, OSGStaticInput*);
#endif
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Accessing the cache data.                                    */
   /*! \{                                                                 */ 
   EntryIterator begin ();
   EntryIterator end   ();
   CacheData*           findShare(const OSG::NodePtr& node);
   CacheData&           getShare (const OSG::NodePtr& node);
   CacheData&           getNode  (const EntryValue&   n);
   const CacheData&     getNode  (const EntryValue&   n) const;
   CacheData&           getNode  (const OSG::NodePtr& n);
   const CacheData&     getNode  (const OSG::NodePtr& n) const;
   CacheData&           operator[]  (const OSG::NodePtr& n);
   const CacheData&     operator[]  (const OSG::NodePtr& n) const;
   CacheData&       operator[]  (EntryValue& n);
   const CacheData& operator[]  (const EntryValue& n) const;
   CacheData&           getParent  (const OSG::NodePtr& n, int i=0);
   const CacheData&     getParent  (const OSG::NodePtr& n, int i=0) const;

   void             clearColCache  ();
   void             clearAdapter   (u32 id, u32 len=0);
   void             destroyAdapter (u32 id, u32 len=0);
   void             collectAdapter (u32 id, 
				    AdapterVector& all, bool del=false);
   void             collectAdapter (u32 id, 
				    AdapterVector& all, const NodePtr& node, bool del=false);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Class methods.                                               */
   /*! \{                                                                 */   
   static OpenSGCache&     the ();
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
   bool callEnterFunctor (const OSG::NodePtr& node);
   bool callLeaveFunctor (const OSG::NodePtr& node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Internal methods.                                            */
   /*! \{                                                                 */   
   void        registerFunctors (void);
   static bool defaultFunction  (OSG::CNodePtr&, OSGStaticInput*);
   void        applyIntern (const OSG::NodePtr& node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

   std::vector<Functor>  m_enterFunctors;
   std::vector<Functor>  m_leaveFunctors;

   OSG::NodePtr     m_currentNode;
   MaterialStack    m_materials;
   OSG::Matrix      m_currentMatrix;
   OSGStaticInput*  m_hierarchy;

   Entry            m_data;
   SharesContainer  m_shares;
};

#else


/*! \brief Cache for storing secondary structures with OpenSG. This singleton class
    is used in the following way: 
    \begin{itemize}
    \item Register secondary structure at the cache: OSGCache::the().setHierarchy(&inst);
    \item Create secondary structure for an OpenSG subgraph with OSGCache::the().apply(subgraph);
    \item Use secondary structure
    \item Memory management is done by the secondary structure: inst.clear()/inst.destroy(). Do not
    use OSGCache::the().clearAdapter(..) directly!
    \end{itemize} 
 */
class OSG_GENVISLIB_DLLMAPPING OpenSGCache
{
public:
   struct NodePtrLess {
      typedef NodePtr                   PtrType;
      typedef PtrType::StoredObjectType StoredObjectType;
      static size_t value (const PtrType& key) {
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
      bool operator() (const PtrType& a, const PtrType& b) const {
	 return value(a) < value(b);
      } 
   };
   typedef OpenSGCache                                     Cache;
   typedef GenvisCache                                     CacheData;
   typedef std::stack<OSG::MaterialPtr>                    MaterialStack;
   typedef std::stack<OSG::Matrix>                         MatrixStack;
   typedef std::map<OSG::NodeCorePtr,CacheData*,FCPtrLess<OSG::NodeCorePtr> >
           SharesContainer;
   typedef std::map<OSG::NodePtr,OSG::NodePtr,NodePtrLess> Entry;
   typedef Entry::iterator                                 EntryIterator;
   typedef std::vector<BVolAdapterBaseP>                   AdapterVector;
   typedef MFNodePtr::StorageType                          ChildContainer;

#ifdef OSG_NOFUNCTORS
   typedef bool (*FunctorFunc) (OSG::CNodePtr&, OSGStaticInput*);
   struct Functor
   {
   public:
      FunctorFunc _func;
      virtual bool call (OSG::CNodePtr& cnode, OSGStaticInput* input) {
	return _func(cnode, input);
      }
   };
   static Functor cacheFunctionFunctor2 (FunctorFunc func)
   {
      Functor result;
      result._func = func;
      return result;
   }
#else
    typedef OSG::ArgsCollector<OSGStaticInput*>                                           FunctorArgs;
    typedef OSG::TypedFunctor2Base<bool, OSG::CPtrRefCallArg<OSG::CNodePtr>, FunctorArgs> Functor;
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
   OSGStaticInput*  getHierarchy () const;
   void             setHierarchy (OSGStaticInput* hier);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Clearing the cache.                                          */
   /*! \{                                                                 */   
   void             clear ();
   void             clear (const OSG::NodePtr& node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Preparing the cache from the OpenSG scenegraph.              */
   /*! \{                                                                 */
   void             apply         (const OSG::NodePtr& node);
   void             addNode       (const OSG::NodePtr& node);
   void             collectLeaves (std::vector<CacheData*>& all, const NodePtr& node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members valid during preparation.                            */
   /*! \{                                                                 */   
   OSG::NodePtr       getCurrentNode () const;
   void               setCurrentNode (const OSG::NodePtr& node);
   OSG::MaterialPtr   getCurrentMaterial () const;
   void               pushMaterial (const OSG::MaterialPtr& node);
   void               popMaterial  ();
   OSG::Matrix&       getCurrentMatrix ();
   const OSG::Matrix& getCurrentMatrix () const;
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
   EntryIterator    begin ();
   EntryIterator    end   ();
   CacheData*       findShare  (const OSG::NodePtr& node);
   CacheData&       getShare   (const OSG::NodePtr& node);
   CacheData&       getNode    (const OSG::NodePtr& node) const;
   CacheData&       getNode    (const Entry::value_type& node) const;
   CacheData&       operator[] (const OSG::NodePtr& node) const;
   CacheData&       operator[] (const Entry::value_type& node) const;
   CacheData&       getParent  (const OSG::NodePtr& node, int i=0) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Internal methods for memory management.                      */
   /*! \{                                                                 */
   void              clearColCache  ();
   void              clearAdapter   (u32 id, u32 len=0);
   void              destroyAdapter (u32 id, u32 len=0);
   void              collectAdapter (u32 id, 
				     AdapterVector& all, bool del=false);
   void              collectAdapter (u32 id, 
				     AdapterVector& all, const NodePtr& node, bool del=false);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Functors.                                                    */
   /*! \{                                                                 */   
   static bool staticEnterMaterialGroup (OSG::CNodePtr&, OSGStaticInput*);
   static bool staticLeaveMaterialGroup (OSG::CNodePtr&, OSGStaticInput*);
   static bool staticEnterTransform (OSG::CNodePtr&, OSGStaticInput*);
   static bool staticLeaveTransform (OSG::CNodePtr&, OSGStaticInput*);
   static bool staticEnterGeometry  (OSG::CNodePtr&, OSGStaticInput*);
   static bool staticEnterSwitch    (OSG::CNodePtr&, OSGStaticInput*);
#ifndef OSG_NOFUNCTORS
   bool functorEnterMaterialGroup   (OSG::CNodePtr&, OSGStaticInput*);
   bool functorLeaveMaterialGroup   (OSG::CNodePtr&, OSGStaticInput*);
   bool functorEnterTransform       (OSG::CNodePtr&, OSGStaticInput*);
   bool functorLeaveTransform       (OSG::CNodePtr&, OSGStaticInput*);
   bool functorEnterGeometry        (OSG::CNodePtr&, OSGStaticInput*);
   bool functorEnterSwitch          (OSG::CNodePtr&, OSGStaticInput*);
#endif
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Class methods.                                               */
   /*! \{                                                                 */   
   static OpenSGCache& the ();
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
   OSG::GenvisCachePtr findNode (const OSG::NodePtr& node) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Calling functors.                                            */
   /*! \{                                                                 */   
   bool callEnterFunctor (const OSG::NodePtr& node);
   bool callLeaveFunctor (const OSG::NodePtr& node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Internal methods.                                            */
   /*! \{                                                                 */   
   void        registerFunctors (void);
   static bool defaultFunction  (OSG::CNodePtr&, OSGStaticInput*);
   void        applyIntern (const OSG::NodePtr& node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

   std::vector<Functor>  m_enterFunctors;
   std::vector<Functor>  m_leaveFunctors;

   OSG::NodePtr     m_currentNode;
   MaterialStack    m_materials;
   OSG::Matrix      m_currentMatrix;
   OSGStaticInput*  m_hierarchy;
   Entry            m_all;
   SharesContainer  m_shares;
   static GenvisCachePtr s_dummy;
};

#endif

/** \brief OSGCache.
 */
typedef OpenSGCache OSGCache;

END_GENVIS_NAMESPACE
#endif
