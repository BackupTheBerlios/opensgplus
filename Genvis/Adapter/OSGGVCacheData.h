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

#ifndef OSGGVCACHEDATA_H
#define OSGGVCACHEDATA_H

#include <vector>
#include "OSGGVBase.h"
#include "OSGGVBVolAdapterBase.h"
#include "OSGNode.h"
#include "OSGMatrix.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief OpenSGCacheData.
 */
class OSG_GENVISLIB_DLLMAPPING OpenSGData
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef OpenSGData                    Self;
   typedef MFNodePtr                     ChildContainer;
   typedef std::vector<BVolAdapterBaseP> AdapterVector;
   typedef AdapterVector                 AdapterContainer;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline OpenSGData ();
   inline OpenSGData (const OSG::NodePtr& node, const OSG::Matrix& matrix);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Data of referenced scenegraph Node.                          */
   /*! \{                                                                 */
   inline OSG::Matrix        getToWorldMatrix () const;
   inline OSG::NodePtr       getOriginal() const;
   inline OSG::NodePtr       getNode    () const;
   inline OSG::NodePtr       getParent  () const;
   inline int                getNumChildren () const;
   inline ChildContainer&    getChildren    () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Adapter matrices.                                            */
   /*! \{                                                                 */
   inline OSG::Matrix&       getAdapterMatrix (unsigned id);
   inline const OSG::Matrix& getAdapterMatrix (unsigned id) const;
   inline void               setAdapterMatrix (unsigned id, const OSG::Matrix& m);
   
   void                      destroyAdapter (unsigned id, unsigned len = 0);
   void                      clearAdapter (unsigned id, unsigned len = 0);
   inline AdapterContainer&  getAdapter   (unsigned id);
   inline const AdapterContainer& getAdapter   (unsigned id) const;
   inline Adapter*&          getAdapter   (unsigned id, unsigned num);
   inline const BVolAdapterBaseP& getAdapter   (unsigned id, unsigned num) const;
   inline void               setAdapter   (unsigned id, Adapter* a);
   inline void               setAdapter   (unsigned id, const AdapterVector& a);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Collision cache.                                             */
   /*! \{                                                                 */
   inline AdapterContainer&  getColCache ();
   inline const AdapterContainer& getColCache () const;
   inline Adapter*&          getColCache (unsigned num);
   inline const BVolAdapterBaseP& getColCache (unsigned num) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   OSG::NodePtr     m_node;
   AdapterContainer m_array[GV_MAX_NUM_ADAPTER];
   AdapterContainer m_cache;
   OSG::Matrix      m_matrix[GV_MAX_NUM_ADAPTER];
};

inline OpenSGData::OpenSGData ()
{
}
inline OpenSGData::OpenSGData (const OSG::NodePtr& node, const OSG::Matrix&)
{
   m_node   = node;
#if 0
   for (unsigned id=0; id<GV_MAX_NUM_ADAPTER; ++id) {
      m_matrix[id] = matrix;
   }
#endif
}
inline OpenSGData::ChildContainer& 
OpenSGData::getChildren () const
{
   assert(m_node != NullFC);
   return *(m_node->getMFChildren());
}
inline int  OpenSGData::getNumChildren () const
{
   assert(m_node != NullFC);
   return m_node->getNChildren();
}
inline OSG::Matrix OpenSGData::getToWorldMatrix () const
{
   return m_node->getToWorld();
}

inline OSG::NodePtr OpenSGData::getNode    () const
{
   return m_node;
}
inline OSG::NodePtr OpenSGData::getOriginal() const
{
   return getNode();
}
inline OSG::NodePtr OpenSGData::getParent  () const
{
   if (m_node != OSG::NullFC) {
      return m_node->getParent();
   } else {
      return OSG::NullFC;
   }
}

inline OSG::Matrix& OpenSGData::getAdapterMatrix (unsigned id)
{
   assert(id < GV_MAX_NUM_ADAPTER);
   return m_matrix[id];
}
inline const OSG::Matrix& OpenSGData::getAdapterMatrix (unsigned id) const
{
   assert(id < GV_MAX_NUM_ADAPTER);
   return m_matrix[id];
}
inline void 
OpenSGData::setAdapterMatrix (unsigned id, const OSG::Matrix& matrix)
{
   assert(id < GV_MAX_NUM_ADAPTER);
   m_matrix[id] = matrix;
}
inline OpenSGData::AdapterContainer& 
OpenSGData::getAdapter(unsigned id)
{
   assert(id < GV_MAX_NUM_ADAPTER);
   //Self* nonconstThis = (Self*)this;
   return m_array[id];
}
inline const OpenSGData::AdapterContainer& 
OpenSGData::getAdapter(unsigned id) const
{
   assert(id < GV_MAX_NUM_ADAPTER);
   return m_array[id];
}
inline Adapter*& OpenSGData::getAdapter(unsigned id, unsigned num)
{
   assert(id < GV_MAX_NUM_ADAPTER);
   assert(num <= m_array[id].size());
   //Self* nonconstThis = (Self*)this;
   return *(m_array[id].end()-num);
}
inline const BVolAdapterBaseP& 
OpenSGData::getAdapter(unsigned id, unsigned num) const
{
   assert(id < GV_MAX_NUM_ADAPTER);
   assert(num <= m_array[id].size());
   return *(m_array[id].end()-num);
}
inline void OpenSGData::setAdapter(unsigned id, Adapter* a) 
{
   assert(id < GV_MAX_NUM_ADAPTER);
   m_array[id].push_back(a);
}
inline void OpenSGData::setAdapter(unsigned id, const AdapterContainer& a) 
{
   assert(id < GV_MAX_NUM_ADAPTER);
   m_array[id].insert(m_array[id].end(), a.begin(), a.end());
}

inline OpenSGData::AdapterContainer& OpenSGData::getColCache ()
{
   //Self* nonconstThis = (Self*)this;
   return m_cache;
}
inline const OpenSGData::AdapterContainer& OpenSGData::getColCache () const
{
   return m_cache;
}
inline Adapter*& OpenSGData::getColCache (unsigned num)
{
   assert(num <= m_cache.size());
   //Self* nonconstThis = (Self*)this;
   return *(m_cache.end()-num);
}
inline const BVolAdapterBaseP& 
OpenSGData::getColCache (unsigned num) const
{
   assert(num <= m_cache.size());
   return *(m_cache.end()-num);
}

END_GENVIS_NAMESPACE
#endif




