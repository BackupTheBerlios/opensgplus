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

#ifndef OSGGVTIMER_H
#define OSGGVTIMER_H

#include <assert.h>
#ifdef WIN32
#  include "OSGGL.h"
#else
#  include <sys/time.h>
#endif
#include "OSGGVBase.h"
#include "OSGGVReal.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Timer, averaging several timings.
 */
class OSG_GENVISLIB_DLLMAPPING Timer
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline Timer  (u32 size=1);
   inline ~Timer ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */   
   /*! Get array index, where next timing is stored. */
   inline u32           getCurrentIndex () const;
   /*! Size of averaging window. */
   inline u32           size () const;
   /*! Size of averaging window. */
   inline u32           getSize () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Lowlevel Timing.                                             */
   /*! \{                                                                 */   
   static inline genvis::i64 getCurrentTime ();
   static inline Real      getTimeUnits   ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Timing.                                                      */
   /*! \{                                                                 */   
   /*! Reset timer, also done in the constructor. */
   inline void        reset ();
   /*! Start timing. */
   inline void        start ();
   /*! Stop timing. 
       \return Timer ticks (Unit getTimeUnits()) elapsed. */
   inline genvis::i64 stop  ();
   /*! Get average of last getSize() timings. If there are less, then returns
       the average of all timings done so far.
    */
   inline Real      averageTime () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   genvis::i64* m_time;
   u32     m_p;
   u32     m_size;
};

inline Timer::Timer (u32 size)
  : m_size(size), m_p(0)
{
   m_time = new genvis::i64[m_size];
   for (u32 i=0; i<m_size; ++i) {
      m_time[i] = -1;
   }
}
inline Timer::~Timer ()
{
   delete[] m_time;
}
inline u32 Timer::getSize () const
{
   return m_size;
}
inline u32 Timer::size () const
{
   return m_size;
}
inline genvis::i64 Timer::getCurrentTime ()
{
#ifdef WIN32
   LARGE_INTEGER count; 
   if (QueryPerformanceCounter(&count)) {
      return (genvis::i64)count.LowPart; 
   } else {
      return timeGetTime();
   }
#else
   long s;
   long us;
   struct timeval stop;
   gettimeofday(&stop, NULL);
   s = stop.tv_sec;
   us = stop.tv_usec; 
   return us+s*1000;
#endif
}
inline Real Timer::getTimeUnits ()
{
#ifdef WIN32
   LARGE_INTEGER freq; 
   if (QueryPerformanceFrequency(&freq)) {
      return 1000.0/(freq.HighPart*pow(2, 32)+freq.LowPart);
   } else {
      return 1.0;
   }
#else
   return 1.0;
#endif
}

inline u32 Timer::getCurrentIndex () const
{
   return m_p;
}

inline void Timer::reset ()
{
   m_p = 0;
   for (u32 i=0; i<m_size; ++i) {
      m_time[i] = -1;
   }
}
inline void Timer::start ()
{
   m_time[m_p] = getCurrentTime();
}
inline genvis::i64 Timer::stop ()
{
   u32 prev = m_p;
   if (m_size > 1) {
      m_p = (m_p+1u) % m_size;
      m_time[prev] = getCurrentTime() - m_time[prev];
   } else {
      m_time[prev] = getCurrentTime() - m_time[prev];
   }
   //assert(m_time[prev] >= 0);
   return m_time[prev];
}
inline Real Timer::averageTime () const
{
   if (m_size > 1) {
      u32  i;
      u32  count = 0;
      Real avg   = 0.0;
      for (i=0; i<m_size; ++i) {
        if (m_time[i] > 0) {
	   avg += m_time[i];
	   ++count;
	}
      }
      return avg/count*getTimeUnits();
   } else {
      return m_time[0]*getTimeUnits();
   }
}

END_GENVIS_NAMESPACE

#endif
