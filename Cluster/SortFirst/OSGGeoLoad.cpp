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

#include "OSGConfig.h"
#include "OSGBaseFunctions.h"
#include "OSGFaceIterator.h"
#include "OSGGeoLoad.h"

OSG_USING_NAMESPACE

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static Char8 cvsid_cpp[] = "@(#)$Id: OSGGeoLoad.cpp,v 1.7 2002/02/16 04:28:10 vossg Exp $";
    static Char8 cvsid_hpp[] = OSG_GEOLOADHEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

/** \class osg::GeoLoad
 *  \ingroup ClusterLib
 *  \brief GeoLoad holds the load caused by geometry rendering
 *
 **/

// how to estimate ? 
Real32 GeoLoad::_primTransformPerSec=10000000;
Real32 GeoLoad::_primRenderPerSec   =1000000;
Real32 GeoLoad::_pixelWritePerSec   =2500000;
Real32 GeoLoad::_pixelReadPerSec    =2500000;

/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

/*! Constructor
 */

GeoLoad::GeoLoad(NodePtr node):
    _node(node),
    _faces(0)
{
    updateGeometry();
} 

/*! copy Constructor
 */

GeoLoad::GeoLoad(const GeoLoad &source):
    _node(source._node)
{
    _min[0]  = source._min[0];
    _min[1]  = source._min[1];
    _max[0]  = source._max[0];
    _max[1]  = source._max[1];
    _faces   = source._faces;
    _visible = source._visible;
}

/*-------------------------------------------------------------------------*/
/*                             Destructor                                  */

/** Destructor documentation
 **/
GeoLoad::~GeoLoad(void)
{
}


/** line clipping 
 *  
 * Clip lines of the bounding volume at the front clipping plane
 * The points in pnt are sorted so that the invertation of one
 * bit of the "from" index results in a valid "to" index of en edge
 * 
 **/

void GeoLoad::clipLine(int from,
                       Real32 rNear,
                       vector<Pnt3f> &pnt,
                       vector<Pnt3f> &clip) 
{
    int to;
    Real32 l;
    for(int i=1;i<8;i<<=1)
    {
        to=from^i;
        // different sides ?
        if( (pnt[from][2] < rNear) !=
            (pnt[to][2]   < rNear) )
        {
            l=(rNear - pnt[from][2]) / (pnt[to][2] - pnt[from][2]);
            clip.push_back(Pnt3f( pnt[from] + ( pnt[to] - pnt[from] ) * l ));
        }
    }
}

/** Update the view dependend load parameters
 **/

void GeoLoad::updateView(Matrix &viewing,
                         Matrix &projection,
                         Real32 rNear,
                         UInt32 width,
                         UInt32 height)
{
    Vec3f  vol[2];
    Pnt3f  pnt;
    Real32 minx,miny;
    Real32 maxx,maxy;

    // get whole transformation
    Matrix m=_node->getToWorld();
    m.multLeft(viewing);
    // get transformed volume
    _node->updateVolume();
    DynamicVolume volume=_node->getVolume();
    // bug in osg base
    /*
    if(volume.isEmpty())
    {
        _visible=false;
        return;
    }
    */
    volume.transform(m);
    // get min,max
    volume.getBounds(vol[0], vol[1]);
    // min < rNear
    if(vol[0][2] > -rNear)
    {
        _visible=false;
        return;
    }
    if(vol[1][2] > -rNear)
    {
        vol[1][2] = -rNear;
    }
    // create corners of a bounding box
    for(int i=0;i<8;++i)
    {
        projection.multFullMatrixPnt(Pnt3f( vol[ (i   )&1 ][0] ,
                                            vol[ (i>>1)&1 ][1] ,
                                            vol[ (i>>2)&1 ][2]) , pnt);

        if(i>0)
        {
            if(minx > pnt[0]) minx = pnt[0];
            if(miny > pnt[1]) miny = pnt[1];
            if(maxx < pnt[0]) maxx = pnt[0];
            if(maxy < pnt[1]) maxy = pnt[1];
        }
        else
        {
            maxx = minx = pnt[0];
            maxy = miny = pnt[1];
        }
    }
    // visible ?
    if(maxx<-1 || maxy<-1 ||
       minx> 1 || miny> 1)
    {
        _visible = false;
    }
    else
    {
        _min[0]=(Int32)(width * ( minx + 1.0 ) / 2.0);
        _max[0]=(Int32)(width * ( maxx + 1.0 ) / 2.0);
        _min[1]=(Int32)(height * ( miny + 1.0 ) / 2.0);
        _max[1]=(Int32)(height * ( maxy + 1.0 ) / 2.0);
        _visible = true;
    }
}

void GeoLoad::updateGeometry()
{
    NodeCorePtr core;
    GeometryPtr geo;

    _faces = 0;
    core=_node->getCore();
    if(_node->getCore() == NullFC)
        return;
    geo=GeometryPtr::dcast(core);
    if(geo == NullFC)
        return;
    // count faces
    for(FaceIterator f=geo->beginFaces() ; f!=geo->endFaces() ; ++f)
    {
        ++_faces;
    }
}
    
