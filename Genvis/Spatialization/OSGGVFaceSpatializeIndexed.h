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
//   $\Revision$
//   $\Date$
//                                                                            
//=============================================================================

#ifndef OSGGVFACESPATIALIZEINDEXED_H
#define OSGGVFACESPATIALIZEINDEXED_H

#include "OSGGVBase.h"
#include "OSGGVInput.h"
#include "OSGGVSingleTraverser.h"
#include "OSGGVGroup.h"

#include "OSGGroup.h"
#include "OSGGeometry.h"

BEGIN_GENVIS_NAMESPACE

template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING FaceSpatializeIndexed
{
public:
   typedef BoundingVolume<Real>                                  BVol;
   typedef typename SingleTraverserBase<BasicTraits>::ResultType ResultT;
   typedef OpenSGFaceBase<BasicTraits>                           AdapterType;
   typedef BVolGroupBase                                         GroupType;
   typedef BVolAdapterBase                                       GeneralType; 
   typedef typename BasicTraits::TransformType                   TransformType; 

   FaceSpatializeIndexed ();
   ~FaceSpatializeIndexed ();

   OSG::NodePtr getRoot  () const;
   void         setDepth (u32 value);
   u32          getDepth () const;
   void         setFlat  (bool flag);
   bool         getFlat  () const;

   bool     InitEnter  (GeneralType* a, const TransformType& m);
   bool     InitLeave  (GeneralType* a, const TransformType& m);
   ResultT  InnerEnter (GroupType*   g0);
   ResultT  InnerLeave (GroupType*   g0);
   ResultT  LeafEnter  (AdapterType* p0);

   static OSG::GroupPtr getGroupCore ();
   static void          setGroupCore (const OSG::GroupPtr& group);

private:
   class OSG_GENVISLIB_DLLMAPPING CategoryRaw
   {
   public:
     struct GeometryPtrHasher 
     {
       typedef GeometryPtr               PtrType;
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
     
     CategoryRaw  (OSG::NodePtr root, 
		   OpenSGFaceBase<OpenSGTraits>* node, const FaceIterator& face);
     ~CategoryRaw ();
     
     void    begin   (OpenSGFaceBase<OpenSGTraits>* node, const FaceIterator& face);
     void    end     ();
     void    addData (OpenSGFaceBase<OpenSGTraits>* node, const FaceIterator& face);

     std::ostream&   dump (std::ostream& os) const;

     inline bool     isThisCategory (const FaceIterator& face) const {
	bool faceColor = hasColorAttributes(face);
	bool faceTex   = hasTexAttributes(face);
	return (face.getGeometry()->getMaterial() == m_material) && 
	  //(face.getLength() == m_faceLength) &&
	  (   ( m_hasColor && faceColor || !m_hasColor && !faceColor)
	      && ( m_hasTex   && faceTex   || !m_hasTex   && !faceTex));
     }
     static inline bool    hasColorAttributes (const FaceIterator& face) {
	return face.getColorIndex(0)>=0;
     }
     static inline bool    hasTexAttributes   (const FaceIterator& face) {
	return face.getTexCoordsIndex(0)>=0;
     }
     static inline bool    hasNormalAttributes(const FaceIterator& face) {
        return face.getNormalIndex(0)>=0;
     }
     static inline bool    isRawCategory (const FaceIterator& face) {
	return !hasColorAttributes(face) && !hasTexAttributes(face);
     }
     static inline bool    isColorCategory (const FaceIterator& face) {
        return !hasTexAttributes(face) && hasColorAttributes(face);
     }
     static inline bool    isTexCategory (const FaceIterator& face) {
	return hasTexAttributes(face) && !hasColorAttributes(face);
     }
     static inline bool    isColorTexCategory (const FaceIterator& face) {
	return hasTexAttributes(face) && hasColorAttributes(face);
     }
     
   protected:
     struct quad {
       u32 position;
       u32 normal;
       u32 color;
       u32 tex1;
     };
     typedef GeometryPtrHasher                        HashFunctor;
     typedef std::map<GeometryPtr, quad, HashFunctor> HashMap;
     typedef typename HashMap::iterator               HashMapIterator;
     typedef std::pair<HashMapIterator,bool>          HashMapPair;
     u32                     m_quadOffset;
     bool                    m_hasNormal;
     bool                    m_hasColor;
     bool                    m_hasTex;
     HashMap                 m_offset;
     HashMapIterator         m_offsetIt;
     quad                    m_indexOffset;
     u32                     m_indexStride;
     OSGObjectBase           m_original;
     // OSG mesh data
     OSG::NodePtr            m_node;
     OSG::GeometryPtr        m_geom;
     OSG::MaterialPtr        m_material;
     OSG::GeoPositions3fPtr  m_coord; 
     OSG::GeoNormals3fPtr    m_normal;
     OSG::GeoIndicesUI32Ptr  m_index;
     OSG::GeoPLengthsUI32Ptr m_len;
     OSG::GeoPTypesUI8Ptr    m_type;
     OSG::GeoColors3fPtr     m_color; 
     OSG::GeoTexCoords2fPtr  m_texCoord;  
   };
   class OSG_GENVISLIB_DLLMAPPING CategoryColor : public CategoryRaw
   {
   public:
     CategoryColor (OSG::NodePtr root,
		    OpenSGFaceBase<OpenSGTraits>* node, const FaceIterator& face);
     ~CategoryColor ();
     
     void    begin   (OpenSGFaceBase<OpenSGTraits>* node, const FaceIterator& face);
     void    end     ();
     void    addData (OpenSGFaceBase<OpenSGTraits>* node, const FaceIterator& face);
   };
   class OSG_GENVISLIB_DLLMAPPING CategoryGeneral : public CategoryColor
   {
   public:
     CategoryGeneral  (OSG::NodePtr root,
		       OpenSGFaceBase<OpenSGTraits>* node, const FaceIterator& face);
     ~CategoryGeneral ();
     
     void    begin   (OpenSGFaceBase<OpenSGTraits>* node, const FaceIterator& face);
     void    end     ();
     void    addData (OpenSGFaceBase<OpenSGTraits>* node, const FaceIterator& face);
   };
   class OSG_GENVISLIB_DLLMAPPING Category
   {
   public:
     Category  (OSG::NodePtr root);
     ~Category ();

     OSG::NodePtr getRoot () const;
     void         addData (OpenSGFaceBase<OpenSGTraits>* arg);
     
   private:
     typedef std::vector<CategoryGeneral*>        CategoryList; 
     typedef std::map<OSG::NodePtr, OSG::NodePtr> TransparentList;
#if 0
     class MaterialPtrCmp
     {
     public:
       bool operator() (const OSG::MaterialPtr& a, const OSG::MaterialPtr& b) const {
         return int(a.getCPtr()) < int(b.getCPtr());
       }
     };
     typedef std::map<OSG::MaterialPtr, CategoryList, MaterialPtrCmp> SearchStructure;
#endif
     unsigned        m_numGeom;
     unsigned        m_totalFaces;
     CategoryList    m_all;
     TransparentList m_trans;
     OSG::NodePtr    m_root;
   };

   static OSG::GroupPtr s_group;
   OSG::NodePtr m_root;
   OSG::NodePtr m_rootLevel;
   u32          m_depth;
   u32          m_level;
   bool         m_flat;
   Category*    m_ifs;  
   std::vector<Category*> m_ifsStack;  
};

template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING FaceSpatializeIndexedTraits
{
public:
   typedef typename FaceSpatializeIndexed<BasicTraits>::BVol          BVol;
   typedef typename FaceSpatializeIndexed<BasicTraits>::ResultT       ResultT;
   typedef FaceSpatializeIndexed<BasicTraits>                         ObjectT;
   typedef typename FaceSpatializeIndexed<BasicTraits>::GeneralType   GeneralType;
   typedef typename FaceSpatializeIndexed<BasicTraits>::AdapterType   AdapterType;
   typedef typename FaceSpatializeIndexed<BasicTraits>::GroupType     GroupType;
   typedef typename FaceSpatializeIndexed<BasicTraits>::TransformType TransformType; 

   typedef InitSingleFunctor2<bool,GeneralType,TransformType,ObjectT> InitFunctorT;
   typedef SingleFunctor<ResultT,GroupType,ObjectT>   InnerFunctorT;
   typedef SingleFunctor<ResultT,AdapterType,ObjectT> LeafFunctorT;

   static  InitFunctorT  createInitFunctor      (ObjectT* obj);
   static  InitFunctorT  createInitLeaveFunctor (ObjectT* obj);
   static  InnerFunctorT createInnerFunctor     (ObjectT* obj);
   static  InnerFunctorT createInnerLeaveFunctor(ObjectT* obj);
   static  LeafFunctorT  createLeafFunctor      (ObjectT* obj);
};

END_GENVIS_NAMESPACE
#endif

