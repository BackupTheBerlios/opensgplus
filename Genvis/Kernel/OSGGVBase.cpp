//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.4 $
//   $Date: 2004/12/21 17:41:57 $
//                                                                            
//=============================================================================

#include <iostream>
#include <malloc.h>
#include "OSGGVBase.h"
USING_GENVIS_NAMESPACE

std::ostream& genvis::GV_stream = std::cout;

#ifdef GV_VERBOSE
bool genvis::evalVerbosity ()
{
   char* env = getenv("GV_VERBOSE");
   if (env != NULL) {
      if ((strcmp(env, "true")==0) 
	  || (strcmp(env, "TRUE")==0)
	  || (strcmp(env, "True")==0)) {
	 GV_stream << "GENVIS: verbose mode due to GV_VERBOSE=true"
		   << std::endl; 
	 return true;
      } else {
	 GV_stream << "GENVIS: silent mode due to GV_VERBOSE!=true"
		   << std::endl; 
	 return false;
      }
   }
   return false;
}

bool genvis::GV_verbose = evalVerbosity();
#endif
