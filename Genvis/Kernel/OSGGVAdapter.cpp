//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.2 $
//   $Date: 2004/12/20 15:58:23 $
//                                                                            
//=============================================================================

#include "OSGGVAdapter.h"

USING_GENVIS_NAMESPACE

u32 Adapter::getNextId ()
{
   static u32 nextId = 0;
   u32 id = nextId++;
   assert(id < GV_MAX_NUM_ADAPTER);
   return id;
}

