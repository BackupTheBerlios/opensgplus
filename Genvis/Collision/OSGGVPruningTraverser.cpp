//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.1 $
//   $Date: 2003/09/11 16:20:30 $
//                                                                            
//-----------------------------------------------------------------------------
// Radix sorting and pruning based on code 
// by Pierre Terdiman, www.codercorner.com
//=============================================================================

#include "OSGGVPruningTraverser.h"
#include "OSGGVBVolCollision.h"
#include "OSGGVProfiler.h"

USING_GENVIS_NAMESPACE
OSG_USING_NAMESPACE

#define INVALIDATE_RANKS	mCurrentSize|=0x80000000
#define VALIDATE_RANKS		mCurrentSize&=0x7fffffff
#define CURRENT_SIZE		(mCurrentSize&0x7fffffff)
#define INVALID_RANKS		(mCurrentSize&0x80000000)

#define CHECK_RESIZE(n)																		\
	if(n!=mPreviousSize)																	\
	{																						\
				if(n>mCurrentSize)	Resize(n);												\
		else						ResetRanks();											\
		mPreviousSize = n;																	\
	}

#define CREATE_HISTOGRAMS(type, buffer)														\
	/* Clear counters/histograms */															\
	ZeroMemory(mHistogram, 256*4*sizeof(u32));											\
																							\
	/* Prepare to count */																	\
	u8* p = (u8*)input;																\
	u8* pe = &p[nb*4];																	\
	u32* h0= &mHistogram[0];		/* Histogram for first pass (LSB)	*/					\
	u32* h1= &mHistogram[256];	/* Histogram for second pass		*/					\
	u32* h2= &mHistogram[512];	/* Histogram for third pass			*/					\
	u32* h3= &mHistogram[768];	/* Histogram for last pass (MSB)	*/					\
																							\
	bool AlreadySorted = true;	/* Optimism... */											\
																							\
	if(INVALID_RANKS)																		\
	{																						\
		/* Prepare for temporal coherence */												\
		type* Running = (type*)buffer;														\
		type PrevVal = *Running;															\
																							\
		while(p!=pe)																		\
		{																					\
			/* Read input buffer in previous sorted order */								\
			type Val = *Running++;;															\
			/* Check whether already sorted or not */										\
			if(Val<PrevVal)	{ AlreadySorted = false; break; } /* Early out */				\
			/* Update for next iteration */													\
			PrevVal = Val;																	\
																							\
			/* Create histograms */															\
			h0[*p++]++;	h1[*p++]++;	h2[*p++]++;	h3[*p++]++;									\
		}																					\
																							\
		/* If all input values are already sorted, we just have to return and leave the */	\
		/* previous list unchanged. That way the routine may take advantage of temporal */	\
		/* coherence, for example when used to sort transparent faces.					*/	\
		if(AlreadySorted)																	\
		{																					\
			mNbHits++;																		\
			for(u32 i=0;i<nb;i++)	mRanks[i] = i;										\
			return *this;																	\
		}																					\
	}																						\
	else																					\
	{																						\
		/* Prepare for temporal coherence */												\
		u32* Indices = mRanks;															\
		type PrevVal = (type)buffer[*Indices];												\
																							\
		while(p!=pe)																		\
		{																					\
			/* Read input buffer in previous sorted order */								\
			type Val = (type)buffer[*Indices++];											\
			/* Check whether already sorted or not */										\
			if(Val<PrevVal)	{ AlreadySorted = false; break; } /* Early out */				\
			/* Update for next iteration */													\
			PrevVal = Val;																	\
																							\
			/* Create histograms */															\
			h0[*p++]++;	h1[*p++]++;	h2[*p++]++;	h3[*p++]++;									\
		}																					\
																							\
		/* If all input values are already sorted, we just have to return and leave the */	\
		/* previous list unchanged. That way the routine may take advantage of temporal */	\
		/* coherence, for example when used to sort transparent faces.					*/	\
		if(AlreadySorted)	{ mNbHits++; return *this;	}									\
	}																						\
																							\
	/* Else there has been an early out and we must finish computing the histograms */		\
	while(p!=pe)																			\
	{																						\
		/* Create histograms without the previous overhead */								\
		h0[*p++]++;	h1[*p++]++;	h2[*p++]++;	h3[*p++]++;										\
	}

