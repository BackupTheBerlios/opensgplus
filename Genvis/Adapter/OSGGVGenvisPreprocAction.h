// ---------------------------------------------------------------
// File:    $Id: OSGGVGenvisPreprocAction.h,v 1.2 2003/09/19 21:32:14 fuenfzig Exp $
// Author:  Christoph Fuenfzig, <c.fuenfzig@cg.cs.tu-bs.de>
// ---------------------------------------------------------------
// OpenSG action filling the OpenSG cache

#ifndef _OSGGENVISPREPROCACTION_H_
#define _OSGGENVISPREPROCACTION_H_
#ifdef __sgi
#pragma once
#endif

#include <stack>
#include <vector>
#include "OSGBaseTypes.h"
#include "OSGAction.h"
#include "OSGNode.h"
#include "OSGGeometry.h"
#include "OSGMaterialGroup.h"
#include "OSGMFFieldContainerTypes.h"
#include "OSGGVCache.h"

OSG_BEGIN_NAMESPACE

/*! \brief GenvisPreprocAction class
 */
class OSG_GENVISLIB_DLLMAPPING GenvisPreprocAction
{
public:
   GenvisPreprocAction ();

   static GenvisPreprocAction* create(void);
	
   static inline genvis::OSGCache& getCache ();

   Action::ResultE   apply (NodePtr root);

private:
    class Preproc
    {
    public:
      typedef std::stack<MaterialPtr> MaterialStack;

      inline Preproc (genvis::OSGCache& cache);

      inline Action::ResultE enter (NodePtr& n);
      inline Action::ResultE leave(NodePtr& n, Action::ResultE res);

      inline void  pushMaterial (const MaterialPtr& m);
      inline void  popMaterial  ();
      inline const MaterialStack& getMaterials () const;

    protected:
      static Action::ResultE staticEnterMaterialGroup (MaterialGroupPtr& arg, 
						       Preproc* action);
      static Action::ResultE staticLeaveMaterialGroup (MaterialGroupPtr& arg, 
						       Preproc* action);
      static Action::ResultE staticEnterGeometry      (GeometryPtr& arg, 
						       Preproc* action);

      genvis::OSGCache&            _cache;
      MaterialStack                _materials;
    };
};

typedef GenvisPreprocAction* GenvisPreprocActionP;

#include "OSGGVGenvisPreprocAction.inl"

OSG_END_NAMESPACE

#endif
