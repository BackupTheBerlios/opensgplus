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
//   $Revision: 1.2 $
//   $Date: 2004/03/12 13:16:55 $
//                                                                            
//=============================================================================

#ifndef OSGGVOPTIMIZER_H
#define OSGGVOPTIMIZER_H

#include "OSGGVBase.h"
#include "OSGGVIndices.h"
#include "OSGGVOracle.h"
#include "OSGGVAdapter.h"
#include "OSGGVGroup.h"
#include "OSGGVAdapterFactory.h"

BEGIN_GENVIS_NAMESPACE

template <class GROUP>
class OSG_GENVISLIB_DLLMAPPING Optimizer
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef typename GROUP::BVol            BVol;
   typedef GROUP                           GroupType;
   typedef typename GroupType::FactoryType FactoryType;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   Optimizer (Oracle<BVol>*                      oracle,
	      const std::vector<Adapter*>& leafNodes,
	      unsigned                     maxDepth = 50,
	      unsigned                     minObjects = 256);
   virtual ~Optimizer ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Member.                                                      */
   /*! \{                                                                 */
   inline void                     setDataStore (std::vector<GroupType*>* store);
   inline std::vector<GroupType*>& getDataStore () const;
   inline const std::vector<Adapter*>& getLeafNodes () const;
   inline unsigned                 getCurrentDepth () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Optimization.                                                */
   /*! \{                                                                 */
   GroupType*       optimize ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   /*---------------------------------------------------------------------*/
   /*! \name Internal.                                                    */
   /*! \{                                                                 */
   /*! Optimize subrange idx of the index lists.
       @param idx(in)      Subrange
       @param bvol(in/out) Bounding volume for subrange
       @param level(in)    Current level in hierarchy
       @return node for subrange in hierarchy.                            */
   BVolAdapterBase* optimize (const VectorIndices& idx, 
			      BVol&                bvol,
			      unsigned             level);
   /*! Subdivide range idx of sorted index list dim at position pos.
       Sorted index lists are updated.
       @param idx(in)   Range
       @param idx1(out) Left subrange
       @param idx2(out) Right subrange
       @param dim(in)   Index list used for subdivision
       @param pos(in)   Position of subdivision                           */
   void             subdivSorted (const VectorIndices& idx,
				  VectorIndices&       idx1,
				  VectorIndices&       idx2,
				  unsigned             pos, 
				  unsigned             dim);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

   Oracle<BVol>*  m_oracle;
   unsigned m_maxLevel;
   unsigned m_currentLevel;
   unsigned m_minObjects;

   /*! indices into the array m_allNodes, sorted according to the different
       criteria specified by m_oracle.                                    */
   std::vector<std::vector<unsigned> >  m_nodes;
   const std::vector<Adapter*>*         m_allNodes;
   std::vector<GroupType*>*             m_store;
};

template <class GROUP>
inline unsigned Optimizer<GROUP>::getCurrentDepth () const
{
   return m_currentLevel+1;
}
template <class GROUP>
inline void Optimizer<GROUP>::setDataStore (std::vector<GroupType*>* store)
{
   m_store = store;
}
template <class GROUP>
inline std::vector<typename Optimizer<GROUP>::GroupType*>& 
Optimizer<GROUP>::getDataStore () const
{
   assert(m_store != NULL);
   return *m_store;
}
template <class GROUP>
inline const std::vector<Adapter*>& 
Optimizer<GROUP>::getLeafNodes () const
{
   assert(m_allNodes != NULL);
   return *m_allNodes;
}

END_GENVIS_NAMESPACE
#endif
