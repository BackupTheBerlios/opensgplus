//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $\Revision$
//   $\Date$
//                                                                            
//=============================================================================

#include "OSGGVFaceSpatialize.h"
#include "OSGGVBVolAdapter.h"

#include "OSGMaterial.h"
#include "OSGMaterialGroup.h"
#include "OSGGeoFunctions.h"
OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING FaceSpatialize<OpenSGTraits>;
template class OSG_GENVISLIB_DLLMAPPING FaceSpatializeTraits<OpenSGTraits>;

// internal classes 
template <class BasicTraits>
void FaceSpatialize<BasicTraits>::CategoryRaw::begin (const FaceIterator& face)
{
   m_coord  = GeoPositions3f::create();
   m_index  = GeoIndicesUI32::create();	
   m_len    = GeoPLengthsUI32::create();	
   m_type   = GeoPTypesUI8::create();	
   // store material of this geometry
   m_material = face.getGeometry()->getMaterial();
   // create geometry node
   m_node = Node::create();
   m_geom = Geometry::create();
   beginEditCP(m_node);
   m_node->setCore(m_geom);
   endEditCP(m_node);
   // assign geometry fields
   beginEditCP(m_geom);
   m_geom->setMaterial(m_material);
   m_geom->setPositions(m_coord);
   {
   //if (m_hasNormal) {
      m_normal = GeoNormals3f::create();
      m_geom->setNormals(m_normal);
   }
   m_geom->setIndices(m_index);
   m_geom->setTypes(m_type);
   m_geom->setLengths(m_len);
   endEditCP(m_geom);
   // edit geometry field contents
   beginEditCP(m_coord);
   beginEditCP(m_normal);
   beginEditCP(m_index);
   beginEditCP(m_type);
   beginEditCP(m_len);
   // store face type of this geometry
   //m_faceLength = face.getLength();
   GeoPTypesUI8::StoredFieldType* t = m_type->getFieldPtr();
   //t->addValue((m_faceLength==3 ? GL_TRIANGLES : GL_QUADS));
   t->addValue(GL_TRIANGLES); t->addValue(GL_QUADS);
   m_quadOffset = 0;
}
template <class BasicTraits>
void FaceSpatialize<BasicTraits>::CategoryRaw::end ()
{
   // store length of index sequence #PRIMS * (3 resp. 4)
   GeoPLengthsUI32::StoredFieldType* l = m_len->getFieldPtr();
   //l->addValue(m_index->getSize());   
   l->addValue(m_quadOffset); l->addValue(m_index->getSize()-m_quadOffset);
   // end edit geometry field contents
   endEditCP(m_len);
   endEditCP(m_type);
   endEditCP(m_index);
   endEditCP(m_normal);
   endEditCP(m_coord);
}
template <class BasicTraits>
void FaceSpatialize<BasicTraits>::CategoryRaw::addData (OpenSGFaceBase<OpenSGTraits>* node,
							const FaceIterator&           face)
{
   GeoPositions3f::StoredFieldType*  p = m_coord->getFieldPtr();
   GeoNormals3f::StoredFieldType*    n = m_normal->getFieldPtr();
   //GeoIndicesUI32::StoredFieldType*  i = m_index->getFieldPtr();

   GeoPositionsPtr faceP = node->getPositions();
   addRefCP(faceP);
   u32 k;
   for (k=0; k<faceP->getSize(); ++k) {
      p->addValue(faceP->getValue(k));
      if (face.getLength()==3) { 
	 m_index->insertValue(p->size()-1, m_quadOffset++);
      } else {
	 m_index->addValue(p->size()-1);
      }
   }
   if (!m_hasNormal) {
#if 1
      Vec3f p0(faceP->getValue(0));
      Vec3f p1(faceP->getValue(1));
      Vec3f p2(faceP->getValue(2));
      p2 -= p1; p0 -= p1; 
      if (m_ccw) {
	 p0.crossThis(p2); p0.normalize();
	 n->addValue(p0);
      } else {
	 p2.crossThis(p0); p2.normalize();
	 n->addValue(p2);
      }
#endif
   } else { // per-vertex normals or per-face normals
      GeoNormalsPtr faceN = node->getNormals();
      addRefCP(faceN);
      for (k=0; k<faceN->getSize(); ++k) {
	 n->addValue(faceN->getValue(k));
      }
      subRefCP(faceN);
   }
   subRefCP(faceP);
}
template <class BasicTraits>
FaceSpatialize<BasicTraits>::CategoryRaw::CategoryRaw (NodePtr root,
						       const FaceIterator& face)
  :  m_ccw(true), 
     //m_faceLength(0),
     m_hasNormal(hasNormalAttributes(face)), 
     m_hasColor(hasColorAttributes(face)),
     m_hasTex(hasTexAttributes(face))
{
   CategoryRaw::begin(face);
   beginEditCP(root);
   root->addChild(m_node);
   endEditCP(root);
}
template <class BasicTraits>
FaceSpatialize<BasicTraits>::CategoryRaw::~CategoryRaw ()
{
   CategoryRaw::end();
}