#define CHECK_PASS_VALIDITY(pass)															\
	/* Shortcut to current counters */														\
	u32* CurCount = &mHistogram[pass<<8];												\
																							\
	/* Reset flag. The sorting pass is supposed to be performed. (default) */				\
	bool PerformPass = true;																\
																							\
	/* Check pass validity */																\
																							\
	/* If all values have the same byte, sorting is useless. */								\
	/* It may happen when sorting bytes or words instead of dwords. */						\
	/* This routine actually sorts words faster than dwords, and bytes */					\
	/* faster than words. Standard running time (O(4*n))is reduced to O(2*n) */				\
	/* for words and O(n) for bytes. Running time for floats depends on actual values... */	\
																							\
	/* Get first byte */																	\
	u8 UniqueVal = *(((u8*)input)+pass);												\
																							\
	/* Check that byte's counter */															\
	if(CurCount[UniqueVal]==nb)	PerformPass=false;


RadixSort::RadixSort() 
  : mRanks(NULL), mRanks2(NULL), mCurrentSize(0), mTotalCalls(0), mNbHits(0)
{
#ifndef GV_RADIX_LOCAL
	// Allocate input-independent ram
	mHistogram	= new u32[256*4];
	mOffset		= new u32[256];
#endif
	// Initialize indices
	INVALIDATE_RANKS;
}
RadixSort::~RadixSort()
{
   // Release everything
#ifndef GV_RADIX_LOCAL
   delete[] mOffset;
   delete[] mHistogram;
#endif
   delete[] mRanks2;
   delete[] mRanks;
}

bool RadixSort::Resize(unsigned nb)
{
   // Free previously used ram
   delete[] mRanks2;
   delete[] mRanks;

   // Get some fresh one
   mRanks  = new u32[nb];
   mRanks2 = new u32[nb];

   return true;
}
void RadixSort::CheckResize(unsigned nb)
{
   unsigned CurSize = CURRENT_SIZE;
   if (nb!=CurSize) {
      if (nb>CurSize)	Resize(nb);
      mCurrentSize = nb;
      INVALIDATE_RANKS;
   }
}

