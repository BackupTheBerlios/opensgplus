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
//   $Date: 2004/03/12 13:16:55 $
//                                                                            
//=============================================================================

#ifndef OSGGVBVOLHIERARCHY_H
#define OSGGVBVOLHIERARCHY_H

#include "OSGGVBase.h"
#include "OSGGVStaticInput.h"
#include "OSGGVTraits.h"
#include "OSGGVInput.h"
#include "OSGGVAdapterFactory.h"
#include "OSGGeoFunctions.h"

BEGIN_GENVIS_NAMESPACE

template <class BVOL> class Oracle;
template <class BVOL> class BVolGroup;

/*! \brief Base for bounding volume hierarchies in scenegraph parts.
 */
template<class BasicTraits> 
class OSG_GENVISLIB_DLLMAPPING BVolHierarchyBase 
: public StaticInput<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef StaticInput<BasicTraits>           Inherited;
   typedef typename Inherited::TransformType  TransformType;
   typedef typename Inherited::GeomObjectType GeomObjectType;
   typedef typename Inherited::GeomFaceType   GeomFaceType;
   typedef typename Inherited::Cache          Cache;
   typedef typename Inherited::CacheData      CacheData;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   BVolHierarchyBase ();
   virtual ~BVolHierarchyBase ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Hierarchy creation.                                          */
   /*! \{                                                                 */   
   virtual void         setParameter (u32 method,
				      u32 maxLevel   = 50,
				      u32 minObjects = 256) = 0;
   virtual void         setParameter (const char* description,
				      u32 maxLevel   = 50,
				      u32 minObjects = 256) = 0;
   virtual inline u32      getDepth () const;
   virtual BVolGroupBase*  getRoot  () const = 0;
   virtual void            process   (const GeomObjectType& node) = 0;
   virtual void            hierarchy (const GeomObjectType& node=NullFC) = 0;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   u32            m_depth;
   GeomObjectType m_last;
};
typedef  BVolHierarchyBase<OpenSGTraits> OSGBVolHierarchy;

template<class BasicTraits> 
inline u32 BVolHierarchyBase<BasicTraits>::getDepth () const
{
   return m_depth;
}


/*! \brief Bounding volume hierarchies on scenegraph objects.
 */
template<class BasicTraits, class InputTraits> 
class OSG_GENVISLIB_DLLMAPPING ObjectBVolHierarchy 
: public BVolHierarchyBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef BVolHierarchyBase<BasicTraits>           Inherited;
   typedef typename Inherited::TransformType        TransformType;
   typedef typename Inherited::GeomObjectType       GeomObjectType;
   typedef typename Inherited::GeomFaceType         GeomFaceType;
   typedef typename Inherited::Cache                Cache;
   typedef typename Inherited::CacheData            CacheData;

   typedef typename InputTraits::BVol               BVol;
   typedef typename InputTraits::AdapterType        AdapterType;
   typedef typename InputTraits::GroupType          GroupType;
   typedef typename GroupType::FactoryType          InnerFactory;
   typedef typename AdapterType::FactoryType        LeafFactory;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   ObjectBVolHierarchy ();
   virtual ~ObjectBVolHierarchy ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Interface StaticInput.                                       */
   /*! \{                                                                 */
   virtual void destroy ();
   virtual void clear   ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Interface BVolHierarchyBase.                                 */
   /*! \{                                                                 */
   virtual void         setParameter (u32 method,
				      u32 maxLevel   = 50,
				      u32 minObjects = 256);
   virtual void         setParameter (const char* description,
				      u32 maxLevel   = 50,
				      u32 minObjects = 256);
   virtual GroupType* getRoot   () const;
   virtual void       addAdapter(const TransformType&  m2w,
				 const GeomObjectType& obj);
   virtual void       process   (const GeomObjectType& node);
   virtual void       hierarchy (const GeomObjectType& node=NullFC);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Cache.                                                       */
   /*! \{                                                                 */
   virtual void registerFunctors ();
   static bool staticEnterGeometry (osg::CNodePtr& cnode, OSGStaticInput* input);
#ifndef OSG_NOFUNCTORS
   bool functorEnterGeometry       (osg::CNodePtr& cnode, OSGStaticInput* input);
#endif
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   Oracle<BVol>*                 m_oracle;
   u32                      m_maxLevel;
   u32                      m_minObjects;
   GroupType*                    m_root;
   std::vector<GroupType*>       m_inner;
   std::vector<AdapterType*>     m_leaf;
};
typedef  ObjectBVolHierarchy<OpenSGTraits,OpenSGObjectInput<K6Dop> > OSGObjectHierarchy;


/*! \brief Bounding volume hierarchies on scenegraph faces 
    (triangles, quadrangles, polygons).
 */
