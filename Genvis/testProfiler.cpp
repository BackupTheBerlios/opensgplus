// test built-in profiler
//
// parameters:
//    none
// keys:
//    none

#include <iostream>
#include "OSGGVBase.h"
#include "OSGGVProfiler.h"

USING_GENVIS_NAMESPACE
OSG_USING_NAMESPACE

void stdSleep (u32 msec)
{
#ifdef WIN32
   Sleep(msec);
#else
    if( (msec * 1000) > 1000000) {
        ::sleep(msec / 1000);
    } else {
        ::usleep(msec * 1000);
    }
#endif
}

int main(int argc, char* argv[])
{
   // We must call Reset() at the start of each frame
   Profiler::the().Reset();
   
   // Profile some stuff
   Profiler::the().StartProfile("Frame");
   {
     Profiler::the().StartProfile("InnerLoop0");
     {
       stdSleep(8);
     }
     Profiler::the().EndProfile("InnerLoop0");


     Profiler::the().StartProfile("InnerLoop1");
     {
       Profiler::the().StartProfile("Stuff0");
       {
	 Profiler::the().StartProfile("More stuff");
	 { 
	   stdSleep(2);
	 }
	 Profiler::the().EndProfile("More stuff");
       }
       Profiler::the().EndProfile("Stuff0");

       Profiler::the().StartProfile("Stuff1");
       {
	 stdSleep(2);
       }
       Profiler::the().EndProfile("Stuff1");
     }
     Profiler::the().EndProfile("InnerLoop1");


     Profiler::the().StartProfile("InnerLoop2");
     {
       stdSleep(4);
     }
     Profiler::the().EndProfile("InnerLoop2");
   }
   Profiler::the().EndProfile("Frame");

   // Dump results
   Profiler::the().dump(std::cout);

   return 0;
}