RadixSort& RadixSort::Sort(const u32* input, u32 nb, bool signedvalues)
{
   // Checkings
  if (!input || !nb || nb&0x80000000)	return *this;

   // Stats
   mTotalCalls++;

   // Resize lists if needed
   CheckResize(nb);

#ifdef GV_RADIX_LOCAL
   // Allocate histograms & offsets on the stack
   u32 mHistogram[256*4];
   u32 mOffset[256];
#endif

   // Create histograms (counters). Counters for all passes are created in one run.
   // Pros:	read input buffer once instead of four times
   // Cons:	mHistogram is 4Kb instead of 1Kb
   // We must take care of signed/unsigned values for temporal coherence.... I just
   // have 2 code paths even if just a single opcode changes. Self-modifying code, someone?
   if (!signedvalues)	{ CREATE_HISTOGRAMS(u32, input);	}
   else				{ CREATE_HISTOGRAMS(i32, input);	}

   // Compute #negative values involved if needed
   u32 NbNegativeValues = 0;
   if (signedvalues) {
     // An efficient way to compute the number of negatives values we'll have to deal with is simply to sum the 128
     // last values of the last histogram. Last histogram because that's the one for the Most Significant Byte,
     // responsible for the sign. 128 last values because the 128 first ones are related to positive numbers.
     u32* h3= &mHistogram[768];
     for(u32 i=128;i<256;i++)	NbNegativeValues += h3[i];	// 768 for last histogram, 128 for negative part
   }

   // Radix sort, j is the pass number (0=LSB, 3=MSB)
   for (u32 j=0;j<4;j++) {
      CHECK_PASS_VALIDITY(j);

      // Sometimes the fourth (negative) pass is skipped because all numbers are negative and the MSB is 0xFF (for example). This is
      // not a problem, numbers are correctly sorted anyway.
      if (PerformPass) {
	 u32 i;
	 // Should we care about negative values?
	 if (j!=3 || !signedvalues) {
	    // Here we deal with positive values only
	    // Create offsets
	    mOffset[0] = 0;
	    for(i=1;i<256;i++)		mOffset[i] = mOffset[i-1] + CurCount[i-1];
	 } else {
	    // This is a special case to correctly handle negative integers. They're sorted in the right order but at the wrong place.
	    // Create biased offsets, in order for negative numbers to be sorted as well
	    mOffset[0] = NbNegativeValues;												// First positive number takes place after the negative ones
	    for(i=1;i<128;i++)		mOffset[i] = mOffset[i-1] + CurCount[i-1];	// 1 to 128 for positive numbers
	    // Fixing the wrong place for negative values
	    mOffset[128] = 0;
	    for(i=129;i<256;i++)			mOffset[i] = mOffset[i-1] + CurCount[i-1];
	 }

	 // Perform Radix Sort
	 u8* InputBytes	= (u8*)input;
	 InputBytes += j;
	 if (INVALID_RANKS) {
	    for (i=0;i<nb;i++)	mRanks2[mOffset[InputBytes[i<<2]]++] = i;
	    VALIDATE_RANKS;
	 } else {
	    u32* Indices		= mRanks;
	    u32* IndicesEnd	= &mRanks[nb];
	    while (Indices!=IndicesEnd) {
	       u32 id = *Indices++;
	       mRanks2[mOffset[InputBytes[id<<2]]++] = id;
	    }
	 }

	 // Swap pointers for next pass. Valid indices - the most recent ones - are in mRanks after the swap.
	 u32* Tmp	= mRanks;	mRanks = mRanks2; mRanks2 = Tmp;
      }
   }
   return *this;
}

