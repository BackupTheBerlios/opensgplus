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
//   $Revision: 1.1 $
//   $Date: 2003/09/11 16:20:29 $
//                                                                            
//=============================================================================

#ifndef OSGGVBVOLHIERARCHY_H
#define OSGGVBVOLHIERARCHY_H

#include "OSGGVBase.h"
#include "OSGGVStaticInput.h"
#include "OSGGVTraits.h"
#include "OSGGVInput.h"
#include "OSGGVAdapterFactory.h"

BEGIN_GENVIS_NAMESPACE

class Oracle;
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
   typedef StaticInput<BasicTraits> Inherited;
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
   virtual void         setParameter (Oracle*  method,
				      unsigned maxLevel   = 50,
				      unsigned minObjects = 256);
   virtual void         setParameter (SplitAlgorithm method,
				      unsigned maxLevel   = 50,
				      unsigned minObjects = 256);
   virtual BVolGroupBase*  getRoot   () const = 0;
   virtual inline void     addAdapter (const TransformType&  m2w,
				       const GeomObjectType& obj);
   virtual unsigned        hierarchy () = 0;
   virtual inline unsigned hierarchyNode (const NodePtr& node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};
typedef  BVolHierarchyBase<OpenSGTraits> OSGBVolHierarchy;

template<class BasicTraits> 
inline void BVolHierarchyBase<BasicTraits>::addAdapter (const TransformType&,
							const GeomObjectType&)
{
}
template<class BasicTraits> 
inline unsigned BVolHierarchyBase<BasicTraits>::hierarchyNode (const NodePtr&) 
{ 
   return 0; 
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
   void         setParameter (Oracle*  method,
			      unsigned maxLevel   = 50,
			      unsigned minObjects = 256);
   void         setParameter (SplitAlgorithm method,
			      unsigned maxLevel   = 50,
			      unsigned minObjects = 256);
   virtual GroupType* getRoot   () const;
   virtual void       addAdapter(const TransformType&  m2w,
				 const GeomObjectType& obj);
   virtual unsigned   hierarchy ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Cache.                                                       */
   /*! \{                                                                 */
   virtual void registerFunctors ();
   static void staticEnterGeometry (osg::CNodePtr&, OSGStaticInput*);
#ifndef OSG_NOFUNCTORS
   void functorEnterGeometry       (osg::CNodePtr&, OSGStaticInput*);
#endif
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   static Oracle*     createOracle (SplitAlgorithm method);

   Oracle*                       m_oracle;
   bool                          m_deleteOracle;
   unsigned                      m_maxLevel;
   unsigned                      m_minObjects;
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
   void               setParameter (Oracle*  method,
				    unsigned maxLevel   = 50,
				    unsigned minObjects = 256);
   void               setParameter (SplitAlgorithm method,
				    unsigned maxLevel   = 50,
				    unsigned minObjects = 256);
   virtual void       addAdapter (const TransformType&  m2w,
				  const GeomObjectType& obj);
   virtual GroupType* getRoot   () const;
   virtual unsigned   hierarchy ();
   virtual unsigned   hierarchyNode (const NodePtr& node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Cache.                                                       */
   /*! \{                                                                 */
   virtual void registerFunctors ();
   static void staticEnterGeometry (osg::CNodePtr&, OSGStaticInput*);
#ifndef OSG_NOFUNCTORS
   void functorEnterGeometry       (osg::CNodePtr&, OSGStaticInput*);
#endif
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   static Oracle*     createOracle (SplitAlgorithm method);

   Oracle*                       m_oracle;
   bool                          m_deleteOracle;
   unsigned                      m_maxLevel;
   unsigned                      m_minObjects;
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
   void               setParameter (Oracle*  method,
				    unsigned maxLevel   = 50,
				    unsigned minObjects = 256);
   void               setParameter (SplitAlgorithm method,
				    unsigned maxLevel   = 50,
				    unsigned minObjects = 256);
   virtual void       addAdapter (const TransformType&  m2w,
				  const GeomObjectType& obj);
   virtual GroupType* getRoot   () const;
   virtual unsigned   hierarchy ();
   virtual unsigned   hierarchyNode (const NodePtr& node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Cache.                                                       */
   /*! \{                                                                 */
   virtual void registerFunctors ();
   static void staticEnterGeometry (osg::CNodePtr&, OSGStaticInput*);
#ifndef OSG_NOFUNCTORS
   void functorEnterGeometry       (osg::CNodePtr&, OSGStaticInput*);
#endif
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   static Oracle*     createOracle (SplitAlgorithm method);

   Oracle*                       m_oracle;
   bool                          m_deleteOracle;
   unsigned                      m_maxLevel;
   unsigned                      m_minObjects;
   GroupType*                    m_root;
   std::vector<GroupType*>       m_inner;
   std::vector<AdapterType*>     m_leaf;
};
typedef  PrimitiveBVolHierarchy<OpenSGTraits, OpenSGPrimitiveInput<K6Dop> > OSGPrimitiveHierarchy;

END_GENVIS_NAMESPACE
#endif

