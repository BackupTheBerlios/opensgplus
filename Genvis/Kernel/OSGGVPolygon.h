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

#ifndef POLYGON_H
#define POLYGON_H

#include <assert.h>
#include "OSGGVBase.h"
#include "OSGGVMesh.h"

BEGIN_GENVIS_NAMESPACE

// Polygon with stored points

/*! \brief Polygon base class.
 */
typedef  MeshFaceBase                       Polygon3Base;
/*! \brief Polygon set base class.
 */
typedef  Mesh<MeshAttribsBase,MeshFaceBase> Polygon3SetBase;

/*! \brief Polygon edge for polygon with stored points.
 */
class OSG_GENVISLIB_DLLMAPPING Polygon3Edge : public MeshEdgeBase
{ 
public: 
   PointClass coord;
};
/*! \brief Polygon with stored points.
 */
class OSG_GENVISLIB_DLLMAPPING Polygon3 : public MeshFace<MeshAttribsBase,Polygon3Edge>
{
public:
   Polygon3();
   ~Polygon3 ();

   PointClass  getPoint  (const MeshEdgeBase& e) const;
   PointClass  getPoint  (u32 i) const;
   VectorClass  getNormal (const MeshEdgeBase& e) const;
   VectorClass  getNormal (u32 i) const;
};
/*! \brief Polygon set with stored points.
 */
typedef Mesh<MeshAttribsBase,Polygon3> Polygon3Set;


// Polygon with indexed points

/*! \brief Polygon with indexed points.
 */
class OSG_GENVISLIB_DLLMAPPING Polygon3Attribs
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef std::vector<PointClass>  Container;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Data members.                                                */
   /*! \{                                                                 */
   Container points;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};
class Polygon3Indexed;
/*! \brief Polygon edge for polygon with indexed points.
 */
class OSG_GENVISLIB_DLLMAPPING Polygon3EdgeIndexed : public MeshEdgeBase
{ 
public: 
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef std::list<Polygon3EdgeIndexed> Container;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   inline Polygon3EdgeIndexed (Polygon3Indexed* face);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */
   inline u32 size () const;
   inline PointClass    getPoint  () const;
   inline VectorClass    getNormal () const;
   inline Polygon3EdgeIndexed* getOpposit() const;
   inline void                 setOpposit(Polygon3EdgeIndexed* e);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Navigation.                                                  */
   /*! \{                                                                 */
   inline Polygon3EdgeIndexed* getNext () const;
   inline Polygon3EdgeIndexed* getPrev () const;
   inline Polygon3EdgeIndexed* getCCW  () const;
   inline Polygon3EdgeIndexed* getCW   () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Construction/Deletion in face.                               */
   /*! \{                                                                 */
   Polygon3EdgeIndexed* newEdgeBefore ();
   Polygon3EdgeIndexed* newEdgeAfter  ();
   void                 erase  ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Data members.                                                */
   /*! \{                                                                 */
   u32             point;
   Polygon3EdgeIndexed* opp;
   Container::iterator  index;
   Polygon3Indexed*     face;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};
/*! \brief Polygon with indexed points.
 */
class OSG_GENVISLIB_DLLMAPPING Polygon3Indexed : public MeshFaceBase
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef std::list<Polygon3EdgeIndexed> Container;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   Polygon3Indexed ();
   ~Polygon3Indexed ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Construction/Deletion in face.                               */
   /*! \{                                                                 */
   Polygon3EdgeIndexed* firstEdge () const;
   Polygon3EdgeIndexed* getEdge (u32 i) const;
   Polygon3EdgeIndexed* newEdge ();
   Polygon3EdgeIndexed* newEdgeBefore (Polygon3EdgeIndexed* e);
   Polygon3EdgeIndexed* newEdgeAfter  (Polygon3EdgeIndexed* e);
   void                 erase   (Polygon3EdgeIndexed* e);
   void                 eraseAll();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */
   inline u32 size () const;
   PointClass  getPoint  (const MeshEdgeBase& e) const;
   PointClass  getPoint  (u32 i) const;
   VectorClass  getNormal (const MeshEdgeBase& e) const;
   VectorClass  getNormal (u32 i) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Data members.                                                */
   /*! \{                                                                 */
   Container        edges;
   Polygon3Attribs* externalAttribs;
   u32              index;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};
/*! \brief Polygon set with indexed points.
 */
class OSG_GENVISLIB_DLLMAPPING Polygon3SetIndexed
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef std::list<Polygon3Indexed>           Container;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   inline Polygon3SetIndexed ();
   virtual inline ~Polygon3SetIndexed ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */
   inline u32  size () const;
   Polygon3Indexed* getFace (u32 i) const;
   Polygon3Indexed* newFace ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Data members.                                                */
   /*! \{                                                                 */
   Polygon3Attribs attribs;
   Container       faces;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};


inline Polygon3EdgeIndexed::Polygon3EdgeIndexed (Polygon3Indexed* _face)
  : MeshEdgeBase()
{
   point = 0;
   opp   = NULL;
   face  = _face;
}
inline Polygon3EdgeIndexed* Polygon3EdgeIndexed::getOpposit() const
{
   return opp;
}
inline void  Polygon3EdgeIndexed::setOpposit(Polygon3EdgeIndexed* e)
{
   opp    = e;
   e->opp = this;
}
inline Polygon3EdgeIndexed* Polygon3EdgeIndexed::getNext () const
{
   Container::iterator it = index;
   if (++it == face->edges.end()) {
      it = face->edges.begin();
   }
   return it.operator->();
}
inline Polygon3EdgeIndexed* Polygon3EdgeIndexed::getPrev () const
{
   Container::iterator it = index;
   if (it == face->edges.begin()) {
      it = face->edges.end();
   }
   --it;
   return it.operator->();
}
inline Polygon3EdgeIndexed* Polygon3EdgeIndexed::getCCW  () const
{
   return getPrev()->getOpposit();
}
inline Polygon3EdgeIndexed* Polygon3EdgeIndexed::getCW  () const
{
   return getOpposit()->getNext();
}
inline PointClass  Polygon3EdgeIndexed::getPoint  () const
{
   return face->getPoint(*this);
}
inline VectorClass  Polygon3EdgeIndexed::getNormal () const
{
   return face->getNormal(*this);
}
inline void  Polygon3EdgeIndexed::erase ()
{
   Polygon3EdgeIndexed* opp = getOpposit();
   face->erase(this);
   opp->face->erase(opp);
}


inline u32 Polygon3Indexed::size () const
{
   return edges.size();
}
inline Polygon3EdgeIndexed* Polygon3Indexed::firstEdge () const
{
   return (Polygon3EdgeIndexed*)edges.begin().operator->();
}


inline Polygon3SetIndexed::Polygon3SetIndexed ()
{
}
inline Polygon3SetIndexed::~Polygon3SetIndexed ()
{
}
inline u32 Polygon3SetIndexed::size () const
{
   return faces.size();
}

END_GENVIS_NAMESPACE
#endif



