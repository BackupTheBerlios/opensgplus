/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *               Copyright (C) 2000-2002 by the OpenSG Forum                 *
 *                                                                           *
 *                            www.opensg.org                                 *
 *                                                                           *
 *   contact: dirk@opensg.org, gerrit.voss@vossg.org, jbehr@zgdv.de          *
 *                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 *                                License                                    *
 *                                                                           *
 * This library is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU Library General Public License as published    *
 * by the Free Software Foundation, version 2.                               *
 *                                                                           *
 * This library is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public         *
 * License along with this library; if not, write to the Free Software       *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
 *                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>

#include "OSGGeometryClustered.h"
#include <OSGConfig.h>
#include <OSGGL.h>
#include <OSGGLEXT.h>
#include <OSGAction.h>
#include <OSGDrawAction.h>
#include <OSGRenderAction.h>
#include <OSGIntersectAction.h>
#include <OSGRenderAction.h>
#include <OSGGeoFunctions.h>
#include <OSGViewport.h>
#include <OSGCamera.h>
#include <OSGTriangleIterator.h>

OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

static const u32 sizePool = 3;
#undef GV_COLORPRIMITIVES

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::GeometryClustered

*/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

void GeometryClustered::initMethod (void)
{
    DrawAction::registerEnterDefault(getClassType(), 
        osgTypedMethodFunctor2BaseCPtrRef<Action::ResultE, MaterialDrawablePtr, 
              CNodePtr, Action *>(&MaterialDrawable::drawActionHandler));

    IntersectAction::registerEnterDefault(getClassType(), 
        osgTypedMethodFunctor2BaseCPtrRef<Action::ResultE, GeometryPtr,  
              CNodePtr, Action *>(&Geometry::intersect));

    RenderAction::registerEnterDefault(getClassType(), 
        osgTypedMethodFunctor2BaseCPtrRef<Action::ResultE, MaterialDrawablePtr,  
              CNodePtr, Action *>(&MaterialDrawable::renderActionHandler));
}


/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  private                                                                 -
\*-------------------------------------------------------------------------*/

/*----------------------- constructors & destructors ----------------------*/

GeometryClustered::GeometryClustered(void) :
    Inherited()
{
}

GeometryClustered::GeometryClustered(const GeometryClustered &source) :
    Inherited(source)
{
}

GeometryClustered::~GeometryClustered(void)
{
}

enum DataElem {
   UNKNOWN_DE = 0,

   VERTEX_DE, VERTEX_TEXTURECOORD_DE, VERTEX_NORMAL_DE,
   FACE_DE,
   
   LIB_MTL_DE,
   USE_MTL_DE,
   
   GROUP_DE, SMOOTHING_GROUP_DE, OBJECT_DE
};
static std::map<std::string, DataElem> _dataElemMap;
static void fillDataElemMap ()
{
   if (_dataElemMap.empty()) {
      _dataElemMap[""]        = UNKNOWN_DE;

      _dataElemMap["v"]       = VERTEX_DE;
      _dataElemMap["vt"]      = VERTEX_TEXTURECOORD_DE;
      _dataElemMap["vn"]      = VERTEX_NORMAL_DE;
      _dataElemMap["f"]       = FACE_DE;
      _dataElemMap["fo"]      = FACE_DE;
      _dataElemMap["mtllib"]  = LIB_MTL_DE;
      _dataElemMap["usemtl"]  = USE_MTL_DE;
      _dataElemMap["g"]       = GROUP_DE;
      _dataElemMap["s"]       = SMOOTHING_GROUP_DE;
      _dataElemMap["o"]       = OBJECT_DE;
   }
}

void GeometryClustered::fillPositions (std::istream& is)
{
   fillDataElemMap();

   Real32 x,y,z;
   Pnt3f  vec3f;
   Vec2f  vec2f;
   DataElem dataElem;
   Char8 strBuf[8192], *token, *nextToken;
   Int32 strBufSize = sizeof(strBuf)/sizeof(Char8);
   Int32 index, posIndex, indexType;
   Int32 i,j,n;
   std::string elem;
   std::map<std::string, DataElem>::const_iterator elemI;
   GeoPositionsPtr coordPtr = GeoPositions3f::create();
   if (is) {
      beginEditCP(coordPtr);

      for (is >> elem; !is.eof(); is >> elem) {
         if (elem[0] == '#' || elem[0] == '$') {
           is.ignore(INT_MAX, '\n');
	 } else {
	   elemI = _dataElemMap.find(elem);
	   dataElem = ((elemI == _dataElemMap.end()) ? UNKNOWN_DE : elemI->second );
	   switch (dataElem) {
              case OBJECT_DE:
              case GROUP_DE:
              case SMOOTHING_GROUP_DE:
                is.ignore(INT_MAX, '\n');
                break;
              case VERTEX_DE:
                is >> x >> y >> z;
                vec3f.setValues(x,y,z);
                coordPtr->push_back(vec3f);
                break;
              case VERTEX_TEXTURECOORD_DE:
                is >> x >> y;
                vec2f.setValues(x,y);
                break;
              case VERTEX_NORMAL_DE:
                is >> x >> y >> z;
                break;
              case LIB_MTL_DE:
                is >> elem;
                is.ignore(INT_MAX, '\n');
                break;
              case USE_MTL_DE:
                is >> elem;
                is.ignore(INT_MAX, '\n');
                break;
              case FACE_DE:
              case UNKNOWN_DE:
              default:
                FWARNING (( "fillPositions: Unknown obj data elem: %s\n", elem.c_str()));
                is.ignore(INT_MAX, '\n');
                break;
              }
	 }
	 if (is.peek() == 'f') {
	    break;
	 }
      }
      endEditCP(coordPtr);
      getOffsetFaces() = is.tellg();

      // positions cache
      GeometryClusteredPtr tmpPtr(this);
      beginEditCP(tmpPtr, PositionsFieldMask);
      tmpPtr->setPositions(coordPtr);
      endEditCP  (tmpPtr, PositionsFieldMask);
   }
}

/*----------------------------- class specific ----------------------------*/

void GeometryClustered::changed(BitVector whichField, UInt32 origin)
{
   if (whichField & ModelFilenameFieldMask) {
      _fileStream.open(getModelFilename().c_str(), std::ios_base::in | std::ios_base::binary);
      fillPositions(_fileStream);
   }
   if (whichField & NumCellsFieldMask) {
      delete getGrid();
      setGrid(NULL);
   }
   // invalidate the dlist cache
   if (getDlistCache()) {
       if (getGLId() == 0) {
	  GeometryClusteredPtr tmpPtr(this);
	  beginEditCP(tmpPtr, GLIdFieldMask);
	  setGLId(Window::registerGLObject(
		  osgTypedMethodVoidFunctor2ObjCPtrPtr<GeometryClusteredPtr,
		  Window,
		  UInt32>(tmpPtr, &GeometryClustered::handleGL),
		  1));
	  endEditCP(tmpPtr, GLIdFieldMask);
       }
       Window::refreshGLObject(getGLId());
   } else {
       if (getGLId() != 0) {
	  Window::destroyGLObject(getGLId(), 1);
       }
       setGLId(0);
   }
   Inherited::changed(whichField, origin);
}
void GeometryClustered::onCreate (const GeometryClustered*)
{
   // if we're in startup this is the prototype, which shouldn't have an id
   if (GlobalSystemState == Startup)
      return;

   GeometryClusteredPtr tmpPtr(this);
   beginEditCP(tmpPtr, GLIdFieldMask);
   setGLId(Window::registerGLObject(
	   osgTypedMethodVoidFunctor2ObjCPtrPtr<GeometryClusteredPtr,
	   Window,
	   UInt32>(tmpPtr, &GeometryClustered::handleGL),
	   1));
   endEditCP(tmpPtr, GLIdFieldMask);
}

void GeometryClustered::dump(      UInt32    , 
                         const BitVector ) const
{
    SLOG << "Dump GeometryClustered NI" << std::endl;
}


