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

#include <OSGConfig.h>

#include "OSGGenvisCache.h"

OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

/*! \class osg::GenvisCache

*/

/*----------------------- constructors & destructors ----------------------*/

GenvisCache::GenvisCache(void) :
    Inherited()
{
   Inherited::getAdapterMatrix().getValues().resize(GV_MAX_NUM_ADAPTER);
}

GenvisCache::GenvisCache(const GenvisCache &source) :
    Inherited(source)
{
}

GenvisCache::~GenvisCache(void)
{
}

/*----------------------------- class specific ----------------------------*/

void GenvisCache::initMethod (void)
{
}

OSG::NodePtr GenvisCache::getNode   () const
{
   if (getParents().getSize() == 0) {
      return NullFC;
   }
   NodePtr node = NodePtr::dcast(getParents()[0]);
   assert(node != NullFC);
   return node;
}
OSG::Matrix GenvisCache::getToWorldMatrix () const
{
   return getNode()->getToWorld();
}

OSG::NodePtr GenvisCache::getParent () const
{
   return getNode()->getParent();
}

GenvisCache::ChildContainer& GenvisCache::getChildren ()
{
   return getNode()->getMFChildren()->getValues();
}
const GenvisCache::ChildContainer& GenvisCache::getChildren () const
{
   return getNode()->getMFChildren()->getValues();
}
int  GenvisCache::getNumChildren () const
{
   return getNode()->getNChildren();
}

const Matrix& GenvisCache::getAdapterMatrix (unsigned id) const
{
   assert(id < GV_MAX_NUM_ADAPTER);
   return Inherited::getAdapterMatrix().getValue(id);
}
void GenvisCache::setAdapterMatrix (unsigned id, const OSG::Matrix& matrix)
{
   assert(id < GV_MAX_NUM_ADAPTER);
   beginEditCP(GenvisCachePtr(this));
   Inherited::getAdapterMatrix(id) = matrix;
   endEditCP(GenvisCachePtr(this));
}

GenvisCache::AdapterContainer& GenvisCache::getAdapter(unsigned id)
{
   switch (id) {
   default:
   case 0:
     return Inherited::getBVolAdapter().getValues();
   case 1:
     return Inherited::getAdapter2().getValues();
   case 2:
     return Inherited::getAdapter3().getValues();
   }
}
BVolAdapterBaseP& GenvisCache::getAdapter(unsigned id, unsigned num)
{
   switch (id) {
   default:
   case 0:
     assert(num <= Inherited::getBVolAdapter().size());
     return *(Inherited::getBVolAdapter().end()-num);
   case 1:
     assert(num <= Inherited::getAdapter2().size());
     return *(Inherited::getAdapter2().end()-num);
   case 2:
     assert(num <= Inherited::getAdapter3().size());
     return *(Inherited::getAdapter3().end()-num);
   }
}
const GenvisCache::AdapterContainer& GenvisCache::getAdapter(unsigned id) const
{
   switch (id) {
   default:
   case 0:
     return Inherited::getBVolAdapter().getValues();
   case 1:
     return Inherited::getAdapter2().getValues();
   case 2:
     return Inherited::getAdapter3().getValues();
   }
}
const BVolAdapterBaseP& GenvisCache::getAdapter(unsigned id, unsigned num) const
{
   switch (id) {
   default:
   case 0:
     assert(num <= Inherited::getBVolAdapter().size());
     return *(Inherited::getBVolAdapter().end()-num);
   case 1:
     assert(num <= Inherited::getAdapter2().size());
     return *(Inherited::getAdapter2().end()-num);
   case 2:
     assert(num <= Inherited::getAdapter3().size());
     return *(Inherited::getAdapter3().end()-num);
   }
}

void           GenvisCache::setAdapter(unsigned id, BVolAdapterBaseP a) 
{
   switch (id) {
   default:
   case 0:
     beginEditCP(GenvisCachePtr(this));
     Inherited::getBVolAdapter().getValues().push_back(a);
     endEditCP(GenvisCachePtr(this));
     break;
   case 1:
     beginEditCP(GenvisCachePtr(this));
     Inherited::getAdapter2().getValues().push_back(a);
     endEditCP(GenvisCachePtr(this));
     break;
   case 2:
     beginEditCP(GenvisCachePtr(this));
     Inherited::getAdapter3().getValues().push_back(a);
     endEditCP(GenvisCachePtr(this));
     break;
   }
}
void           GenvisCache::setAdapter(unsigned id, const AdapterVector& a) 
{
   switch (id) {
   default:
   case 0:
     beginEditCP(GenvisCachePtr(this));
     Inherited::getBVolAdapter().getValues().insert(getBVolAdapter().end(), 
						    a.begin(), a.end());
     endEditCP(GenvisCachePtr(this));
     break;
   case 1:
     beginEditCP(GenvisCachePtr(this));
     Inherited::getAdapter2().getValues().insert(getAdapter2().end(), 
						 a.begin(), a.end());
     endEditCP(GenvisCachePtr(this));
     break;
   case 2:
     beginEditCP(GenvisCachePtr(this));
     Inherited::getAdapter3().getValues().insert(getAdapter3().end(), 
						 a.begin(), a.end());
     endEditCP(GenvisCachePtr(this));
     break;
   }
}