template <class BasicTraits>
std::ostream& FaceSpatialize<BasicTraits>::CategoryRaw::dump (std::ostream& os) const
{
   os << "CategoryRaw(material=" << m_material 
      << ", ";
   if (m_hasNormal) { 
     os << m_normal->getSize() << " normals";
   } else {
     os << " no normals";
   }
   os << ", ";
   if (m_hasColor) { 
     os << m_color->getSize() << " colors";
   } else {
     os << " no colors";
   }
   os << ", ";
   if (m_hasTex) { 
      os << m_texCoord->getSize() << " tex coords";
   } else {
      os << " no tex coords";
   }
   os << ")" << std::flush;
   return os;
}

template <class BasicTraits>
void FaceSpatialize<BasicTraits>::CategoryColor::begin (const FaceIterator&)
{
   if (m_hasColor) {
      m_color = GeoColors3f::create();

      beginEditCP(m_geom);
      m_geom->setColors(m_color);
      endEditCP(m_geom);
      beginEditCP(m_color);
   }
}
template <class BasicTraits>
void FaceSpatialize<BasicTraits>::CategoryColor::end ()
{
   if (m_hasColor) {
      endEditCP(m_color);
   }
}
template <class BasicTraits>
void FaceSpatialize<BasicTraits>::CategoryColor::addData (OpenSGFaceBase<OpenSGTraits>* node,
							  const FaceIterator&           face)
{
   CategoryRaw::addData(node, face);
   if (m_hasColor) {
      GeoColors3f::StoredFieldType* c = m_color->getFieldPtr();
      for (u32 k=0; k<face.getLength(); ++k) {
	 c->addValue(face.getColor(k));
      }
      assert(m_coord->getSize() == m_color->getSize());
   }
}
template <class BasicTraits>
FaceSpatialize<BasicTraits>::CategoryColor::CategoryColor (NodePtr root,
							   const FaceIterator& face)
  : CategoryRaw(root, face)
{
   CategoryColor::begin(face);
}
template <class BasicTraits>
FaceSpatialize<BasicTraits>::CategoryColor::~CategoryColor ()
{
   CategoryColor::end();
}


template <class BasicTraits>
void FaceSpatialize<BasicTraits>::CategoryGeneral::begin (const FaceIterator&)
{
   if (m_hasTex) {
      m_texCoord = GeoTexCoords2f::create();

      beginEditCP(m_geom);
      m_geom->setTexCoords(m_texCoord);
      endEditCP(m_geom);
      beginEditCP(m_texCoord);
   }
}
template <class BasicTraits>
void FaceSpatialize<BasicTraits>::CategoryGeneral::end ()
{
   if (m_hasTex) {
      endEditCP(m_texCoord);
   }
}
template <class BasicTraits>
void FaceSpatialize<BasicTraits>::CategoryGeneral::addData (OpenSGFaceBase<OpenSGTraits>* node,
							    const FaceIterator& face)
{
   CategoryColor::addData(node, face);
   if (m_hasTex) {
      GeoTexCoords2f::StoredFieldType* t = m_texCoord->getFieldPtr();
      for (u32 k=0; k<face.getLength(); ++k) {
	 t->addValue(face.getTexCoords(k));
      }
      assert(m_coord->getSize() == m_texCoord->getSize());
   }
}
template <class BasicTraits>
FaceSpatialize<BasicTraits>::CategoryGeneral::CategoryGeneral (NodePtr root,
							       const FaceIterator& face)
  : CategoryColor(root, face)
{
   CategoryGeneral::begin(face); 
}
template <class BasicTraits>
FaceSpatialize<BasicTraits>::CategoryGeneral::~CategoryGeneral ()
{
   CategoryGeneral::end();
}