RadixSort& RadixSort::Sort(const float* input2, u32 nb)
{
   // Checkings
   if(!input2 || !nb || nb&0x80000000)	return *this;

   // Stats
   mTotalCalls++;

   u32* input = (u32*)input2;
   
   // Resize lists if needed
   CheckResize(nb);

#ifdef GV_RADIX_LOCAL
   // Allocate histograms & offsets on the stack
   u32 mHistogram[256*4];
   u32 mOffset[256];
#endif

   // Create histograms (counters). Counters for all passes are created in one run.
   // Pros:	read input buffer once instead of four times
   // Cons:	mHistogram is 4Kb instead of 1Kb
   // Floating-point values are always supposed to be signed values, so there's only one code path there.
   // Please note the floating point comparison needed for temporal coherence! Although the resulting asm code
   // is dreadful, this is surprisingly not such a performance hit - well, I suppose that's a big one on first
   // generation Pentiums....We can't make comparison on integer representations because, as Chris said, it just
   // wouldn't work with mixed positive/negative values....
   { CREATE_HISTOGRAMS(float, input2); }

   // Compute #negative values involved if needed
   u32 NbNegativeValues = 0;
   // An efficient way to compute the number of negatives values we'll have to deal with is simply to sum the 128
   // last values of the last histogram. Last histogram because that's the one for the Most Significant Byte,
   // responsible for the sign. 128 last values because the 128 first ones are related to positive numbers.
   u32* h3= &mHistogram[768];
   for(u32 i=128;i<256;i++)	NbNegativeValues += h3[i];	// 768 for last histogram, 128 for negative part

   // Radix sort, j is the pass number (0=LSB, 3=MSB)
   u32 i, j;
   for (j=0;j<4;j++) {
      // Should we care about negative values?
      if (j!=3) {
	 // Here we deal with positive values only
	 CHECK_PASS_VALIDITY(j);

	 if (PerformPass) {
	    // Create offsets
	    mOffset[0] = 0;
	    for(i=1;i<256;i++)		mOffset[i] = mOffset[i-1] + CurCount[i-1];
	    // Perform Radix Sort
	    u8* InputBytes = (u8*)input;
	    InputBytes += j;
	    if (INVALID_RANKS) {
	       for(i=0;i<nb;i++)	mRanks2[mOffset[InputBytes[i<<2]]++] = i;
	       VALIDATE_RANKS;
	    } else {
	       u32* Indices		= mRanks;
	       u32* IndicesEnd	= &mRanks[nb];
	       while (Indices!=IndicesEnd) {
		  u32 id = *Indices++;
		  mRanks2[mOffset[InputBytes[id<<2]]++] = id;
	       }
	    }
	    // Swap pointers for next pass. Valid indices - the most recent ones - are in mRanks after the swap.
	    u32* Tmp	= mRanks;	mRanks = mRanks2; mRanks2 = Tmp;
	 }
      } else {
	 // This is a special case to correctly handle negative values
	 CHECK_PASS_VALIDITY(j);
	 
	 if (PerformPass) {
	    // Create biased offsets, in order for negative numbers to be sorted as well
	    mOffset[0] = NbNegativeValues;												// First positive number takes place after the negative ones
	    for(i=1;i<128;i++)		mOffset[i] = mOffset[i-1] + CurCount[i-1];	// 1 to 128 for positive numbers
	    // We must reverse the sorting order for negative numbers!
	    mOffset[255] = 0;
	    for(i=0;i<127;i++)		mOffset[254-i] = mOffset[255-i] + CurCount[255-i];	// Fixing the wrong order for negative values
	    for(i=128;i<256;i++)	mOffset[i] += CurCount[i];							// Fixing the wrong place for negative values
	    // Perform Radix Sort
	    if (INVALID_RANKS) {
	       for (i=0;i<nb;i++) {
		  u32 Radix = input[i]>>24;							// Radix byte, same as above. AND is useless here (unsigned).
		  // ### cmp to be killed. Not good. Later.
		  if(Radix<128)		mRanks2[mOffset[Radix]++] = i;		// Number is positive, same as above
		  else				mRanks2[--mOffset[Radix]] = i;		// Number is negative, flip the sorting order
	       }
	       VALIDATE_RANKS;
	    } else {
	       for (i=0;i<nb;i++) {
		  u32 Radix = input[mRanks[i]]>>24;							// Radix byte, same as above. AND is useless here (unsigned).
		  // ### cmp to be killed. Not good. Later.
		  if(Radix<128)		mRanks2[mOffset[Radix]++] = mRanks[i];		// Number is positive, same as above
		  else				mRanks2[--mOffset[Radix]] = mRanks[i];		// Number is negative, flip the sorting order
	       }
	    }
	    // Swap pointers for next pass. Valid indices - the most recent ones - are in mRanks after the swap.
	    u32* Tmp	= mRanks;	mRanks = mRanks2; mRanks2 = Tmp;
	 } else	{
	    // The pass is useless, yet we still have to reverse the order of current list if all values are negative.
	    if (UniqueVal>=128) {
	       if (INVALID_RANKS) {
		  // ###Possible?
		  for(i=0;i<nb;i++)	mRanks2[i] = nb-i-1;
		  VALIDATE_RANKS;
	       } else {
		  for(i=0;i<nb;i++)	mRanks2[i] = mRanks[nb-i-1];
	       }
	       
	       // Swap pointers for next pass. Valid indices - the most recent ones - are in mRanks after the swap.
	       u32* Tmp	= mRanks;	mRanks = mRanks2; mRanks2 = Tmp;
	    }
	 }
      }
   }
   return *this;
}


