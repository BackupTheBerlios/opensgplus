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
//   $Date: 2004/12/20 15:56:35 $
//                                                                            
//=============================================================================

#ifndef OSGGVCACHEDATA_H
#define OSGGVCACHEDATA_H

#include <vector>
#include "OSGGVBase.h"
#include "OSGGVBVolAdapterBase.h"
#include "OSGNode.h"
#include "OSGMatrix.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief OpenSGCacheData.
 */
class OSG_GENVISLIB_DLLMAPPING OpenSGData
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef OpenSGData                    Self;
   typedef Self*                         SelfP;
   typedef Adapter*                      AdapterP;
   typedef MFNodePtr                     ChildContainer;
   typedef std::vector<BVolAdapterBaseP> AdapterVector;
   typedef AdapterVector                 AdapterContainer;
   typedef std::map<SelfP,AdapterContainer,std::less<SelfP> > ColCacheContainer;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   OpenSGData ();
   OpenSGData (const OSG::NodePtr& node, const OSG::Matrix& matrix);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Data of referenced scenegraph Node.                          */
   /*! \{                                                                 */
   OSG::Matrix        getToWorldMatrix () const;
   OSG::NodePtr       getOriginal() const;
   OSG::NodePtr       getNode    () const;
   OSG::NodePtr       getParent  () const;
   i32                getNumChildren () const;
   ChildContainer&    getChildren    () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Adapter matrices.                                            */
   /*! \{                                                                 */
   OSG::Matrix&       getAdapterMatrix (u32 id);
   const OSG::Matrix& getAdapterMatrix (u32 id) const;
   void               setAdapterMatrix (u32 id, const OSG::Matrix& m);
   OSG::Matrix&       getFrameMatrix ();
   const OSG::Matrix& getFrameMatrix () const;
   void               setFrameMatrix (const OSG::Matrix& m);
   
   void                      destroyAdapter (u32 id, u32 len = 0);
   void                      clearAdapter (u32 id, u32 len = 0);
   AdapterContainer&  getAdapter   (u32 id);
   const AdapterContainer& getAdapter   (u32 id) const;
   AdapterP&          getAdapter   (u32 id, u32 num);
   const AdapterP& getAdapter   (u32 id, u32 num) const;
   void               setAdapter   (u32 id, AdapterP a);
   void               setAdapter   (u32 id, const AdapterVector& a);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Collision cache.                                             */
   /*! \{                                                                 */
   void                    clearColCache ();
   AdapterContainer&       getColCache (Self& data);
   const AdapterContainer& getColCache (Self& data) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   OSG::NodePtr      m_node;
   AdapterContainer  m_array[GV_MAX_NUM_ADAPTER];
   OSG::Matrix       m_matrix[GV_MAX_NUM_ADAPTER];
   OSG::Matrix       m_frame;
   ColCacheContainer m_cache;
};

END_GENVIS_NAMESPACE
#endif




