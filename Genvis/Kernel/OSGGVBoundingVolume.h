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
//   $Date: 2004/12/20 15:58:23 $
//                                                                            
//=============================================================================

#ifndef OSGGVBOUNDINGVOLUME_H
#define OSGGVBOUNDINGVOLUME_H

#include <iostream>
#include "OSGGVBase.h"
#include "OSGGVReal.h"
#include "OSGVolume.h"

BEGIN_GENVIS_NAMESPACE

/** Bounding volume base class. */
template <class REAL>
class OSG_GENVISLIB_DLLMAPPING BoundingVolume : public Volume
{
public:
   inline BoundingVolume ();
   virtual inline ~BoundingVolume ();

   virtual void update () = 0;

   virtual PointClass  getCenter () const = 0;

   virtual REAL*       minVector () = 0;
   virtual REAL*       maxVector () = 0;
   virtual const REAL* minVector () const = 0;
   virtual const REAL* maxVector () const = 0;

   virtual REAL        difference (unsigned k) const = 0;
   virtual REAL        average    (unsigned k) const = 0;

   virtual REAL        getMinmaxFraction () const = 0;
   virtual REAL        getDiameter    () const = 0;
   virtual REAL        getSurfaceArea () const = 0;
   virtual REAL        getVolume      () const = 0;

   virtual bool        calcIntersect (const PointClass&  origin, 
				      const VectorClass& dir,
				      REAL& dist) const = 0;
   virtual inline bool checkIntersect (const PointClass&  origin, 
				       const VectorClass& dir,
				       const REAL& dist) const;
   virtual bool        checkIntersect (const PointClass& point) const = 0;
   virtual bool        checkIntersect (const BoundingVolume<REAL>& box) const = 0;
   virtual bool        checkIntersect (const PointClass& p1,
				       const PointClass& p2,
				       const PointClass& p3) const = 0;

   virtual void        unify (REAL size) = 0;
   virtual void        unify (const BoundingVolume<REAL>& box) = 0;
   virtual void        unify (const PointClass&           point) = 0;

   virtual inline void draw ()          const;
   virtual inline void drawWireframe () const;

   virtual inline std::ostream& dumpInternal (std::ostream& os) const;

    // OpenSG Volume interface
    /*---------------------------------------------------------------------*/
    /*! \name                   Get Values                                 */
    /*! \{                                                                 */

    virtual inline void   getCenter (OSG::Pnt3f& center) const;

    virtual inline float  getScalarVolume (void) const;

    virtual inline void   getBounds (OSG::Pnt3f& min, OSG::Pnt3f& max) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Extending                                  */
    /*! \{                                                                 */

    virtual inline void extendBy (const OSG::Pnt3f& pt);

    virtual inline void extendBy (const OSG::Volume& volume);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                  Intersection                                */
    /*! \{                                                                 */

    virtual inline bool intersect (const OSG::Pnt3f& point) const;

    virtual inline bool intersect (const OSG::Line& line) const;

    virtual inline bool intersect (const OSG::Line& line,
				   float& enter, float& exit) const;

    virtual inline bool intersect (const OSG::Volume& volume) const;

    virtual inline bool isOnSurface (const OSG::Pnt3f& point) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Transformation                             */
    /*! \{                                                                 */

    virtual inline void transform (const OSG::Matrix& matrix);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Output                                  */
    /*! \{                                                                 */

    virtual inline void dump(OSG::UInt32          uiIndent = 0,
			     const OSG::BitVector bvFlags  = 0) const;

    /*! \}                                                                 */
};
typedef BoundingVolume<Real>   OSGBoundingVolume;

template <class REAL>
inline std::ostream& operator << (std::ostream& os, 
				  const BoundingVolume<REAL>& box)
{
   return box.dumpInternal(os);
}

template <class REAL>
inline BoundingVolume<REAL>::BoundingVolume ()
  : Volume()
{
}
template <class REAL>
inline BoundingVolume<REAL>::~BoundingVolume ()
{
}

template <class REAL>
inline bool BoundingVolume<REAL>::checkIntersect (const PointClass&  origin, 
						  const VectorClass& dir,
						  const REAL& dist) const
{
   REAL d = dist;
   return calcIntersect(origin, dir, d);
}
template <class REAL>
inline void BoundingVolume<REAL>::draw ()          const
{
}
template <class REAL>
inline void BoundingVolume<REAL>::drawWireframe () const
{
}

template <class REAL>
inline std::ostream& BoundingVolume<REAL>::dumpInternal (std::ostream& os) const
{
   return os;
}     




template <class REAL>
inline void   BoundingVolume<REAL>::getCenter (OSG::Pnt3f& center) const
{
   center.setValue(getCenter());
}

template <class REAL>
inline float BoundingVolume<REAL>::getScalarVolume (void) const
{
   return getVolume();
}

template <class REAL>
inline void   BoundingVolume<REAL>::getBounds (OSG::Pnt3f& min, OSG::Pnt3f& max) const
{
   min.setValues(minVector()[0], minVector()[1], minVector()[2]);
   max.setValues(maxVector()[0], maxVector()[1], maxVector()[2]);
}

template <class REAL>
inline void BoundingVolume<REAL>::extendBy (const OSG::Pnt3f& pt)
{
   unify(pt);
}

template <class REAL>
inline void BoundingVolume<REAL>::extendBy (const OSG::Volume& volume)
{
   // CF too conservative
   OSG::Pnt3f minBound, maxBound;
   volume.getBounds(minBound, maxBound);
   unify(minBound);
   unify(maxBound);
}

template <class REAL>
inline bool BoundingVolume<REAL>::intersect (const OSG::Pnt3f& point) const
{
   return checkIntersect(point);
}

template <class REAL>
inline bool BoundingVolume<REAL>::intersect (const OSG::Line& line) const
{
   REAL dist = -1;
   return checkIntersect(line.getPosition(), line.getDirection(), dist);
}

template <class REAL>
inline bool BoundingVolume<REAL>::intersect (const OSG::Line& line,
					     float& enter, float& exit) const
{
   bool result = checkIntersect(line.getPosition(), line.getDirection(), enter);
   if (result) {
      checkIntersect(line.getPosition()+enter*line.getDirection(), 
		    line.getDirection(), 
		    exit);
   }
   return result;
}

template <class REAL>
inline bool BoundingVolume<REAL>::intersect (const OSG::Volume&) const
{
   return false;
}

template <class REAL>
inline bool BoundingVolume<REAL>::isOnSurface (const OSG::Pnt3f&) const
{
   return false;
}

template <class REAL>
inline void BoundingVolume<REAL>::transform (const OSG::Matrix&)
{
}

template <class REAL>
inline void BoundingVolume<REAL>::dump(OSG::UInt32,
				       const OSG::BitVector) const
{
   dumpInternal(SLOG);
}

END_GENVIS_NAMESPACE
#endif