template <class BasicTraits>
bool PruningTraverser<BasicTraits>::completePruning
(CacheData& data0, const typename CacheData::AdapterContainer& list0, 
 unsigned           axis,
 const VectorClass& axisDir,
 PairsContainer&    result)
{
   // Checks
   if (list0.size() == 0) {
      return false;
   }

   // Allocate some temporary data
   m_MinPosList0.resize(list0.size());
   m_MaxPosList0.resize(list0.size());

   // 1) Build main list using the primary axis
   unsigned i;
   if (axis < 3) {
     for (i=0; i<list0.size(); ++i) 
       calcInterval(data0,
		    static_cast<BVolAdapterBase*>(list0[i]), axis, 
		    m_MinPosList0[i], m_MaxPosList0[i]);
   } else {
     for (i=0; i<list0.size(); ++i) 
       calcInterval(data0, 
		    static_cast<BVolAdapterBase*>(list0[i]), axisDir, 
		    m_MinPosList0[i], m_MaxPosList0[i]);
   }

   // 2) Sort the list
   // CF to be changed to member for coherence
   static RadixSort RS0;
   u32* Sorted0 = RS0.Sort(m_MinPosList0.begin(), list0.size()).GetRanks();

   // 3) Prune the list
   const u32* const LastSorted = &Sorted0[list0.size()];
   const u32* RunningAddress = Sorted0;
   u32 Index0, Index1;
   while (RunningAddress<LastSorted && Sorted0<LastSorted) {
      Index0 = *Sorted0++;
      
      while (RunningAddress<LastSorted 
	     && m_MinPosList0[*RunningAddress++]<m_MinPosList0[Index0]);

      const u32* RunningAddress2 = RunningAddress;

      while (RunningAddress2<LastSorted 
	     && m_MinPosList0[Index1 = *RunningAddress2++]<=m_MaxPosList0[Index0]) {
	if (Index0!=Index1) {
	   result.push_back(PairsPair(Index0, Index1));
	}
      }
   }
   return true;
}

template <class BasicTraits>
bool PruningTraverser<BasicTraits>::bipartitePruning
(CacheData& data0, const typename CacheData::AdapterContainer& list0, 
 CacheData& data1, const typename CacheData::AdapterContainer& list1, 
 unsigned           axis,
 const VectorClass& axisDir,
 PairsContainer&    result)
{
   // Checkings
  if (list0.size() == 0 || list1.size() == 0) {
     return false;
  }

  // Allocate some temporary data
  m_MinPosList0.resize(list0.size());
  m_MaxPosList0.resize(list0.size());
  m_MinPosList1.resize(list1.size());
  m_MaxPosList1.resize(list1.size());
  
  // 1) Build main lists using the primary axis
  unsigned i;
  if (axis < 3) {
    for (i=0; i<list0.size(); ++i) 
      calcInterval(data0, static_cast<BVolAdapterBase*>(list0[i]), axis, 
		   m_MinPosList0[i], m_MaxPosList0[i]);
    for (i=0; i<list1.size(); ++i) 
      calcInterval(data1, static_cast<BVolAdapterBase*>(list1[i]), axis, 
		   m_MinPosList1[i], m_MaxPosList1[i]);
  } else {
    for (i=0; i<list0.size(); ++i) 
      calcInterval(data0, static_cast<BVolAdapterBase*>(list0[i]), axisDir, 
		   m_MinPosList0[i], m_MaxPosList0[i]);
    for (i=0; i<list1.size(); ++i) 
      calcInterval(data1, static_cast<BVolAdapterBase*>(list1[i]), axisDir, 
		   m_MinPosList1[i], m_MaxPosList1[i]);
  }
  
  // 2) Sort the lists
  static RadixSort RS0, RS1;	// Static for coherence.
  u32* Sorted0 = RS0.Sort(m_MinPosList0.begin(), list0.size()).GetRanks();
  u32* Sorted1 = RS1.Sort(m_MinPosList1.begin(), list1.size()).GetRanks();
  
  // 3) Prune the lists
  u32 Index0, Index1;
  
  const u32* const LastSorted0 = &Sorted0[list0.size()];
  const u32* const LastSorted1 = &Sorted1[list1.size()];
  const u32* RunningAddress0 = Sorted0;
  const u32* RunningAddress1 = Sorted1;

  while (RunningAddress1<LastSorted1 && Sorted0<LastSorted0) {
    Index0 = *Sorted0++;
    
    while (RunningAddress1<LastSorted1 
	   && m_MinPosList1[*RunningAddress1]<m_MinPosList0[Index0]) {
      RunningAddress1++;
    }
    
    const u32* RunningAddress2_1 = RunningAddress1;
    
    while (RunningAddress2_1<LastSorted1 
	   && m_MinPosList1[Index1 = *RunningAddress2_1++]<=m_MaxPosList0[Index0]) {
      result.push_back(PairsPair(Index0, Index1));
    }
  }
  
  ////
  while (RunningAddress0<LastSorted0 && Sorted1<LastSorted1) {
    Index0 = *Sorted1++;
    
    while(RunningAddress0<LastSorted0 
	  && m_MinPosList0[*RunningAddress0]<=m_MinPosList1[Index0]) {
      RunningAddress0++;
    }
    
    const u32* RunningAddress2_0 = RunningAddress0;
    
    while(RunningAddress2_0<LastSorted0 
	  && m_MinPosList0[Index1 = *RunningAddress2_0++]<=m_MaxPosList1[Index0]) {
      result.push_back(PairsPair(Index1, Index0));
    }
  } 
  return true;
}

