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
//   $Date: 2003/09/11 16:20:30 $
//                                                                            
//=============================================================================

#ifndef OSGGVPRUNINGTRAVERSER_H
#define OSGGVPRUNINGTRAVERSER_H

#include "OSGGVBase.h"
#include "OSGGVInput.h"
#include "OSGGVAllTraverser.h"

BEGIN_GENVIS_NAMESPACE

// Histogram and offset arrays as local variables
#define GV_RADIX_LOCAL

/*! \brief Radix sorting.
 */
class  OSG_GENVISLIB_DLLMAPPING RadixSort
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   RadixSort();
   ~RadixSort();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Sorting methods.                                             */
   /*! \{                                                                 */   
   /*! Main sort routine.
       This one is for integer values. After the call, mRanks contains a list 
       of indices in sorted order, i.e. in the order you may process your data.
       \param		input			[in] a list of integer values to sort
       \param		nb				[in] number of values to sort, must be < 2^31
       \param		signedvalues	[in] true to handle negative values, false if you know your input buffer only contains positive values
       \return		Self-Reference
   */
   RadixSort& Sort(const u32* input, u32 nb, bool signedvalues=true);
   /*! Main sort routine.
       This one is for floating-point values. After the call, mRanks contains a list 
       of indices in sorted order, i.e. in the order you may process your data.
       \param		input			[in] a list of floating-point values to sort
       \param		nb				[in] number of values to sort, must be < 2^31
       \return		Self-Reference
       \warning	only sorts IEEE floating-point values
   */
   RadixSort& Sort(const float* input, u32 nb);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Access to results.                                           */
   /*! \{                                                                 */   
   /*! List of indices in sorted order, 
       i.e. in the order you may further process your data
   */
   inline u32* GetRanks () const;
   /*! mIndices2 gets trashed on calling the sort routine, 
       but otherwise you can recycle it the way you want.
    */
   inline u32* GetRecyclable () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Usage statistics.                                            */
   /*! \{                                                                 */   
   //! Returns the total number of calls to the radix sorter.
   inline unsigned GetNbTotalCalls () const;
   //! Returns the number of premature exits due to temporal coherence.
   inline unsigned GetNbHits () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   /*---------------------------------------------------------------------*/
   /*! \name Internal methods.                                            */
   /*! \{                                                                 */    
   void	CheckResize (unsigned nb);
   /*! Resizes the inner lists.
       \param  nb[in] new size (number of dwords)
       \return true if successful.
   */
   bool	Resize (unsigned nb);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

#ifndef GV_RADIX_LOCAL
   //!< Counters for each byte
   u32*			mHistogram;
   //!< Offsets (nearly a cumulative distribution function)
   u32*			mOffset;
#endif
   //!< Current size of the indices list
   unsigned		mCurrentSize;
   //!< Two lists, swapped each pass
   u32*			mRanks;
   u32*			mRanks2;
   // Stats
   unsigned		mTotalCalls;
   unsigned		mNbHits;
};

inline u32* RadixSort::GetRanks () const	
{ 
   return mRanks;		
}
inline u32* RadixSort::GetRecyclable () const	
{ 
   return mRanks2;		
}
inline unsigned RadixSort::GetNbTotalCalls () const	
{ 
   return mTotalCalls;	
}
inline unsigned RadixSort::GetNbHits () const	
{ 
   return mNbHits;		
}


/*! \brief Traverser for all-pairs collision detection. 
    This class prunes the list of all pairs
    (sorts the projection intervals of all participants with Radix sort) 
    and calls the double traverser for each pair of the pruning list.
 */
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING PruningTraverser : public AllTraverserBase<BasicTraits>
{
public:
   typedef AllTraverserBase<BasicTraits>       Inherited;
   typedef typename BasicTraits::TransformType TransformType;
   typedef std::pair<unsigned, unsigned>       PairsPair;
   typedef std::vector<PairsPair>              PairsContainer;
   typedef std::vector<Real>                   PosContainer;

   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline PruningTraverser ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Apply.                                                       */
   /*! \{                                                                 */   
   virtual bool apply   (const OSG::NodePtr& node0, const OSG::NodePtr& node1);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   /*---------------------------------------------------------------------*/
   /*! \name Compute projection interval.                                 */
   /*! \{                                                                 */   
   void calcInterval (CacheData& data, 
		      BVolAdapterBase* adapter,
		      unsigned         axis, 
		      Real&            minBound, 
		      Real&            maxBound);
   void calcInterval (CacheData& data, 
		      BVolAdapterBase*   adapter,
		      const VectorClass& axisDir,
		      Real&              minBound, 
		      Real&              maxBound);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Pruning.                                                     */
   /*! \{                                                                 */   
   bool bipartitePruning (CacheData& data0, 
			  const typename CacheData::AdapterContainer& list0, 
			  CacheData& data1, 
			  const typename CacheData::AdapterContainer& list1, 
			  unsigned           axis,
			  const VectorClass& axisDir,
			  PairsContainer&    result);
   bool completePruning  (CacheData& data0,
			  const typename CacheData::AdapterContainer& list0, 
			  unsigned           axis,
			  const VectorClass& axisDir,
			  PairsContainer&    result);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

   PairsContainer m_pairs;
   PosContainer   m_MinPosList0;
   PosContainer   m_MinPosList1;
   PosContainer   m_MaxPosList0;
   PosContainer   m_MaxPosList1;
};

template class OSG_GENVISLIB_DLLMAPPING PruningTraverser<OpenSGTraits>;

template <class BasicTraits>
inline PruningTraverser<BasicTraits>::PruningTraverser ()
  : Inherited ()
{
}

END_GENVIS_NAMESPACE

#endif



