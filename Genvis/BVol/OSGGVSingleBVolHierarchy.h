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

#ifndef OSGGVSINGLEBVOLHIERARCHY_H
#define OSGGVSINGLEBVOLHIERARCHY_H

#include "OSGGVBase.h"
#include "OSGGVTraits.h"
#include "OSGGVInput.h"
#include "OSGGVStaticInput.h"
#include "OSGGVAdapterFactory.h"

BEGIN_GENVIS_NAMESPACE

template <class BVOL> class Oracle;
template <class BVOL> class BVolGroup;

/*! \brief Base for bounding volume hierarchies in scenegraph parts.
 */
template<class BasicTraits> 
class OSG_GENVISLIB_DLLMAPPING SingleBVolHierarchyBase : public StaticInput<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef StaticInput<BasicTraits>                 Inherited;
   typedef typename BasicTraits::GeomCoreType       GeomCoreType;
   typedef typename Inherited::TransformType        TransformType;
   typedef typename Inherited::GeomObjectType       GeomObjectType;
   typedef typename Inherited::GeomFaceType         GeomFaceType;
   typedef typename Inherited::Cache                Cache;
   typedef typename Inherited::CacheData            CacheData;
   //typedef std::map<GeomCoreType,CacheData*,FCPtrLess<GeomCoreType> > SharesContainer;
   /*! Mode for hierarchy creation.
       \begin{description}
       \item[Local]
       \item[LocalShared]
       \item[Global]
       \end{description}                                       */
   enum ModeType {
      Local, LocalShared, Global
   };
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   SingleBVolHierarchyBase ();
   virtual ~SingleBVolHierarchyBase ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name CoordinateSystem.                                            */
   /*! \{                                                                 */   
   inline void     setCoordinateSystem (ModeType mode);
   inline ModeType getCoordinateSystem () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Hierarchy update.                                            */
   /*! \{                                                                 */   
   virtual inline void updateHierarchy ();
   virtual inline void updateHierarchy (NodePtr node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Hierarchy creation.                                          */
   /*! \{                                                                 */  
   virtual inline void         setParameter (u32 method,
				      u32 maxLevel   = 50,
				      u32 minObjects = 256);
   virtual inline void         setParameter (const char* description,
				      u32 maxLevel   = 50,
				      u32 minObjects = 256);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   ModeType m_mode;
};
typedef  SingleBVolHierarchyBase<OpenSGTraits> OSGSingleBVolHierarchyBase;

template<class BasicTraits> 
inline void         
SingleBVolHierarchyBase<BasicTraits>::setCoordinateSystem (ModeType mode)
{
   m_mode = mode;
}
template<class BasicTraits> 
inline typename SingleBVolHierarchyBase<BasicTraits>::ModeType     
SingleBVolHierarchyBase<BasicTraits>::getCoordinateSystem () const
{
   return m_mode;
}

template<class BasicTraits> 
inline void 
SingleBVolHierarchyBase<BasicTraits>::updateHierarchy ()
{
}
template<class BasicTraits> 
inline void 
SingleBVolHierarchyBase<BasicTraits>::updateHierarchy (NodePtr node)
{
}
template<class BasicTraits> 
inline void 
SingleBVolHierarchyBase<BasicTraits>::setParameter    (u32, u32, u32)
{
}
template<class BasicTraits> 
inline void 
SingleBVolHierarchyBase<BasicTraits>::setParameter    (const char*, u32, u32)
{
}


