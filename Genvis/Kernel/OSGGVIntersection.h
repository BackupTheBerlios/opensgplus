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
//   $Revision: 1.2 $
//   $Date: 2003/09/19 21:50:11 $
//                                                                            
//=============================================================================

#ifndef OSGGVINTERSECTION_H
#define OSGGVINTERSECTION_H

#include <assert.h>
#include "OSGGVBase.h"
#include "OSGGVReal.h"
#include "OSGGVBoundingVolume.h"

BEGIN_GENVIS_NAMESPACE

class Ray;
class BVolAdapterBase;

/*! \brief Base class for additional data stored in Intersection objects.
 */
class OSG_GENVISLIB_DLLMAPPING IntersectionData
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   virtual inline ~IntersectionData ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};

inline IntersectionData::~IntersectionData ()
{
}

/*! \brief Normal in intersection point.
 */
class OSG_GENVISLIB_DLLMAPPING IntersectionNormal : public IntersectionData
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   inline IntersectionNormal ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */
   inline VectorClass  getSurfaceNormal () const;
   inline void   setSurfaceNormal (const VectorClass& normal);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   VectorClass m_normal;
};

inline IntersectionNormal::IntersectionNormal ()
  : m_normal(0,0,1)
{
}

inline VectorClass  IntersectionNormal::getSurfaceNormal () const
{
   return m_normal;
}
inline void   IntersectionNormal::setSurfaceNormal (const VectorClass& normal)
{
   m_normal = normal;
}


/*! \brief Ray intersection.
 */
class OSG_GENVISLIB_DLLMAPPING Intersection
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   inline Intersection ();
   inline Intersection (Ray&             ray, 
			BVolAdapterBase* from = NULL,
			Real            dist = -1);
   inline ~Intersection ();
   inline void  init (Ray&             ray, 
		      BVolAdapterBase* from = NULL,
		      Real            dist = -1);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Additional data.                                             */
   /*! \{                                                                 */
   inline void              clearData ();
   inline void              setData (IntersectionData* data);
   inline IntersectionData* getData () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Ray.                                                         */
   /*! \{                                                                 */ 
   inline Ray&              getRay ();
   inline const Ray&        getRay () const;
   inline void              setRay (Ray& ray);

   inline BVolAdapterBase*  getFrom () const;
   inline void              setFrom (BVolAdapterBase* from);

   inline BVolAdapterBase*  getTo () const;
   inline void              setTo (BVolAdapterBase* to);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Intersection distance on ray.                                */
   /*! \{                                                                 */ 
   inline Real             getDist    () const;
   inline void              setDist    (Real dist);
   inline bool              updateDist (Real dist);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   Ray*              m_ray;  //in/out
   BVolAdapterBase*  m_from; //in

   Real             m_dist; //in/out
   BVolAdapterBase*  m_to;   //out
   IntersectionData* m_data;
};

inline void Intersection::init (Ray&             ray, 
				BVolAdapterBase* from,
				Real            dist)
{
   m_ray  = &ray; 
   m_from = from; 
   m_to   = NULL; 
   m_dist = dist;
   m_data = NULL;
}
inline Intersection::Intersection ()
{
   m_ray  = NULL; 
   m_from = NULL; 
   m_to   = NULL; 
   m_dist = -1;
   m_data = NULL;
}
inline Intersection::Intersection (Ray&       ray, 
				   BVolAdapterBase* from,
				   Real            dist)
{
   init(ray, from, dist);
}
inline Intersection::~Intersection ()
{
}

inline void              Intersection::clearData ()
{
   delete m_data;
   m_data = NULL;
}
inline void              Intersection::setData (IntersectionData* data)
{
   clearData();
   m_data = data;
}
inline IntersectionData* Intersection::getData () const
{
   return m_data;
}

inline void        Intersection::setRay (Ray& ray)
{
   m_ray = &ray;
}
inline Ray&        Intersection::getRay ()
{
   assert(m_ray != NULL);
   return *m_ray;
}
inline const Ray&        Intersection::getRay () const
{
   assert(m_ray != NULL);
   return *m_ray;
}

inline void              Intersection::setFrom (BVolAdapterBase* from)
{
   m_from = from;
}
inline BVolAdapterBase*  Intersection::getFrom () const
{
   return m_from;
}

inline BVolAdapterBase*  Intersection::getTo () const
{
   return m_to;
}
inline void              Intersection::setTo (BVolAdapterBase* to)
{
   m_to = to;
}

inline Real             Intersection::getDist () const
{
   return m_dist;
}
inline void              Intersection::setDist (Real dist)
{
   m_dist = dist;
}
inline bool              Intersection::updateDist (Real dist)
{
   if (getDist() < 0 || dist < getDist()) {
      setDist(dist);
      return true;
   }
   return false;
}

END_GENVIS_NAMESPACE
#endif

