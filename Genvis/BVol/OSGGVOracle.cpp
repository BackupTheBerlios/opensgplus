//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.4 $
//   $Date: 2004/12/22 18:45:01 $
//                                                                            
//=============================================================================

#include <algorithm>
#include <string.h>
#include "OSGGVOracle.h"
#include "OSGGVBVolFunctions.h"
USING_GENVIS_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING Oracle<K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING Oracle<K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING Oracle<K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING Oracle<K26Dop>;
template class OSG_GENVISLIB_DLLMAPPING Oracle<K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING Oracle<K20Dop>;

template class OSG_GENVISLIB_DLLMAPPING MainAxesSorted<K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING MainAxesSorted<K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING MainAxesSorted<K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING MainAxesSorted<K26Dop>;
template class OSG_GENVISLIB_DLLMAPPING MainAxesSorted<K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING MainAxesSorted<K20Dop>;

template class OSG_GENVISLIB_DLLMAPPING AllAxesSorted<K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING AllAxesSorted<K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING AllAxesSorted<K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING AllAxesSorted<K26Dop>;
template class OSG_GENVISLIB_DLLMAPPING AllAxesSorted<K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING AllAxesSorted<K20Dop>;

template <class BVOL>
std::vector<Oracle<BVOL>*>& Oracle<BVOL>::getOraclesInternal ()
{
   static std::vector<Oracle<BVOL>*> s_instances;
   return s_instances;
}
template <class BVOL>
const std::vector<Oracle<BVOL>*>& Oracle<BVOL>::getOracles ()
{
   return getOraclesInternal();
}
template <class BVOL>
void          Oracle<BVOL>::registerOracle (Oracle<BVOL>* instance)
{
   getOraclesInternal().push_back(instance);
}


class ComponentCompare
{
public:
   inline ComponentCompare    (u32 dimension,
			  const std::vector<Adapter*>& allNodes);

  inline bool operator() (u32 node1, u32 node2) const;

protected:
   u32 m_dimension;
   const std::vector<Adapter*>& m_allNodes;
};
inline ComponentCompare::ComponentCompare (u32 dimension,
					   const std::vector<Adapter*>& allNodes)
  : m_dimension(dimension), m_allNodes(allNodes)
{
}
inline bool ComponentCompare::operator() (u32 node1, u32 node2) const
{
   return static_cast<BVolAdapterBase*>(m_allNodes[node1])->getCenter()[m_dimension]
     < static_cast<BVolAdapterBase*>(m_allNodes[node2])->getCenter()[m_dimension];
}

class ProjCompare
{
public:
   inline ProjCompare     (const VectorClass& direction,
			   const std::vector<Adapter*>& allNodes);

   inline bool operator() (u32 node1, u32 node2);

protected:
   VectorClass m_direction;
   const std::vector<Adapter*>& m_allNodes;
};
inline ProjCompare::ProjCompare (const VectorClass& direction,
				 const std::vector<Adapter*>& allNodes)
  : m_direction(direction), m_allNodes(allNodes)
{
}
inline bool ProjCompare::operator() (u32 node1, u32 node2)
{
   Real value1 = m_direction.dot(static_cast<BVolAdapterBase*>(m_allNodes[node1])->getCenter());
   Real value2 = m_direction.dot(static_cast<BVolAdapterBase*>(m_allNodes[node2])->getCenter());
   return value1 < value2;
}


template <class BVOL>
void MainAxesSorted<BVOL>::sortIndexList(std::vector<u32>& index,
					 const std::vector<Adapter*>& allNodes,
					 u32 d)
{
   std::vector<u32>::iterator begin = index.begin();
   std::vector<u32>::iterator end   = index.end();
   std::sort(begin, end, ComponentCompare(d, allNodes));
}

template <class BVOL>
void AllAxesSorted<BVOL>::sortIndexList(std::vector<u32>& index,
					const std::vector<Adapter*>& allNodes,
					u32 d)
{
   std::vector<u32>::iterator begin = index.begin();
   std::vector<u32>::iterator end   = index.end();
   std::sort(begin, end, ProjCompare(BVOL::getDirection()[d], allNodes));
}








