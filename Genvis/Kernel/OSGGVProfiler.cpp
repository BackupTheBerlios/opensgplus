//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.4 $
//   $Date: 2003/09/25 16:17:57 $
//                                                                            
//-----------------------------------------------------------------------------
// Hierarchical profiling based on code 
// by Pierre Terdiman, www.codercorner.com
//=============================================================================

#include <algorithm>
#include "OSGGVProfiler.h"
#ifndef WIN32
#  include <sys/time.h>
#endif

// sum up times for blocks with same name
#define GV_PROFILER_SUMNAMES

USING_GENVIS_NAMESPACE

static u32 gBase = 0;
static void SetBaseTime(u32 time)	
{ 
   gBase = time; 
}
static u32  GetBaseTime()				
{ 
   return gBase; 
}

#ifdef WIN32
/*! This function initializes the profiler by counting the cpuid overhead.
    This is done 3 times on purpose, since cpuid takes a longer time to execute the first times it's called.
    "cpuid" is used before rdtsc to prevent out-of-sequence execution from producing wrong results.
    For more details, read Intel's application notes "Using the RDTSC instruction for performance monitoring".
    \see		StartProfile
    \see		EndProfile
*/
__forceinline void InitProfiler()
{
  u32 cyc, Base;
  _asm {
      cpuid
      rdtsc
      mov		cyc, eax
      cpuid
      rdtsc
      sub		eax, cyc
      mov		Base, eax
      
      cpuid
      rdtsc
      mov		cyc, eax
      cpuid
      rdtsc
      sub		eax, cyc
      mov		Base, eax
      
      cpuid
      rdtsc
      mov		cyc, eax
      cpuid
      rdtsc
      sub		eax, cyc
      mov		Base, eax
  }
  SetBaseTime(Base);
}

/*! This function starts recording the number of cycles elapsed.
    \param		val[out] address of a 32 bits value where the system should store the result.
    \see		EndProfile
    \see		InitProfiler
*/
__forceinline void	StartProfile(u32& val)
{
  __asm{
      cpuid
      rdtsc
      mov		ebx, val
      mov		[ebx], eax
  }
}

/*! This function ends recording the number of cycles elapsed.
    \param		val		[out] address to store the number of cycles elapsed since the last StartProfile.
    \see		StartProfile
    \see		InitProfiler
*/
__forceinline void	EndProfile(u32& val)
{
  __asm{
      cpuid
      rdtsc
      mov		ebx, val
      sub		eax, [ebx]
      mov		[ebx], eax
  }
  val-=GetBaseTime();
}
#else // other platforms
inline void InitProfiler()
{
}
inline void	StartProfile(u32& val)
{
   struct timeval stop;
   gettimeofday(&stop, NULL);
   val = u32(stop.tv_sec)*1000+u32(stop.tv_usec);
}
inline void	EndProfile(u32& val)
{
   struct timeval stop;
   gettimeofday(&stop, NULL);
   val = u32(stop.tv_sec)*1000+u32(stop.tv_usec) - val;
}
#endif

Profiler::Profiler() 
  : mUserData(0), mCallback(NULL), mEvent(NULL), mRecursionLevel(0), mNbRecords(0), mIsReady(false), mEndingPending(false)
{
}
Profiler::~Profiler()
{
}
Profiler& Profiler::the ()
{
   static Profiler instance;
   return instance;
}

void Profiler::Reset()
{
	if(!gBase)	InitProfiler();

	while (!mCyclesCounters.empty()) mCyclesCounters.pop();
	while (!mOrder.empty())          mOrder.pop();
	mRecorder.clear();
	mNbRecords			= 0;
	mRecursionLevel		= 0;
	mEndingPending		= false;
	// Mark as ready to go
	mIsReady			= true;
}

bool Profiler::_StartProfile(const char* label)
{
	// The starting callback must be called before starting the profile (not to take its cycles into account)
	if(mCallback && label && mEvent)
	{
		// The callback is used on a given user-defined event
		if(strcmp(label, mEvent)==0)
		{
			(mCallback)(true, mUserData);
			mEndingPending = true;
		}
	}

	// Keep track of recursion level
	mRecursionLevel++;

	// Keep track of order
	mOrder.push(mNbRecords++);

	// We begin a new cycle
	unsigned Value;
	::StartProfile(Value);

	mCyclesCounters.push(Value);

	return true;
}

struct Same { 
  Same (u32 level, const char* label) {
     mLevel = level;
     mLabel = label;
  }
  bool operator() (const ProfileData& e) { 
     return (e.RecursionLevel == mLevel && strcmp(e.Label, mLabel) == 0); 
  }	
private:
  u32         mLevel;
  const char* mLabel;
};
bool Profiler::_EndProfile(const char* label)
{
	// End previous cycle
	u32 Value;
	if (mCyclesCounters.empty())	return false;
	Value = mCyclesCounters.top(); mCyclesCounters.pop();

	::EndProfile(Value);

	// Get order back
	u32 Order;
	if (mOrder.empty())	return false;
	Order = mOrder.top(); mOrder.pop();

	// Keep track of it
#ifndef GV_PROFILER_SUMNAMES
	mRecorder.push_back(ProfileData(Order, label, Value, 0, --mRecursionLevel));
#else
	static unsigned radius = 20;
	typedef std::vector<ProfileData>::iterator Iterator;
	Iterator begin = (mRecorder.size()<=radius
			  ? mRecorder.begin() 
			  : mRecorder.end()-radius);
	Same cmp(--mRecursionLevel, label);
	Iterator found = std::find_if(begin, mRecorder.end(), cmp);
	if (found != mRecorder.end()) {
	   found->NbCycles += Value;
	} else {
	   mRecorder.push_back(ProfileData(Order, label, Value, 0, mRecursionLevel));
	}
#endif

	// The ending callback must be called after having ended the profile
	if(mCallback && label && mEvent && mEndingPending) {
		if(strcmp(label, mEvent)==0)	(mCallback)(false, mUserData);
		mEndingPending = false;
	}
	return true;
}

struct Local { 
  bool operator() (const ProfileData& e0, const ProfileData& e1) { 
     return (e0.Order < e1.Order); 
  }	
};
ProfileData* Profiler::GetResults(u32& nbrecords, u64& totalnbcycles)
{
	// Checkings
	if(!mIsReady)	return NULL;
	// Get number of records
	nbrecords = mRecorder.size();
	// Check there's at least one
	if (!nbrecords)	return NULL;

	// Reorganize the records (sorting)
	Local cmp;
	std::sort(mRecorder.begin(), mRecorder.end(), cmp);
	ProfileData* Data = &(*mRecorder.begin());

	// Compute total number of elapsed cycles
	totalnbcycles = 0;
	// Loop through records...
	u32 i;
	for (i=0; i<nbrecords; i++) {
	   // ...and only keep root-level ones
	   if (Data[i].RecursionLevel == 0) {
	      totalnbcycles += Data[i].NbCycles;
	   }
	}
	Real Total(totalnbcycles);

	// Loop through records
	for (i=0;i<nbrecords;i++)
	{
		// Get current record
		ProfileData* CurDat = &Data[i];

		// Get current recursion level
		u32 CurLevel = CurDat->RecursionLevel;

		// Special case for root-level...
		if (CurLevel == 0) {
		  CurDat->Percents = Real(CurDat->NbCycles) * 100.0f / Total;
		} else {
		  // Look for the current subtotal, from previous recursion level
		  u32 j = i-1;
		  while (Data[j].RecursionLevel != (CurLevel-1)) --j;
		  CurDat->Percents = Real(CurDat->NbCycles) * 100.0f / Real(Data[j].NbCycles);
		}
	}

	// Must call Reset() again before re-using the profiler
	mIsReady = false;

	return Data;
}

void Profiler::dump (std::ostream& os)
{
   // Display results
   u32 NbRecords = 0;
   u64 TotalNbCycles = 0;
   ProfileData* PD = GetResults(NbRecords, TotalNbCycles);
   os.setf(std::ostream::fixed);
   if (NbRecords > 0) {
      for (u32 i=0; i<NbRecords; ++i) {
	 for (u32 j=0; j<PD[i].RecursionLevel*4; ++j) os << ' ';
	 os << PD[i].Label << " : " << Real(PD[i].NbCycles) << " cycles ("
	    << PD[i].Percents << "%)"
	    << std::endl;
      }
   }
   os.unsetf(std::ostream::fixed);
}