GenvisCache::AdapterContainer& GenvisCache::getColCache ()
{
   return Inherited::getCollisionCache().getValues();
}
BVolAdapterBaseP& GenvisCache::getColCache (unsigned num)
{
   assert(num <= Inherited::getCollisionCache().size());
   return *(Inherited::getCollisionCache().end()-num);
}
const GenvisCache::AdapterContainer& GenvisCache::getColCache () const
{
   return Inherited::getCollisionCache().getValues();
}
const BVolAdapterBaseP& GenvisCache::getColCache (unsigned num) const
{
   assert(num <= Inherited::getCollisionCache().size());
   return *(Inherited::getCollisionCache().end()-num);
}


void GenvisCache::destroyAdapter (unsigned id, unsigned len)
{
   clearAdapter(id, len);
}
void GenvisCache::clearAdapter (unsigned id, unsigned len)
{
   switch (id) {
   default:
   case 0: {
     beginEditCP(GenvisCachePtr(this));
     AdapterContainer& data = Inherited::getBVolAdapter().getValues();
     data.erase(data.begin(), 
		(len > 0 && len < data.size()
		 ? data.begin()+len 
		 : data.end()));
     endEditCP(GenvisCachePtr(this));
     break;
   }
   case 1: {
     beginEditCP(GenvisCachePtr(this));
     AdapterContainer& data = Inherited::getAdapter2().getValues();
     data.erase(data.begin(), 
		(len > 0 && len < data.size()
		 ? data.begin()+len 
		 : data.end()));
     endEditCP(GenvisCachePtr(this));
     break;
   }
   case 2: {
     beginEditCP(GenvisCachePtr(this));
     AdapterContainer& data = Inherited::getAdapter3().getValues();
     data.erase(data.begin(), 
		(len > 0 && len < data.size()
		 ? data.begin()+len 
		 : data.end()));
     endEditCP(GenvisCachePtr(this));
     break;
   }
   }
}


void GenvisCache::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);
}

void GenvisCache::dump(UInt32 in, const BitVector ) const
{
   SLOG << "GenvisCache {" << std::endl;
   SLOG << " AdapterMatrix " << Inherited::getAdapterMatrix().getSize() << std::endl;
   UInt32 i;
   for (i=0; i<Inherited::getAdapterMatrix().getSize(); ++i) {
      SLOG << " " << Inherited::getAdapterMatrix().getValue(i);
   }
   SLOG << std::endl;
   SLOG << " BVolAdapter " << Inherited::getBVolAdapter().getSize() << std::endl;
   for (i=0; i<Inherited::getBVolAdapter().getSize(); ++i) {
      SLOG << " " << Inherited::getBVolAdapter().getValue(i);
   }
   SLOG << std::endl;
   SLOG << " Adapter2 " << Inherited::getAdapter2().getSize() << std::endl;
   for (i=0; i<Inherited::getAdapter2().getSize(); ++i) {
      SLOG << " " << Inherited::getAdapter2().getValue(i);
   }
   SLOG << std::endl;
   SLOG << " Adapter3 " << Inherited::getAdapter3().getSize() << std::endl;
   for (i=0; i<Inherited::getAdapter3().getSize(); ++i) {
      SLOG << " " << Inherited::getAdapter3().getValue(i);
   }
   SLOG << std::endl;
   SLOG << " CollisionCache " << Inherited::getCollisionCache().getSize() << std::endl;
   for (i=0; i<Inherited::getCollisionCache().getSize(); ++i) {
      SLOG << " " << Inherited::getCollisionCache().getValue(i);
   }
   SLOG << std::endl << " }" << std::endl;
}


/*------------------------------------------------------------------------*/
/*                              cvs id's                                  */

#ifdef OSG_SGI_CC
#pragma set woff 1174
#endif

#ifdef OSG_LINUX_ICC
#pragma warning( disable : 177 )
#endif

namespace
{
    static Char8 cvsid_cpp       [] = "@(#)$Id: OSGGenvisCache.cpp,v 1.1 2003/09/11 16:20:29 fuenfzig Exp $";
    static Char8 cvsid_hpp       [] = OSGGENVISCACHEBASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGGENVISCACHEBASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGGENVISCACHEFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