void GeometryClustered::fillGrid ()
{
#ifndef GV_CLUSTERED_ADAPTIVE
#if 1
   TriangleIterator end = endTriangles();
   for (TriangleIterator tri = beginTriangles(); 
	tri!=end; 
	++tri) {
      // fetch grid cells
      CellData& cell0 = getGrid()->primitives(tri.getPosition(0));
      CellData& cell1 = getGrid()->primitives(tri.getPosition(1));
      CellData& cell2 = getGrid()->primitives(tri.getPosition(2));

      Vec3f norm = tri.getPosition(0).subZero().cross(tri.getPosition(1).subZero())
	           +tri.getPosition(1).subZero().cross(tri.getPosition(2).subZero())
	           +tri.getPosition(2).subZero().cross(tri.getPosition(0).subZero());
      // update cell data
      cell0->getNormal() += norm;
      if (&cell0 != &cell1) {
	 cell1->getNormal() += norm;
      }
      if (&cell0 != &cell2 && &cell1 != &cell2) {
	 cell2->getNormal() += norm;
      }
      cell0->setNumPoints(cell0->getNumPoints() + 1);
      cell0->getPointRep() += tri.getPosition(0).subZero();
      cell1->setNumPoints(cell1->getNumPoints() + 1);
      cell1->getPointRep() += tri.getPosition(1).subZero();
      cell2->setNumPoints(cell2->getNumPoints() + 1);
      cell2->getPointRep() += tri.getPosition(2).subZero();
   }
   Real32 slen;
   for (i64 i=0; i<getGrid()->getNumVoxels(); ++i) {
      CellData& cell = getGrid()->primitives(i);
      if (cell->getNumPoints() > 0) { // calc center
	 cell->getPointRep() *= 1.0f/cell->getNumPoints();
	 slen = getNormalScale() / cell->getNormal().length();
	 cell->getNormal() *= slen;
      }
   }
#else
   UInt32 i, j;
   TriangleIterator end = endTriangles();
   for (TriangleIterator tri = beginTriangles(); 
	tri!=end; 
	++tri) {
      // fetch grid cells
      CellData& cell0 = getGrid()->primitives(tri.getPosition(0));
      CellData& cell1 = getGrid()->primitives(tri.getPosition(1));
      CellData& cell2 = getGrid()->primitives(tri.getPosition(2));
      Vec3f  n2(tri.getPosition(0).subZero().cross(tri.getPosition(1).subZero())
                +tri.getPosition(1).subZero().cross(tri.getPosition(2).subZero())
                +tri.getPosition(2).subZero().cross(tri.getPosition(0).subZero()));
      Matrix triQuad(tri.getPosition(0), tri.getPosition(1), tri.getPosition(2));
      Vec4f  n(n2[0], n2[1], n2[2], -triQuad.det3());
      for (i=0; i<4; ++i) {
	 for (j=0; j<4; ++j) {
	    triQuad[i][j] = n[i]*n[j];
	 }
      }
      // update cell data
      cell0->getQuadric().add(triQuad);
      cell0->getNormal() += n2;
      cell0->setNumPoints(cell0->getNumPoints()+1);
      if (&cell0 != &cell1) {
	 cell1->getQuadric().add(triQuad);
	 cell1->getNormal() += n2;
	 cell1->setNumPoints(cell1->getNumPoints()+1);
      }
      if (&cell0 != &cell2 && &cell1 != &cell2) {
	 cell2->getQuadric().add(triQuad);
	 cell2->getNormal() += n2;
	 cell2->setNumPoints(cell2->getNumPoints()+1);
      }
   }
   Matrix invQuad;
   Vec3f  b; 
   K6Dop  cellBox;
   u32    k;
   for (u32 x=0; x<getGrid()->getNumVoxelsDim()[0]; ++x) {
      for (u32 y=0; y<getGrid()->getNumVoxelsDim()[1]; ++y) {
	 for (u32 z=0; z<getGrid()->getNumVoxelsDim()[2]; ++z) {
	   CellData& cell = getGrid()->primitives(x, y, z);
	   if (cell->getNumPoints() > 0) { // calc center
	      cell->getNormal().normalize();
	      bool invertable = invQuad.invertFrom3(cell->getQuadric());
	      assert(invertable);
	      b.setValues(-cell->getQuadric()[3][0], -cell->getQuadric()[3][1], -cell->getQuadric()[3][2]);
	      invQuad.multVecMatrix(b, (Vec3f&)cell->getPointRep());
#if 1
	      getGrid()->getVoxel(cellBox, x, y, z);
	      if ((k=cellBox.checkNotIntersect(cell->getPointRep()))) { // problem case
		 --k;
	 	 cell->getPointRep()[k%3] = stdClamp(cellBox.minVector()[k], cell->getPointRep()[k], cellBox.maxVector()[k]);
		 if (k%3 < 1) {
		    cell->getPointRep()[1] = stdClamp(cellBox.minVector()[1], cell->getPointRep()[1], cellBox.maxVector()[1]);
		 }
		 if (k%3 < 2) {
		    cell->getPointRep()[2] = stdClamp(cellBox.minVector()[2], cell->getPointRep()[2], cellBox.maxVector()[2]);
		 }
	      }
#else
	      getGrid()->getVoxel(cellBox, x, y, z);
	      if ((k=cellBox.checkNotIntersect(cell->getPointRep()))) { // problem case
		 do {
		   Vec3f t = cell->getPointRep() - cellBox.getCenter();
		   k -= 1;
		   if (k < 3) {
		     SLOG << "+" << (cell->getPointRep()[k]-cellBox.maxVector()[k])/t[k] << std::endl;
		     t *= (cell->getPointRep()[k]-cellBox.maxVector()[k])/t[k];
		     cell->getPointRep() -= t;
		   } else {
		     k -= 3;
		     SLOG << "+" << (cell->getPointRep()[k]-cellBox.minVector()[k])/t[k] << std::endl;
		     t *= (cell->getPointRep()[k]-cellBox.minVector()[k])/t[k];
		     cell->getPointRep() -= t;
		   }
		 } while ((k=cellBox.checkNotIntersect(cell->getPointRep())));
	      }
#endif
	   }
	 }
      }
   }
#endif
#else
   UInt32 j; 
   TriangleIterator end = endTriangles();
   for (TriangleIterator tri = beginTriangles(); 
	tri!=end; 
	++tri) {
      Vec3f norm = tri.getPosition(0).subZero().cross(tri.getPosition(1).subZero())
                   +tri.getPosition(1).subZero().cross(tri.getPosition(2).subZero())
                   +tri.getPosition(2).subZero().cross(tri.getPosition(0).subZero());
      // fetch grid cells
      SetUnionGrid::CellType* intern0 = getGrid()->getLeafCell(tri.getPosition(0));
      // sort into lowest three levels
      for (j=0; j<3 && intern0 != NULL; intern0 = intern0->getParent(), ++j) {
	 // create new set element
	 CellData& cell = intern0->getContainer();
	 // update cell data
	 cell->setNumPoints(cell->getNumPoints() + 1);
	 cell->getPointRep() += tri.getPosition(0).subZero();
	 cell->getNormal()   += norm;
      } 
      SetUnionGrid::CellType* intern1 = getGrid()->getLeafCell(tri.getPosition(1));
      for (j=0; j<3 && intern1 != NULL; intern1 = intern1->getParent(), ++j) {
	 // create new set element
	 CellData& cell = intern1->getContainer();
	 // update cell data
	 cell->setNumPoints(cell->getNumPoints() + 1);
	 cell->getPointRep() += tri.getPosition(1).subZero();
	 cell->getNormal()   += norm;
      } 
      SetUnionGrid::CellType* intern2 = getGrid()->getLeafCell(tri.getPosition(2));
      for (j=0; j<3 && intern2 != NULL; intern2 = intern2->getParent(), ++j) {
	 // create new set element
	 CellData& cell = intern2->getContainer();
	 // update cell data
	 cell->setNumPoints(cell->getNumPoints() + 1);
	 cell->getPointRep() += tri.getPosition(2).subZero();
	 cell->getNormal()   += norm;
      } 
   }
   // traverse all grid cells
   Pnt3f center;
   getParents()[0]->getVolume().getCenter(center);
   Real32 slen;
   u32 maxCode  = getGrid()->getMaxCode();
   for (u32 xCode=0; xCode<maxCode; ++xCode) {
      for (u32 yCode=0; yCode<maxCode; ++yCode) {
	 for (u32 zCode=0; zCode<maxCode; ++zCode) {
	   SetUnionGrid::CellType* intern = getGrid()->getCell(xCode, yCode, zCode, 0);
	   CellData& cell = intern->getContainer();
	   if (cell->getNumPoints() > 0) { // calc center
	      cell->getPointRep() *= 1.0f/cell->getNumPoints();
	      //cell->setNumPoints(0);
	      slen = getNormalScale() / cell->getNormal().length();
	      cell->getNormal() *= slen;
	   } 
	   if ((xCode & 0x1) == 0 && (yCode & 0x1) == 0 && (zCode & 0x1) == 0) {
	      intern = intern->getParent();
	      CellData& cell = intern->getContainer();
	      if (cell->getNumPoints() > 0) { // calc center
		 cell->getPointRep() *= 1.0f/cell->getNumPoints();
		 //cell->setNumPoints(0);
		 slen = getNormalScale() / cell->getNormal().length();
		 cell->getNormal() *= slen;
	      }
	      if ((xCode & 0x3) == 0 && (yCode & 0x3) == 0 && (zCode & 0x3) == 0) {
		 intern = intern->getParent();
		 CellData& cell = intern->getContainer();
		 if (cell->getNumPoints() > 0) { // calc center
		    cell->getPointRep() *= 1.0f/cell->getNumPoints();
		    //cell->setNumPoints(0);
		    slen = getNormalScale() / cell->getNormal().length();
		    cell->getNormal() *= slen;
		 }
	      }
	   }
	 }
      }
   }
#endif
}

