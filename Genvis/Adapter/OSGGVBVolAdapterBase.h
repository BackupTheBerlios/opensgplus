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
//   $Date: 2004/12/20 15:56:34 $
//                                                                            
//=============================================================================

#ifndef OSGGVBVOLADAPTERBASE_H
#define OSGGVBVOLADAPTERBASE_H

#include "OSGGVBase.h"
#include "OSGGVReal.h"
#include "OSGGVAdapter.h"
#include "OSGGVBoundingVolume.h"
#include "OSGGVKDop.h"
#include "OSGGVRay.h"
#include "OSGGVIntersection.h"

BEGIN_GENVIS_NAMESPACE

/*! Base for adapter classes used in a bounding volume hierarchy.
    The interface consists of
    \begin{itemize}
    \item Bounding volume 
    \item Geometric information like center point, surface area
    \item Parent node in the hierarchy
    \item Distinguishing inner and leaf nodes
    \item Intersect with a ray and normal in the intersection point
    \item Drawing in shaded mode and in wireframe mode
    \item Output to a stream
    \end{itemize}
*/
class OSG_GENVISLIB_DLLMAPPING BVolAdapterBase : public Adapter
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef Adapter              Inherited;
   typedef BVolAdapterBase      Self;
   typedef BoundingVolume<Real> BVol;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   inline BVolAdapterBase ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Category in hierarchy.                                       */
   /*! \{                                                                 */
   virtual inline bool              isLeaf  () const;
   virtual inline  bool             isInner () const;
   virtual inline bool              isSibling (BVolAdapterBase* sib) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Parent.                                                      */
   /*! \{                                                                 */
   inline  void                     setParent (BVolAdapterBase* parent);
   virtual inline  BVolAdapterBase* getParent () const;
   inline  u32                      getDepth() const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Update after deformation.                                    */
   /*! \{                                                                 */
   virtual void    update ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Bounding volume.                                             */
   /*! \{                                                                 */
   virtual const BoundingVolume<Real>& getBoundingVolume () const = 0;
   virtual Real       getSurfaceArea ();
   virtual PointClass getCenter();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Ray intersection.                                            */
   /*! \{                                                                 */
   virtual bool    checkIntersect   (const Intersection& in);
   virtual bool    calcIntersect    (Intersection& hit);
   virtual VectorClass getSurfaceNormal (const Intersection& hit);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Draw.                                                        */
   /*! \{                                                                 */
   virtual void draw          ();
   virtual void drawWireframe ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Dump.                                                        */
   /*! \{                                                                 */
   virtual std::ostream& dump (std::ostream& os) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Identifier.                                                  */
   /*! \{                                                                 */
   static u32 getAdapterId ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   BVolAdapterBase* m_parent;
};
// CF to be changed
typedef Adapter* BVolAdapterBaseP;

inline BVolAdapterBase::BVolAdapterBase () 
  : m_parent(NULL)
{
}

inline bool BVolAdapterBase::isLeaf () const
{
   return true;
}
inline bool BVolAdapterBase::isInner () const
{
   return !isLeaf();
}

inline BVolAdapterBase* BVolAdapterBase::getParent () const
{
   return m_parent;
}
inline void BVolAdapterBase::setParent (BVolAdapterBase* parent)
{
   m_parent = parent;
}
inline bool      BVolAdapterBase::isSibling (BVolAdapterBase* sib) const
{
   assert(sib != NULL);
   return (getParent() != NULL) && (getParent() == sib->getParent());
}
inline u32 BVolAdapterBase::getDepth () const
{
   u32 depth = 0;
   BVolAdapterBase* parent = getParent();
   while (parent != NULL && parent->isInner()) {
      ++depth;
      parent = parent->getParent();
   }
   return depth;
}


/*! Base class for adapters used in a bounding volume hierarchy.
    Interface is augmented for bounding volume actually used.
*/
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING BVolAdapter : public BVolAdapterBase
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef BVolAdapter<BVOL>    Self;
   typedef BVolAdapterBase      Inherited;
   typedef BVOL                 BVol;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   inline BVolAdapter ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Bounding Volume.                                             */
   /*! \{                                                                 */
   inline const BVOL& getBVol () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};

template <class BVOL>
inline BVolAdapter<BVOL>::BVolAdapter () 
{
}

template <class BVOL>
inline const BVOL& BVolAdapter<BVOL>::getBVol () const
{
   return static_cast<const BVol&>(getBoundingVolume());
}

END_GENVIS_NAMESPACE
#endif



