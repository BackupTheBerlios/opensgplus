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

#include "OSGGVFaceSpatializeIndexed.h"
#include "OSGGVBVolAdapter.h"

#include "OSGMaterial.h"
#include "OSGMaterialGroup.h"
#include "OSGGeoFunctions.h"
OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING FaceSpatializeIndexed<OpenSGTraits>;
template class OSG_GENVISLIB_DLLMAPPING FaceSpatializeIndexedTraits<OpenSGTraits>;

// internal classes
template <class BasicTraits>
void FaceSpatializeIndexed<BasicTraits>::CategoryRaw::begin (OpenSGFaceBase<OpenSGTraits>* node, const FaceIterator& face)
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
   m_indexStride = 1;
   m_indexOffset.position = 0;
   m_geom->getIndexMapping().addValue(Geometry::MapPosition);
   if (m_hasNormal) {
      m_normal = GeoNormals3f::create();
      m_geom->setNormals(m_normal);
      m_geom->getIndexMapping().addValue(Geometry::MapNormal);
      m_indexOffset.normal = m_indexOffset.position+1;
      ++m_indexStride;
   } else {
      m_indexOffset.normal = m_indexOffset.position;
   }
   if (m_hasColor) { 
      m_geom->getIndexMapping().addValue(Geometry::MapColor);
      m_indexOffset.color = m_indexOffset.normal+1;
      ++m_indexStride; 
   } else {
      m_indexOffset.color = m_indexOffset.normal;
   }
   if (m_hasTex) {
      m_geom->getIndexMapping().addValue(Geometry::MapTexCoords);
      m_indexOffset.tex1 = m_indexOffset.color+1;
      ++m_indexStride; 
   } else {
      m_indexOffset.tex1 = m_indexOffset.color;
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
void FaceSpatializeIndexed<BasicTraits>::CategoryRaw::end ()
{
   // store length of index sequence #PRIMS * (3 resp. 4)
   GeoPLengthsUI32::StoredFieldType* l = m_len->getFieldPtr();
   //l->addValue(m_index->getSize());   
   assert(m_quadOffset%m_indexStride == 0);
   l->addValue(m_quadOffset/m_indexStride); 
   assert((m_index->getSize()-m_quadOffset)%m_indexStride == 0);
   l->addValue((m_index->getSize()-m_quadOffset)/m_indexStride);
   // end edit geometry field contents
   endEditCP(m_len);
   endEditCP(m_type);
   endEditCP(m_index);
   endEditCP(m_normal);
   endEditCP(m_coord);
   // optimize geometry: 5 iterations, create strips and fans
   createOptimizedPrimitives(m_geom, 5, true, true, 8, false);
}
template <class BasicTraits>
void FaceSpatializeIndexed<BasicTraits>::CategoryRaw::addData (OpenSGFaceBase<OpenSGTraits>* node, const FaceIterator& face)
{
   GeoPositions3f::StoredFieldType*  p = m_coord->getFieldPtr();
   GeoNormals3f::StoredFieldType*    n = m_normal->getFieldPtr();
   //GeoIndicesUI32::StoredFieldType*  i = m_index->getFieldPtr();

   // find offset of positions and normals in the new geometry
   u32 i, k;
   m_offsetIt = m_offset.find(face.getGeometry());
   if (m_offsetIt == m_offset.end()) {
      // insert new offsets entry into map
      HashMapPair offsetPair = 
	m_offset.insert(HashMap::value_type(face.getGeometry(), quad()));
      m_offsetIt = offsetPair.first;

      m_offsetIt->second.position = m_coord->size();
      GeoPositionsPtr faceP = m_original.getPositions();
      addRefCP(faceP);
      for (k=0; k<faceP->getSize(); ++k) {
	 p->addValue(faceP->getValue(k));
      }
      if (m_hasNormal) {
 	 m_offsetIt->second.normal = m_normal->size();
	 GeoNormalsPtr faceN = m_original.getNormals();
	 addRefCP(faceN);
	 for (k=0; k<faceN->getSize(); ++k) {
	    n->addValue(faceN->getValue(k));
	 }
	 subRefCP(faceN);
      }
      subRefCP(faceP);
   }

   // insert indices
   if (face.getLength() == 3) {
      for (k=0; k<3; ++k) {
	 m_index->insertValue(face.getPositionIndex(k)+m_offsetIt->second.position, m_quadOffset++);
	 i = 1;
	 if (m_hasNormal) {
	    m_index->insertValue(face.getNormalIndex(k)+m_offsetIt->second.normal,     m_quadOffset++);
	    ++i;
	 }
	 for (; i<m_indexStride; ++i) {
	    m_index->insertValue(0, m_quadOffset++);
	 }
      }
   } else {
      for (k=0; k<4; ++k) {
	 m_index->addValue(face.getPositionIndex(k)+m_offsetIt->second.position);
	 i = 1;
	 if (m_hasNormal) {
	    m_index->addValue(face.getNormalIndex(k)+m_offsetIt->second.normal);
	    ++i;
	 }
	 for (; i<m_indexStride; ++i) {
	    m_index->addValue(0);
	 }
      }
   }
}
template <class BasicTraits>
FaceSpatializeIndexed<BasicTraits>::CategoryRaw::CategoryRaw (NodePtr root, 
			  OpenSGFaceBase<OpenSGTraits>* node, const FaceIterator& face)
  :  m_hasNormal(hasNormalAttributes(face)), 
     m_hasColor(hasColorAttributes(face)),
     m_hasTex(hasTexAttributes(face)),
     m_original(node->getObjectAdapter().getNode())
{
   // create object adapter 
   m_original.setObjectAdapter(&node->getObjectAdapter());

   CategoryRaw::begin(node, face);
   beginEditCP(root);
   root->addChild(m_node);
   endEditCP(root);
}
template <class BasicTraits>
FaceSpatializeIndexed<BasicTraits>::CategoryRaw::~CategoryRaw ()
{
   CategoryRaw::end();
}

template <class BasicTraits>
std::ostream& FaceSpatializeIndexed<BasicTraits>::CategoryRaw::dump (std::ostream& os) const
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
void FaceSpatializeIndexed<BasicTraits>::CategoryColor::begin (OpenSGFaceBase<OpenSGTraits>*, const FaceIterator&)
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
void FaceSpatializeIndexed<BasicTraits>::CategoryColor::end ()
{
   if (m_hasColor) {
      endEditCP(m_color);
   }
}
template <class BasicTraits>
void FaceSpatializeIndexed<BasicTraits>::CategoryColor::addData (OpenSGFaceBase<OpenSGTraits>* node,
								 const FaceIterator&           face)
{
   u32 offsetSize = m_offset.size();
   CategoryRaw::addData(node, face);
   if (m_hasColor) { 
      if (offsetSize < m_offset.size()) {
 	 m_offsetIt->second.color = m_color->size();
	 GeoColors3f::StoredFieldType* c = m_color->getFieldPtr();
	 GeoColorsPtr faceC = m_original.getColors();
	 for (u32 k=0; k<faceC->getSize(); ++k) {
	    c->addValue(faceC->getValue(k));
	 }
      }

      // set indices for colors
      if (face.getLength() == 3) {
 	 u32 i = m_quadOffset - 3*m_indexStride + m_indexOffset.color;
	 for (u32 k=0; k<3; ++k) {
	    m_index->setValue(face.getColorIndex(k)+m_offsetIt->second.color, i);
	    assert(face.getColorIndex(k)+m_offsetIt->second.color < m_color->size());
	    i += m_indexStride;
	 }
      } else {
 	 u32 i = m_index->size() - 4*m_indexStride + m_indexOffset.color;
	 for (u32 k=0; k<4; ++k) {
	    m_index->setValue(face.getColorIndex(k)+m_offsetIt->second.color, i);
	    assert(face.getColorIndex(k)+m_offsetIt->second.color < m_color->size());
	    i += m_indexStride;
	 }
      }
   }
}
template <class BasicTraits>
FaceSpatializeIndexed<BasicTraits>::CategoryColor::CategoryColor (NodePtr root, 
								  OpenSGFaceBase<OpenSGTraits>* node, const FaceIterator& face)
  : CategoryRaw(root, node, face)
{
   CategoryColor::begin(node, face);
}
template <class BasicTraits>
FaceSpatializeIndexed<BasicTraits>::CategoryColor::~CategoryColor ()
{
   CategoryColor::end();
}


template <class BasicTraits>
void FaceSpatializeIndexed<BasicTraits>::CategoryGeneral::begin (OpenSGFaceBase<OpenSGTraits>*, const FaceIterator&)
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
void FaceSpatializeIndexed<BasicTraits>::CategoryGeneral::end ()
{
   if (m_hasTex) {
      endEditCP(m_texCoord);
   }
}
template <class BasicTraits>
void FaceSpatializeIndexed<BasicTraits>::CategoryGeneral::addData (OpenSGFaceBase<OpenSGTraits>* node, const FaceIterator& face)
{
   u32 offsetSize = m_offset.size();
   CategoryColor::addData(node, face);
   if (m_hasTex) {
      if (offsetSize < m_offset.size()) {
 	 m_offsetIt->second.tex1 = m_texCoord->size();
	 GeoTexCoords2f::StoredFieldType* t = m_texCoord->getFieldPtr();
	 GeoTexCoordsPtr faceT = m_original.getTexCoords();
	 for (u32 k=0; k<faceT->getSize(); ++k) {
	    t->addValue(faceT->getValue(k));
	 }
      }

      // set indices for TexCoords
      if (face.getLength() == 3) {
 	 u32 i = m_quadOffset - 3*m_indexStride + m_indexOffset.tex1;
	 for (u32 k=0; k<3; ++k) {
	    m_index->setValue(face.getTexCoordsIndex(k)+m_offsetIt->second.tex1, i);
	    assert(face.getTexCoordsIndex(k)+m_offsetIt->second.tex1 < m_texCoord->size());
	    i += m_indexStride;
	 }
      } else {
 	 u32 i = m_index->size() - 4*m_indexStride + m_indexOffset.tex1;
	 for (u32 k=0; k<4; ++k) {
	    m_index->setValue(face.getTexCoordsIndex(k)+m_offsetIt->second.tex1, i);
	    assert(face.getTexCoordsIndex(k)+m_offsetIt->second.tex1 < m_texCoord->size());
	    i += m_indexStride;
	 }
      }
   }
}
template <class BasicTraits>
FaceSpatializeIndexed<BasicTraits>::CategoryGeneral::CategoryGeneral (NodePtr root, 
								      OpenSGFaceBase<OpenSGTraits>* node, const FaceIterator& face)
  : CategoryColor(root, node, face)
{
   CategoryGeneral::begin(node, face); 
}
template <class BasicTraits>
FaceSpatializeIndexed<BasicTraits>::CategoryGeneral::~CategoryGeneral ()
{
   CategoryGeneral::end();
}


template <class BasicTraits>
void FaceSpatializeIndexed<BasicTraits>::Category::addData (OpenSGFaceBase<OpenSGTraits>* arg)
{
   FaceIterator face = arg->getOriginal();
   if (
       face.getGeometry()->getMaterial()->isTransparent() // transparent face
       && m_trans.find(arg->getObjectAdapter().getNode()) != m_trans.end() // not processed
       ) { 
      // create object adapter
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

   typename CategoryList::iterator c;
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
      c = m_all.insert(m_all.end(), new CategoryGeneral(matNode, arg, face));
#else
      c = m_all.insert(m_all.end(), new CategoryGeneral(m_root, arg, face));
#endif
      ++m_numGeom;
   }
   (*c)->addData(arg, face);
}

template <class BasicTraits>
FaceSpatializeIndexed<BasicTraits>::Category::Category (NodePtr root)
  : m_root(root), m_totalFaces(0), m_numGeom(0)
{
}
template <class BasicTraits>
FaceSpatializeIndexed<BasicTraits>::Category::~Category ()
{
#ifdef GV_VERBOSE
  if (GV_verbose && m_numGeom > 0) {
      GV_stream << "Category statistics: " 
		<< std::endl
		<< m_totalFaces/Real(m_numGeom) << " faces per geometry node"
		<< std::endl;
   }
#endif
   for (typename CategoryList::iterator c = m_all.begin();
	c != m_all.end();
	++c) {
      delete (*c);
   }
}

template <class BasicTraits>
NodePtr FaceSpatializeIndexed<BasicTraits>::Category::getRoot () const
{
   return m_root;
}


template <class BasicTraits>
FaceSpatializeIndexed<BasicTraits>::FaceSpatializeIndexed ()
  : m_ifs(NULL), m_depth(0), m_flat(false)
{
#if 1
   if (getGroupCore() == NullFC) {
      setGroupCore(Group::create());
   }
#endif
}
template <class BasicTraits>
FaceSpatializeIndexed<BasicTraits>::~FaceSpatializeIndexed ()
{
}

template <class BasicTraits>
GroupPtr FaceSpatializeIndexed<BasicTraits>::s_group;

template <class BasicTraits>
GroupPtr FaceSpatializeIndexed<BasicTraits>::getGroupCore ()
{
   return s_group;
   //return Group::create();
}
template <class BasicTraits>
void          FaceSpatializeIndexed<BasicTraits>::setGroupCore (const GroupPtr& group)
{
   setRefdCP(s_group, group);
}

template <class BasicTraits>
NodePtr FaceSpatializeIndexed<BasicTraits>::getRoot () const
{ 
   return m_root;
}
template <class BasicTraits>
void         FaceSpatializeIndexed<BasicTraits>::setDepth (unsigned depth)
{
   m_depth = depth;
}
template <class BasicTraits>
unsigned     FaceSpatializeIndexed<BasicTraits>::getDepth () const
{
   return m_depth;
}

template <class BasicTraits>
void         FaceSpatializeIndexed<BasicTraits>::setFlat (bool flat)
{
   m_flat = flat;
}
template <class BasicTraits>
bool         FaceSpatializeIndexed<BasicTraits>::getFlat () const
{
   return m_flat;
}

template <class BasicTraits>
bool     FaceSpatializeIndexed<BasicTraits>::InitEnter  (GeneralType*, const TransformType&)
{
   m_level = 0;
   Thread::getCurrentChangeList()->clearAll();
   return true;
}
template <class BasicTraits>
bool     FaceSpatializeIndexed<BasicTraits>::InitLeave  (GeneralType*, const TransformType&)
{
   assert(m_ifsStack.size() == 0);
   return true;
}
template <class BasicTraits>
typename FaceSpatializeIndexed<BasicTraits>::ResultT  
FaceSpatializeIndexed<BasicTraits>::InnerEnter (GroupType*)
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
typename FaceSpatializeIndexed<BasicTraits>::ResultT  
FaceSpatializeIndexed<BasicTraits>::InnerLeave (GroupType*)
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
typename FaceSpatializeIndexed<BasicTraits>::ResultT  
FaceSpatializeIndexed<BasicTraits>::LeafEnter  (AdapterType* p0)
{
   m_ifs->addData(p0);
   return SingleTraverserBase<BasicTraits>::CONTINUE;
}


template <class BasicTraits>
typename FaceSpatializeIndexedTraits<BasicTraits>::InitFunctorT  
FaceSpatializeIndexedTraits<BasicTraits>::createInitFunctor  (ObjectT* obj)
{
   typename InitFunctorT::InitMethodT f = &(ObjectT::InitEnter);
   return InitFunctorT(obj, f);
}
template <class BasicTraits>
typename FaceSpatializeIndexedTraits<BasicTraits>::InitFunctorT  
FaceSpatializeIndexedTraits<BasicTraits>::createInitLeaveFunctor  (ObjectT* obj)
{
   typename InitFunctorT::InitMethodT f = &(ObjectT::InitLeave);
   return InitFunctorT(obj, f);
}
template <class BasicTraits>
typename FaceSpatializeIndexedTraits<BasicTraits>::InnerFunctorT 
FaceSpatializeIndexedTraits<BasicTraits>::createInnerFunctor (ObjectT* obj)
{
   typename InnerFunctorT::DispatchMethodT f = &(ObjectT::InnerEnter);
   return InnerFunctorT(obj, f);
}
template <class BasicTraits>
typename FaceSpatializeIndexedTraits<BasicTraits>::InnerFunctorT 
FaceSpatializeIndexedTraits<BasicTraits>::createInnerLeaveFunctor (ObjectT* obj)
{
   typename InnerFunctorT::DispatchMethodT f = &(ObjectT::InnerLeave);
   return InnerFunctorT(obj, f);
}
template <class BasicTraits>
typename FaceSpatializeIndexedTraits<BasicTraits>::LeafFunctorT  
FaceSpatializeIndexedTraits<BasicTraits>::createLeafFunctor  (ObjectT* obj)
{
   typename LeafFunctorT::DispatchMethodT f = &(ObjectT::LeafEnter);
   return LeafFunctorT(obj, f);
}