void GeometryClustered::fillGridFile (std::istream& is)
{
   fillDataElemMap();
#ifndef GV_CLUSTERED_ADAPTIVE
   if (is) {
      Real32 x,y,z;
      DataElem dataElem;
      Char8 strBuf[8192], *token, *nextToken;
      Int32 strBufSize = sizeof(strBuf)/sizeof(Char8);
      Int32 index, posIndex, indexType;
      Int32 i, j, n, primCount[3];
      Int32 tri[3];
      std::string elem;
      std::map<std::string, DataElem>::const_iterator elemI;

      for (is >> elem; !is.eof(); is >> elem) {
         if (elem[0] == '#' || elem[0] == '$') {
           is.ignore(INT_MAX, '\n');
	 } else {
	   elemI    = _dataElemMap.find(elem);
	   dataElem = ((elemI == _dataElemMap.end()) ? UNKNOWN_DE : elemI->second );
	   switch (dataElem) {
              case OBJECT_DE:
              case GROUP_DE:
              case SMOOTHING_GROUP_DE:
                is.ignore(INT_MAX, '\n');
                break;
              case VERTEX_DE:
                primCount[0]++;
                is >> x >> y >> z;
                break;
              case VERTEX_TEXTURECOORD_DE:
                primCount[1]++;
                is >> x >> y;
                break;
              case VERTEX_NORMAL_DE:
                primCount[2]++;
                is >> x >> y >> z;
                break;
              case LIB_MTL_DE:
                is >> elem;
                is.ignore(INT_MAX, '\n');
                break;
              case USE_MTL_DE:
                is >> elem;
                is.ignore(INT_MAX, '\n');
                break;
              case FACE_DE:
                is.get(strBuf,strBufSize);
                
                indexType=0; n=0;
                for (token = strBuf; token && *token; token = nextToken) {
                    for (; *token == '/'; token++)
                      indexType++;
                    for (; isspace(*token); token++)
                      indexType = 0;
                    index = strtol(token, &nextToken, 10);
                    if (token == nextToken)
                      break;

                    if (index > 0)
                      index--;
                    else
                      index =  primCount[indexType] + index;

                    if (indexType == 0) { // position index
	 	       tri[n++] = index;
		       if (n == 3) {
			  Vec3f n2 = getPositions()->getValue(tri[0]).subZero().cross(getPositions()->getValue(tri[1]).subZero())
			             +getPositions()->getValue(tri[1]).subZero().cross(getPositions()->getValue(tri[2]).subZero())
                                     +getPositions()->getValue(tri[2]).subZero().cross(getPositions()->getValue(tri[0]).subZero());
			  // fetch grid cells
			  CellData& cell0 = getGrid()->primitives(getPositions()->getValue(tri[0]));
			  CellData& cell1 = getGrid()->primitives(getPositions()->getValue(tri[1]));
			  CellData& cell2 = getGrid()->primitives(getPositions()->getValue(tri[2]));
			  // update cell data
			  cell0->getNormal() += n2;
			  if (&cell0 != &cell1) {
			    cell1->getNormal() += n2;
			  }
			  if (&cell0 != &cell2 && &cell1 != &cell2) {
			    cell2->getNormal() += n2;
			  }
			  cell0->setNumPoints(cell0->getNumPoints() + 1);
			  cell0->getPointRep() += getPositions()->getValue(tri[0]).subZero();
			  cell1->setNumPoints(cell1->getNumPoints() + 1);
			  cell1->getPointRep() += getPositions()->getValue(tri[1]).subZero();
			  cell2->setNumPoints(cell2->getNumPoints() + 1);
			  cell2->getPointRep() += getPositions()->getValue(tri[2]).subZero();
			  n = 0;
		       }
		    }
		}
                break;
              case UNKNOWN_DE:
              default:
                FWARNING (( "fillGridFile: Unknown obj data elem: %s\n", elem.c_str()));
                is.ignore(INT_MAX, '\n');
                break;
              }
	 }
      }
      // traverse all grid cells
      Real32 slen;
      for (i64 i=0; i<getGrid()->getNumVoxels(); ++i) {
	 CellData& cell = getGrid()->primitives(i);
	 if (cell->getNumPoints() > 0) { // calc center
	   cell->getPointRep() *= 1.0f/cell->getNumPoints();
	   slen = getNormalScale() / cell->getNormal().length();
	   cell->getNormal() *= slen;
	 }
      }
   }
#else
   if (is) {
      Real32 x,y,z;
      DataElem dataElem;
      Char8 strBuf[8192], *token, *nextToken;
      Int32 strBufSize = sizeof(strBuf)/sizeof(Char8);
      Int32 index, posIndex, indexType;
      Int32 i, j, n, primCount[3];
      Int32 tri[3];
      std::string elem;
      std::map<std::string, DataElem>::const_iterator elemI;

      for (is >> elem; !is.eof(); is >> elem) {
         if (elem[0] == '#' || elem[0] == '$') {
           is.ignore(INT_MAX, '\n');
	 } else {
	   elemI    = _dataElemMap.find(elem);
	   dataElem = ((elemI == _dataElemMap.end()) ? UNKNOWN_DE : elemI->second );
	   switch (dataElem) {
              case OBJECT_DE:
              case GROUP_DE:
              case SMOOTHING_GROUP_DE:
                is.ignore(INT_MAX, '\n');
                break;
              case VERTEX_DE:
                primCount[0]++;
                is >> x >> y >> z;
                break;
              case VERTEX_TEXTURECOORD_DE:
                primCount[1]++;
                is >> x >> y;
                break;
              case VERTEX_NORMAL_DE:
                primCount[2]++;
                is >> x >> y >> z;
                break;
              case LIB_MTL_DE:
                is >> elem;
                is.ignore(INT_MAX, '\n');
                break;
              case USE_MTL_DE:
                is >> elem;
                is.ignore(INT_MAX, '\n');
                break;
              case FACE_DE:
                is.get(strBuf,strBufSize);
                
                indexType=0; n=0;
                for (token = strBuf; token && *token; token = nextToken) {
                    for (; *token == '/'; token++)
                      indexType++;
                    for (; isspace(*token); token++)
                      indexType = 0;
                    index = strtol(token, &nextToken, 10);
                    if (token == nextToken)
                      break;

                    if (index > 0)
                      index--;
                    else
                      index =  primCount[indexType] + index;

                    if (indexType == 0) { // position index
		       if (index >= getPositions()->getSize()) {
			  SWARNING << "read " << is.tellg() << ": index out of range!" << std::endl;
			  continue;
		       }
	 	       tri[n++] = index;
		       if (n == 3) {
			  Vec3f n2 = getPositions()->getValue(tri[0]).subZero().cross(getPositions()->getValue(tri[1]).subZero())
                                     +getPositions()->getValue(tri[1]).subZero().cross(getPositions()->getValue(tri[2]).subZero())
                                     +getPositions()->getValue(tri[2]).subZero().cross(getPositions()->getValue(tri[0]).subZero());
			  // fetch grid cells
			  SetUnionGrid::CellType* intern0 = getGrid()->getLeafCell(getPositions()->getValue(tri[0]));
			  // sort into lowest three levels
			  for (j=0; j<3 && intern0 != NULL; intern0 = intern0->getParent(), ++j) {
			    // create new set element
			    CellData& cell = intern0->getContainer();
			    // update cell data
			    cell->setNumPoints(cell->getNumPoints() + 1);
			    cell->getPointRep() += getPositions()->getValue(tri[0]).subZero();
			    cell->getNormal()   += n2;
			  } 
			  SetUnionGrid::CellType* intern1 = getGrid()->getLeafCell(getPositions()->getValue(tri[1]));
			  for (j=0; j<3 && intern1 != NULL; intern1 = intern1->getParent(), ++j) {
			    // create new set element
			    CellData& cell = intern1->getContainer();
			    // update cell data
			    cell->setNumPoints(cell->getNumPoints() + 1);
			    cell->getPointRep() += getPositions()->getValue(tri[1]).subZero();
			    cell->getNormal()   += n2;
			  } 
			  SetUnionGrid::CellType* intern2 = getGrid()->getLeafCell(getPositions()->getValue(tri[2]));
			  for (j=0; j<3 && intern2 != NULL; intern2 = intern2->getParent(), ++j) {
			    // create new set element
			    CellData& cell = intern2->getContainer();
			    // update cell data
			    cell->setNumPoints(cell->getNumPoints() + 1);
			    cell->getPointRep() += getPositions()->getValue(tri[2]).subZero();
			    cell->getNormal()   += n2;
			  } 
			  n = 0;
		       }
		    }
		}
                break;
              case UNKNOWN_DE:
              default:
                FWARNING (( "fillGridFile: Unknown obj data elem: %s\n", elem.c_str()));
                is.ignore(INT_MAX, '\n');
                break;
              }
	 }
      }
      Real32 slen;
      // traverse all grid cells
      // CF could be done recursively
      u32 maxCode  = getGrid()->getMaxCode();
      for (u32 xCode=0; xCode<maxCode; ++xCode) {
	 for (u32 yCode=0; yCode<maxCode; ++yCode) {
	    for (u32 zCode=0; zCode<maxCode; ++zCode) {
	       SetUnionGrid::CellType* intern = getGrid()->getCell(xCode, yCode, zCode, 0);
	       CellData& cell = intern->getContainer();
	       if (cell->getNumPoints() > 0) { // calc center
		  cell->getPointRep() *= 1.0f/cell->getNumPoints();
		  //cell->setNumPoints(0);
		  slen = getNormalScale() / cell->getNormal().length();
		  cell->getNormal() *= slen;
	       } 
	       if ((xCode & 0x1) == 0 && (yCode & 0x1) == 0 && (zCode & 0x1) == 0) {
		  intern = intern->getParent();
		  CellData& cell = intern->getContainer();
		  if (cell->getNumPoints() > 0) { // calc center
		     cell->getPointRep() *= 1.0f/cell->getNumPoints();
		     //cell->setNumPoints(0);
		     slen = getNormalScale() / cell->getNormal().length();
		     cell->getNormal() *= slen;
		  }
		  if ((xCode & 0x3) == 0 && (yCode & 0x3) == 0 && (zCode & 0x3) == 0) {
		     intern = intern->getParent();
		     CellData& cell = intern->getContainer();
		     if (cell->getNumPoints() > 0) { // calc center
		       cell->getPointRep() *= 1.0f/cell->getNumPoints();
		       //cell->setNumPoints(0);
		       slen = getNormalScale() / cell->getNormal().length();
		       cell->getNormal() *= slen;
		     }
		  }
	       }
	    }
	 }
      }
   }
#endif
}


// GV_CLUSTERED_ADAPTIVE: always use pool 0
CellData* GeometryClustered::getPoolEntry (UInt32 i, bool omit)
{
#ifdef GV_CLUSTERED_ADAPTIVE
   SetUnionGrid::CellType* intern = getGrid()->getLeafCell(getPositions()->getValue(i));
   CellData& cell = intern->getContainer();
   return &cell;
#else
   CellData& cell = getGrid()->primitives(getPositions()->getValue(i));
   return &cell;
#endif
}
// GV_CLUSTERED_ADAPTIVE: always use pool 0
const CellData* GeometryClustered::getPoolEntry (UInt32 i) const
{
#ifdef GV_CLUSTERED_ADAPTIVE
   SetUnionGrid::CellType* intern = getGrid()->getLeafCell(getPositions()->getValue(i));
   return &(intern->getContainer());
#else
   const CellData& cell = getGrid()->primitives(getPositions()->getValue(i));
   return &cell;
#endif
}
// GV_CLUSTERED_ADAPTIVE: choose pool by distance from eye
CellData* GeometryClustered::getPoolEntry (const Pnt3f& eye, 
					   Real32 minDist, Real32 maxDist, 
					   UInt32 i, 
					   bool omit)
{
#ifdef GV_CLUSTERED_ADAPTIVE
# if 1
   SetUnionGrid::CellType* intern = getGrid()->getLeafCell(getPositions()->getValue(i));
   //SLOG << intern->getContainer()->getNumPoints() << "/" << intern->getParent()->getContainer()->getNumPoints() << "/" << intern->getParent()->getParent()->getContainer()->getNumPoints() << std::endl;
   if (intern->getContainer()->getNumPoints() < 8*intern->getParent()->getContainer()->getNumPoints()) {
      CellData& cell = intern->getContainer();
      return &cell;
   }
   intern = intern->getParent();
   if (intern->getContainer()->getNumPoints() < 8*intern->getParent()->getContainer()->getNumPoints()) {
      CellData& cell = intern->getContainer();
      return &cell;
   }
   intern = intern->getParent();
   CellData& cell = intern->getContainer();
   return &cell;
# else
   Int32 pindex;
#if 0
   Real32 dist = (getPositions()->getValue(i)-eye).squareLength();
   pindex = Int32(log10(dist))-Int32(log10(maxDist-minDist));
   if (pindex < 0) {
     pindex = 0;
   } 
   if (dist >= sizePool) {
     pindex = sizePool-1;
   }
#else
   Real32 dist = (getPositions()->getValue(i)-eye).squareLength();
   pindex = Int32(sizePool*(dist-minDist)/(maxDist-minDist));
#endif
   assert(pindex < 3);
   SetUnionGrid::CellType* intern = getGrid()->getLeafCell(getPositions()->getValue(i));
   if (pindex >= 1) {
      intern = intern->getParent();
   }
   if (pindex >= 2) {
      intern = intern->getParent();
   }
   CellData& cell = intern->getContainer();
   return &cell;
# endif
#else
   CellData& cell = getGrid()->primitives(getPositions()->getValue(i));
   return &cell;
#endif
}
// GV_CLUSTERED_ADAPTIVE: choose pool by distance from eye
const CellData* GeometryClustered::getPoolEntry (const Pnt3f& eye, 
						 Real32 minDist, Real32 maxDist, 
						 UInt32 i) const
{
#ifdef GV_CLUSTERED_ADAPTIVE
# if 1
   SetUnionGrid::CellType* intern = getGrid()->getLeafCell(getPositions()->getValue(i));
   //SLOG << intern->getContainer()->getNumPoints() << "/" << intern->getParent()->getContainer()->getNumPoints() << "/" << intern->getParent()->getParent()->getContainer()->getNumPoints() << std::endl;
   if (intern->getContainer()->getNumPoints() < 8*intern->getParent()->getContainer()->getNumPoints()) {
      CellData& cell = intern->getContainer();
      return &cell;
   }
   intern = intern->getParent();
   if (intern->getContainer()->getNumPoints() < 8*intern->getParent()->getContainer()->getNumPoints()) {
      CellData& cell = intern->getContainer();
      return &cell;
   }
   intern = intern->getParent();
   CellData& cell = intern->getContainer();
   return &cell;
# else
   Int32 pindex;
#if 0
   Real32 dist = (getPositions()->getValue(i)-eye).squareLength();
   pindex = Int32(log10(dist))-Int32(log10(maxDist-minDist));
   if (pindex < 0) {
     pindex = 0;
   } 
   if (dist >= sizePool) {
     pindex = sizePool-1;
   }
#else
   Real32 dist = (getPositions()->getValue(i)-eye).squareLength();
   pindex = Int32(sizePool*(dist-minDist)/(maxDist-minDist));
#endif
   assert(pindex < 3);
   SetUnionGrid::CellType* intern = getGrid()->getLeafCell(getPositions()->getValue(i));
   if (pindex >= 1) {
      intern = intern->getParent();
   }
   if (pindex >= 2) {
      intern = intern->getParent();
   }
   return &(intern->getContainer());
# endif
#else
   CellData& cell = getGrid()->primitives(getPositions()->getValue(i));
   return &cell;
#endif
}


