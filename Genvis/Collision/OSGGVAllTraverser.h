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
//   $Date: 2004/12/20 15:57:43 $
//                                                                            
//=============================================================================

#ifndef OSGGVALLTRAVERSER_H
#define OSGGVALLTRAVERSER_H

#include "OSGGVBase.h"
#include "OSGGVTraits.h"
#include "OSGGVStaticInput.h"
#include "OSGGVInput.h"
#include "OSGGVDoubleTraverser.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Base traverser for all-pairs collision detection.
 */
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING AllTraverserBase
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef typename BasicTraits::Cache          Cache;
   typedef typename BasicTraits::CacheData      CacheData;
   typedef typename BasicTraits::GeomObjectType GeomObjectType;
   typedef DoubleTraverserBase<BasicTraits>     DoubleTraverser;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline AllTraverserBase ();
   virtual inline ~AllTraverserBase ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Double traverser used for pairwise collision detection.      */
   /*! \{                                                                 */
   inline void                   setDoubleTraverser (DoubleTraverser* doubleTraverser);
   inline DoubleTraverser&       getDoubleTraverser ();
   inline const DoubleTraverser& getDoubleTraverser () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name DataBase for collision result.                               */
   /*! \{                                                                 */
   virtual inline DataBase<BasicTraits>&       getData ();
   virtual inline const DataBase<BasicTraits>& getData () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Apply.                                                       */
   /*! \{                                                                 */
   virtual bool apply   (const GeomObjectType& node0, const GeomObjectType& node1) = 0;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   static inline CacheData& getCacheData (CacheData& data, BVolAdapterBase* adapter);

   DoubleTraverser* m_doubleTraverser;
};
typedef  AllTraverserBase<OpenSGTraits>  OSGAllTraverser;

template <class BasicTraits>
inline AllTraverserBase<BasicTraits>::AllTraverserBase ()
  : m_doubleTraverser(NULL)
{
}
template <class BasicTraits>
inline AllTraverserBase<BasicTraits>::~AllTraverserBase ()
{
}

#if 0
template <class BasicTraits>
inline typename AllTraverserBase<BasicTraits>::Cache& 
AllTraverserBase<BasicTraits>::getCache ()
{
   return OSGCache::the();
}
#endif

template <class BasicTraits>
inline void AllTraverserBase<BasicTraits>::setDoubleTraverser (DoubleTraverser* doubleTraverser)
{
   m_doubleTraverser = doubleTraverser;
}

template <class BasicTraits>
inline typename AllTraverserBase<BasicTraits>::DoubleTraverser&       
AllTraverserBase<BasicTraits>::getDoubleTraverser ()
{
   assert(m_doubleTraverser != NULL);
   return *m_doubleTraverser;
}
template <class BasicTraits>
inline const typename AllTraverserBase<BasicTraits>::DoubleTraverser& 
AllTraverserBase<BasicTraits>::getDoubleTraverser () const
{
   assert(m_doubleTraverser != NULL);
   return *m_doubleTraverser;
}

template <class BasicTraits>
inline DataBase<BasicTraits>&       AllTraverserBase<BasicTraits>::getData ()
{
   return getDoubleTraverser().getData();
}
template <class BasicTraits>
inline const DataBase<BasicTraits>& AllTraverserBase<BasicTraits>::getData () const
{
   return getDoubleTraverser().getData();
}

template <class BasicTraits>
inline typename AllTraverserBase<BasicTraits>::CacheData& 
AllTraverserBase<BasicTraits>::getCacheData (CacheData& data, BVolAdapterBase* group)
{
   if (group->getParent() == NULL) { // no parent
      return data;
   }
   // triangle adapter
   OpenSGTriangleBase<BasicTraits>*  tri = 
     dynamic_cast<OpenSGTriangleBase<BasicTraits>*>(group->getParent());
   if (tri != NULL) {
      return tri->getObjectAdapter();
   }
   // object adapter
   OpenSGObjectBase<BasicTraits>* obj = 
     dynamic_cast<OpenSGObjectBase<BasicTraits>*>(group->getParent());
   if (obj != NULL) {
      return obj->getObjectAdapter();
   }
   // no adapter
   return data;
}


/*! \brief Traverser for all-pairs collision detection. 
           This class just calls the double traverser for each pair.
 */
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING AllTraverser : public AllTraverserBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef AllTraverserBase<BasicTraits>       Inherited;
   typedef typename Inherited::Cache           Cache;
   typedef typename Inherited::CacheData       CacheData;
   typedef typename Inherited::GeomObjectType  GeomObjectType;
   typedef typename Inherited::DoubleTraverser DoubleTraverser;

   typedef typename BasicTraits::TransformType TransformType;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline AllTraverser ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Apply.                                                       */
   /*! \{                                                                 */   
   virtual bool apply   (const GeomObjectType& node0, const GeomObjectType& node1);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};

template <class BasicTraits>
inline AllTraverser<BasicTraits>::AllTraverser ()
  : Inherited ()
{
}

END_GENVIS_NAMESPACE

#endif



