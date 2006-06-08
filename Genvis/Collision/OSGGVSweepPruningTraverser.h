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
//   $Date: 2006/06/08 17:07:39 $
//                                                                            
//=============================================================================

#ifndef OSGGVSWEEPPRUNINGTRAVERSER_H
#define OSGGVSWEEPPRUNINGTRAVERSER_H

#include "OSGGVBase.h"
#include "OSGGVInput.h"
#include "OSGGVPruningTraverser.h"

BEGIN_GENVIS_NAMESPACE

class SAPElement;
class SAPEndPoint;
class SAPBox;

class OSG_GENVISLIB_DLLMAPPING SAPPairData
{
public:
   typedef std::pair<u32, u32>       PairsPair;
   typedef std::vector<PairsPair>    PairsContainer;

   SAPPairData();
   ~SAPPairData();
   
   bool Init       (u32 nb_objects);
   
   void AddPair    (u32 id1, u32 id2);
   void RemovePair (u32 id1, u32 id2);
   
   void DumpPairs  (PairsContainer& pairs) const;
   inline u32          getNumPairs () const   { return mNbObjects; }
   inline SAPElement** getPairsArray () const { return mArray; }

private:
   // Internal methods
   SAPElement*	GetFreeElem (u32 id, SAPElement* next, u32* remap=NULL);
   void	        FreeElem    (SAPElement* elem);
   void		Release     ();
   
   u32		mNbElements;		//!< Total number of elements in the pool
   u32		mNbUsedElements;	//!< Number of used elements
   SAPElement*	mElementPool;		//!< Array of mNbElements elements
   SAPElement*	mFirstFree;		//!< First free element in the pool
   
   u32		mNbObjects;		//!< Max number of objects we can handle
   SAPElement**	mArray;			//!< Pointers to pool
};

/*! \brief Traverser for all-pairs collision detection. 
    This class prunes the list of all pairs
    (sorts the projection intervals of all participants with Radix sort) 
    and calls the double traverser for each pair of the pruning list.
 */
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING SweepPruningTraverser : public PruningTraverser<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef PruningTraverser<BasicTraits>       Inherited;
   typedef typename Inherited::Cache           Cache;
   typedef typename Inherited::CacheData       CacheData;
   typedef typename Inherited::GeomObjectType  GeomObjectType;
   typedef typename Inherited::DoubleTraverser DoubleTraverser;
   typedef typename BasicTraits::TransformType TransformType;

   typedef std::pair<u32, u32>                 PairsPair;
   typedef std::vector<PairsPair>              PairsContainer;
   typedef std::vector<Real>                   PosContainer;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline SweepPruningTraverser ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Apply.                                                       */
   /*! \{                                                                 */   
   virtual void update (const GeomObjectType& node);
   virtual bool apply  (const GeomObjectType& node0, const GeomObjectType& node1);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   /*---------------------------------------------------------------------*/
   /*! \name Pruning.                                                     */
   /*! \{                                                                 */   
   // Internal methods
   bool			CheckListsIntegrity();
   bool                 init (const typename CacheData::AdapterContainer& list0,
			      CacheData&                            data0,
			      const typename CacheData::AdapterContainer& list1,
			      CacheData&                            data1);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

   SAPPairData	  mPairs;   
   u32		  mNbObjects;
   SAPBox*	  mBoxes;
   SAPEndPoint*	  mList[3];
   CacheData*     m_data0;
   CacheData*     m_data1;
};
typedef SweepPruningTraverser<OpenSGTraits> OSGSweepPruningTraverser;


template <class BasicTraits>
inline SweepPruningTraverser<BasicTraits>::SweepPruningTraverser ()
  : Inherited (),
  mNbObjects(0),
  mBoxes(NULL),
  m_data0(NULL),
  m_data1(NULL)
{
   mList[0] = mList[1] = mList[2] = NULL;
}

END_GENVIS_NAMESPACE

#endif



