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
//   $Revision: 1.3 $
//   $Date: 2004/12/20 15:58:23 $
//                                                                            
//=============================================================================

#ifndef OSGGVPROFILER_H
#define OSGGVPROFILER_H

#include <stack>
#include <vector>
#include "OSGGVBase.h"

BEGIN_GENVIS_NAMESPACE

/*! User-callback, called by the profiler on starting/ending profiling a given process.
    \param begin[in]    true when profile starts, false when profile ends
    \param userdata[in] user-defined data
*/
typedef void	(*PROFILE_CALLBACK)	(bool begin, u32 userdata);

/*! ProfileData
*/
struct OSG_GENVISLIB_DLLMAPPING ProfileData
{
  inline ProfileData ();
  inline ProfileData (u32 order, const char* label, 
		      u64 nbCycles, Real percents, u32 level);

  //!< Insertion order
  u32	Order;			
  //!< User-defined label	
  const char* Label;				
  //!< Number of elapsed cycles
  u64	NbCycles;			
  //!< NbCycles * 100 / TotalNbCycles
  Real	Percents;			
  //!< Level of recursion
  u32	RecursionLevel;		
};

inline ProfileData::ProfileData ()
{
}
inline ProfileData::ProfileData (u32  order, 
				 const char* label, 
				 u64  nbCycles, 
				 Real percents, 
				 u32  level)
  : Order(order), Label(label), NbCycles(nbCycles), Percents(percents), RecursionLevel(level)
{
}


/*! Hierarchical Profiler.
*/
class OSG_GENVISLIB_DLLMAPPING Profiler
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   Profiler();
   ~Profiler();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Singleton.                                                   */
   /*! \{                                                                 */     
   static Profiler& the ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Init.                                                        */
   /*! \{                                                                 */
   /*! Resets the profiler.
       Call it once in your application. If you don't call this method first, 
       subsequent calls to the profiler will be ineffective and exit early. 
       This is done on purpose so that the profiler's overhead is minimal when not it's not used.
   */
   void         Reset();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Profiling.                                                   */
   /*! \{                                                                 */
   inline bool  StartProfile(const char* label=NULL);
   inline bool  EndProfile(const char* label=NULL);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Result.                                                      */
   /*! \{                                                                 */
   /*! Gets the list of complete results.
       \param		nbrecords[out]     Number of records in the list
       \param		totalnbcycles[out] Total number of counted cycles
       \return		the list of profiled items
   */
   ProfileData* GetResults(u32& nbrecords, u64& totalnbcycles);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Callback control.                                            */
   /*! \{                                                                 */
   inline void	SetUserData (u32 data);
   inline void  SetCallback (PROFILE_CALLBACK callback);
   inline void	SetCallbackEvent (const char* label);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Dump.                                                        */
   /*! \{                                                                 */
   void   dump (std::ostream& os);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   /*---------------------------------------------------------------------*/
   /*! \name Internal methods.                                            */
   /*! \{                                                                 */
   // Internal methods
   /*! Starts a new profile. Successive calls are recorded in a LIFO stack.
     \param  label [in] address of a constant reference label (e.g. "Rendering")
     \return true if successful.
   */
   bool _StartProfile(const char* label);
   /*! Ends last profile.
     \param  label[in] address of a constant reference label (e.g. "Rendering")
     \return true if successful.
   */
   bool _EndProfile(const char* label);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

   // User callback
   //!< User-defined data sent to the possible callback
   u32				mUserData;	
   //!< User-defined callback		
   PROFILE_CALLBACK	mCallback;			
   //!< Event triggering off the callback
   const char*			mEvent;				
   
   //!< Elapsed cycles
   std::stack<u32>		mCyclesCounters;	
   //!< Insertion orders
   std::stack<u32>               mOrder;				
   //!< Data records
   std::vector<ProfileData>	mRecorder;			
   //!< Current number of recorded events
   u32				mNbRecords;		
   //!< Current recursion level	
   u32				mRecursionLevel;	
   //!< true if Reset() has been called
   bool				mIsReady;		
   //!< true if the ending callback is pending	
   bool				mEndingPending;		
};

inline bool Profiler::StartProfile(const char* label)
{
   // Check the user has called Reset() beforehand.
   if(!mIsReady)	return false;
   return _StartProfile(label);
}
inline bool Profiler::EndProfile(const char* label)
{
   // Check the user has called Reset() beforehand.
   if(!mIsReady)	return false;
   return _EndProfile(label);
}
inline void Profiler::SetUserData(u32 data)				
{ 
   mUserData = data;		
}
inline void Profiler::SetCallback(PROFILE_CALLBACK callback)	
{ 
   mCallback = callback;	
}
inline void Profiler::SetCallbackEvent(const char* label)		
{ 
   mEvent    = label;	
}

END_GENVIS_NAMESPACE
#endif