/*-------------------------------------------------------------------------*/
/*                             Assignment                                  */

/*! assignment
 */
GeoLoad& GeoLoad::operator = (const GeoLoad &source)
{
    if(this == &source)
        return *this;
    _min[0]  = source._min[0];
    _min[1]  = source._min[1];
    _max[0]  = source._max[0];
    _max[1]  = source._max[1];
    _faces   = source._faces;
    _visible = source._visible;
    _node = source._node;
    return *this;
}

/*-------------------------------------------------------------------------*/
/*                             dump                                        */

void GeoLoad::dump(void)
{
    if(_visible)
    {
        SLOG << "Min/Max     :"  
             << _min[0] << " " << _min[1] << " / "
             << _max[0] << " " << _max[1] << endl;
    }
    else
    {
        SLOG << "invisible " << endl; 
    }
    SLOG << "Faces       :" << _faces << endl;
    SLOG << "Server load :" << getServerLoad(_min,_max,false) << endl;
    SLOG << "Client load :" << getClientLoad(_min,_max,false) << endl;
}

/*-------------------------------------------------------------------------*/
/*                             get                                         */

Real32 GeoLoad::getServerLoad(Int32 min[2],
                              Int32 max[2],
                              bool clientRendering)
{
    if(clientRendering || _visible==false)
        return 0;

    Int32 pixMin[2];
    Int32 pixMax[2];

    pixMin[0] = osgMax(min[0],_min[0]);
    pixMin[1] = osgMax(min[1],_min[1]);
    pixMax[0] = osgMin(max[0],_max[0]);
    pixMax[1] = osgMin(max[1],_max[1]);

    if(pixMin[0] > pixMax[0] ||
       pixMin[1] > pixMax[1])
    {
        // no overlap. should cause no load
        return 0;
    }

    Real32 pixels=(pixMax[0] - pixMin[0] + 1) * (pixMax[1] - pixMin[1] + 1);
    Real32 size=pixels / ((_max[0] - _min[0] + 1) * (_max[1] - _min[1] + 1));

    Real32 t = 
        pixels / _pixelWritePerSec +
        _faces / _primTransformPerSec +
        _faces * size / _primRenderPerSec;

    return t;
}

Real32 GeoLoad::getClientLoad(Int32 min[2],
                              Int32 max[2],
                              bool clientRendering)
{
    Int32 pixMin[2];
    Int32 pixMax[2];

    if(_visible==false)
        return 0;

    pixMin[0] = osgMax(min[0],_min[0]);
    pixMin[1] = osgMax(min[1],_min[1]);
    pixMax[0] = osgMin(max[0],_max[0]);
    pixMax[1] = osgMin(max[1],_max[1]);
    Real32 pixels=(pixMax[0] - pixMin[0] + 1) * (pixMax[1] - pixMin[1] + 1);

    Real32 t,size;
    if(clientRendering)
    {
        size=pixels / ((_max[0] - _min[0] + 1) * (_max[1] - _min[1] + 1));
        t = _faces / _primTransformPerSec +
            _faces * size / _primRenderPerSec;
    }
    else
    {
        t=pixels / _pixelReadPerSec;
    }
    return t;
}

/** Return min valuse in window coordinates
 **/
const Int32 *GeoLoad::getMin()
{
    return _min;
}

/** Return max valuse in window coordinates
 **/
const Int32 *GeoLoad::getMax()
{
    return _max;
}

/** Is the geometry visible in the current viewport
 **/
bool GeoLoad::isVisible()
{
    return _visible;
}

/** Check if one part of the geometry lays in the given region
 *
 * \param min    [ minx, miny ]
 * \param max    [ maxx, maxy ]
 *
 **/
bool GeoLoad::checkRegion( Int32 min[2],
                           Int32 max[2] )
{
    if(min[0] > _max[0] ||
       max[0] < _min[0] ||
       min[1] > _max[1] ||
       max[1] < _min[1])
        return false;
    else
        return true;
}


Real32 GeoLoad::getRenderingLoad( Int32 min[2],
                                  Int32 max[2] )
{
    Real32 load,a,b,s;
    Int32 pixMin[2];
    Int32 pixMax[2];

    if(_visible==false)
        return 0;

    pixMin[0] = osgMax(min[0],_min[0]);
    pixMin[1] = osgMax(min[1],_min[1]);
    pixMax[0] = osgMin(max[0],_max[0]);
    pixMax[1] = osgMin(max[1],_max[1]);
    // not in region
    if(pixMin[0] > pixMax[0] ||
       pixMin[1] > pixMax[1])
        return 0;
    load=_faces;
    s = _max[0] - _min[0] + 1;
    a = (pixMin[0] - _min[0]    ) / s;
    b = (pixMax[0] - _min[0] + 1) / s;
    load *= 2*b - b*b - a*a;

    s = _max[1] - _min[1] + 1;
    a = (pixMin[1] - _min[1]    ) / s;
    b = (pixMax[1] - _min[1] + 1) / s;
    load *= 2*b - b*b - a*a;

    return load;
}