typedef void (OSG_APIENTRY *pumpFunc)(GLubyte * data);
typedef void (OSG_APIENTRY *multiPumpFunc)(GLenum which, GLubyte * data);


// Some helper arrays for function selection
// indexed by data type and dimension

static const int formatBase = GL_BYTE;
static const int numFormats = GL_DOUBLE - GL_BYTE + 1;

static char *formatNames[] =
{   "GL_BYTE", "GL_UNSIGNED_BYTE", "GL_SHORT", "GL_UNSIGNED_SHORT",
    "GL_INT", "GL_UNSIGNED_INT", "GL_FLOAT", "GL_2_BYTES",
    "GL_3_BYTES", "GL_4_BYTES", "GL_DOUBLE"
};

// little helper class for function init

class glextFuncInit
{
    public:

        glextFuncInit(char *name, UInt32 format, UInt32 dim) :
            _name(name), _format(format), _dim(dim) {};

        void init(UInt32 (&extids)[numFormats][4])
        {
            extids[_format - formatBase][_dim] =
                                        Window::registerFunction(_name);
        }

    private:

        char   *_name;
        UInt32  _format;
        UInt32  _dim;
};

static glextFuncInit secondaryColorInitFuncs[8] = {
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glSecondaryColor3bvEXT",
                  GL_BYTE,
                  3),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glSecondaryColor3ubvEXT",
                  GL_UNSIGNED_BYTE,
                  3),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glSecondaryColor3svEXT",
                  GL_SHORT,
                  3),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glSecondaryColor3usvEXT",
                  GL_UNSIGNED_SHORT,
                  3),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glSecondaryColor3ivEXT",
                  GL_INT,
                  3),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glSecondaryColor3uivEXT",
                  GL_UNSIGNED_INT,
                  3),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glSecondaryColor3fvEXT",
                  GL_FLOAT,
                  3),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glSecondaryColor3dvEXT",
                  GL_DOUBLE,
                  3)
};

static glextFuncInit multiTexCoordsInitFuncs[16] = {
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord1svARB",
                  GL_SHORT,
                  1),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord2svARB",
                  GL_SHORT,
                  2),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord3svARB",
                  GL_SHORT,
                  3),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord4svARB",
                  GL_SHORT,
                  4),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord1ivARB",
                  GL_INT,
                  1),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord2ivARB",
                  GL_INT,
                  2),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord3ivARB",
                  GL_INT,
                  3),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord4ivARB",
                  GL_INT,
                  4),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord1fvARB",
                  GL_FLOAT,
                  1),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord2fvARB",
                  GL_FLOAT,
                  2),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord3fvARB",
                  GL_FLOAT,
                  3),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord4fvARB",
                  GL_FLOAT,
                  4),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord1dvARB",
                  GL_DOUBLE,
                  1),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord2dvARB",
                  GL_DOUBLE,
                  2),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord3dvARB",
                  GL_DOUBLE,
                  3),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord4dvARB",
                  GL_DOUBLE,
                  4)
};

// The real pumping functions

static pumpFunc PositionFuncs[numFormats][4] = {
    { NULL, NULL, NULL, NULL },                           // GL_BYTE
    { NULL, NULL, NULL, NULL },                           // GL_UNSIGNED_BYTE
    { NULL, (pumpFunc)glVertex2sv,
      (pumpFunc)glVertex3sv, (pumpFunc)glVertex4sv },     // GL_SHORT
    { NULL, NULL, NULL, NULL },                           // GL_UNSIGNED_SHORT
    { NULL, (pumpFunc)glVertex2iv,
      (pumpFunc)glVertex3iv, (pumpFunc)glVertex4iv },     // GL_INT
    { NULL, NULL, NULL, NULL },                           // GL_UNSIGNED_INT
    { NULL, (pumpFunc)glVertex2fv,
      (pumpFunc)glVertex3fv, (pumpFunc)glVertex4fv },     // GL_FLOAT
    { NULL, NULL, NULL, NULL },                           // GL_2_BYTES
    { NULL, NULL, NULL, NULL },                           // GL_3_BYTES
    { NULL, NULL, NULL, NULL },                           // GL_4_BYTES
    { NULL, (pumpFunc)glVertex2dv,
      (pumpFunc)glVertex3dv, (pumpFunc)glVertex4dv },     // GL_DOUBLE
};

static pumpFunc NormalFuncs[numFormats][4] = {
    { NULL, NULL, (pumpFunc)glNormal3sv, NULL },          // GL_BYTE
    { NULL, NULL, NULL, NULL },                           // GL_UNSIGNED_BYTE
    { NULL, NULL, (pumpFunc)glNormal3sv, NULL },          // GL_SHORT
    { NULL, NULL, NULL, NULL },                           // GL_UNSIGNED_SHORT
    { NULL, NULL, (pumpFunc)glNormal3iv, NULL },          // GL_INT
    { NULL, NULL, NULL, NULL },                           // GL_UNSIGNED_INT
    { NULL, NULL, (pumpFunc)glNormal3fv, NULL },          // GL_FLOAT
    { NULL, NULL, NULL, NULL },                           // GL_2_BYTES
    { NULL, NULL, NULL, NULL },                           // GL_3_BYTES
    { NULL, NULL, NULL, NULL },                           // GL_4_BYTES
    { NULL, NULL, (pumpFunc)glNormal3dv, NULL },          // GL_DOUBLE
};

static pumpFunc ColorFuncs[numFormats][4] = {
    { NULL, NULL,
      (pumpFunc)glColor3bv, (pumpFunc)glColor4bv },       // GL_BYTE
    { NULL, NULL,
      (pumpFunc)glColor3ubv, (pumpFunc)glColor4ubv },     // GL_UNSIGNED_BYTE
    { NULL, NULL,
      (pumpFunc)glColor3sv, (pumpFunc)glColor4sv },       // GL_SHORT
    { NULL, NULL,
      (pumpFunc)glColor3usv, (pumpFunc)glColor4usv },     // GL_UNSIGNED_SHORT
    { NULL, NULL,
      (pumpFunc)glColor3iv, (pumpFunc)glColor4iv },       // GL_INT
    { NULL, NULL,
      (pumpFunc)glColor3uiv, (pumpFunc)glColor4uiv },     // GL_UNSIGNED_INT
    { NULL, NULL,
      (pumpFunc)glColor3fv, (pumpFunc)glColor4fv },       // GL_FLOAT
    { NULL, NULL, NULL, NULL },                           // GL_2_BYTES
    { NULL, NULL, NULL, NULL },                           // GL_3_BYTES
    { NULL, NULL, NULL, NULL },                           // GL_4_BYTES
    { NULL, NULL,
      (pumpFunc)glColor3dv, (pumpFunc)glColor4dv },       // GL_DOUBLE
};

static UInt32 SecColorIDs[numFormats][4];

static pumpFunc TexCoordsFuncs[numFormats][4] = {
    { NULL, NULL, NULL, NULL },                           // GL_BYTE
    { NULL, NULL, NULL, NULL },                           // GL_UNSIGNED_BYTE
    { (pumpFunc)glTexCoord1sv, (pumpFunc)glTexCoord2sv,
      (pumpFunc)glTexCoord3sv, (pumpFunc)glTexCoord4sv }, // GL_SHORT
    { NULL, NULL, NULL, NULL },                           // GL_UNSIGNED_SHORT
    { (pumpFunc)glTexCoord1iv, (pumpFunc)glTexCoord2iv,
      (pumpFunc)glTexCoord3iv, (pumpFunc)glTexCoord4iv }, // GL_INT
    { NULL, NULL, NULL, NULL },                           // GL_UNSIGNED_INT
    { (pumpFunc)glTexCoord1fv, (pumpFunc)glTexCoord2fv,
      (pumpFunc)glTexCoord3fv, (pumpFunc)glTexCoord4fv }, // GL_FLOAT
    { NULL, NULL, NULL, NULL },                           // GL_2_BYTES
    { NULL, NULL, NULL, NULL },                           // GL_3_BYTES
    { NULL, NULL, NULL, NULL },                           // GL_4_BYTES
    { (pumpFunc)glTexCoord1dv, (pumpFunc)glTexCoord2dv,
      (pumpFunc)glTexCoord3dv, (pumpFunc)glTexCoord4dv }, // GL_DOUBLE
};

