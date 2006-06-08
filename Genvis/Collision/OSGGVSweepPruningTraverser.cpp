//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.1 $
//   $Date: 2006/06/08 17:07:39 $
//                                                                            
//-----------------------------------------------------------------------------
// Radix sorting and pruning based on code 
// by Pierre Terdiman, www.codercorner.com
//=============================================================================

#include "OSGGVSweepPruningTraverser.h"
#include "OSGGVBVolCollision.h"
#include "OSGGVBVolFunctions.h"
#include "OSGGVProfiler.h"
OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

BEGIN_GENVIS_NAMESPACE

class SAPElement
{
public:
  inline					SAPElement()	
  {}
  inline					SAPElement(u32 id, SAPElement* next) : mID(id), mNext(next)	
  {}
  inline					~SAPElement()
  {}
  
  u32		mID;
  SAPElement*	mNext;
};

class SAPBox
{
public:
  SAPEndPoint*	Min[3];
  SAPEndPoint*	Max[3];
};

class SAPEndPoint
{
public:
  Real		Value;		// Min or Max value
  SAPEndPoint*	Previous;	// Previous EndPoint whose Value is smaller than ours (or NULL)
  SAPEndPoint*	Next;		// Next EndPoint whose Value is greater than ours (or NULL)
  u32		Data;		// Parent box ID *2 | MinMax flag
  
  inline	void			SetData(u32 box_id, u32 is_max, u32 is_list0)			
  { Data = (box_id<<2)|(is_max<<1)|(is_list0);	}
  inline	bool			IsList0()							const	
  { return ((Data & 1) == 0);			}
  inline	u32			List   ()							const	
  { return Data & 1;				}
  inline	bool			IsMax()								const	
  { return ((Data & 2) == 2);			}
  inline	u32			GetBoxID()							const	
  { return Data>>2;				}
  
  inline	void InsertAfter(SAPEndPoint* element)
  {
    if(this!=element && this!=element->Next) {
       // Remove
       if(Previous)	Previous->Next = Next;
       if(Next)		Next->Previous = Previous;
      
       // Insert
       Next = element->Next;
       if(Next)	Next->Previous = this;
       
       element->Next = this;
       Previous = element;
    }
  }

  inline	void InsertBefore(SAPEndPoint* element)
  {
    if(this!=element && this!=element->Previous) {
       // Remove
       if(Previous)	Previous->Next = Next;
       if(Next)		Next->Previous = Previous;

       // Insert
       Previous = element->Previous;
       element->Previous = this;
       
       Next = element;
       if(Previous)	Previous->Next = this;
    }
  }
};

SAPPairData::SAPPairData() 
  : mNbElements		(0),
    mNbUsedElements	(0),
    mElementPool	(NULL),
    mFirstFree		(NULL),
    mNbObjects		(0),
    mArray		(NULL)
{
}
SAPPairData::~SAPPairData()
{
   Release();
}

void SAPPairData::Release()
{
   mNbElements		= 0;
   mNbUsedElements	= 0;
   mNbObjects		= 0;
   delete[] mElementPool;
   delete[] mArray;
}

static inline void Sort(u32& id0, u32& id1)
{
  if (id0>id1) {
     std::swap(id0, id1);
  }
}

