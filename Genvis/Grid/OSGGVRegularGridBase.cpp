//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.2 $
//   $Date: 2004/12/20 15:55:46 $
//                                                                            
//=============================================================================

#include "OSGGVRegularGridBase.h"
USING_GENVIS_NAMESPACE

RegularGridBase::RegularGridBase ()
  : K6Dop(), m_numVoxels(0)
{
}


RegularGridBase::~RegularGridBase ()
{
}

void RegularGridBase::init (const PointClass& cen,
			    Real hlx, Real hly, Real hlz,
			    u32 vnx, u32 vny, u32 vnz)
{
   K6Dop::init(cen, hlx,hly,hlz);

   // for safety: add one voxel in each dimension
   m_num.setValues(vnx+1, vny+1, vnz+1);
   m_strides.setValues(1, m_num[0], m_num[0]*m_num[1]);
   m_numVoxels  = (u64)m_strides[2] * (u64)m_num[2];

   m_vh         .setValues(hlx/vnx,           hly/vny,           hlz/vnz);
   m_vlength    .setValues(2.0f*m_vh[0],      2.0f*m_vh[1],      2.0f*m_vh[2]);
   m_inv_vlength.setValues(1.0f/m_vlength[0], 1.0f/m_vlength[1], 1.0f/m_vlength[2]);

   m_vcen = cen - VectorClass(hlx, hly, hlz) + m_vh;
}
