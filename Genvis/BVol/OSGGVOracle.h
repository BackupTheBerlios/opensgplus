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
//   $Revision: 1.4 $
//   $Date: 2004/12/22 18:45:01 $
//                                                                            
//=============================================================================

#ifndef OSGGVORACLE_H
#define OSGGVORACLE_H

#include "OSGGVBase.h"
#include "OSGGVSingleton.h"
#include "OSGGVIndices.h"
#include "OSGGVBVolAdapterBase.h"
#include "OSGGVBVolFunctions.h"

BEGIN_GENVIS_NAMESPACE

/*! Interface for partitioning adapter vectors into two groups.
*/
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING Oracle
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline Oracle ();
   virtual inline ~Oracle ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Description.                                                 */
   /*! \{                                                                 */
   virtual const char* getDescription () const = 0;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Partitioning.                                                */
   /*! \{                                                                 */
   /*! Number of index lists, the optimizer creates for the adapter list. */
   virtual u32 getDimension () const = 0;
   /*! Sort index list for dimension d. 
       @param index(out)   Index list for dimension d.
       @param allNodes(in) Adapter list from cache.
       @param d(in)        Dimension to be processed. */
   virtual void sortIndexList (std::vector<u32>&       index,
			       const std::vector<Adapter*>& allNodes,
			       u32                     d) = 0;
   /*! Choose splitting of a subrange of the adapter list.
       @param index(in)    Index lists.
       @param allNodes(in) Adapter list from cache. 
       @param range(in)    Subrange of the index lists. 
       @param bvol(in)     Bounding volume for the subrange of the adapter list.
       @param d(out)       Dimension to be splitted.
       @param i(out)       Splitting index.  
       @return true iff splitting should be performed. */  
   virtual bool doSplitting (std::vector< std::vector<u32> >& index,
			     const std::vector<Adapter*>&          allNodes,
			     const VectorIndices&                  range,
			     const BoundingVolume<Real>&           bvol,
			     u32&                             d, 
			     u32&                             i) = 0;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Registration.                                                */
   /*! \{                                                                 */
   /*! Oracle classes are always singletons. The base class Oracle<BVOL> keeps a
       list of all singletons. With registerOracle the singleton is registered with
       this list. */
   static void                              registerOracle (Oracle<BVOL>* instance);
   /*! Get const list of all registered oracles. */
   static const std::vector<Oracle<BVOL>*>& getOracles ();
   /*! Get singleton with identifier id. 
       @return Returns NULL if not existing. */
   static Oracle<BVOL>*                     getOracle (u32 id,
						       bool nullFail=false);
   /*! Get singleton with name desc. 
       @return Returns NULL if not existing. */
   static Oracle<BVOL>*                     getOracle (const char* desc, 
						       bool nullFail=false);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! Helper function. Calculates the bounding volume bvol by unifying the bounding volumes 
       in the given range of the index list index. */
   static void  calcBoundingVolume (std::vector< std::vector<u32> >& index,
				    const std::vector<Adapter*>&          allNodes,
				    const VectorIndices&  range,
				    BoundingVolume<Real>& bvol);

   /*! Calculates the bounding volume bvol for the given range of the index list index. 
       Default implementation here uses the static method calcBoundingVolume. */
   virtual void calcBVol (std::vector< std::vector<u32> >& index,
			  const std::vector<Adapter*>&          allNodes,
			  const VectorIndices&  range,
			  BoundingVolume<Real>& bvol);
   /*! Updates the bounding volume bvol for range so that it is a valid bounding volume 
       for subdivRange (created by subdividing in dimension dim). 
       Default implementation here uses the static method calcBoundingVolume for subdivRange. */
   virtual void updateBVol (std::vector< std::vector<u32> >& index,
			    const std::vector<Adapter*>&          allNodes,
			    const VectorIndices&  range,
			    u32 dim,
			    const VectorIndices&  subdivRange,
			    BoundingVolume<Real>& bvol);

protected:
   /*! Get list of all registered oracles. */
   static std::vector<Oracle<BVOL>*>& getOraclesInternal ();
};

