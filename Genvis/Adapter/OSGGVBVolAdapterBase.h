// For Copyright and Legal Information see file license_vrroom.doc
// ---------------------------------------------------------------
// File:    $Id: OSGGVBVolAdapterBase.h,v 1.1 2003/09/11 16:20:29 fuenfzig Exp $
// Author:  Gordon Mueller, <gordon@vrroom.de>
// ---------------------------------------------------------------

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
   typedef BVolAdapterBase      Self;
   typedef Adapter              Inherited;
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
   inline  bool                     isInner () const;
   virtual inline bool              isSibling (BVolAdapterBase* sib) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Parent.                                                      */
   /*! \{                                                                 */
   inline  void                     setParent (BVolAdapterBase* parent);
   virtual inline  BVolAdapterBase* getParent () const;
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
   virtual bool    checkIntersect   (const Ray& ray);
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
   static  inline unsigned getAdapterId ();
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
inline unsigned BVolAdapterBase::getAdapterId ()
{
   static unsigned id = getNextId();
   return id;
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



