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

#ifndef OSGGVFACESPATIALIZE_H
#define OSGGVFACESPATIALIZE_H

#include "OSGGVBase.h"
#include "OSGGVInput.h"
#include "OSGGVSingleTraverser.h"
#include "OSGGVGroup.h"

#include "OSGGroup.h"
#include "OSGGeometry.h"

BEGIN_GENVIS_NAMESPACE

class OSG_GENVISLIB_DLLMAPPING CategoryRaw
{
public:
   CategoryRaw  (OSG::NodePtr root,
		 const FaceIterator& face);
   ~CategoryRaw ();

   inline void    setCCWDirection (bool flag=true);

   void    begin   (const FaceIterator& face);
   void    end     ();
   void    addData (OpenSGFaceBase<OpenSGTraits>* node,
		    const FaceIterator& face);

   inline bool    isThisCategory (const FaceIterator& face) const;

   std::ostream&   print (std::ostream& os) const;

   static inline bool    hasColorAttributes (const FaceIterator& face);
   static inline bool    hasTexAttributes   (const FaceIterator& face);
   static inline bool    hasNormalAttributes(const FaceIterator& face);

   static inline bool    isRawCategory (const FaceIterator& face);
   static inline bool    isColorCategory (const FaceIterator& face);
   static inline bool    isTexCategory (const FaceIterator& face);
   static inline bool    isColorTexCategory (const FaceIterator& face);

protected:
   bool                    m_ccw;
   unsigned                m_faceLength;
   bool                    m_hasNormal;
   bool                    m_hasColor;
   bool                    m_hasTex;

   // OSG mesh data
   OSG::NodePtr            m_node;
   OSG::MaterialPtr        m_material;
   OSG::GeoPositions3fPtr  m_coord; 
   OSG::GeoNormals3fPtr    m_normal;
   OSG::GeoIndicesUI32Ptr  m_index;
   OSG::GeoPLengthsUI32Ptr m_len;
   OSG::GeoPTypesUI8Ptr	   m_type;
   OSG::GeoColors3fPtr     m_color; 
   OSG::GeoTexCoords2fPtr  m_texCoord;  
};

inline bool CategoryRaw::hasColorAttributes (const FaceIterator& face)
{
   return face.getColorIndex(0)>=0;
}
inline bool CategoryRaw::hasTexAttributes (const FaceIterator& face)
{
   return face.getTexCoordsIndex(0)>=0;
}
inline bool CategoryRaw::hasNormalAttributes (const FaceIterator& face)
{
   return face.getNormalIndex(0)>=0;
}
inline bool CategoryRaw::isRawCategory (const FaceIterator& face)
{
   return !hasColorAttributes(face) && !hasTexAttributes(face);
}
inline bool CategoryRaw::isColorCategory (const FaceIterator& face)
{
   return !hasTexAttributes(face) && hasColorAttributes(face);
}
inline bool CategoryRaw::isTexCategory (const FaceIterator& face)
{
   return hasTexAttributes(face) && !hasColorAttributes(face);
}
inline bool CategoryRaw::isColorTexCategory (const FaceIterator& face)
{
   return hasTexAttributes(face) && hasColorAttributes(face);
}
inline bool CategoryRaw::isThisCategory (const FaceIterator& face) const
{
   bool faceColor = hasColorAttributes(face);
   bool faceTex   = hasTexAttributes(face);
   return (face.getGeometry()->getMaterial() == m_material) && 
     (face.getLength() == m_faceLength) &&
     (   ( m_hasColor && faceColor || !m_hasColor && !faceColor)
      && ( m_hasTex   && faceTex   || !m_hasTex   && !faceTex));
}
inline void CategoryRaw::setCCWDirection (bool ccw)
{
   m_ccw = ccw;
}


class OSG_GENVISLIB_DLLMAPPING CategoryColor : public CategoryRaw
{
public:
   CategoryColor (OSG::NodePtr root,
		  const FaceIterator& face);
   ~CategoryColor ();

   void    begin   (const FaceIterator& face);
   void    end     ();
   void    addData (OpenSGFaceBase<OpenSGTraits>* node,
		    const FaceIterator& face);
};
class OSG_GENVISLIB_DLLMAPPING CategoryGeneral : public CategoryColor
{
public:
   CategoryGeneral  (OSG::NodePtr root,
		     const FaceIterator& face);
   ~CategoryGeneral ();

   void    begin   (const FaceIterator& face);
   void    end     ();
   void    addData (OpenSGFaceBase<OpenSGTraits>* node,
		    const FaceIterator& face);
};

class OSG_GENVISLIB_DLLMAPPING Category
{
public:
   Category  (OSG::NodePtr root);
   ~Category ();

   OSG::NodePtr getRoot () const;
   void         addData (OpenSGFaceBase<OpenSGTraits>* arg);

private:
   typedef std::vector<CategoryGeneral*> CategoryList; 
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
   OSG::NodePtr    m_root;
};

template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING FaceSpatialize
{
public:
   typedef BoundingVolume<Real>                         BVol;
   typedef SingleTraverserBase<BasicTraits>::ResultType ResultT;
   typedef OpenSGFaceBase<BasicTraits>                  AdapterType;
   typedef BVolGroupBase                                GroupType;
   typedef BVolAdapterBase                              GeneralType; 
   typedef typename BasicTraits::TransformType          TransformType; 

   FaceSpatialize ();
   ~FaceSpatialize ();

   OSG::NodePtr getRoot () const;
   void         setLevel (unsigned level);
   unsigned     getLevel () const;
   void         setFlat (bool flag);
   bool         getFlat () const;

   bool     InitEnter  (GeneralType* a, const TransformType& m);
   bool     InitLeave  (GeneralType* a, const TransformType& m);
   ResultT  InnerEnter (GroupType*   g0);
   ResultT  InnerLeave (GroupType*   g0);
   ResultT  LeafEnter  (AdapterType* p0);

   static OSG::GroupPtr getGroupCore ();
   static void          setGroupCore (const OSG::GroupPtr& group);

private:
   static OSG::GroupPtr s_group;
   OSG::NodePtr m_root;
   OSG::NodePtr m_rootLevel;
   unsigned     m_level;
   unsigned     m_current;
   bool         m_flat;
   Category*    m_ifs;  
   std::vector<Category*> m_ifsStack;  
};

template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING FaceSpatializeTraits
{
public:
   typedef typename FaceSpatialize<BasicTraits>::BVol          BVol;
   typedef typename FaceSpatialize<BasicTraits>::ResultT       ResultT;
   typedef FaceSpatialize<BasicTraits>                         ObjectT;
   typedef typename FaceSpatialize<BasicTraits>::GeneralType   GeneralType;
   typedef typename FaceSpatialize<BasicTraits>::AdapterType   AdapterType;
   typedef typename FaceSpatialize<BasicTraits>::GroupType     GroupType;
   typedef typename FaceSpatialize<BasicTraits>::TransformType TransformType; 

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

