/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *             Copyright (C) 2000,2001 by the OpenSG Forum                   *
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

#include "OSGConfig.h"
#include "OSGVolume.h"

OSG_BEGIN_NAMESPACE

/***************************************************************************\
 *                               Types                                     *
\***************************************************************************/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/


/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/


/*-------------------------- constructor ----------------------------------*/

/*! Default constructor - leaves box totally empty
*/

inline
ShadowVolume::ShadowVolume() : Volume() {;}

/// Constructor given bounds
inline
ShadowVolume::ShadowVolume ( const UInt16& s )
    : Volume(), _numPlanes(s)
{
    _planes=new Plane[_numPlanes];
}

/// Copy Constructor
inline
ShadowVolume::ShadowVolume(const ShadowVolume &obj)
        : Volume(obj), _numPlanes(obj._numPlanes)
{
	_planes=new Plane[_numPlanes];
	for(UInt16 i=0; i<_numPlanes; i++)
		_planes[i]=obj._planes[i];
}

/// Destructor
inline
ShadowVolume::~ShadowVolume() {
	delete[] _planes;
}

/*------------------------------ feature ----------------------------------*/

/// Returns the near plane
inline
const Plane & ShadowVolume::getNear   (void) const
{
    return _planes[0];
}

/// get method
inline
const Plane *ShadowVolume::getPlanes(void) 
{
    return _planes;
}

/// get method
inline
const Plane *ShadowVolume::getPlanes(void) const
{
    return _planes;
}

/*-------------------------- extending ------------------------------------*/

inline
void ShadowVolume::extendBy(const Volume &volume)
{
	// not implemented
}

inline
void ShadowVolume::extendBy(const ShadowVolume &volume)
{
	// not implemented
}

/*-------------------------- intersection ---------------------------------*/

inline
bool ShadowVolume::intersect (const Volume &volume) const
{
	// not implemented
	return(1);
}

inline
bool ShadowVolume::intersect(const ShadowVolume &volume) const
{
	// not implemented
	return(1);
}

/*-------------------------- operation ------------------------------------*/

/// Inequality comparisons
inline
bool operator != (const ShadowVolume &b1, const ShadowVolume &b2)
{
    return !(b1 == b2);
}

/*------------------------------ feature ----------------------------------*/


/// set method
inline
void ShadowVolume::setPlane(const Plane& p, const UInt16 i){
	if(i>=_numPlanes){
		std::cerr << "\nARGL!\n\n";
		return;
	}
	_planes[i]=p;
}

/// Returns the center of a box
inline
void ShadowVolume::getCenter(Pnt3f &center) const
{
	// not implemented
    return;
}

/// Gives the volume of the frustum
inline
Real32 ShadowVolume::getScalarVolume() const
{
	// not implemented
    return 0.0;
}

/// Gives the boundaries of the volume
inline
void ShadowVolume::getBounds(Pnt3f &OSG_CHECK_ARG(minPnt), 
                              Pnt3f &OSG_CHECK_ARG(maxPnt)) const
{
    // not implemented !!!
    return;
}

/*-------------------------- extending ------------------------------------*/

/// Extends Frustum3f (if necessary) to contain given 3D point
inline
void ShadowVolume::extendBy(const Pnt3f &OSG_CHECK_ARG(pt))
{
    // not implemented !!!
    return;
}

/*-------------------------- intersection ---------------------------------*/

/// Returns true if intersection of given point and Frustum3f is not empty
inline
bool ShadowVolume::intersect(const Pnt3f &point) const
{
    int i;
    bool retCode = true;

  for (i = 0; i < _numPlanes; i++) {
	  /*
    if ( ( _planes[i].getNormal().x() * point.x() +
                     _planes[i].getNormal().y() * point.x() +
                     _planes[i].getNormal().z() * point.x() +
                     _planes[i].getDistanceFromOrigin()) < 0 ) {
      retCode = false;
      break;
    }
   */
    if(_planes[i].isInHalfSpace(point)==false){
      retCode = false;
      break;
    }
    
  }

  return retCode;
}


/** intersect the box with the given Line */
inline
bool ShadowVolume::intersect (const Line &line) const
{
	// not implented
    return 0;
}



/** intersect the box with the given Line */
inline
bool ShadowVolume::intersect ( const Line &line,
                                Real32 &minDist, Real32 &maxDist ) const
{
	// not implented
    return 0;
}

inline
bool ShadowVolume::isOnSurface (const Pnt3f &OSG_CHECK_ARG(point)) const
{
    // not implemented
    return false;
}


/// Transforms Frustum3f by matrix, enlarging Frustum3f to contain result
inline
void ShadowVolume::transform(const Matrix &OSG_CHECK_ARG(m))
{
    // not implemented
    return;
}

/// Assignment operator
inline
const ShadowVolume &ShadowVolume::operator =(const ShadowVolume &b1)
{
    for (int i = 0; i < _numPlanes; i++)
        _planes[i] = b1._planes[i];

    return *this;
}

/// print the volume */
inline
void ShadowVolume::dump(      UInt32    OSG_CHECK_ARG(uiIndent), 
                         const BitVector OSG_CHECK_ARG(bvFlags )) const
{
	// not implemented...
}
/*
/// Equality comparisons
OSG_BASE_DLLMAPPING
bool operator ==(const ShadowVolume &b1, const ShadowVolume &b2)
{
	// not implemented...
    if(b1._numPlanes == b2._numPlanes){
	    for(int i=0; i<b1._numPlanes; i++){
		    if(b1.planes[i]!=b2.planes[i])
			    return(false);
	    }
	    return(true);
    }
    return(false);
}
*/

/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

OSG_END_NAMESPACE

