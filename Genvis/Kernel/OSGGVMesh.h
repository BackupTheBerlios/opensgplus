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
//   $Date: 2004/12/20 15:58:23 $
//                                                                            
//=============================================================================

#ifndef OSGGVMESH_H
#define OSGGVMESH_H

#include <vector>
#include <list>
#include "OSGGVBase.h"
#include "OSGGVReal.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Base class for mesh-wide attributes.
 */
class OSG_GENVISLIB_DLLMAPPING MeshAttribsBase
{ 
public: 
};

/*! \brief Base class for edges.
 */
class OSG_GENVISLIB_DLLMAPPING MeshEdgeBase
{ 
public: 
};

/*! \brief Base class for faces.
 */
class OSG_GENVISLIB_DLLMAPPING MeshFaceBase
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   virtual inline ~MeshFaceBase ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */
   virtual u32 size () const = 0;
   virtual PointClass  getPoint  (const MeshEdgeBase& e) const = 0;
   virtual PointClass  getPoint  (u32 i) const = 0;
   virtual VectorClass  getNormal (const MeshEdgeBase& e) const = 0;
   virtual VectorClass  getNormal (u32 i) const = 0;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};

inline MeshFaceBase::~MeshFaceBase ()
{
}

/*! \brief Face class storing a vector of edges.
 */
template < class MESHATTRIBS,
           class MESHEDGE,
           class MESHEDGECONTAINER = std::vector<MESHEDGE> >
class OSG_GENVISLIB_DLLMAPPING MeshFace : public MeshFaceBase
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   inline MeshFace ();
   virtual inline ~MeshFace ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */
   virtual inline u32 size () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Data members.                                                */
   /*! \{                                                                 */
   MESHEDGECONTAINER    edges;
   MESHATTRIBS* externalAttribs;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};

template <class MESHATTRIBS, class MESHEDGE, class MESHEDGECONTAINER>
inline MeshFace<MESHATTRIBS,MESHEDGE,MESHEDGECONTAINER>::MeshFace ()
  : MeshFaceBase(), 
    externalAttribs(NULL)
{
}
template <class MESHATTRIBS, class MESHEDGE, class MESHEDGECONTAINER>
inline MeshFace<MESHATTRIBS,MESHEDGE,MESHEDGECONTAINER>::~MeshFace ()
{
}

template <class MESHATTRIBS, class MESHEDGE, class MESHEDGECONTAINER>
inline u32 MeshFace<MESHATTRIBS,MESHEDGE,MESHEDGECONTAINER>::size () const
{
   return edges.size();
}

/*! \brief Mesh class storing a vector of faces.
 */
template < class MESHATTRIBS,
           class MESHFACE,
           class MESHFACECONTAINER = std::vector<MESHFACE> >
class OSG_GENVISLIB_DLLMAPPING Mesh
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   inline Mesh ();
   virtual inline ~Mesh ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */
   inline u32                     size () const;
   virtual inline MESHFACE&       getFace (u32 i);
   virtual inline const MESHFACE& getFace (u32 i) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Data members.                                                */
   /*! \{                                                                 */
   MESHATTRIBS       attribs;
   MESHFACECONTAINER faces;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};

template <class MESHATTRIBS, class MESHFACE, class MESHFACECONTAINER>
inline MESHFACE& 
Mesh<MESHATTRIBS,MESHFACE,MESHFACECONTAINER>::getFace (u32 i)
{
   faces[i].externalAttribs = &attribs;
   return faces[i];
}
template <class MESHATTRIBS, class MESHFACE, class MESHFACECONTAINER>
inline const MESHFACE& 
Mesh<MESHATTRIBS,MESHFACE,MESHFACECONTAINER>::getFace (u32 i) const
{
   faces[i].externalAttribs = &attribs;
   return faces[i];
}

template <class MESHATTRIBS, class MESHFACE, class MESHFACECONTAINER>
inline Mesh<MESHATTRIBS,MESHFACE,MESHFACECONTAINER>::Mesh()
{
}
template <class MESHATTRIBS, class MESHFACE, class MESHFACECONTAINER>
inline Mesh<MESHATTRIBS,MESHFACE,MESHFACECONTAINER>::~Mesh()
{
}

template <class MESHATTRIBS, class MESHFACE, class MESHFACECONTAINER>
inline u32 Mesh<MESHATTRIBS,MESHFACE,MESHFACECONTAINER>::size () const
{
   return faces.size();
}

END_GENVIS_NAMESPACE
#endif