template <class BasicTraits>
void FaceSpatialize<BasicTraits>::Category::addData (OpenSGFaceBase<OpenSGTraits>* arg)
{
   FaceIterator face = arg->getOriginal();
   if (
       face.getGeometry()->getMaterial()->isTransparent() // transparent face
       && m_trans.find(arg->getObjectAdapter().getNode()) != m_trans.end() // not processed
       ) { 
      OSGObjectBase convert(arg->getObjectAdapter().getNode());
      convert.setObjectAdapter(&arg->getObjectAdapter());
      NodePtr             newNode = Node::create();
      m_trans[convert.getOriginal()] = newNode;
      GeometryPtr         oldCore = GeometryPtr::dcast(convert.getOriginal()->getCore());
      GeometryPtr         newCore = oldCore->clone();
      beginEditCP(newNode);
      newNode->setCore(newCore);
      endEditCP(newNode);
      beginEditCP(newCore);
      newCore->setPositions(convert.getPositions());
      newCore->setNormals  (convert.getNormals());
      endEditCP(newCore);

      beginEditCP(m_root);
      m_root->addChild(newNode);
      endEditCP(m_root);

      return;
   }
   ++m_totalFaces;

   CategoryList::iterator c;
   for (c = m_all.begin();
	c != m_all.end();
	++c) {
      if ((*c)->isThisCategory(face)) {
         break;
      }
   }
   if (c == m_all.end()) { // new category
#if 0
      MaterialGroupPtr mat = MaterialGroup::create();
      beginEditCP(mat);
      mat->setMaterial(face.getGeometry()->getMaterial());
      endEditCP(mat);
      NodePtr matNode = Node::create();
      beginEditCP(matNode);
      matNode->setCore(mat);
      endEditCP(matNode);
      beginEditCP(m_root);
      m_root->addChild(matNode);
      endEditCP(m_root);
      c = m_all.insert(m_all.end(), new CategoryGeneral(matNode, face));
#else
      c = m_all.insert(m_all.end(), new CategoryGeneral(m_root, face));
#endif
      ++m_numGeom;
   }
   (*c)->addData(arg, face);
}

template <class BasicTraits>
FaceSpatialize<BasicTraits>::Category::Category (NodePtr root)
  : m_root(root), m_totalFaces(0), m_numGeom(0)
{
}
template <class BasicTraits>
FaceSpatialize<BasicTraits>::Category::~Category ()
{
#ifdef GV_VERBOSE
  if (GV_verbose && m_numGeom > 0) {
      GV_stream << "Category statistics: " 
		<< std::endl
		<< m_totalFaces/Real(m_numGeom) << " faces per geometry node"
		<< std::endl;
   }
#endif
   for (CategoryList::iterator c = m_all.begin();
	c != m_all.end();
	++c) {
      delete (*c);
   }
}

template <class BasicTraits>
NodePtr FaceSpatialize<BasicTraits>::Category::getRoot () const
{
   return m_root;
}



template <class BasicTraits>
FaceSpatialize<BasicTraits>::FaceSpatialize ()
  : m_ifs(NULL), m_depth(0), m_flat(false)
{
#if 1
   if (getGroupCore() == NullFC) {
      setGroupCore(Group::create());
   }
#endif
}
template <class BasicTraits>
FaceSpatialize<BasicTraits>::~FaceSpatialize ()
{
}

template <class BasicTraits>
GroupPtr FaceSpatialize<BasicTraits>::s_group;

template <class BasicTraits>
GroupPtr FaceSpatialize<BasicTraits>::getGroupCore ()
{
   return s_group;
   //return Group::create();
}
template <class BasicTraits>
void          FaceSpatialize<BasicTraits>::setGroupCore (const GroupPtr& group)
{
   setRefdCP(s_group, group);
}

template <class BasicTraits>
NodePtr FaceSpatialize<BasicTraits>::getRoot () const
{ 
   return m_root;
}
template <class BasicTraits>
void         FaceSpatialize<BasicTraits>::setDepth (unsigned depth)
{
   m_depth = depth;
}
template <class BasicTraits>
unsigned     FaceSpatialize<BasicTraits>::getDepth () const
{
   return m_depth;
}

