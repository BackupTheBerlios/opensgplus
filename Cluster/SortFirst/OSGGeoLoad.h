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

#ifndef _GEOLOAD_H_
#define _GEOLOAD_H_
#ifdef __sgi
#pragma once
#endif

#include <OSGBaseTypes.h>
#include <OSGClusterDef.h>
#include <OSGGeometry.h>
#include <OSGCamera.h>
#include <OSGViewport.h>

OSG_BEGIN_NAMESPACE

#define FACE_DISTRIBUTION_SAMPLING_COUNT 16

class OSG_CLUSTERLIB_DLLMAPPING GeoLoad 
{
    /*==========================  PUBLIC  =================================*/
  public:

    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    GeoLoad(NodePtr node,bool useFaceDistribution=true);
    GeoLoad(const GeoLoad &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~GeoLoad(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                  load calculation                            */
    /*! \{                                                                 */

    void updateView        ( Matrix &viewing,
                             Matrix &projection,
                             Real32 near,
                             UInt32 width,
                             UInt32 height                );
    void updateGeometry    (                              );
    bool checkRegion       ( Int32 min[2],
                             Int32 max[2]                 );
    bool isVisible         (                              ) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Set                                     */
    /*! \{                                                                 */

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Get                                     */
    /*! \{                                                                 */

    NodePtr           getNode() const;
    const Int32 *     getMin();
    const Int32 *     getMax();
    UInt32            getFaces();
    Real32            getVisibleFraction( const Int32 wmin[2],
                                          const Int32 wmax[2],
                                                Int32 viswmin[2],
                                                Int32 viswmax[2] );
    bool              getVisibleArea    ( const Int32 wmin[2],
                                          const Int32 wmax[2],
                                                Int32 viswmin[2],
                                                Int32 viswmax[2] );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   your_operators                             */
    /*! \{                                                                 */

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Assignment                                */
    /*! \{                                                                 */

    GeoLoad& operator = (const GeoLoad &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Comparison                                */
    /*! \{                                                                 */

    bool operator < (const GeoLoad &other) const;
    //bool operator == (const GeoLoad &other) const;
    //bool operator != (const GeoLoad &other) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                        Dump                                  */
    /*! \{                                                                 */

    void dump(void);

    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/
  protected:

    /*---------------------------------------------------------------------*/
    /*! \name                     utilities                                */
    /*! \{                                                                 */

    Real32 getFaceDistribution(Real32 cut);

    /*! \}                                                                 */

    /*---------------------------------------------------------------------*/
    /*! \name                        Members                               */
    /*! \{                                                                 */

    NodePtr               _node;
    GeometryPtr           _geometry;
    UInt32                _faces;
    Int32                 _min[2];
    Int32                 _max[2];
    vector<Real32>        _faceDistribution;
    bool                  _visible;
    bool                  _useFaceDistribution;
    Real32                _areaSize;

    /*! \}                                                                 */

    /*==========================  PRIVATE  ================================*/
  private:

};

OSG_END_NAMESPACE

#define OSG_GEOLOADHEADER_CVSID "@(#)$Id: OSGGeoLoad.h,v 1.8 2002/06/08 20:33:34 marcus Exp $"

#endif /* _GEOLOAD_H_ */
