//=============================================================================
//                                                                            
//                               OpenMesh                                     
//        Copyright (C) 2002 by Computer Graphics Group, RWTH Aachen          
//                           www.openmesh.org                                 
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
//   $Date: 2003/07/11 14:46:51 $
//                                                                            
//=============================================================================
#ifndef OSGOPENMESHSUPPORT_H
#define OSGOPENMESHSUPPORT_H

#include <OpenSG/OSGGeometry.h>
//#include <boost/progress.hpp>

// -------------------------------------------------------------------------------------------------

namespace osg
{   
   
   
   // -------------------------------------------- convert OpenMesh Geometry to OpenSG Geometry ----

   template<typename OM> GeometryPtr initGeometry(const OM& om);   
      
   
   // arbitrary polygonal mesh
   template<typename OM>
   GeometryPtr
   initGeometry(const OM& om)
   {
      //cout << "Polygons" << endl;
      
      GeoPositions3fPtr   points  = GeoPositions3f::create();
      GeoNormals3fPtr     normals = GeoNormals3f::create();
      GeoIndicesUI32Ptr   indices = GeoIndicesUI32::create();   
      GeoPTypesPtr        types   = GeoPTypesUI8::create();
      GeoPLengthsPtr      lengths = GeoPLengthsUI32::create();  
      
      GeoPositions3f::StoredFieldType *point  = points->getFieldPtr();
      GeoNormals3f::StoredFieldType   *normal = normals->getFieldPtr();
      GeoIndicesUI32::StoredFieldType *index  = indices->getFieldPtr();
      
      // fill data
      beginEditCP(points);
      beginEditCP(normals);
      beginEditCP(types);
      beginEditCP(indices);
      beginEditCP(lengths);
      
      std::map<typename OM::VertexHandle, unsigned int> indexMap;
      typename OM::ConstVertexIter v_it   = om.vertices_begin();
      typename OM::ConstVertexIter v_last = om.vertices_end();
      unsigned int omIndex = 0u;
      while( v_it != v_last ) // vertices
      {
         typename OM::Point const omPoint   = om.point( *v_it );
         typename OM::Point const omNormal  = om.vertex(v_it.handle()).normal();
         
         indexMap[v_it.handle()] = omIndex;
         point->addValue( Pnt3f(omPoint[0], omPoint[1], omPoint[2]) );
         normal->addValue( Vec3f(omNormal[0], omNormal[1], omNormal[2]) );
         ++v_it;
         ++omIndex;
      }
      
      typename OM::ConstFaceIter f_it   = om.faces_begin();
      typename OM::ConstFaceIter f_last = om.faces_end();
      unsigned int countTriangle = 0;
      unsigned int omType;

      typename OM::ConstFaceVertexIter fv_iter = om.cfv_iter(f_it.handle());
      while( f_it != f_last )
      {
         typename OM::ConstFaceVertexIter fv_iter = om.cfv_iter(f_it.handle());
         unsigned int countVertexPerFace = 0;
         for(; fv_iter; ++fv_iter)
         {
            index->addValue( indexMap[fv_iter.handle()] );
            ++countVertexPerFace;
         }
         types->addValue( countVertexPerFace==3 ? GL_TRIANGLES : GL_POLYGON );
         lengths->addValue( countVertexPerFace );
         ++f_it;
      }
      
      endEditCP(points);
      endEditCP(normals);
      endEditCP(types);
      endEditCP(indices);
      endEditCP(lengths);
      
      // setup geometry
      GeometryPtr pGeo = Geometry::create();
      
      pGeo->setPositions( points );
      pGeo->setNormals( normals );
      pGeo->getIndexMapping().addValue( Geometry::MapPosition | 
                                        Geometry::MapNormal );
      pGeo->setIndices( indices );   
      pGeo->setTypes( types );       
      pGeo->setLengths( lengths );   
      
      //      
      return pGeo;
   }
   

   
// -------------------------------------------------------------------------------------------------
} // end of namespace
// -------------------------------------------------------------------------------------------------
#endif
// =================================================================================================
