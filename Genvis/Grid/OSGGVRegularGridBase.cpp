//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.1 $
//   $Date: 2003/09/11 16:20:30 $
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
			    i32 vnx, i32 vny, i32 vnz)
{
   K6Dop::init(cen, hlx,hly,hlz);

   // for safety: add one voxel in each dimension
   m_num.setValues(vnx+1, vny+1, vnz+1);
   m_strides.setValues(1, m_num[0], m_num[0]*m_num[1]);
   m_numVoxels  = (i64)m_strides[2] * (i64)m_num[2];

   m_vh         .setValues(hlx/vnx,           hly/vny,           hlz/vnz);
   m_vlength    .setValues(2.0f*m_vh[0],      2.0f*m_vh[1],      2.0f*m_vh[2]);
   m_inv_vlength.setValues(1.0f/m_vlength[0], 1.0f/m_vlength[1], 1.0f/m_vlength[2]);

   m_vcen = cen - VectorClass(hlx, hly, hlz) + m_vh;
}
