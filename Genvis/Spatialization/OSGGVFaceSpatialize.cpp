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

#include "OSGMaterial.h"
OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING FaceSpatialize<OpenSGTraits>;
template class OSG_GENVISLIB_DLLMAPPING FaceSpatializeTraits<OpenSGTraits>;


void CategoryRaw::begin (const FaceIterator& face)
{
   m_coord  = GeoPositions3f::create();
   m_normal = GeoNormals3f::create();
   m_index  = GeoIndicesUI32::create();	
   m_len    = GeoPLengthsUI32::create();	
   m_type   = GeoPTypesUI8::create();	

   m_material = face.getGeometry()->getMaterial();
   m_node = Node::create();
   GeometryPtr geom = Geometry::create();
   beginEditCP(m_node);
   m_node->setCore(geom);
   endEditCP(m_node);
   beginEditCP(geom);
   geom->setMaterial(m_material);
   geom->setPositions(m_coord);
   geom->setNormals(m_normal);
   geom->setIndices(m_index);
   geom->setTypes(m_type);
   geom->setLengths(m_len);
   endEditCP(geom);
   beginEditCP(m_coord);
   beginEditCP(m_normal);
   beginEditCP(m_index);
   beginEditCP(m_type);
   beginEditCP(m_len);
   m_faceLength = face.getLength();
   GeoPTypesUI8::StoredFieldType* t = m_type->getFieldPtr();
   t->addValue((m_faceLength==3 ? GL_TRIANGLES : GL_QUADS));
}
void CategoryRaw::end ()
{
   GeoPLengthsUI32::StoredFieldType* l = m_len->getFieldPtr();
   l->addValue(m_index->getSize());   
   endEditCP(m_len);
   endEditCP(m_type);
   endEditCP(m_index);
   endEditCP(m_normal);
   endEditCP(m_coord);
}
void CategoryRaw::addData (OpenSGFaceBase<OpenSGTraits>* node,
			   const FaceIterator&           )
{
   GeoPositions3f::StoredFieldType*  p = m_coord->getFieldPtr();
   GeoNormals3f::StoredFieldType*    n = m_normal->getFieldPtr();
   GeoIndicesUI32::StoredFieldType*  i = m_index->getFieldPtr();

   GeoPositionsPtr faceP = node->getPositions();
   addRefCP(faceP);
   int k;
   for (k=0; k<faceP->getSize(); ++k) {
      p->addValue(faceP->getValue(k));
      i->addValue(p->size()-1);
   }
   if (!m_hasNormal) {
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
CategoryRaw::CategoryRaw (NodePtr root,
			  const FaceIterator& face)
  :  m_ccw(true), m_faceLength(0),
     m_hasNormal(hasNormalAttributes(face)), 
     m_hasColor(hasColorAttributes(face)),
     m_hasTex(hasTexAttributes(face))
{
   CategoryRaw::begin(face);
   beginEditCP(root);
   root->addChild(m_node);
   endEditCP(root);
}
CategoryRaw::~CategoryRaw ()
{
   CategoryRaw::end();
}

std::ostream&   CategoryRaw::print (std::ostream& os) const
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

void CategoryColor::begin (const FaceIterator&)
{
   if (m_hasColor) {
      m_color = GeoColors3f::create();

      GeometryPtr geom = GeometryPtr::dcast(m_node->getCore());
      beginEditCP(geom);
      geom->setColors(m_color);
      endEditCP(geom);
      beginEditCP(m_color);
   }
}
void CategoryColor::end ()
{
   if (m_hasColor) {
      endEditCP(m_color);
   }
}
void CategoryColor::addData (OpenSGFaceBase<OpenSGTraits>* node,
			     const FaceIterator&           face)
{
   CategoryRaw::addData(node, face);
   if (m_hasColor) {
      GeoColors3f::StoredFieldType* c = m_color->getFieldPtr();
      for (int k=0; k<face.getLength(); ++k) {
	 c->addValue(face.getColor(k));
      }
      assert(m_coord->getSize() == m_color->getSize());
   }
}
CategoryColor::CategoryColor (NodePtr root,
			      const FaceIterator& face)
  : CategoryRaw(root, face)
{
   CategoryColor::begin(face);
}
CategoryColor::~CategoryColor ()
{
   CategoryColor::end();
}


void CategoryGeneral::begin (const FaceIterator&)
{
   if (m_hasTex) {
      m_texCoord = GeoTexCoords2f::create();

      GeometryPtr geom = GeometryPtr::dcast(m_node->getCore());
      beginEditCP(geom);
      geom->setTexCoords(m_texCoord);
      endEditCP(geom);
      beginEditCP(m_texCoord);
   }
}
void    CategoryGeneral::end ()
{
   if (m_hasTex) {
      endEditCP(m_texCoord);
   }
}
void         CategoryGeneral::addData (OpenSGFaceBase<OpenSGTraits>* node,
				       const FaceIterator& face)
{
   CategoryColor::addData(node, face);
   if (m_hasTex) {
      GeoTexCoords2f::StoredFieldType* t = m_texCoord->getFieldPtr();
      for (int k=0; k<face.getLength(); ++k) {
	 t->addValue(face.getTexCoords(k));
      }
      assert(m_coord->getSize() == m_texCoord->getSize());
   }
}
CategoryGeneral::CategoryGeneral (NodePtr root,
				  const FaceIterator& face)
  : CategoryColor(root, face)
{
   CategoryGeneral::begin(face); 
}
CategoryGeneral::~CategoryGeneral ()
{
   CategoryGeneral::end();
}


void    Category::addData (OpenSGFaceBase<OpenSGTraits>* arg)
{
   FaceIterator face = arg->getOriginal();
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
      c = m_all.insert(m_all.end(), new CategoryGeneral(m_root, face));
      ++m_numGeom;
   }
   (*c)->addData(arg, face);
}

Category::Category (NodePtr root)
  : m_root(root), m_totalFaces(0), m_numGeom(0)
{
}
Category::~Category ()
{
#ifdef GV_VERBOSE
  if (GV_verbose && m_numGeom > 0) {
      std::cout << "Category statistics: " 
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

NodePtr Category::getRoot () const
{
   return m_root;
}


template <class BasicTraits>
FaceSpatialize<BasicTraits>::FaceSpatialize ()
  : m_ifs(NULL), m_level(0), m_flat(false)
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
void         FaceSpatialize<BasicTraits>::setLevel (unsigned level)
{
   m_level = level;
}
template <class BasicTraits>
unsigned     FaceSpatialize<BasicTraits>::getLevel () const
{
   return m_level;
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
   m_current = 0;
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
FaceSpatialize<BasicTraits>::ResultT  FaceSpatialize<BasicTraits>::InnerEnter (GroupType*)
{
  if (m_current <= m_level) {
     if (m_flat) {
       if (m_current == 0) {
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
       if (m_current > 0) {
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
  ++m_current;
  return SingleTraverserBase<BasicTraits>::CONTINUE;
}
template <class BasicTraits>
FaceSpatialize<BasicTraits>::ResultT  FaceSpatialize<BasicTraits>::InnerLeave (GroupType*)
{
  --m_current;
  if (m_current <= m_level) {
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
FaceSpatialize<BasicTraits>::ResultT  FaceSpatialize<BasicTraits>::LeafEnter  (AdapterType* p0)
{
   m_ifs->addData(p0);
   return SingleTraverserBase<BasicTraits>::CONTINUE;
}


template <class BasicTraits>
FaceSpatializeTraits<BasicTraits>::InitFunctorT  
FaceSpatializeTraits<BasicTraits>::createInitFunctor  (ObjectT* obj)
{
   InitFunctorT::InitMethodT f = &(ObjectT::InitEnter);
   return InitFunctorT(obj, f);
}
template <class BasicTraits>
FaceSpatializeTraits<BasicTraits>::InitFunctorT  
FaceSpatializeTraits<BasicTraits>::createInitLeaveFunctor  (ObjectT* obj)
{
   InitFunctorT::InitMethodT f = &(ObjectT::InitLeave);
   return InitFunctorT(obj, f);
}
template <class BasicTraits>
FaceSpatializeTraits<BasicTraits>::InnerFunctorT 
FaceSpatializeTraits<BasicTraits>::createInnerFunctor (ObjectT* obj)
{
   InnerFunctorT::DispatchMethodT f = &(ObjectT::InnerEnter);
   return InnerFunctorT(obj, f);
}
template <class BasicTraits>
FaceSpatializeTraits<BasicTraits>::InnerFunctorT 
FaceSpatializeTraits<BasicTraits>::createInnerLeaveFunctor (ObjectT* obj)
{
   InnerFunctorT::DispatchMethodT f = &(ObjectT::InnerLeave);
   return InnerFunctorT(obj, f);
}
template <class BasicTraits>
FaceSpatializeTraits<BasicTraits>::LeafFunctorT  
FaceSpatializeTraits<BasicTraits>::createLeafFunctor  (ObjectT* obj)
{
   LeafFunctorT::DispatchMethodT f = &(ObjectT::LeafEnter);
   return LeafFunctorT(obj, f);
}
