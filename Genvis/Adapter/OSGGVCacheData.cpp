#include "OSGGVCacheData.h"
USING_GENVIS_NAMESPACE

OpenSGData::OpenSGData ()
{
}
OpenSGData::OpenSGData (const OSG::NodePtr& node, const OSG::Matrix& matrix)
{
   m_node = node;
   for (u32 id=0; id<GV_MAX_NUM_ADAPTER; ++id) {
      m_matrix[id] = matrix;
   }
}
OpenSGData::ChildContainer& 
OpenSGData::getChildren () const
{
   assert(m_node != NullFC);
   return *(m_node->getMFChildren());
}
i32  OpenSGData::getNumChildren () const
{
   assert(m_node != NullFC);
   return m_node->getNChildren();
}
OSG::Matrix OpenSGData::getToWorldMatrix () const
{
   return m_node->getToWorld();
}

OSG::Matrix&       OpenSGData::getFrameMatrix ()
{
   return m_frame;
}
const OSG::Matrix& OpenSGData::getFrameMatrix () const
{
   return m_frame;
}
void               OpenSGData::setFrameMatrix (const OSG::Matrix& m)
{
   m_frame = m;
}

OSG::NodePtr OpenSGData::getNode    () const
{
   return m_node;
}
OSG::NodePtr OpenSGData::getOriginal() const
{
   return getNode();
}
OSG::NodePtr OpenSGData::getParent  () const
{
   if (m_node != OSG::NullFC) {
      return m_node->getParent();
   } else {
      return OSG::NullFC;
   }
}

OSG::Matrix& OpenSGData::getAdapterMatrix (u32 id)
{
   assert(id < GV_MAX_NUM_ADAPTER);
   return m_matrix[id];
}
const OSG::Matrix& OpenSGData::getAdapterMatrix (u32 id) const
{
   assert(id < GV_MAX_NUM_ADAPTER);
   return m_matrix[id];
}
void OpenSGData::setAdapterMatrix (u32 id, const OSG::Matrix& matrix)
{
   assert(id < GV_MAX_NUM_ADAPTER);
   m_matrix[id] = matrix;
}
OpenSGData::AdapterContainer& OpenSGData::getAdapter(u32 id)
{
   assert(id < GV_MAX_NUM_ADAPTER);
   return m_array[id];
}
const OpenSGData::AdapterContainer& OpenSGData::getAdapter(u32 id) const
{
   assert(id < GV_MAX_NUM_ADAPTER);
   return m_array[id];
}
OpenSGData::AdapterP& OpenSGData::getAdapter(u32 id, u32 num)
{
   assert(id < GV_MAX_NUM_ADAPTER);
   assert(num <= m_array[id].size());
   return *(m_array[id].end()-num);
}
const OpenSGData::AdapterP& OpenSGData::getAdapter(u32 id, u32 num) const
{
   assert(id < GV_MAX_NUM_ADAPTER);
   assert(num <= m_array[id].size());
   return *(m_array[id].end()-num);
}
void OpenSGData::setAdapter(u32 id, AdapterP a) 
{
   assert(id < GV_MAX_NUM_ADAPTER);
   m_array[id].push_back(a);
}
void OpenSGData::setAdapter(u32 id, const AdapterContainer& a) 
{
   assert(id < GV_MAX_NUM_ADAPTER);
   m_array[id].insert(m_array[id].end(), a.begin(), a.end());
}

void OpenSGData::clearColCache ()
{
   m_cache.clear();
}
OpenSGData::AdapterContainer& OpenSGData::getColCache (Self& data)
{
   return m_cache[&data];
}
const OpenSGData::AdapterContainer& OpenSGData::getColCache (Self& data) const
{
   SelfP             nonconstThis = (SelfP)this;
   AdapterContainer& result = nonconstThis->m_cache[&data];
   return result;
}

void OpenSGData::destroyAdapter (u32 id, u32 len)
{
   clearAdapter(id, len);
}
void OpenSGData::clearAdapter (u32 id, u32 len)
{
   assert(id < GV_MAX_NUM_ADAPTER); 
   AdapterVector::iterator end = (len > 0 && len < m_array[id].size()
				  ? m_array[id].begin()+len 
				  : m_array[id].end());
   m_array[id].erase(m_array[id].begin(), end);
   m_cache.clear();
}

