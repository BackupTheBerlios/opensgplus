/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *             Copyright (C) 2000,2001 by the OpenSG Forum                   *
 *                                                                           *
 *                            www.opensg.org                                 *
 *                                                                           *
 *   contact: dirk@opensg.org, gerrit.voss@vossg.org, jbehr@zgdv.de          *
 *                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 *                                License                                    *
 *                                                                           *
 * This library is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU Library General Public License as published    *
 * by the Free Software Foundation, version 2.                               *
 *                                                                           *
 * This library is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public         *
 * License along with this library; if not, write to the Free Software       *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
 *                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>

#include "OSGConfig.h"

#ifdef OSG_STREAM_IN_STD_NAMESPACE
#include <iostream>
#else
#include <iostream.h>
#endif

#include <OSGLog.h>
#include <OSGFieldContainer.h>
#include <OSGFieldContainerPtr.h>
#include <OSGNode.h>
#include <OSGNodeCore.h>
#include <OSGAction.h>
#include <OSGMaterialGroup.h>
#include <OSGGVGenvisPreprocAction.h>

OSG_USING_NAMESPACE


GenvisPreprocAction::GenvisPreprocAction ()
{
}

Action::ResultE GenvisPreprocAction::Preproc::enter (NodePtr& n) 
{
  _cache.addNode(n);

  NodeCorePtr      core = n->getCore();
  MaterialGroupPtr mat  = MaterialGroupPtr::dcast(core);
  if (mat != NullFC) {
     return staticEnterMaterialGroup(mat, this);
  }
  GeometryPtr geom = GeometryPtr::dcast(core);
  if (geom != NullFC) {
     return staticEnterGeometry(geom, this);
  }
  return Action::Continue;
}
Action::ResultE GenvisPreprocAction::Preproc::leave(NodePtr& n, Action::ResultE res)
{
  NodeCorePtr      core = n->getCore();
  MaterialGroupPtr mat  = MaterialGroupPtr::dcast(core);
  if (mat != NullFC) {
     return staticLeaveMaterialGroup(mat, this);
  }
  return Action::Continue;
}

Action::ResultE GenvisPreprocAction::Preproc::staticEnterMaterialGroup (MaterialGroupPtr& arg,
									GenvisPreprocAction::Preproc* action)
{
   action->pushMaterial(arg->getMaterial());

   return Action::Continue; 
}

Action::ResultE GenvisPreprocAction::Preproc::staticLeaveMaterialGroup (MaterialGroupPtr&,
									GenvisPreprocAction::Preproc* action)
{
   action->popMaterial();

   return Action::Continue; 
}

Action::ResultE GenvisPreprocAction::Preproc::staticEnterGeometry  (GeometryPtr& arg,
								    GenvisPreprocAction::Preproc* action)
{
   if (!action->getMaterials().empty()) {
      arg->setMaterial(action->getMaterials().top());
   }
   return Action::Continue; 
}

GenvisPreprocActionP GenvisPreprocAction::create( void )
{
   return new GenvisPreprocAction();
}

Action::ResultE GenvisPreprocAction::apply (NodePtr root)
{
   Preproc code(getCache());
   return traverse(root, 
		   osgTypedMethodFunctor1ObjPtrCPtrRef<Action::ResultE,Preproc,NodePtr>
		   (&code, &Preproc::enter),
		   osgTypedMethodFunctor2ObjPtrCPtrRef<Action::ResultE,Preproc,NodePtr,Action::ResultE>
		   (&code, &Preproc::leave));
}