template<class BasicTraits, class InputTraits> 
class OSG_GENVISLIB_DLLMAPPING SingleBVolHierarchy 
: public SingleBVolHierarchyBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef SingleBVolHierarchyBase<BasicTraits>     Inherited;
   typedef typename Inherited::TransformType        TransformType;
   typedef typename Inherited::GeomObjectType       GeomObjectType;
   typedef typename Inherited::GeomFaceType         GeomFaceType;
   typedef typename Inherited::Cache                Cache;
   typedef typename Inherited::CacheData            CacheData;

   typedef typename BasicTraits::GeomTriangleType       GeomTriangleType;
   typedef typename InputTraits::BVol                   BVol;
   typedef typename InputTraits::AdapterType            AdapterType;
   typedef typename InputTraits::GroupType              GroupType;
   typedef typename GroupType::FactoryType              InnerFactory;
   typedef typename AdapterType::FactoryType            LeafFactory;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   SingleBVolHierarchy ();
   virtual ~SingleBVolHierarchy ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Memory management.                                           */
   /*! \{                                                                 */   
   virtual void destroy ();
   virtual void clear   ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Adapter creation.                                            */
   /*! \{                                                                 */   
   void         addAdapter   (const GeomObjectType& obj);
   virtual void addAdapter   (const TransformType&  m2w,
			      const GeomObjectType& obj);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Hierarchy creation.                                          */
   /*! \{                                                                 */   
   virtual void         setParameter (u32 method,
				      u32 maxLevel   = 50,
				      u32 minObjects = 256);
   virtual void         setParameter (const char* description,
				      u32 maxLevel   = 50,
				      u32 minObjects = 256);
   virtual void process      (const GeomObjectType& node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Hierarchy update.                                            */
   /*! \{                                                                 */   
   virtual void updateHierarchy ();
   virtual void updateHierarchy (NodePtr node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

public:
   virtual void registerFunctors ();
   static bool staticEnterGeometry (CNodePtr& cnode, OSGStaticInput* input);
#ifndef OSG_NOFUNCTORS
   bool functorEnterGeometry       (CNodePtr& cnode, OSGStaticInput* input);
#endif

protected:
   void                 hierarchyLocal  (const NodePtr& node);
   void                 hierarchyGlobal (const NodePtr& node);

   Oracle<BVol>*                 m_oracle;
   u32                      m_maxLevel;
   u32                      m_minObjects;
   std::vector<GroupType*>       m_inner;
   std::vector<AdapterType*>     m_leaf;
};
typedef  SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleInput<K6Dop> > OSGSingleBVolHierarchy;

template<class BVOL> 
class OSG_GENVISLIB_DLLMAPPING SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >
: public SingleBVolHierarchyBase<OpenSGTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef OpenSGTraits                               BasicTraits;
   typedef OpenSGTriangleAlignedInput<BVOL>           InputTraits;
   typedef SingleBVolHierarchyBase<BasicTraits>       Inherited;
   typedef typename Inherited::TransformType          TransformType;
   typedef typename Inherited::GeomObjectType         GeomObjectType;
   typedef typename Inherited::GeomFaceType           GeomFaceType;
   typedef typename Inherited::Cache                  Cache;
   typedef typename Inherited::CacheData              CacheData;

   typedef typename BasicTraits::GeomTriangleType       GeomTriangleType;
   typedef typename InputTraits::BVol                   BVol;
   typedef typename InputTraits::AdapterType            AdapterType;
   typedef typename InputTraits::GroupType              GroupType;
   typedef typename GroupType::FactoryType              InnerFactory;
   typedef typename AdapterType::FactoryType            LeafFactory;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   SingleBVolHierarchy ();
   virtual ~SingleBVolHierarchy ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Clear.                                                       */
   /*! \{                                                                 */   
   virtual void destroy ();
   virtual void clear   ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Adapter creation.                                            */
   /*! \{                                                                 */   
   void         addAdapter   (const GeomObjectType& obj);
   virtual void addAdapter   (const TransformType&  m2w,
			      const GeomObjectType& obj);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Hierarchy creation.                                          */
   /*! \{                                                                 */   
   virtual void         setParameter (u32 method,
				      u32 maxLevel   = 50,
				      u32 minObjects = 256);
   virtual void         setParameter (const char* description,
				      u32 maxLevel   = 50,
				      u32 minObjects = 256);
   virtual void process      (const GeomObjectType& node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Hierarchy update.                                            */
   /*! \{                                                                 */   
   virtual void updateHierarchy ();
   virtual void updateHierarchy (NodePtr node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

public:
   virtual void registerFunctors ();
   static bool staticEnterGeometry (CNodePtr& cnode, OSGStaticInput* input);
#ifndef OSG_NOFUNCTORS
   bool functorEnterGeometry       (CNodePtr& cnode, OSGStaticInput* input);
#endif

protected:
   void                 hierarchyLocal  (const NodePtr& node);
   void                 hierarchyGlobal (const NodePtr& node);

   Oracle<BVol>*                 m_oracle;
   u32                      m_maxLevel;
   u32                      m_minObjects;
   std::vector<GroupType*>       m_inner;
   std::vector<AdapterType*>     m_leaf;
};

END_GENVIS_NAMESPACE
#endif