template <class BasicTraits>
void PruningTraverser<BasicTraits>::calcInterval (CacheData& data, 
						  BVolAdapterBase*   adapter,
						  const VectorClass& axisDir,
						  Real&              minBound, 
						  Real&              maxBound)
{
   const BoundingVolume<Real>& bvol = adapter->getBoundingVolume();
   PointClass center(bvol.getCenter());
   // get cache data for adapter
   CacheData&    d = getCacheData(data, adapter);
   TransformType m = d.getToWorldMatrix(); m.mult(center);
#if 0
   Vec3f dir0(m[0][0], m[0][1], m[0][2]); 
   Vec3f dir1(m[1][0], m[1][1], m[1][2]); 
   Vec3f dir2(m[2][0], m[2][1], m[2][2]); 
   Real proj0 = axisDir.dot(dir0);
   Real proj1 = axisDir.dot(dir1);
   Real proj2 = axisDir.dot(dir2);
   Real diam = bvol.difference(0) * stdAbs(proj0)
     + bvol.difference(1) * stdAbs(proj1)
     + bvol.difference(2) * stdAbs(proj2);
   
   minBound = maxBound = axisDir.dot(center);
   minBound -= diam; 
   maxBound += diam; 
#else
   Real diam = bvol.getDiameter()/m.det3();   
   minBound = maxBound = axisDir.dot(center);
   minBound -= diam; 
   maxBound += diam; 
#endif
}
template <class BasicTraits>
void PruningTraverser<BasicTraits>::calcInterval (CacheData& data, 
						  BVolAdapterBase*   adapter,
						  unsigned           axis, 
						  Real&              minBound, 
						  Real&              maxBound)
{
   const BoundingVolume<Real>& bvol = adapter->getBoundingVolume();
   PointClass center(bvol.getCenter());
   // get cache data for adapter
   CacheData&    d = getCacheData(data, adapter);
   TransformType m = d.getToWorldMatrix(); m.mult(center);
   assert(axis < 3);
#if 0
   Real diam = bvol.difference(0) * stdAbs(m[0][axis])
     + bvol.difference(1) * stdAbs(m[1][axis])
     + bvol.difference(2) * stdAbs(m[2][axis]);
   
   minBound = maxBound = center[axis];
   minBound -= diam; 
   maxBound += diam; 
#else
   Real diam = bvol.getDiameter()/m.det3();   
   minBound = maxBound = center[axis];
   minBound -= diam; 
   maxBound += diam; 
#endif
}

