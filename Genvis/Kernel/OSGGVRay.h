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
//   $Revision: 1.1 $
//   $Date: 2003/09/11 16:20:30 $
//                                                                            
//=============================================================================

#ifndef OSGGVRAY_H
#define OSGGVRAY_H 

#include <assert.h>
#include "OSGGVBase.h"
#include "OSGGVReal.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Ray.
 */
class OSG_GENVISLIB_DLLMAPPING Ray
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   inline Ray (const PointClass& origin, const VectorClass& dir);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Intersection.                                                */
   /*! \{                                                                 */
   inline bool calcIntersect (const PointClass& p0, const PointClass& p1, const PointClass& p2, 
			      Real& t, VectorClass& normal) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */
   inline PointClass&        getOrigin ();
   inline const PointClass&  getOrigin () const;
   inline VectorClass&        getDirection ();
   inline const VectorClass&  getDirection () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   PointClass m_origin;
   VectorClass m_dir;
};

inline Ray::Ray (const PointClass& origin, const VectorClass& dir)
  : m_origin(origin), m_dir(dir)
{
}

inline PointClass&  Ray::getOrigin ()
{
   return m_origin;
}
inline const PointClass&  Ray::getOrigin () const
{
   return m_origin;
}
inline VectorClass&  Ray::getDirection ()
{
   return m_dir;
}
inline const VectorClass&  Ray::getDirection () const
{
   return m_dir;
}

inline bool Ray::calcIntersect (const PointClass& p0, const PointClass& p1, const PointClass& p2, 
				Real& t, VectorClass& normal) const
{
   static FloatingComparator<Real> comp(1E-6f);

   VectorClass dir1 = p1 - p0;
   VectorClass dir2 = p2 - p0;

   VectorClass a = getDirection().cross( dir2 );
   VectorClass b = getOrigin() - p0;
   VectorClass c;

   Real d = dir1.dot( a );
   Real u = b.dot( a );
   Real v;

   if (comp.positive(d)) {
      if ( u < 0 || u > d ) {
	 return false;
      }

      c = b.cross( dir1 );
      v = getDirection().dot( c );
      if ( v < 0 || u + v > d ) {
	 return false;
      }
   } else if (comp.negative(d)) {
      if ( u > 0 || u < d ) {
	 return false;
      }

      c = b.cross( dir1 );
      v = getDirection().dot( c );
      if ( v > 0 || u + v < d ) {
	 return false; 
      }
   } else {
      return false;
   }

   // intersection case
   Real id = 1.f / d;
   t        = dir2.dot( c ) * id;
   if (t < 0) {
      return false;
   }

   normal   = a;
   return true;
}

END_GENVIS_NAMESPACE
#endif
