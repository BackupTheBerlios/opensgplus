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
//=============================================================================

#include <iostream>
#include <malloc.h>
#include "OSGGVBase.h"
USING_GENVIS_NAMESPACE

#if defined(WIN32)
void genvis::getProcessVMConsumption(SIZE_T& processVMSize, SIZE_T& processVMUsed)
{
  // Get my process memory status counters:
  MEMORYSTATUS processMemoryStatus;
  GlobalMemoryStatus(&processMemoryStatus);
  
  processVMSize = processMemoryStatus.dwTotalVirtual;
  
  SIZE_T processVMFree = processMemoryStatus.dwAvailVirtual;
  processVMUsed = processVMSize - processVMFree;
}

bool genvis::getProcessHeapMemoryConsumption(unsigned int& heapBlocksUsed, unsigned int& heapMemoryUsed)
{
  bool retVal = false;
  heapBlocksUsed = 0;
  heapMemoryUsed = 0;
  
  _HEAPINFO hinfo;
  hinfo._pentry = NULL; // NULL means a pointer to the beginning of the heap.
  
  // Iterate on the heap entries and count the allocated memory blocks:
  // -----------------------------------------------------------------
  // Get the first heap entry:
  int heapstatus = _heapwalk(&hinfo);
  
  while (heapstatus == _HEAPOK) {
      // If this is a "Used" entry:
      if (hinfo._useflag == _USEDENTRY)
                {
		  // Accumulate the current entry and its size:
		  heapBlocksUsed++;
		  heapMemoryUsed += hinfo._size;
                }
      
      // Get the next heap entry:
      heapstatus = _heapwalk(&hinfo);
  }
  
  // Return true iff we manage to traverse the entire heap:
  if (heapstatus == _HEAPEND) {
    retVal = true;
  }
  
  return retVal;
}
#endif

#ifdef GV_VERBOSE
bool genvis::evalVerbosity ()
{
   char* env = getenv("GV_VERBOSE");
   if (env != NULL) {
      if ((strcmp(env, "true")==0) 
	  || (strcmp(env, "TRUE")==0)
	  || (strcmp(env, "True")==0)) {
	 std::cout << "GENVIS: verbose mode due to GV_VERBOSE=true"
		   << std::endl; 
	 return true;
      } else {
	 std::cout << "GENVIS: silent mode due to GV_VERBOSE!=true"
		   << std::endl; 
	 return false;
      }
   }
   return false;
}

bool genvis::GV_verbose = evalVerbosity();
#endif