template <class BasicTraits>
bool PruningTraverser<BasicTraits>::apply 
(const OSG::NodePtr& node0, const OSG::NodePtr& node1)
{
   CacheData& data0 = getCache()[node0];
   CacheData& data1 = getCache()[node1];

   Profiler::the().StartProfile("PruningTraverser.Init");
   getData().Init();
   // clearing necessary to keep coherency?
   m_pairs.clear();
   const typename CacheData::AdapterContainer& list0 = 
     data0.getAdapter(BVolAdapterBase::getAdapterId());
   const typename CacheData::AdapterContainer& list1 = 
     data1.getAdapter(BVolAdapterBase::getAdapterId());
   unsigned maxNumPairs = list0.size()*list1.size();
   Profiler::the().EndProfile("PruningTraverser.Init");

   Profiler::the().StartProfile("PruningTraverser.Pruning");
   Profiler::the().StartProfile("PruningTraverser.DetermineAxis");
   static int         axis = 3;
   static VectorClass axisDir(1.0f, 0.0f, 0.0f);
   static unsigned    minNumPairs = maxNumPairs;
   static int         minAxis = -1;
   static unsigned    refresh = true;
   static unsigned    axisRefresh = 100;
   // try all three axes 2, 1, 0 and find minimum number of pairs
   if (refresh && axis > 0) {
      --axis; 
   } else {
      refresh = false;
      axis = minAxis;
   }
   Profiler::the().EndProfile("PruningTraverser.DetermineAxis");
   if (node0 == node1) {
      Profiler::the().StartProfile("PruningTraverser.CompletePruning");
      completePruning(data0, list0, axis, axisDir, m_pairs);
      Profiler::the().EndProfile("PruningTraverser.CompletePruning");
   } else {
      Profiler::the().StartProfile("PruningTraverser.BipartitePruning");
      bipartitePruning(data0, list0, data1, list1, axis, axisDir, m_pairs);
      Profiler::the().EndProfile("PruningTraverser.BipartitePruning");
   }
   Profiler::the().StartProfile("PruningTraverser.DetermineAxis");
   if (--axisRefresh == 0) {
      refresh     = true;
      axisRefresh = 100;
      minNumPairs = maxNumPairs;
      minAxis = -1;
      axis    = 3;
   } else if (refresh && minNumPairs > m_pairs.size()) {
      minNumPairs = m_pairs.size();
      minAxis     = axis;
   }
   Profiler::the().EndProfile("PruningTraverser.DetermineAxis");
   Profiler::the().EndProfile("PruningTraverser.Pruning");

   Profiler::the().StartProfile("PruningTraverser.Loop");
   bool result = false;
   for (PairsContainer::const_iterator 
	  it=m_pairs.begin(); it!=m_pairs.end(); ++it) {
      Profiler::the().StartProfile("PruningTraverser.Prepare");
      // get adapter and transformation of pairs
      Profiler::the().StartProfile("PruningTraverser.GetCacheData");
      BVolAdapterBase* first = 
	static_cast<BVolAdapterBase*>(list0[it->first]);
      CacheData&     d0 = getCacheData(data0, first);
      BVolAdapterBase* second = 
	static_cast<BVolAdapterBase*>(list1[it->second]);
      CacheData&     d1 = getCacheData(data1, second);
      Profiler::the().EndProfile("PruningTraverser.GetCacheData");
      Profiler::the().StartProfile("PruningTraverser.WorldMatrix");
      TransformType m0 = d0.getToWorldMatrix();
      TransformType m1 = d1.getToWorldMatrix();
      Profiler::the().EndProfile("PruningTraverser.WorldMatrix");
      Profiler::the().EndProfile("PruningTraverser.Prepare");

      Profiler::the().StartProfile("PruningTraverser.DoubleTraverser");
      bool thisResult = getDoubleTraverser().apply(d0, first,  m0, 
						   d1, second, m1);
      Profiler::the().EndProfile("PruningTraverser.DoubleTraverser");
      result |= thisResult;
   }
   Profiler::the().EndProfile("PruningTraverser.Loop");
   return result;
}
