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
//   $Date: 2004/12/20 15:57:11 $
//                                                                            
//=============================================================================

#ifndef OSGGVGROUP_H
#define OSGGVGROUP_H

#include <vector>
#include <assert.h>
#include "OSGGVBase.h"
#include "OSGGVBVolAdapterBase.h"
#include "OSGGVKDop.h"
#include "OSGGVAdapterFactory.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Interface for inner nodes of bounding volume hierarchy.
 */
class OSG_GENVISLIB_DLLMAPPING BVolGroupInterface : public BVolAdapterBase
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef BVolAdapterBase          Inherited;
   typedef BoundingVolume<Real>     BVol;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   inline  BVolGroupInterface ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Interface BVolAdapterBase.                                   */
   /*! \{                                                                 */
   virtual bool isLeaf() const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Accessing child nodes.                                       */
   /*! \{                                                                 */
   virtual u32                     size () const = 0;
   virtual BVolAdapterBase*        getSon (u32 i) const = 0;
   /*! \brief One leaf node of this group. */
   virtual inline BVolAdapterBase* findLeaf () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};

inline BVolGroupInterface::BVolGroupInterface ()
{   
}
inline BVolAdapterBase* BVolGroupInterface::findLeaf () const
{
   BVolAdapterBase* node = getSon(0);
   while (node != NULL && node->isInner()) {
      node = ((BVolGroupInterface*)node)->getSon(0);
   }
   return node;
}


/*! \brief Base class for inner nodes storing the children explicitly.
 */
class OSG_GENVISLIB_DLLMAPPING BVolGroupBase : public BVolGroupInterface
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef BVolGroupInterface            Inherited;
   typedef BVolGroupBase                 Self;
   typedef BoundingVolume<Real>          BVol;

   typedef std::vector<BVolAdapterBase*> Container;
   typedef Container::iterator           Iterator;
   typedef Container::const_iterator     ConstIterator;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   inline  BVolGroupBase ();
   virtual ~BVolGroupBase ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Member.                                                      */
   /*! \{                                                                 */
   inline void              setValid (bool valid=true);
   inline bool              isValid  () const;
   inline u32               getNumLeafs() const;
   inline Container&        getSons ();
   inline const Container&  getSons () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Interface BVolGroupInterface.                                */
   /*! \{                                                                 */
   virtual u32              size () const;
   virtual BVolAdapterBase* getSon (u32 i) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Dump.                                                        */
   /*! \{                                                                 */
   virtual std::ostream&    dump (std::ostream& os) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   Container m_sons;
   bool      m_valid;
};

inline BVolGroupBase::Container& BVolGroupBase::getSons ()
{  
   return m_sons;      
}
inline const BVolGroupBase::Container& BVolGroupBase::getSons () const
{
   return m_sons;   
}

inline BVolGroupBase::BVolGroupBase ()
  : m_valid(false)
{   
}

inline void BVolGroupBase::setValid (bool valid)
{
   m_valid = valid;
}
inline bool BVolGroupBase::isValid () const
{
   return m_valid;
}

inline u32 BVolGroupBase::getNumLeafs() const
{
   u32 leafs = 0;
   for (ConstIterator iter = getSons().begin();
	iter != getSons().end(); 
	++iter) {
     if ((*iter)->isInner()) {
        leafs += static_cast<BVolGroupBase*>(*iter)->getNumLeafs();
     } else {
        ++leafs;
     }
   }
   return leafs;
}


/*! \brief Class for inner nodes, storing the children explicitly and having a
    k-DOP bounding volume.
 */
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING BVolGroup : public BVolGroupBase
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef BVolGroupBase         Inherited;
   typedef BVolGroup<BVOL>       Self;
   typedef FactoryHeap<Self>     FactoryType;
   typedef BVOL                  BVol;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   inline  BVolGroup ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Update bounding volume.                                      */
   /*! \{                                                                 */
   inline void init (u32 first, u32 last,
		     const std::vector< std::vector<u32> >& index,
		     const std::vector<Adapter*>&           nodes);
   virtual inline void update ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Bounding volume.                                             */
   /*! \{                                                                 */
   inline BVOL&                               getBVol ();
   inline const BVOL&                         getBVol () const;
   virtual inline const BoundingVolume<Real>& getBoundingVolume () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Ray intersection.                                            */
   /*! \{                                                                 */
   /*! Calc intersection with a ray, by
       first checking intersection with bounding volume
       and calculating intersection with all children.
       @param  hit Geometric result.
       @return Returns iff there is an intersection.                      */
   virtual bool calcIntersect     (Intersection& hit);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Draw.                                                        */
   /*! \{                                                                 */
   virtual void draw          ();
   virtual void drawWireframe ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   /*---------------------------------------------------------------------*/
   /*! \name Internal Methods.                                            */
   /*! \{                                                                 */
   inline void updateBoundingVolume ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

   BVOL m_bvol;
};

template <class BVOL>
inline BVolGroup<BVOL>::BVolGroup ()
{   
}
template <class BVOL>
inline void BVolGroup<BVOL>::init (u32,
				   u32,
				   const std::vector< std::vector<u32> >&,
				   const std::vector<Adapter*>&)
{   
}
template <class BVOL>
inline void BVolGroup<BVOL>::updateBoundingVolume ()
{
   m_bvol.setEmpty();
   for (Iterator iter = getSons().begin();   
	iter != getSons().end(); 
	++iter) {
      m_bvol.unify(static_cast<BVolAdapter<BVOL>*>(*iter)->getBVol());
   }
}
template <class BVOL>
inline const BoundingVolume<Real>& BVolGroup<BVOL>::getBoundingVolume () const
{
   if (!isValid()) {
      Self* nonconstThis = (Self*)this;
      nonconstThis->updateBoundingVolume();
      nonconstThis->setValid();
   }
   return m_bvol;
}
template <class BVOL>
inline BVOL& BVolGroup<BVOL>::getBVol ()
{
   getBoundingVolume();
   return m_bvol;
}
template <class BVOL>
inline const BVOL& BVolGroup<BVOL>::getBVol () const
{
   getBoundingVolume();
   return m_bvol;
}

template <class BVOL>
inline void BVolGroup<BVOL>::update ()
{
   setValid(false);
}

END_GENVIS_NAMESPACE
#endif

