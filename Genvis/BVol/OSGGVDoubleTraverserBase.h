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
//   $Date: 2004/03/12 13:16:55 $
//                                                                            
//=============================================================================

#ifndef OSGGVDOUBLETRAVERSERBASE_H
#define OSGGVDOUBLETRAVERSERBASE_H

#include <assert.h>
#include "OSGGVBase.h"
#include "OSGGVTraits.h"
#include "OSGGVCollisionData.h"

BEGIN_GENVIS_NAMESPACE

template <class BasicTraits> class DoubleTraverserBase;

/*! \brief DataBase for double traversals.
 */
template <class BasicTraits>
class DataBase
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef typename BasicTraits::Cache      Cache;
   typedef typename BasicTraits::CacheData  CacheData;
   typedef DoubleTraverserBase<BasicTraits> DoubleTraverser;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline DataBase ();
   virtual inline ~DataBase ();
   virtual inline bool  Init ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Traverser.                                                   */
   /*! \{                                                                 */
   inline void                   setTraverser (const DoubleTraverser* trav);
   inline const DoubleTraverser* getTraverser () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Member.                                                      */
   /*! \{                                                                 */   
   inline void          setStream (std::ostream* os);
   inline std::ostream* getStream () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Dump.                                                        */
   /*! \{                                                                 */   
   virtual inline void  dump (std::ostream& os);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   std::ostream*          m_output;
   const DoubleTraverser* m_trav;
};

typedef DataBase<OpenSGTraits> OSGDataBase;


template <class BasicTraits>
inline DataBase<BasicTraits>::DataBase ()
  : m_output(NULL), m_trav(NULL)
{
}
template <class BasicTraits>
inline DataBase<BasicTraits>::~DataBase ()
{
}

template <class BasicTraits>
inline bool   DataBase<BasicTraits>::Init ()
{
   return true;
}
template <class BasicTraits>
inline void DataBase<BasicTraits>::setStream (std::ostream* os)
{
   m_output = os;
}
template <class BasicTraits>
inline std::ostream* DataBase<BasicTraits>::getStream () const
{
   return m_output;
}
template <class BasicTraits>
inline void DataBase<BasicTraits>::dump (std::ostream& os)
{
   os << "DataBase";
}

template <class BasicTraits>
inline void 
DataBase<BasicTraits>::setTraverser (const DoubleTraverser* t)
{
   m_trav = t;
}
template <class BasicTraits>
inline const typename DataBase<BasicTraits>::DoubleTraverser* 
DataBase<BasicTraits>::getTraverser () const
{
   return m_trav;
}

class OSG_GENVISLIB_DLLMAPPING DoubleTraverserInt
{
protected:
   static Adapter* s_dummy;
};


/*! \brief Base class for parallel traversal of two bounding volume hierarchies.
 */
template <class BasicTraits>
class DoubleTraverserBase 
: public DoubleTraverserInt
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef typename BasicTraits::Cache          Cache;
   typedef typename BasicTraits::CacheData      CacheData;
   typedef typename BasicTraits::GeomObjectType GeomObjectType;
   typedef typename BasicTraits::TransformType  TransformType;
   /*! Result type.                                                       */
   enum ResultType {
      QUIT,    // quit double dispatch
      CONTINUE // continue double dispatch
   };
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline DoubleTraverserBase ();
   virtual inline ~DoubleTraverserBase ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Member.                                                      */
   /*! \{                                                                 */   
   inline void   setUseCoherency (bool flag);
   inline bool   getUseCoherency () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name DataBase for collision result.                               */
   /*! \{                                                                 */
   virtual DataBase<BasicTraits>&       getData () = 0;
   virtual const DataBase<BasicTraits>& getData () const = 0;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Apply.                                                       */
   /*! \{                                                                 */ 
   virtual bool apply (const GeomObjectType& node0, const GeomObjectType& node1) = 0;
   virtual bool apply (CacheData& parent0, CacheData& data0, BVolAdapterBase* node0, 
		       CacheData& parent1, CacheData& data1, BVolAdapterBase* node1, 
		       const TransformType& m0 = TransformType::identity(),
		       const TransformType& m1 = TransformType::identity()) = 0;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   bool m_coherency;
};
typedef DoubleTraverserBase<OpenSGTraits>  OSGDoubleTraverser;

template <class BasicTraits>
inline DoubleTraverserBase<BasicTraits>::DoubleTraverserBase ()
  : m_coherency(false)
{
}
template <class BasicTraits>
inline DoubleTraverserBase<BasicTraits>::~DoubleTraverserBase ()
{
}

template <class BasicTraits>
inline void   DoubleTraverserBase<BasicTraits>::setUseCoherency (bool flag)
{
   m_coherency = flag;
}
template <class BasicTraits>
inline bool DoubleTraverserBase<BasicTraits>::getUseCoherency () const
{
   return m_coherency;
}

END_GENVIS_NAMESPACE

#endif