static inline bool checkIntersect (Real aMin[3], Real aMax[3], const SAPBox& b)
{
  if (   b.Max[0]->Value < aMin[0] || aMax[0] < b.Min[0]->Value
      || b.Max[1]->Value < aMin[1] || aMax[1] < b.Min[1]->Value
      || b.Max[2]->Value < aMin[2] || aMax[2] < b.Min[2]->Value) {
     return false;
  }
  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Initializes.
 *	\param		nb_objects	[in] 
 *	\return		true if success
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SAPPairData::Init(u32 nb_objects)
{
   // Make sure everything has been released
   Release();
   if(!nb_objects)	return false;
   
   mArray = new SAPElement*[nb_objects];
   //CHECKALLOC(mArray);
   ZeroMemory(mArray, nb_objects*sizeof(SAPElement*));
   mNbObjects = nb_objects;
   
   return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Remaps a pointer when pool gets resized.
 *	\param		element	[in/out] remapped element
 *	\param		delta	[in] offset in bytes
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void Remap(SAPElement*& element, u32 delta)
{
   if(element)	element = (SAPElement*)(u32(element) + delta);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Gets a free element in the pool.
 *	\param		id		[in] element id
 *	\param		next	[in] next element
 *	\param		remap	[out] possible remapping offset
 *	\return		the new element
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SAPElement* SAPPairData::GetFreeElem(u32 id, SAPElement* next, u32* remap)
{
   if(remap)	*remap = 0;

   SAPElement* FreeElem;
   if(mFirstFree) {
      // Recycle
      FreeElem = mFirstFree;
      mFirstFree = mFirstFree->mNext;	// First free = next free (or NULL)
   } else {
      if(mNbUsedElements==mNbElements) {
	 // Resize
	 mNbElements = mNbElements ? (mNbElements<<1) : 2;

	 SAPElement* NewElems = new SAPElement[mNbElements];

	 if(mNbUsedElements)	CopyMemory(NewElems, mElementPool, mNbUsedElements*sizeof(SAPElement));

	 // Remap everything
	 {
	   u32 Delta = u32(NewElems) - u32(mElementPool);
	   
	   for(u32 i=0;i<mNbUsedElements;i++)	Remap(NewElems[i].mNext, Delta);
	   for(u32 i=0;i<mNbObjects;i++)		Remap(mArray[i], Delta);

	   Remap(mFirstFree, Delta);
	   Remap(next, Delta);

	   if(remap)	*remap = Delta;
	 }

	 delete[] mElementPool;
	 mElementPool = NewElems;
      }

      FreeElem = &mElementPool[mNbUsedElements++];
   }

   FreeElem->mID = id;
   FreeElem->mNext = next;

   return FreeElem;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Frees an element of the pool.
 *	\param		elem	[in] element to free/recycle
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SAPPairData::FreeElem(SAPElement* elem)
{
   elem->mNext = mFirstFree;	// Next free
   mFirstFree = elem;
}

// Add a pair to the set.
void SAPPairData::AddPair(u32 id1, u32 id2)
{
   // Order the ids
   Sort(id1, id2);

   assert(id1<mNbObjects);
   if(id1>=mNbObjects)	return;

   // Select the right list from "mArray".
   SAPElement* Current = mArray[id1];

   if(!Current) {
      // Empty slot => create new element
      mArray[id1] = GetFreeElem(id2, NULL);
   } else if(Current->mID>id2) {
      // The list is not empty but all elements are greater than id2 => insert id2 in the front.
      mArray[id1]	= GetFreeElem(id2, mArray[id1]);
   } else {
      // Else find the correct location in the sorted list (ascending order) and insert id2 there.
      while(Current->mNext) {
	 if(Current->mNext->mID > id2)	break;

	 Current = Current->mNext;
      }

      if(Current->mID==id2)	return;	// The pair already exists
		
      //		Current->mNext = GetFreeElem(id2, Current->mNext);
      u32 Delta;
      SAPElement* E = GetFreeElem(id2, Current->mNext, &Delta);
      if(Delta)	Remap(Current, Delta);
      Current->mNext = E;
   }
}

// Delete a pair from the set.
void SAPPairData::RemovePair(u32 id1, u32 id2)
{
   // Order the ids.
   Sort(id1, id2);

   // Exit if the pair doesn't exist in the set
   if(id1>=mNbObjects)	return;

   // Otherwise, select the correct list.
   SAPElement* Current = mArray[id1];
   
   // If this list is empty, the pair doesn't exist.
   if(!Current) return;

   // Otherwise, if id2 is the first element, delete it.
   if(Current->mID==id2) {
      mArray[id1] = Current->mNext;
      FreeElem(Current);
   } else {
      // If id2 is not the first element, start traversing the sorted list.
      while(Current->mNext) {
	 // If we have moved too far away without hitting id2, then the pair doesn't exist
	 if(Current->mNext->mID > id2)	return;

	 // Otherwise, delete id2.
	 if(Current->mNext->mID == id2) {
	    SAPElement* Temp = Current->mNext;
	    Current->mNext = Temp->mNext;
	    FreeElem(Temp);
	    return;
	 }
	 Current = Current->mNext;
      }
   }
}

void SAPPairData::DumpPairs (PairsContainer& pairs) const
{
   for(u32 i=0;i<mNbObjects;i++) {
      SAPElement* Current = mArray[i];
      while (Current != NULL) {
	 assert(Current->mID < mNbObjects);

	 pairs.push_back(std::make_pair(i, Current->mID));
	 Current = Current->mNext;
      }
   }
}

template <class BasicTraits>
bool SweepPruningTraverser<BasicTraits>::CheckListsIntegrity()
{
   for(u32 Axis=0; Axis<3; Axis++) {
      // Find list head
      SAPEndPoint* Current = mList[Axis];
      while(Current->Previous)	Current = Current->Previous;

      u32 Nb = 0;
      
      SAPEndPoint* Previous = NULL;
      while(Current) {
	 Nb++;

	 if(Previous) {
	    assert(Previous->Value <= Current->Value);
	    if(Previous->Value > Current->Value)	return false;
	 }

	 assert(Current->Previous==Previous);
	 if(Current->Previous!=Previous)	return false;

	 Previous = Current;
	 Current = Current->Next;
      }
      
      assert(Nb==mNbObjects*2);
   }
   return true;
}

template <class BasicTraits>
bool SweepPruningTraverser<BasicTraits>::init (const typename CacheData::AdapterContainer& list0,
					       CacheData&                            data0,
					       const typename CacheData::AdapterContainer& list1,
					       CacheData&                            data1)
//(u32 nb_objects, const AABB** boxes)
{
   m_data0 = &data0;
   m_data1 = &data1;

   // 1) Create sorted lists
   mNbObjects = list0.size()+list1.size();

   SNOTICE << "SweepPruningTraverser::init: Calc intervals for " 
	   << list0.size() << "+" << list1.size() << "=" << mNbObjects 
	   << " objects" << std::endl;
   Real* MinPosList[3];
   MinPosList[0] = new Real[mNbObjects]; 
   MinPosList[1] = new Real[mNbObjects]; 
   MinPosList[2] = new Real[mNbObjects]; 
   Real* MaxPosList[3];
   MaxPosList[0] = new Real[mNbObjects]; 
   MaxPosList[1] = new Real[mNbObjects]; 
   MaxPosList[2] = new Real[mNbObjects]; 
   u32 i, ii;
   for (i=0, ii=0; i<list0.size(); ++i, ++ii) {
      calcInterval(data0, static_cast<BVolAdapterBase*>(list0[i]), 0, MinPosList[0][ii], MaxPosList[0][ii]);
      calcInterval(data0, static_cast<BVolAdapterBase*>(list0[i]), 1, MinPosList[1][ii], MaxPosList[1][ii]);
      calcInterval(data0, static_cast<BVolAdapterBase*>(list0[i]), 2, MinPosList[2][ii], MaxPosList[2][ii]);
   }
   for (i=0; i<list1.size(); ++i, ++ii) {
      calcInterval(data1, static_cast<BVolAdapterBase*>(list1[i]), 0, MinPosList[0][ii], MaxPosList[0][ii]);
      calcInterval(data1, static_cast<BVolAdapterBase*>(list1[i]), 1, MinPosList[1][ii], MaxPosList[1][ii]);
      calcInterval(data1, static_cast<BVolAdapterBase*>(list1[i]), 2, MinPosList[2][ii], MaxPosList[2][ii]);
   }

   mBoxes = new SAPBox[mNbObjects];

   Real* Data = new Real[mNbObjects*2+1];
   // necessary for termination in step 2
   Data[mNbObjects*2] = infinity;

   SNOTICE << "SweepPruningTraverser::init: Create EndPoints" << std::endl;
   static RadixSort RS;
   const u32* Sorted = NULL;
   for(u32 Axis=0; Axis<3; ++Axis) {
      mList[Axis] = new SAPEndPoint[mNbObjects*2];

      for(i=0, ii=0; i<list0.size(); ++i, ++ii) {
	//const BoundingVolume<Real>& bvol = getDoubleTraverser().getBoundingVolume(list0[i]);
	Data[ii*2+0] = MinPosList[Axis][ii];
	Data[ii*2+1] = MaxPosList[Axis][ii];
      }
      for(i=0; i<list1.size(); ++i, ++ii) {
	//const BoundingVolume<Real>& bvol = getDoubleTraverser().getBoundingVolume(list1[i]);
	Data[ii*2+0] = MinPosList[Axis][ii];
	Data[ii*2+1] = MaxPosList[Axis][ii];
      }
      // mNbObjects*2+1 instead of mNbObjects*2 necessary for termination in step 2
      Sorted = RS.Sort(Data, mNbObjects*2+1).GetRanks();
      const u32* iter = Sorted;

      SAPEndPoint* PreviousEndPoint = NULL;

      for(i=0;i<mNbObjects*2;i++) {
	u32 SortedIndex	= *iter++;
	Real SortedCoord	= Data[SortedIndex];
	u32 BoxIndex		= SortedIndex>>1;

	assert(BoxIndex<mNbObjects);
	
	SAPEndPoint* CurrentEndPoint = &mList[Axis][SortedIndex];
	CurrentEndPoint->Value		= SortedCoord;
	//			CurrentEndPoint->IsMax		= SortedIndex&1;		// ### could be implicit ?
	//			CurrentEndPoint->ID			= BoxIndex;				// ### could be implicit ?
	CurrentEndPoint->SetData(BoxIndex, SortedIndex&1, (BoxIndex<list0.size() ? 0 : 1));	// ### could be implicit ?
	CurrentEndPoint->Previous	= PreviousEndPoint;
	CurrentEndPoint->Next		= NULL;
	if(PreviousEndPoint)	PreviousEndPoint->Next = CurrentEndPoint;
	
	if(CurrentEndPoint->IsMax()) mBoxes[BoxIndex].Max[Axis] = CurrentEndPoint;
	else			     mBoxes[BoxIndex].Min[Axis] = CurrentEndPoint;
	
	PreviousEndPoint = CurrentEndPoint;
      }
   }
   
   CheckListsIntegrity();

   // 2) Quickly find starting pairs
   mPairs.Init(mNbObjects);
   {
      const u32 Axis0 = 2;
      const u32 Axis1 = 1;
      const u32 Axis2 = 0;
      // Prune the list for Axis0==2
      const u32* const LastSorted = &Sorted[mNbObjects*2];
      const u32*   RunningAddress = Sorted;
      u32 Index0, Index1;
      while (RunningAddress<LastSorted && Sorted<LastSorted) {
	 Index0 = *Sorted++;
	 while ((Index0 & 1) == 1) { // omit Max entries
	    Index0 = *Sorted++;
	 }

	 ++RunningAddress;
	 while ((((*RunningAddress) & 1) == 1 // omit Max entries
		 || Data[*RunningAddress] < Data[Index0])) {
	    ++RunningAddress;
	 }
	 SINFO << " " << Index0 << "/" << RunningAddress << std::endl;

	 const u32* RunningAddress2 = RunningAddress;

	 while (Data[Index1 = *RunningAddress2] <= Data[Index0+1]) { // MaxPosList[Axis0][Index0>>1]) {
	   SINFO << " " << RunningAddress2 << "/" << LastSorted << std::flush;
	       u32 BoxIndex0 = Index0 >> 1;
	       u32 BoxIndex1 = Index1 >> 1;
	       if (checkIntersect(MinPosList[Axis1][BoxIndex0], MaxPosList[Axis1][BoxIndex0], 
				  MinPosList[Axis1][BoxIndex1], MaxPosList[Axis1][BoxIndex1])) {
		 if (checkIntersect(MinPosList[Axis2][BoxIndex0], MaxPosList[Axis2][BoxIndex0], 
				    MinPosList[Axis2][BoxIndex1], MaxPosList[Axis2][BoxIndex1])) {
		   bool elem0 = (BoxIndex0 < list0.size());
		   bool elem1 = (BoxIndex1 < list0.size()); 
		   if ((elem0 ^ elem1)) {
		      mPairs.AddPair(BoxIndex0, BoxIndex1);
		   }
		 }
	       }
	       ++RunningAddress2;
	       while (((*RunningAddress2) & 1) == 1) { // omit Max entries
		  ++RunningAddress2;
	       }
	 }
      }
      SNOTICE << "SweepPruningTraverser::init: Starting pairs " << mPairs.getNumPairs() << std::endl;
   }

   delete[] Data;
   delete[] MinPosList[0]; 
   delete[] MinPosList[1]; 
   delete[] MinPosList[2]; 
   delete[] MaxPosList[0]; 
   delete[] MaxPosList[1]; 
   delete[] MaxPosList[2]; 
      
   return true;
}


template <class BasicTraits>
void SweepPruningTraverser<BasicTraits>::update (const GeomObjectType& node)
//(u32 i, const AABB& box)
{
   if (m_data0 == NULL || m_data1 == NULL) {
      SFATAL << "SweepPruningTraverser not initialized yet!" << std::endl;
      return;
   }

   Profiler::the().StartProfile("SweepPruningTraverser.UpdateSweepAndPrune");
   CacheData& data = Cache::the()[node];

   const typename CacheData::AdapterContainer& list  = 
     data.getAdapter(BVolAdapterBase::getAdapterId());
   const typename CacheData::AdapterContainer& list0 = 
     m_data0->getAdapter(BVolAdapterBase::getAdapterId());
   const typename CacheData::AdapterContainer& list1 = 
     m_data1->getAdapter(BVolAdapterBase::getAdapterId());
   const u32 sizeList0 = list0.size();
   const u32 sizeList1 = list1.size();

   u32 i;
   u32 offset;
   u32 partner = 0;
   if (&data == m_data0) {
      offset  = 0;
      partner = 1;  
   } else if (&data == m_data1) {
      offset  = sizeList0;
      partner = 0;
   } else {
      // CF to be optimized
      for (i=0; i<sizeList0; ++i) {
	if (list0[i] == list[0]) {
	   offset  = i;
	   partner = 1;
	   break;
	}
      }
      if (i == sizeList0) {
	for (i=0; i<sizeList1; ++i) {
	  if (list1[i] == list[0]) {
	     offset  = i+sizeList0;
	     partner = 0;
	     break;
	  }
	}
	if (i == sizeList1) {
	   SFATAL << "SweepPruningTraverser::update: adapter " << list[0] << " not found; update ignored" 
		  << std::endl;
	   return;
	}
      }
   }
   SNOTICE << "SweepPruningTraverser::update: Node offset " << offset 
	   << " for object " << (partner+1)%2 << std::endl;

   for(i=0;i<list.size();i++) {
     // calc bvol of object list[i]
     Real MinPos[3];
     Real MaxPos[3];
     calcInterval(data, static_cast<BVolAdapterBase*>(list[i]), 0, MinPos[0], MaxPos[0]);
     calcInterval(data, static_cast<BVolAdapterBase*>(list[i]), 1, MinPos[1], MaxPos[1]);
     calcInterval(data, static_cast<BVolAdapterBase*>(list[i]), 2, MinPos[2], MaxPos[2]);
     for(u32 Axis=0;Axis<3;Axis++) {
       // Update min
       {
	 SAPEndPoint* const CurrentMin = mBoxes[i+offset].Min[Axis];
	 assert(!CurrentMin->IsMax());
	 
	 const Real Limit = MinPos[Axis];
	 if(Limit == CurrentMin->Value) {
	 } else if(Limit < CurrentMin->Value) {
	   CurrentMin->Value = Limit;
	   
	   // Min is moving left:
	   SAPEndPoint* NewPos = CurrentMin;
	   assert(NewPos);
	   
	   SAPEndPoint* tmp;
	   while((tmp = NewPos->Previous) && tmp->Value > Limit) {
	     NewPos = tmp;
	     
	     if(NewPos->IsMax() && NewPos->List()==partner) {
	       // Our min passed a max => start overlap
	       //u32 SortedIndex = (u32(CurrentMin) - Base)/sizeof(NS_EndPoint);
	       const u32 id0 = CurrentMin->GetBoxID();
	       const u32 id1 = NewPos->GetBoxID();
	       
	       if (id0!=id1 && checkIntersect(MinPos, MaxPos, mBoxes[id1]))	
		 mPairs.AddPair(id0, id1);
	     }
	   }
	   
	   CurrentMin->InsertBefore(NewPos);
	 } else// if(Limit > CurrentMin->Value)
	   {
	     CurrentMin->Value = Limit;
	     
	     // Min is moving right:
	     SAPEndPoint* NewPos = CurrentMin;
	     assert(NewPos);
	     
	     SAPEndPoint* tmp;
	     while((tmp = NewPos->Next) && tmp->Value < Limit) {
	       NewPos = tmp;
	       
	       if(NewPos->IsMax() && NewPos->List()==partner) {
		 // Our min passed a max => stop overlap
		 const u32 id0 = CurrentMin->GetBoxID();
		 const u32 id1 = NewPos->GetBoxID();
		 
		 if (id0!=id1)	
		   mPairs.RemovePair(id0, id1);
	       }
	     }
	     
	     CurrentMin->InsertAfter(NewPos);
	   }
       }
       
       // Update max
       {
	 SAPEndPoint* const CurrentMax = mBoxes[i+offset].Max[Axis];
	 assert(CurrentMax->IsMax());
	 
	 const Real Limit = MaxPos[Axis];
	 if(Limit == CurrentMax->Value) {
	 } else if(Limit > CurrentMax->Value) {
	   CurrentMax->Value = Limit;
	   
	   // Max is moving right:
	   SAPEndPoint* NewPos = CurrentMax;
	   assert(NewPos);
	   
	   SAPEndPoint* tmp;
	   while((tmp = NewPos->Next) && tmp->Value < Limit) {
	      NewPos = tmp;

	      if(!NewPos->IsMax() && NewPos->List()==partner) {
		 // Our max passed a min => start overlap
		 const u32 id0 = CurrentMax->GetBoxID();
		 const u32 id1 = NewPos->GetBoxID();
		 
		 if (id0!=id1 && checkIntersect(MinPos, MaxPos, mBoxes[id1]))	
		    mPairs.AddPair(id0, id1);
	      }
	   }
	   
	   CurrentMax->InsertAfter(NewPos);
	 } else// if(Limit < CurrentMax->Value)
	   {
	     CurrentMax->Value = Limit;
	     
	     // Max is moving left:
	     SAPEndPoint* NewPos = CurrentMax;
	     assert(NewPos);
	     
	     SAPEndPoint* tmp;
	     while((tmp = NewPos->Previous) && tmp->Value > Limit) {
	       NewPos = tmp;
	       
	       if(!NewPos->IsMax() && NewPos->List()==partner) {
		  // Our max passed a min => stop overlap
		  const u32 id0 = CurrentMax->GetBoxID();
		  const u32 id1 = NewPos->GetBoxID();

		  if (id0!=id1)	
		     mPairs.RemovePair(id0, id1);
	       }
	     }

	     CurrentMax->InsertBefore(NewPos);
	   }
       }
     }
   }
   SNOTICE << "SweepPruningTraverser::update: Num pairs " << mPairs.getNumPairs() << std::endl;
   Profiler::the().EndProfile("SweepPruningTraverser.UpdateSweepAndPrune");
}

template <class BasicTraits>
bool SweepPruningTraverser<BasicTraits>::apply 
(const GeomObjectType& node0, const GeomObjectType& node1)
{
   Profiler::the().StartProfile("SweepPruningTraverser.Init");
   Profiler::the().StartProfile("SweepPruningTraverser.GetAdapter");
   CacheData& data0 = Cache::the()[node0];
   CacheData& data1 = Cache::the()[node1];
   const typename CacheData::AdapterContainer& list0 = 
     data0.getAdapter(BVolAdapterBase::getAdapterId());
   const typename CacheData::AdapterContainer& list1 = 
     data1.getAdapter(BVolAdapterBase::getAdapterId());

   getData().Init();
   Profiler::the().EndProfile("SweepPruningTraverser.GetAdapter");

   Profiler::the().StartProfile("SweepPruningTraverser.InitSweepAndPrune");
   if (m_data0 == NULL || m_data1 == NULL) {
      delete[] mBoxes;
      mBoxes = NULL;
      init(list0, data0, list1, data1);
   }
   Profiler::the().EndProfile("SweepPruningTraverser.InitSweepAndPrune");
   Profiler::the().EndProfile("SweepPruningTraverser.Init");

   Profiler::the().StartProfile("SweepPruningTraverser.Loop");
   bool result = false;
   BVolAdapterBase* first;
   BVolAdapterBase* second;
   u32 i, j;
   for (i=0; i<mPairs.getNumPairs(); ++i) {
      for (SAPElement* iter=mPairs.getPairsArray()[i]; iter!=NULL; iter=iter->mNext) {
	Profiler::the().StartProfile("SweepPruningTraverser.Prepare");
	// get adapter and transformation of pairs
	Profiler::the().StartProfile("SweepPruningTraverser.GetCacheData");
	if (i >= list0.size()) {
	  SFATAL << "SweepPruningTraverser::apply: index0=" << i << " not in range [0:" 
		 << list0.size()-1 << "]" << std::endl;
	  continue;
	}
	first = static_cast<BVolAdapterBase*>(list0[i]);
	CacheData&     d0 = getCacheData(data0, first);
	j = iter->mID - list0.size();
	if (j >= list1.size()) {
	  SFATAL << "SweepPruningTraverser::apply: index1=" << j << " not in range [0:" 
		 << list1.size()-1 << "]" << std::endl;
	  continue;
	}
	second = static_cast<BVolAdapterBase*>(list1[j]);
	CacheData&     d1 = getCacheData(data1, second);
	Profiler::the().EndProfile("SweepPruningTraverser.GetCacheData");
	Profiler::the().StartProfile("SweepPruningTraverser.GetWorldMatrix");
	TransformType m0 = d0.getToWorldMatrix();
	TransformType m1 = d1.getToWorldMatrix();
	Profiler::the().EndProfile("SweepPruningTraverser.GetWorldMatrix");
	Profiler::the().EndProfile("SweepPruningTraverser.Prepare");
	
	Profiler::the().StartProfile("SweepPruningTraverser.DoubleTraverser");
	bool thisResult = getDoubleTraverser().apply(d0, d0, first,  
						     d1, d1, second, 
						     m0, m1);
	Profiler::the().EndProfile("SweepPruningTraverser.DoubleTraverser");
	result |= thisResult;    
      }
   }
   Profiler::the().EndProfile("SweepPruningTraverser.Loop");
   return result;
}

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING SweepPruningTraverser<OpenSGTraits>;

END_GENVIS_NAMESPACE
