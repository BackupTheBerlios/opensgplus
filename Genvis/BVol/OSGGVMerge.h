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
//   $Date: 2003/09/11 16:20:29 $
//                                                                            
//=============================================================================

#ifndef OSGGVMERGE_H
#define OSGGVMERGE_H

#include "OSGGVBase.h"
#include "OSGGVGroup.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Merge levels of bounding volume hierarchies. For example
    MergeLevels(1) on a binary hierarchy creates a 4-ary hierarchy.
 */
class OSG_GENVISLIB_DLLMAPPING MergeLevels 
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef BoundingVolume<Real> BVol;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   MergeLevels (unsigned level=1);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */
   inline unsigned getNumLevels () const;
   inline void     setNumLevels (unsigned level);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Merge.                                                       */
   /*! \}                                                                 */
   void     merge (BVolGroupBase* scene);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   /*---------------------------------------------------------------------*/
   /*! \name Internal.                                                    */
   /*! \}                                                                 */
   void merge   (BVolGroupBase* scene,
                 std::vector<BVolAdapterBase*>& addList,
                 unsigned level);
   bool compact (BVolGroupBase* parent, 
		 BVolAdapterBase* son1, 
		 BVolAdapterBase* son2);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

   unsigned m_level;
};

inline unsigned MergeLevels::getNumLevels () const
{
   return m_level;
}
inline void     MergeLevels::setNumLevels (unsigned level)
{
   m_level = level;
}

END_GENVIS_NAMESPACE
#endif