static UInt32 TexCoords1IDs[numFormats][4];

#define TexCoords2IDs TexCoords1IDs
#define TexCoords3IDs TexCoords1IDs

#define pumpSetup( name, typename, getmethod )                              \
    typename name##Ptr;                                                     \
    GLubyte * name##Data = NULL;                                            \
    UInt32 name##Stride;                                                    \
                                                                            \
    name##Ptr = geo->getmethod();                                           \
    if ( name##Ptr != NullFC )                                              \
    {                                                                       \
        name##Data = name##Ptr->getData();                                  \
        if ( ! ( name##Stride = name##Ptr->getStride() ) )                  \
            name##Stride = name##Ptr->getFormatSize() *                     \
            name##Ptr->getDimension();                                      \
    }

#define pumpInternalSetup( name, typename, getmethod, mandatory )           \
    GLubyte * name##Data;                                                   \
    UInt32 name##Stride;                                                    \
    UInt32 name##Ind = 0;                                                   \
    typename name##Ptr;                                                     \
                                                                            \
    name##Ptr = geo->getmethod();                                           \
    if(mandatory && name##Ptr == NullFC)                                    \
    {                                                                       \
        SWARNING << "masterPump: Geometry " << geo << " has no "            \
                 << #name << "s!" << std::endl;                             \
        return;                                                             \
    }                                                                       \
    else if(name##Ptr != NullFC)                                            \
    {                                                                       \
        name##Data = name##Ptr->getData();                                  \
        if(!(name##Stride = name##Ptr->getStride()))                        \
            name##Stride =  name##Ptr->getFormatSize() *                    \
                            name##Ptr->getDimension();                      \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        name##Data = NULL;                                                  \
        name##Stride = 0;                                                   \
    }

#define pumpGLSetup( name, typename, getmethod )                            \
    GLubyte * name##Data;                                                   \
    UInt32 name##Stride;                                                    \
    UInt32 name##Ind = 0;                                                   \
    typename name##Ptr;                                                     \
    pumpFunc name##Func;                                                    \
                                                                            \
    name##Ptr = geo->getmethod();                                           \
    if(name##Ptr != NullFC)                                                 \
    {                                                                       \
        name##Data = name##Ptr->getData();                                  \
        if(!(name##Stride = name##Ptr->getStride()))                        \
            name##Stride = name##Ptr->getFormatSize() *                     \
            name##Ptr->getDimension();                                      \
        if(!(name##Func = name##Funcs[name##Ptr->getFormat() -              \
                                            formatBase]                     \
                                     [name##Ptr->getDimension() - 1]        \
          ) )                                                               \
        {                                                                   \
            SWARNING << "masterPump: Geometry " << geo << " has illegal "   \
                     << #name << "s: " << name##Ptr->getDimension()         \
                     << "D " << formatNames[ name##Ptr->getFormat() -       \
                                            formatBase ]                    \
                     << "!" << std::endl;                                   \
            return;                                                         \
        }                                                                   \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        name##Data = NULL;                                                  \
        name##Func = NULL;                                                  \
        name##Stride = 0;                                                   \
    }

#define pumpGLExtSetup( name, typename, getmethod )                         \
    GLubyte * name##Data;                                                   \
    UInt32 name##Stride;                                                    \
    UInt32 name##Ind = 0;                                                   \
    typename name##Ptr;                                                     \
    pumpFunc name##Func;                                                    \
                                                                            \
    name##Ptr = geo->getmethod();                                           \
    if(name##Ptr != NullFC)                                                 \
    {                                                                       \
        name##Data = name##Ptr->getData();                                  \
        if(!(name##Stride = name##Ptr->getStride()))                        \
            name##Stride = name##Ptr->getFormatSize() *                     \
            name##Ptr->getDimension();                                      \
        if(name##IDs[name##Ptr->getFormat() - formatBase]                   \
                    [name##Ptr->getDimension() - 1] ==                      \
                    Window::invalidFunctionID)                              \
        {                                                                   \
            SWARNING << "masterPump: Geometry " << geo << " has illegal "   \
                     << #name << "s: " << name##Ptr->getDimension()         \
                     << "D " << formatNames[ name##Ptr->getFormat() -       \
                                            formatBase ]                    \
                     << "!" << std::endl;                                   \
            return;                                                         \
        }                                                                   \
        if(!(name##Func = (pumpFunc)win->getFunction(name##IDs              \
                                     [name##Ptr->getFormat() - formatBase]  \
                                     [name##Ptr->getDimension() - 1])       \
          ) )                                                               \
        {                                                                   \
            SWARNING << "masterPump: Geometry " << geo << " uses "          \
                     << #name << "s: " << name##Ptr->getDimension()         \
                     << "D " << formatNames[ name##Ptr->getFormat() -       \
                                            formatBase ]                    \
                     << " which are not supported by Window " << win        \
                     <<  "!" << std::endl;                                  \
            return;                                                         \
        }                                                                   \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        name##Data = NULL;                                                  \
        name##Func = NULL;                                                  \
        name##Stride = 0;                                                   \
    }

#define pumpMultiGLExtSetup(name, typename, getmethod)                      \
    GLubyte * name##Data;                                                   \
    UInt32 name##Stride;                                                    \
    UInt32 name##Ind = 0;                                                   \
    typename name##Ptr;                                                     \
    multiPumpFunc name##Func;                                               \
                                                                            \
    name##Ptr = geo->getmethod();                                           \
    if(name##Ptr != NullFC)                                                 \
    {                                                                       \
        name##Data = name##Ptr->getData();                                  \
        if(!(name##Stride = name##Ptr->getStride() ) )                      \
            name##Stride = name##Ptr->getFormatSize() *                     \
            name##Ptr->getDimension();                                      \
        if(name##IDs[name##Ptr->getFormat() - formatBase]                   \
                    [name##Ptr->getDimension() - 1] ==                      \
                    Window::invalidFunctionID)                              \
        {                                                                   \
            SWARNING << "masterPump: Geometry " << geo << "has illegal "    \
                     << #name << "s: " << name##Ptr->getDimension()         \
                     << "D " << formatNames[ name##Ptr->getFormat() -       \
                                            formatBase ]                    \
                     << "!" << std::endl;                                   \
            return;                                                         \
        }                                                                   \
        if(!(name##Func = (multiPumpFunc)win->getFunction(name##IDs         \
                                     [name##Ptr->getFormat() - formatBase]  \
                                     [name##Ptr->getDimension() - 1])       \
          ) )                                                               \
        {                                                                   \
            SWARNING << "masterPump: Geometry " << geo << "uses "           \
                     << #name << "s: " << name##Ptr->getDimension()         \
                     << "D " << formatNames[ name##Ptr->getFormat() -       \
                                            formatBase ]                    \
                     << "which are not supported by Window " << win         \
                     <<  "!" << std::endl;                                  \
            return;                                                         \
        }                                                                   \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        name##Data = NULL;                                                  \
        name##Func = NULL;                                                  \
        name##Stride = 0;                                                   \
    }


// OpenGL IM Pump for models from files
// if getNumCells() < 2 then draw model without using grid
// else draw model using grid
static void fileGeoPump (std::istream& is, UInt32& ntri,
			 Window* win, Viewport* port, GeometryClustered* geo, 
			 Matrix wcMatrix, const Pnt3f& minBound, const Pnt3f& maxBound)
{
   fillDataElemMap();

   CameraPtr cam = port->getCamera();
   wcMatrix.invert();
   wcMatrix.mult(cam->getBeacon()->getToWorld());
   // calc eye point and center point
   Int32 tri[3];
   Pnt3f  eye (wcMatrix[3][0], wcMatrix[3][1], wcMatrix[3][2]); 
   Vec3f  diff = maxBound-minBound;
   Pnt3f  center = minBound + 0.5f*diff;
   Real32 d = 2*diff.squareLength();
   Real32 minDist = (center-eye).squareLength();
   Real32 maxDist = minDist + d;
   minDist = (minDist > d ? minDist - d : 0);
   // Setup: get all the data
   pumpGLSetup        ( Position,   GeoPositionsPtr, getPositions      );
   Real32 nlen = 0.05f*diff.length();

   // set triangle count zero
   ntri = 0;

   // if it's not empty we need positions
   if (!PositionData) {
        SWARNING << "fileGeoPump: Geometry " << geo << " has no positions!?!"
                 << std::endl;
        return;
   }

   bool omit = true;
   const CellData* set[3];
   // store color on entry
   srand(4711);
   GLfloat current[4];
   glGetFloatv(GL_CURRENT_COLOR, current);

   if (is) {
      Real32 x,y,z;
      DataElem dataElem;
      Char8 strBuf[8192], *token, *nextToken;
      Int32 strBufSize = sizeof(strBuf)/sizeof(Char8);
      Int32 index, posIndex, indexType;
      Int32 i, j, n;
      Int32 primCount[3];
      std::string elem;
      std::map<std::string, DataElem>::const_iterator elemI;

      CellData::firstFrame();
      if (geo->getNumCells() < 2) {
	 glBegin(GL_TRIANGLES);
	 for (is >> elem; !is.eof(); is >> elem) {
	   if (elem[0] == '#' || elem[0] == '$') {
	     is.ignore(INT_MAX, '\n');
	   } else {
	     elemI    = _dataElemMap.find(elem);
	     dataElem = ((elemI == _dataElemMap.end()) ? UNKNOWN_DE : elemI->second );
	     switch (dataElem) {
	     case OBJECT_DE:
	     case GROUP_DE:
	     case SMOOTHING_GROUP_DE:
	       is.ignore(INT_MAX, '\n');
	       break;
	     case VERTEX_DE:
	       primCount[0]++;
	       is >> x >> y >> z;
	       break;
	     case VERTEX_TEXTURECOORD_DE:
	       primCount[1]++;
	       is >> x >> y;
	       break;
	     case VERTEX_NORMAL_DE:
	       primCount[2]++;
	       is >> x >> y >> z;
	       break;
	     case LIB_MTL_DE:
	       is >> elem;
	       is.ignore(INT_MAX, '\n');
	       break;
	     case USE_MTL_DE:
	       is >> elem;
	       is.ignore(INT_MAX, '\n');
	       break;
	     case FACE_DE:
	       is.get(strBuf,strBufSize);
	       CellData::nextFrame ();
#ifdef GV_COLORPRIMITIVES
	       glColor3f(GLfloat(rand())/RAND_MAX, 
			 GLfloat(rand())/RAND_MAX, 
			 GLfloat(rand())/RAND_MAX);
#endif
	       indexType = 0; n = 0;
	       for (token = strBuf; token && *token; token = nextToken) {
		 for (; *token == '/'; token++)
		   indexType++;
		 for (; isspace(*token); token++)
		   indexType = 0;
		 index = strtol(token, &nextToken, 10);
		 if (token == nextToken)
		   break;
		 
		 if (index > 0)
		   index--;
		 else
		   index =  primCount[indexType] + index;

		 if (indexType == 0) { // position index
		   if (index >= geo->getPositions()->getSize()) {
		     SWARNING << "read " << is.tellg() << ": index out of range!" << std::endl;
		     continue;
		   }
		   tri[n++] = index;
		   if (n == 3) {
		      ntri++; 
		      Vec3f normal((geo->getPositions()->getValue(tri[2])-geo->getPositions()->getValue(tri[0]))
				   .cross(geo->getPositions()->getValue(tri[1])-geo->getPositions()->getValue(tri[0])));
		      glNormal3fv(normal.getValues());
		      glVertex3fv(geo->getPositions()->getValue(tri[0]).getValues());
		      glVertex3fv(geo->getPositions()->getValue(tri[1]).getValues());
		      glVertex3fv(geo->getPositions()->getValue(tri[2]).getValues());
		      n = 0;
		   }
		 }
	       }
	       break;
	     case UNKNOWN_DE:
	     default:
	       FWARNING (( "fileGeoPump: Unknown obj data elem: %s\n", elem.c_str()));
	       is.ignore(INT_MAX, '\n');
	       break;
	     }
	   }
	 }
	 glEnd();
      } else {
	 glBegin(GL_TRIANGLES);
	 for (is >> elem; !is.eof(); is >> elem) {
	   if (elem[0] == '#' || elem[0] == '$') {
	     is.ignore(INT_MAX, '\n');
	   } else {
	     elemI    = _dataElemMap.find(elem);
	     dataElem = ((elemI == _dataElemMap.end()) ? UNKNOWN_DE : elemI->second );
	     switch (dataElem) {
	     case OBJECT_DE:
	     case GROUP_DE:
	     case SMOOTHING_GROUP_DE:
	       is.ignore(INT_MAX, '\n');
	       break;
	     case VERTEX_DE:
	       primCount[0]++;
	       is >> x >> y >> z;
	       break;
	     case VERTEX_TEXTURECOORD_DE:
	       primCount[1]++;
	       is >> x >> y;
	       break;
	     case VERTEX_NORMAL_DE:
	       primCount[2]++;
	       is >> x >> y >> z;
	       break;
	     case LIB_MTL_DE:
	       is >> elem;
	       is.ignore(INT_MAX, '\n');
	       break;
	     case USE_MTL_DE:
	       is >> elem;
	       is.ignore(INT_MAX, '\n');
	       break;
	     case FACE_DE:
	       is.get(strBuf,strBufSize);
	       CellData::nextFrame ();
#ifdef GV_COLORPRIMITIVES
	       glColor3f(GLfloat(rand())/RAND_MAX, 
			 GLfloat(rand())/RAND_MAX, 
			 GLfloat(rand())/RAND_MAX);
#endif
	       indexType = 0; n = 0;
	       for (token = strBuf; token && *token; token = nextToken) {
		 for (; *token == '/'; token++)
		   indexType++;
		 for (; isspace(*token); token++)
		   indexType = 0;
		 index = strtol(token, &nextToken, 10);
		 if (token == nextToken)
		   break;
		 
		 if (index > 0)
		   index--;
		 else
		   index =  primCount[indexType] + index;

		 if (indexType == 0) { // position index
		   if (index >= geo->getPositions()->getSize()) {
		     SWARNING << "read " << is.tellg() << ": index out of range!" << std::endl;
		     continue;
		   }
		   const CellData* s = geo->getPoolEntry(eye,minDist,maxDist, index, omit);
		   if (!omit || s->isValid()) {
		      set[n++] = s;
		   }
		   if (n == 3) {
		     ntri++; 
		     glNormal3fv((*(set[0]))->getNormal().getValues());
		     glVertex3fv((*(set[0]))->getPointRep().getValues());
		     glNormal3fv((*(set[1]))->getNormal().getValues());
		     glVertex3fv((*(set[1]))->getPointRep().getValues());
		     glNormal3fv((*(set[2]))->getNormal().getValues());
		     glVertex3fv((*(set[2]))->getPointRep().getValues());
		     n = 0;
		   }
		 }
	       }
	       break;
	     case UNKNOWN_DE:
	     default:
	       FWARNING (( "fileGeoPump: Unknown obj data elem: %s\n", elem.c_str()));
	       is.ignore(INT_MAX, '\n');
	       break;
	     }
	   }
	 }
	 glEnd();

#if 0
	 is.clear();
	 is.seekg(geoBeginFaces);
	 glColor3f(0.0f, 1.0f, 0.0f);
	 CellData::firstFrame();
	 glBegin(GL_LINES);
	 for (is >> elem; !is.eof(); is >> elem) {
	   if (elem[0] == '#' || elem[0] == '$') {
	     is.ignore(INT_MAX, '\n');
	   } else {
	     elemI    = _dataElemMap.find(elem);
	     dataElem = ((elemI == _dataElemMap.end()) ? UNKNOWN_DE : elemI->second );
	     switch (dataElem) {
	     case OBJECT_DE:
	     case GROUP_DE:
	     case SMOOTHING_GROUP_DE:
	       is.ignore(INT_MAX, '\n');
	       break;
	     case VERTEX_DE:
	       primCount[0]++;
	       is >> x >> y >> z;
	       break;
	     case VERTEX_TEXTURECOORD_DE:
	       primCount[1]++;
	       is >> x >> y;
	       break;
	     case VERTEX_NORMAL_DE:
	       primCount[2]++;
	       is >> x >> y >> z;
	       break;
	     case LIB_MTL_DE:
	       is >> elem;
	       is.ignore(INT_MAX, '\n');
	       break;
	     case USE_MTL_DE:
	       is >> elem;
	       is.ignore(INT_MAX, '\n');
	       break;
	     case FACE_DE:
	       is.get(strBuf,strBufSize);
	       CellData::nextFrame ();
#ifdef GV_COLORPRIMITIVES
	       glColor3f(GLfloat(rand())/RAND_MAX, 
			 GLfloat(rand())/RAND_MAX, 
			 GLfloat(rand())/RAND_MAX);
#endif
	       indexType = 0; n = 0;
	       for (token = strBuf; token && *token; token = nextToken) {
		 for (; *token == '/'; token++)
		   indexType++;
		 for (; isspace(*token); token++)
		   indexType = 0;
		 index = strtol(token, &nextToken, 10);
		 if (token == nextToken)
		   break;
		 
		 if (index > 0)
		   index--;
		 else
		   index =  primCount[indexType] + index;

		 if (indexType == 0) { // position index
		   if (index >= geo->getPositions()->getSize()) {
		     SWARNING << "read " << is.tellg() << ": index out of range!" << std::endl;
		     continue;
		   }
		   const CellData* s = geo->getPoolEntry(eye,minDist,maxDist, index, omit);
		   if (!omit || s->isValid()) {
		      set[n++] = s;
		   }
		   if (n == 3) {
		     Pnt3f p = (*(set[0]))->getPointRep();
		     glVertex3fv(p.getValues());
		     p += nlen*(*(set[0]))->getNormal();
		     glVertex3fv(p.getValues());
		     p = (*(set[1]))->getPointRep();
		     glVertex3fv(p.getValues());
		     p += nlen*(*(set[1]))->getNormal();
		     glVertex3fv(p.getValues());
		     p = (*(set[2]))->getPointRep();
		     glVertex3fv(p.getValues());
		     p += nlen*(*(set[2]))->getNormal();
		     glVertex3fv(p.getValues());
		     n = 0;
		   }
		 }
	       }
	       break;
	     case UNKNOWN_DE:
	     default:
	       FWARNING (( "fileGeoPump: Unknown obj data elem: %s\n", elem.c_str()));
	       is.ignore(INT_MAX, '\n');
	       break;
	     }
	   }
	 }
	 glEnd();
#endif
      }
      // restore color on exit
      glColor3fv(current);
   }
}

// OpenGL IM Pump for Geometry models
static void masterGeoPump (UInt32& ntri,
			   Window* win, Viewport* port, GeometryClustered* geo, 
			   Matrix wcMatrix, const Pnt3f& minBound, const Pnt3f& maxBound)
{
    static const Real32 sqrt2 = 1.0f/sqrt(2.0f);

    CameraPtr cam = port->getCamera();
    wcMatrix.invert();
    wcMatrix.mult(cam->getBeacon()->getToWorld());
    // calc eye point and center point
    Pnt3f  eye (wcMatrix[3][0], wcMatrix[3][1], wcMatrix[3][2]); 
    Vec3f  diff = maxBound-minBound;
    Pnt3f  center = minBound + 0.5f*diff;
    Real32 d = 2*diff.squareLength();
    Real32 minDist = (center-eye).squareLength();
    Real32 maxDist = minDist + d;
    minDist = (minDist > d ? minDist - d : 0);
    // Setup: get all the data
    pumpInternalSetup( Type,   GeoPTypesPtr,   getTypes,   true  );
    pumpInternalSetup( Length, GeoPLengthsPtr, getLengths, false );
    pumpInternalSetup( Index,  GeoIndicesPtr,  getIndices, false );

    pumpGLSetup        ( Position,   GeoPositionsPtr, getPositions      );
    pumpGLSetup        ( Color,      GeoColorsPtr,    getColors         );
    pumpGLExtSetup     ( SecColor,   GeoColorsPtr,    getSecondaryColors);
    pumpGLSetup        ( Normal,     GeoNormalsPtr,   getNormals        );
    pumpGLSetup        ( TexCoords,  GeoTexCoordsPtr, getTexCoords      );
    pumpMultiGLExtSetup( TexCoords1, GeoTexCoordsPtr, getTexCoords1     );
    pumpMultiGLExtSetup( TexCoords2, GeoTexCoordsPtr, getTexCoords2     );
    pumpMultiGLExtSetup( TexCoords3, GeoTexCoordsPtr, getTexCoords3     );

    // set triangle count zero
    ntri = 0;

    // check if the node is empty
    if (!TypeData || TypePtr->getSize() == 0) {
        return;
    }
    // if it's not empty we need positions
    if (!PositionData) {
        SWARNING << "masterPump: Geometry " << geo << " has no positions!?!"
                 << std::endl;
        return;
    }
    // find the mapping indices
    UInt16 nmappings = geo->getIndexMapping().size();
    Int16 PositionIndex   = -1,
          NormalIndex     = -1,
          ColorIndex      = -1,
          SecColorIndex   = -1,
          TexCoordsIndex  = -1,
          TexCoords1Index = -1,
          TexCoords2Index = -1,
          TexCoords3Index = -1;

    if (nmappings > 0) { // multi-indexed
        PositionIndex   = geo->calcMappingIndex(Geometry::MapPosition      );
        NormalIndex     = geo->calcMappingIndex(Geometry::MapNormal        );
        ColorIndex      = geo->calcMappingIndex(Geometry::MapColor         );
        SecColorIndex   = geo->calcMappingIndex(Geometry::MapSecondaryColor);
        TexCoordsIndex  = geo->calcMappingIndex(Geometry::MapTexCoords     );
        TexCoords1Index = geo->calcMappingIndex(Geometry::MapTexCoords1    );
        TexCoords2Index = geo->calcMappingIndex(Geometry::MapTexCoords2    );
        TexCoords3Index = geo->calcMappingIndex(Geometry::MapTexCoords3    );
    } else if (IndexData) { // single-indexed
        nmappings = 1;
        PositionIndex =
        NormalIndex =
        ColorIndex =
        SecColorIndex =
        TexCoordsIndex =
        TexCoords1Index =
        TexCoords2Index =
        TexCoords3Index = 0;
    }

    // overall color?
    if (ColorData && ColorPtr->getSize() == 1) {
        ColorFunc(ColorData);
    }

    // Length handling. Special case: no length given

    UInt32 lendummy;
    UInt32 LengthSize;

    // no lengths? use all available data for the first type
    if (!LengthData) {
        LengthSize = 1;
        LengthData = (UChar8*) &lendummy;
        if(IndexData) {
            lendummy = IndexPtr->getSize() / nmappings;
        } else {
            lendummy = PositionPtr->getSize();
        }
    } else {
        LengthSize = LengthPtr->getSize();
    }

    bool omit;
    CellData* set = NULL;
    UInt32    index;
    UInt32*   vind;
    // store color on entry
    srand(4711);
    GLfloat current[4];
    glGetFloatv(GL_CURRENT_COLOR, current);
    for (LengthInd = 0; LengthInd < LengthSize; LengthInd++) {
#ifdef GV_COLORPRIMITIVES
        glColor3f(GLfloat(rand())/RAND_MAX, 
		  GLfloat(rand())/RAND_MAX, 
		  GLfloat(rand())/RAND_MAX);
#endif
        GLuint type = *(TypeData + TypeInd++ * TypeStride);
	switch (type) {
	case GL_QUADS: 
	   ntri += 2;
 	   omit = true;
	   glBegin(GL_TRIANGLE_FAN);
	   break;
	case GL_QUAD_STRIP:
	   if (*(UInt32*)(LengthData + LengthInd * LengthStride) >= 4) {
	      ntri += *(UInt32*)(LengthData + LengthInd * LengthStride)-2;
	   }
 	   omit = false;
	   glBegin(type);
	   break;
	case GL_TRIANGLES: {
	   CellData* setArray[3];
	   glBegin(type);
	   for (UInt32 l=0; l<*(UInt32*)(LengthData + LengthInd * LengthStride); l+=3) {
	     CellData::nextFrame ();
	     // test if triangle collapses then omit OpenGL commands
	     if (IndexData) {
 	        setArray[0] = geo->getPoolEntry(eye,minDist,maxDist,*(UInt32*)(IndexData + IndexStride * (IndexInd + PositionIndex)), true);
 	        setArray[1] = geo->getPoolEntry(eye,minDist,maxDist,*(UInt32*)(IndexData + IndexStride * (IndexInd + nmappings + PositionIndex)), true);
 	        setArray[2] = geo->getPoolEntry(eye,minDist,maxDist,*(UInt32*)(IndexData + IndexStride * (IndexInd + nmappings + PositionIndex + nmappings)), true);
		if (setArray[0] == NULL || setArray[1] == NULL || setArray[2] == NULL) { 
		   IndexInd += 3*nmappings;
		   continue;
		}
	     } else {
 	        setArray[0] = geo->getPoolEntry(eye,minDist,maxDist,index, true);
 	        setArray[1] = geo->getPoolEntry(eye,minDist,maxDist,index+1, true);
 	        setArray[2] = geo->getPoolEntry(eye,minDist,maxDist,index+2, true);
		if (setArray[0] == NULL || setArray[1] == NULL || setArray[2] == NULL) { 
		   index += 3;
		   continue;
		}
	     }
	     ++ntri;
	     for (UInt32 ll=0; ll<3; ++ll) {
	        // perform lookup
	        if (IndexData) {
		   vind = (UInt32*)(IndexData + IndexStride * IndexInd);
		   IndexInd += nmappings;
		   set = setArray[ll];
		} else {
		   vind = &index;
		   ++index;
		   set = setArray[ll];
		}
		// pump data
		if (ColorData) {
		   ColorFunc(ColorData + ColorStride * vind[ColorIndex]);
		}
		if (SecColorData) {
		   SecColorFunc(SecColorData + SecColorStride * vind[SecColorIndex]);
		}
		if (NormalData) {
		   //NormalFunc(NormalData + NormalStride * vind[NormalIndex]);
		}
		glNormal3fv((*set)->getNormal().getValues());
		if (TexCoordsData) {
		   TexCoordsFunc(TexCoordsData + TexCoordsStride * vind[TexCoordsIndex]);
		}
		if (TexCoords1Data) {
		   TexCoords1Func(GL_TEXTURE1_ARB,
				  TexCoords1Data + TexCoords1Stride * vind[TexCoords1Index]);
		}
		if (TexCoords2Data) {
		   TexCoords2Func(GL_TEXTURE2_ARB,
				  TexCoords2Data + TexCoords2Stride * vind[TexCoords2Index]);
		}
		if (TexCoords3Data) {
		   TexCoords3Func(GL_TEXTURE3_ARB,
				  TexCoords3Data + TexCoords3Stride * vind[TexCoords3Index]);
		}
		glVertex3fv((*set)->getPointRep().getValues());
	     }
	   }
	   glEnd();
	   continue;
	}
	case GL_TRIANGLE_STRIP: {
	   CellData::nextFrame ();
	   if (*(UInt32*)(LengthData + LengthInd * LengthStride) >= 3) {
	      ntri += *(UInt32*)(LengthData + LengthInd * LengthStride)-2;
	   }
	   glBegin(type);
	   for(UInt32 l=0; l<*(UInt32*)(LengthData + LengthInd * LengthStride); ++l) {
	     // perform lookup
	     if (IndexData) {
                vind = (UInt32*)(IndexData + IndexStride * IndexInd);
                IndexInd += nmappings;
		// always use the first two vertices
		if (l > 2) {
		  set = geo->getPoolEntry(eye,minDist,maxDist,vind[PositionIndex], true);
		} else {
		  set = geo->getPoolEntry(eye,minDist,maxDist,vind[PositionIndex], false);
		}
		if (set == NULL) { // omit vertex
		  continue; 
		}
		CellData::nextFrame ();
	    } else {
	        vind = &index;
                ++index;
		// always use the first two vertices
		if (l > 2) {
		  set = geo->getPoolEntry(eye,minDist,maxDist,index, true);
		} else {
		  set = geo->getPoolEntry(eye,minDist,maxDist,index, false);
		}
		if (set == NULL) { // omit vertex
		   continue; 
		}
		CellData::nextFrame ();
	    }
	    // pump data
	    if (ColorData) {
	      ColorFunc(ColorData + ColorStride * vind[ColorIndex]);
	    }
	    if (SecColorData) {
	      SecColorFunc(SecColorData + SecColorStride * vind[SecColorIndex]);
	    }
	    if (NormalData) {
	       //NormalFunc(NormalData + NormalStride * vind[NormalIndex]);
	    }
	    glNormal3fv((*set)->getNormal().getValues());
	    if (TexCoordsData) {
	      TexCoordsFunc(TexCoordsData + TexCoordsStride * vind[TexCoordsIndex]);
	    }
	    if (TexCoords1Data) {
	      TexCoords1Func(GL_TEXTURE1_ARB,
			     TexCoords1Data + TexCoords1Stride * vind[TexCoords1Index]);
	    }
	    if (TexCoords2Data) {
	      TexCoords2Func(GL_TEXTURE2_ARB,
			     TexCoords2Data + TexCoords2Stride * vind[TexCoords2Index]);
	    }
	    if (TexCoords3Data) {
	      TexCoords3Func(GL_TEXTURE3_ARB,
			     TexCoords3Data + TexCoords3Stride * vind[TexCoords3Index]);
	    }
	    glVertex3fv((*set)->getPointRep().getValues());
	   }
	   glEnd();
	   continue;
	}
	case GL_TRIANGLE_FAN: {
	   CellData::nextFrame ();
	   if (*(UInt32*)(LengthData + LengthInd * LengthStride) >= 3) {
	      ntri += *(UInt32*)(LengthData + LengthInd * LengthStride)-2;
	   }
	   glBegin(type);
	   for(UInt32 l=0; l<*(UInt32*)(LengthData + LengthInd * LengthStride); ++l) {
	      // perform lookup
	      if (IndexData) {
                vind = (UInt32*)(IndexData + IndexStride * IndexInd);
                IndexInd += nmappings;
		set = geo->getPoolEntry(eye,minDist,maxDist,vind[PositionIndex], true);
		if (set == NULL) { 
		   continue; 
		}
	      } else {
                vind = &index;
                ++index;
		set = geo->getPoolEntry(eye,minDist,maxDist,index, true);
		if (set == NULL) {
		   continue; 
		}
	      }
	      // perform lookup
	      if (ColorData) {
		ColorFunc(ColorData + ColorStride * vind[ColorIndex]);
	      }
	      if (SecColorData) {
		SecColorFunc(SecColorData + SecColorStride * vind[SecColorIndex]);
	      }
	      if (NormalData) {
		 //NormalFunc(NormalData + NormalStride * vind[NormalIndex]);
	      }
	      glNormal3fv((*set)->getNormal().getValues());   
	      if (TexCoordsData) {
		TexCoordsFunc(TexCoordsData + TexCoordsStride * vind[TexCoordsIndex]);
	      }
	      if (TexCoords1Data) {
		TexCoords1Func(GL_TEXTURE1_ARB,
			       TexCoords1Data + TexCoords1Stride * vind[TexCoords1Index]);
	      }
	      if (TexCoords2Data) {
		TexCoords2Func(GL_TEXTURE2_ARB,
			       TexCoords2Data + TexCoords2Stride * vind[TexCoords2Index]);
	      }
	      if (TexCoords3Data) {
		TexCoords3Func(GL_TEXTURE3_ARB,
			       TexCoords3Data + TexCoords3Stride * vind[TexCoords3Index]);
	      }
	      glVertex3fv((*set)->getPointRep().getValues());
	   }
	   glEnd();
	   continue;
	}
	case GL_POLYGON:
	   CellData::nextFrame ();
	   if (*(UInt32*)(LengthData + LengthInd * LengthStride) >= 3) {
	      ntri += *(UInt32*)(LengthData + LengthInd * LengthStride)-2;
	   }
	   omit = true;
	   glBegin(type);
	   break;
	default:
	   CellData::nextFrame ();
	   if (*(UInt32*)(LengthData + LengthInd * LengthStride) >= 3) {
	      ntri += *(UInt32*)(LengthData + LengthInd * LengthStride)-2;
	   }
	   omit = true;
	   glBegin(type);
	   break;
	};
	for(UInt32 l=0; l<*(UInt32*)(LengthData + LengthInd * LengthStride); ++l) {
	   // perform lookup
	   if (IndexData) {
                vind = (UInt32*)(IndexData + IndexStride * IndexInd);
                IndexInd += nmappings;
		set = geo->getPoolEntry(eye,minDist,maxDist,vind[PositionIndex], omit);
		if (set == NULL) { 
		   continue; 
		}
	   } else {
                vind = &index;
                ++index;
		set = geo->getPoolEntry(eye,minDist,maxDist,index, omit);
		if (set == NULL) {
		   continue; 
		}
	   }
	   // pump data
	   if (ColorData) {
	      ColorFunc(ColorData + ColorStride * vind[ColorIndex]);
	   }
	   if (SecColorData) {
	      SecColorFunc(SecColorData + SecColorStride * vind[SecColorIndex]);
	   }
	   if (NormalData) {
	      //NormalFunc(NormalData + NormalStride * vind[NormalIndex]);
	   }
	   glNormal3fv((*set)->getNormal().getValues());
	   if (TexCoordsData) {
	      TexCoordsFunc(TexCoordsData + TexCoordsStride * vind[TexCoordsIndex]);
	   }
	   if (TexCoords1Data) {
	      TexCoords1Func(GL_TEXTURE1_ARB,
			     TexCoords1Data + TexCoords1Stride * vind[TexCoords1Index]);
	   }
	   if (TexCoords2Data) {
	      TexCoords2Func(GL_TEXTURE2_ARB,
			     TexCoords2Data + TexCoords2Stride * vind[TexCoords2Index]);
	   }
	   if (TexCoords3Data) {
	      TexCoords3Func(GL_TEXTURE3_ARB,
			     TexCoords3Data + TexCoords3Stride * vind[TexCoords3Index]);
	   }
	   glVertex3fv((*set)->getPointRep().getValues());
	}
        glEnd();
    }
    // restore color on exit
    glColor3fv(current);
}

void GeometryClustered::handleGL (Window* win, UInt32 idstatus)
{
   Window::GLObjectStatusE mode;
   UInt32 id;

   if (getPositions() == NullFC) {
      return;
   }

   Window::unpackIdStatus(idstatus, id, mode);

   if (mode == Window::initialize || mode == Window::needrefresh || mode == Window::reinitialize) {
      Pnt3f minBound, maxBound;
      getParents()[0]->getVolume().getBounds(minBound, maxBound);
      glNewList(id, GL_COMPILE);
      if (getGrid() == NULL) {
#ifndef GV_CLUSTERED_ADAPTIVE
	 if (getNumCells() > 1) {
	   setGrid(new SetUnionGrid());
	   // init grid
	   CellData::firstFrame ();
	   getGrid()->init(K6Dop(minBound.getValues(), maxBound.getValues()),
			   getNumCells(), 
			   RegularGridBase::MinVoxelsPerDim);
	   if (_fileStream.is_open()) {
	     _fileStream.clear();
	     // set back to face indices section
	     _fileStream.seekg(getOffsetFaces());
	     fillGridFile(_fileStream);
	   } else {
	     fillGrid();
	   }
	   SNOTICE << "RegularGrid created: " << std::endl;
	 }
#else
	 if (getNumCells() > 1) {
	   setGrid(new SetUnionGrid());
	   // init grid
	   CellData::firstFrame ();
	   getGrid()->init(K6Dop(minBound.getValues(), maxBound.getValues()),
			   getNumCells(), 
			   RegularGridBase::MaxVoxels);
	   if (_fileStream.is_open()) {
	     _fileStream.clear();
	     // set back to face indices section
	     _fileStream.seekg(getOffsetFaces());
	     fillGridFile(_fileStream);
	   } else {
	     fillGrid();
	   }
	   SNOTICE << "AdaptiveGrid created: " << std::endl;
	 }
#endif
      }
      if (win != NULL) {
 	 if (_fileStream.is_open()) {
	    _fileStream.clear();
	    // set back to face indices section
	    _fileStream.seekg(getOffsetFaces());
	    //geoBeginFaces = getOffsetFaces();
	    fileGeoPump  (_fileStream, getNumTriangles(), 
			  win, win->getPort(0).getCPtr(), this, getParents()[0]->getToWorld(), minBound, maxBound);
	 } else {
	    masterGeoPump(getNumTriangles(), 
			  win, win->getPort(0).getCPtr(), this, getParents()[0]->getToWorld(), minBound, maxBound);
	 }
      }
      glEndList();
   } else if (mode == Window::destroy) {
      SWARNING << "Delete display list" << std::endl;
      glDeleteLists(id, 1);
   } else if (mode == Window::finaldestroy) {
      SWARNING << "Last geometry user destroyed" << std::endl;
   } else {
      SWARNING << "GeometryClustered(" << this << ")::handleGL: Illegal mode: "
	       << mode << " for id " << id << std::endl;
   }
}

Action::ResultE GeometryClustered::drawPrimitives (DrawActionBase* action)
{
   if (getDlistCache()) {
      action->getWindow()->validateGLObject(getGLId());
      glCallList(getGLId());
   } else {
      // get bounding volume
      NodePtr node = action->getActNode();
      Pnt3f minBound, maxBound;
      node->getVolume().getBounds(minBound, maxBound);

      if (getGrid() == NULL) {
#ifndef GV_CLUSTERED_ADAPTIVE
	 if (getNumCells() > 1) {
	   setGrid(new SetUnionGrid());
	   // init grid
	   CellData::firstFrame ();
	   getGrid()->init(K6Dop(minBound.getValues(), maxBound.getValues()),
			   getNumCells(), 
			   RegularGridBase::MinVoxelsPerDim);
	   if (_fileStream.is_open()) {
	     _fileStream.clear();
	     // set back to face indices section
	     _fileStream.seekg(getOffsetFaces());
	     fillGridFile(_fileStream);
	   } else {
	    fillGrid();
	   }
	   SNOTICE << "RegularGrid created: " << std::endl;
	 }
#else
	 if (getNumCells() > 1) {
	   setGrid(new SetUnionGrid());
	   // init grid
	   CellData::firstFrame ();
	   getGrid()->init(K6Dop(minBound.getValues(), maxBound.getValues()),
			   getNumCells(), 
			   RegularGridBase::MaxVoxels);
	   if (_fileStream.is_open()) {
	     _fileStream.clear();
	     // set back to face indices section
	     _fileStream.seekg(getOffsetFaces());
	     fillGridFile(_fileStream);
	   } else {
	     fillGrid();
	   }
	   SNOTICE << "AdaptiveGrid created: " << std::endl;
	 }
#endif
      }
      // call the pump
      if (_fileStream.is_open()) {
	 _fileStream.clear();
	 // set back to face indices section
	 _fileStream.seekg(getOffsetFaces());
	 fileGeoPump(_fileStream, getNumTriangles(),
		     action->getWindow(), action->getViewport(), this, node->getToWorld(), minBound, maxBound);
      } else {
	 masterGeoPump(getNumTriangles(),
		       action->getWindow(), action->getViewport(), this, node->getToWorld(), minBound, maxBound);
      }
   }

   StatCollector* coll = action->getStatistics();
   if (coll != NULL) {
        StatIntElem* el = coll->getElem(Drawable::statNTriangles,false);
        if (el) {
            el->add(getNumTriangles());

	    UInt32 is;
	    if (getPositions() != NullFC) {
	       is = getPositions()->getSize();
	    } else {
	       is = 0;
	    }
            coll->getElem(Drawable::statNVertices)->add(is);
        }
   }
    
   return Action::Continue;
}

void GeometryClustered::adjustVolume (Volume& volume)
{
    GeoPositionsPtr pos = getPositions();

    volume.setValid();
    volume.setEmpty();

    if(pos == NullFC)
        return;                  // Node has no points, no volume

    for (UInt32 v=0; v < getPositions()->getSize(); ++v) {
       volume.extendBy(getPositions()->getValue(v));
    }
}

/*------------------------------------------------------------------------*/
/*                              cvs id's                                  */

#ifdef OSG_SGI_CC
#pragma set woff 1174
#endif

#ifdef OSG_LINUX_ICC
#pragma warning( disable : 177 )
#endif

namespace
{
    static Char8 cvsid_cpp       [] = "@(#)$Id: OSGGeometryClustered.cpp,v 1.7 2005/01/19 09:26:13 fuenfzig Exp $";
    static Char8 cvsid_hpp       [] = OSGGEOMETRYCLUSTEREDBASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGGEOMETRYCLUSTEREDBASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGGEOMETRYCLUSTEREDFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif


