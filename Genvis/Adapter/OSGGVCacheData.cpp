#include "OSGGVCacheData.h"
USING_GENVIS_NAMESPACE

void OpenSGData::destroyAdapter (unsigned id, unsigned len)
{
   clearAdapter(id, len);
}
void OpenSGData::clearAdapter (unsigned id, unsigned len)
{
   assert(id < GV_MAX_NUM_ADAPTER); 
   AdapterVector::iterator end = (len > 0 && len < m_array[id].size()
				  ? m_array[id].begin()+len 
				  : m_array[id].end());
   m_array[id].erase(m_array[id].begin(), end);
   m_cache.clear();
}