template <class BasicTraits>
void         FaceSpatialize<BasicTraits>::setFlat (bool flat)
{
   m_flat = flat;
}
template <class BasicTraits>
bool         FaceSpatialize<BasicTraits>::getFlat () const
{
   return m_flat;
}

template <class BasicTraits>
bool     FaceSpatialize<BasicTraits>::InitEnter  (GeneralType*, const TransformType&)
{
   m_level = 0;
   Thread::getCurrentChangeList()->clearAll();
   return true;
}
template <class BasicTraits>
bool     FaceSpatialize<BasicTraits>::InitLeave  (GeneralType*, const TransformType&)
{
   assert(m_ifsStack.size() == 0);
   return true;
}
template <class BasicTraits>
typename FaceSpatialize<BasicTraits>::ResultT  
FaceSpatialize<BasicTraits>::InnerEnter (GroupType*)
{
  if (m_level <= m_depth) {
     if (m_flat) {
       if (m_level == 0) {
	  m_root = Node::create();
	  m_rootLevel = m_root;
	  beginEditCP(m_root);
	  m_root->setCore(getGroupCore());
       } else {
	  beginEditCP(m_root);
       }
       m_ifs = new Category(m_root);
     } else {
       NodePtr root = Node::create();
       if (m_level > 0) {
	 m_rootLevel->addChild(root);
       } else {
	 m_root = root;
       }
       m_rootLevel = root;
       beginEditCP(root);
       root->setCore(getGroupCore());
       m_ifs = new Category(root);
     }
     m_ifsStack.push_back(m_ifs);
  }
  ++m_level;
  return SingleTraverserBase<BasicTraits>::CONTINUE;
}
template <class BasicTraits>
typename FaceSpatialize<BasicTraits>::ResultT  
FaceSpatialize<BasicTraits>::InnerLeave (GroupType*)
{
  --m_level;
  if (m_level <= m_depth) {
     delete m_ifs;
     endEditCP(m_rootLevel);
     m_ifsStack.pop_back();
     if (m_ifsStack.size() > 0) {
        m_ifs = m_ifsStack.back();
	m_rootLevel = m_ifs->getRoot();
     }
  }
  return SingleTraverserBase<BasicTraits>::CONTINUE;
}
template <class BasicTraits>
typename FaceSpatialize<BasicTraits>::ResultT  
FaceSpatialize<BasicTraits>::LeafEnter  (AdapterType* p0)
{
   m_ifs->addData(p0);
   return SingleTraverserBase<BasicTraits>::CONTINUE;
}


template <class BasicTraits>
typename FaceSpatializeTraits<BasicTraits>::InitFunctorT  
FaceSpatializeTraits<BasicTraits>::createInitFunctor  (ObjectT* obj)
{
   typename InitFunctorT::InitMethodT f = &(ObjectT::InitEnter);
   return InitFunctorT(obj, f);
}
template <class BasicTraits>
typename FaceSpatializeTraits<BasicTraits>::InitFunctorT  
FaceSpatializeTraits<BasicTraits>::createInitLeaveFunctor  (ObjectT* obj)
{
   typename InitFunctorT::InitMethodT f = &(ObjectT::InitLeave);
   return InitFunctorT(obj, f);
}
template <class BasicTraits>
typename FaceSpatializeTraits<BasicTraits>::InnerFunctorT 
FaceSpatializeTraits<BasicTraits>::createInnerFunctor (ObjectT* obj)
{
   typename InnerFunctorT::DispatchMethodT f = &(ObjectT::InnerEnter);
   return InnerFunctorT(obj, f);
}
template <class BasicTraits>
typename FaceSpatializeTraits<BasicTraits>::InnerFunctorT 
FaceSpatializeTraits<BasicTraits>::createInnerLeaveFunctor (ObjectT* obj)
{
   typename InnerFunctorT::DispatchMethodT f = &(ObjectT::InnerLeave);
   return InnerFunctorT(obj, f);
}
template <class BasicTraits>
typename FaceSpatializeTraits<BasicTraits>::LeafFunctorT  
FaceSpatializeTraits<BasicTraits>::createLeafFunctor  (ObjectT* obj)
{
   typename LeafFunctorT::DispatchMethodT f = &(ObjectT::LeafEnter);
   return LeafFunctorT(obj, f);
}