template<class BasicTraits, class InputTraits> 
class OSG_GENVISLIB_DLLMAPPING FaceBVolHierarchy 
: public BVolHierarchyBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef BVolHierarchyBase<BasicTraits>           Inherited;
   typedef typename Inherited::TransformType        TransformType;
   typedef typename Inherited::GeomObjectType       GeomObjectType;
   typedef typename Inherited::GeomFaceType         GeomFaceType;
   typedef typename Inherited::Cache                Cache;
   typedef typename Inherited::CacheData            CacheData;

   typedef typename InputTraits::BVol               BVol;
   typedef typename InputTraits::AdapterType        AdapterType;
   typedef typename InputTraits::GroupType          GroupType;
   typedef typename GroupType::FactoryType          InnerFactory;
   typedef typename AdapterType::FactoryType        LeafFactory;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   FaceBVolHierarchy ();
   virtual ~FaceBVolHierarchy ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Interface StaticInput.                                       */
   /*! \{                                                                 */
   virtual void destroy ();
   virtual void clear   ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Interface BVolHierarchyBase.                                 */
   /*! \{                                                                 */
   virtual void         setParameter (u32 method,
				      u32 maxLevel   = 50,
				      u32 minObjects = 256);
   virtual void         setParameter (const char* description,
				      u32 maxLevel   = 50,
				      u32 minObjects = 256);
   virtual void       addAdapter (const TransformType&  m2w,
				  const GeomObjectType& obj);
   virtual GroupType* getRoot   () const;
   virtual void       process   (const GeomObjectType& node);
   virtual void       hierarchy (const GeomObjectType& node=NullFC);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Cache.                                                       */
   /*! \{                                                                 */
   virtual void registerFunctors ();
   static bool staticEnterGeometry (osg::CNodePtr& cnode, OSGStaticInput* input);
#ifndef OSG_NOFUNCTORS
   bool functorEnterGeometry       (osg::CNodePtr& cnode, OSGStaticInput* input);
#endif
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   Oracle<BVol>*                 m_oracle;
   u32                      m_maxLevel;
   u32                      m_minObjects;
   GroupType*                    m_root;
   std::vector<GroupType*>       m_inner;
   std::vector<AdapterType*>     m_leaf;
};
typedef  FaceBVolHierarchy<OpenSGTraits, OpenSGFaceInput<K6Dop> > OSGFaceHierarchy;


/*! \brief Bounding volume hierarchies on scenegraph primitives
    (triangle fans, triangle strips, quadrangle strips).
 */
template<class BasicTraits, class InputTraits> 
class OSG_GENVISLIB_DLLMAPPING PrimitiveBVolHierarchy 
: public BVolHierarchyBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef BVolHierarchyBase<BasicTraits>           Inherited;
   typedef typename Inherited::TransformType        TransformType;
   typedef typename Inherited::GeomObjectType       GeomObjectType;
   typedef typename Inherited::GeomFaceType         GeomFaceType;
   typedef typename Inherited::Cache                Cache;
   typedef typename Inherited::CacheData            CacheData;

   typedef typename InputTraits::BVol               BVol;
   typedef typename InputTraits::AdapterType        AdapterType;
   typedef typename InputTraits::GroupType          GroupType;
   typedef typename GroupType::FactoryType          InnerFactory;
   typedef typename AdapterType::FactoryType        LeafFactory;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   PrimitiveBVolHierarchy ();
   virtual ~PrimitiveBVolHierarchy ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Interface StaticInput.                                       */
   /*! \{                                                                 */
   virtual void destroy ();
   virtual void clear   ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Interface BVolHierarchyBase.                                 */
   /*! \{                                                                 */
   virtual void         setParameter (u32 method,
				      u32 maxLevel   = 50,
				      u32 minObjects = 256);
   virtual void         setParameter (const char* description,
				      u32 maxLevel   = 50,
				      u32 minObjects = 256);
   virtual void       addAdapter (const TransformType&  m2w,
				  const GeomObjectType& obj);
   virtual GroupType* getRoot   () const;
   virtual void       process   (const GeomObjectType& node);
   virtual void       hierarchy (const GeomObjectType& node = NullFC);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Cache.                                                       */
   /*! \{                                                                 */
   virtual void registerFunctors ();
   static bool staticEnterGeometry (osg::CNodePtr& cnode, OSGStaticInput* input);
#ifndef OSG_NOFUNCTORS
   bool functorEnterGeometry       (osg::CNodePtr& cnode, OSGStaticInput* input);
#endif
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   Oracle<BVol>*                 m_oracle;
   u32                      m_maxLevel;
   u32                      m_minObjects;
   GroupType*                    m_root;
   std::vector<GroupType*>       m_inner;
   std::vector<AdapterType*>     m_leaf;
};
typedef  PrimitiveBVolHierarchy<OpenSGTraits, OpenSGPrimitiveInput<K6Dop> > OSGPrimitiveHierarchy;

END_GENVIS_NAMESPACE
#endif