template <class BVOL>
inline Oracle<BVOL>::Oracle ()
{
}
template <class BVOL>
inline Oracle<BVOL>::~Oracle ()
{
}
template <class BVOL>
inline void Oracle<BVOL>::calcBoundingVolume (std::vector< std::vector<u32> >& index,
					      const std::vector<Adapter*>&          allNodes,
					      const VectorIndices&  range,
					      BoundingVolume<Real>& bvol)
{
   bvol.setEmpty();
   for (u32 j=range.first; j<range.last; ++j) {
      bvol.unify(static_cast<BVolAdapterBase*>(allNodes[index[0][j]])->getBoundingVolume());
   }
}

#if 1
template <class BVOL>
inline void Oracle<BVOL>::calcBVol (std::vector< std::vector<u32> >& index,
				    const std::vector<Adapter*>&          allNodes,
				    const VectorIndices&  range,
				    BoundingVolume<Real>& bvol)
{
   calcBoundingVolume(index, allNodes, range, bvol);
}

template <class BVOL>
inline void Oracle<BVOL>::updateBVol (std::vector< std::vector<u32> >& index,
				      const std::vector<Adapter*>&          allNodes,
				      const VectorIndices&,
				      u32,
				      const VectorIndices&  subdivRange,
				      BoundingVolume<Real>& bvol)
{
   calcBoundingVolume(index, allNodes, subdivRange, bvol);
}
#else
template <class BVOL>
inline void Oracle<BVOL>::updateBVol (std::vector< std::vector<u32> >& index,
				      const std::vector<Adapter*>&          allNodes,
				      const VectorIndices&,
				      u32 d,
				      const VectorIndices&  subdivRange,
				      BoundingVolume<Real>& bvol)
{
  const BVOL& bv0 = static_cast<BVolAdapter<BVOL>*>(allNodes[index[d][subdivRange.first]])->getBVol();
  const BVOL& bv1 = static_cast<BVolAdapter<BVOL>*>(allNodes[index[d][subdivRange.last-1]])->getBVol();
  bvol.minVector()[d] = bv0.minVector()[d];
  bvol.maxVector()[d] = bv1.maxVector()[d];
}
template <class BVOL>
inline void Oracle<BVOL>::calcBVol (std::vector< std::vector<u32> >& index,
				    const std::vector<Adapter*>&          allNodes,
				    const VectorIndices&  range,
				    BoundingVolume<Real>& bvol)
{
   bvol.setEmpty();
   BVOL& dop = static_cast<BVOL&>(bvol);
   for (u32 j=range.first; j<range.last; ++j) {
      unify(dop, static_cast<BVolAdapter<BVOL>*>(allNodes[index[0][j]])->getBVol());
   }
}
#endif

/*! Base for partitioning adapter vector into two groups
    using sorted lists of projections to the main axes.
*/
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING MainAxesSorted : public Oracle<BVOL>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline MainAxesSorted ();
   virtual inline u32 getDimension () const { return 3; }
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Partitioning.                                                */
   /*! \{                                                                 */
   /*! Sort index list for dimension d. 
       @param index(out)   Index list for dimension d.
       @param allNodes(in) Adapter list from cache.
       @param d(in)        Dimension to be processed. */
   virtual void sortIndexList (std::vector<u32>&       index,
				const std::vector<Adapter*>& allNodes,
				u32                     d);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};

template <class BVOL>
inline MainAxesSorted<BVOL>::MainAxesSorted ()
{
}


/*! Base for partitioning adapter vector into two groups
    using sorted lists of projections to all axes.
*/
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING AllAxesSorted : public Oracle<BVOL>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline AllAxesSorted ();
   virtual inline u32 getDimension () const { return BVOL::Size; }
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Partitioning.                                                */
   /*! \{                                                                 */
   /*! Sort index list for dimension d. 
       @param index(out)   Index list for dimension d.
       @param allNodes(in) Adapter list from cache.
       @param d(in)        Dimension to be processed. */
   virtual void sortIndexList (std::vector<u32>&       index,
				const std::vector<Adapter*>& allNodes,
				u32                     d);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};

template <class BVOL>
inline AllAxesSorted<BVOL>::AllAxesSorted ()
{
}

END_GENVIS_NAMESPACE
#endif






