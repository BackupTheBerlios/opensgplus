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

#ifndef _GEOLOADMANAGER_H_
#define _GEOLOADMANAGER_H_
#ifdef __sgi
#pragma once
#endif

#include <map>
#include <vector>
#include <OSGClusterDef.h>
#include <OSGBaseTypes.h>
#include <OSGGeoLoad.h>

OSG_BEGIN_NAMESPACE

class OSG_CLUSTERLIB_DLLMAPPING GeoLoadManager
{
    /*==========================  PUBLIC  =================================*/
  public:

    typedef std::vector<GeoLoad>        GeoLoadVecT;
    typedef std::vector<Int32>          ResultT;

    /*---------------------------------------------------------------------*/
    /*! \name                      dcast                                   */
    /*! \{                                                                 */

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name        General Fieldcontainer Declaration                    */
    /*! \{                                                                 */

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    GeoLoadManager(void);
    GeoLoadManager(const GeoLoadManager &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Assignment                                 */
    /*! \{                                                                 */

    GeoLoadManager& operator =(const GeoLoadManager &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~GeoLoadManager(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                 Load balancing functions                     */
    /*! \{                                                                 */

    void add         (NodePtr node);
    void balance     (ViewportPtr     vp,
                      UInt32          servers,
                      ResultT        &result);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Get                                     */
    /*! \{                                                                 */

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Set                                     */
    /*! \{                                                                 */

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                        Dump                                  */
    /*! \{                                                                 */

    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/
  protected:

    /*---------------------------------------------------------------------*/
    /*! \name                      Types                                   */
    /*! \{                                                                 */

    class RegionLoad {
    public:
        RegionLoad(GeoLoad *g=NULL);
        void update(Int32 amin[2],Int32 amax[2]);
        Real32   rendering;
        GeoLoad *geometry;
    };
    typedef std::vector<RegionLoad>   RegionLoadVecT;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Fields                                  */
    /*! \{                                                                 */

    GeoLoadVecT           _geoLoad;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Member                                  */
    /*! \{                                                                 */

    void splitRegion (UInt32          servers,
                      RegionLoadVecT &visible,
                      Int32           amin[2],
                      Int32           amax[2],
                      ResultT        &result);
    Real32 findBestCut (RegionLoadVecT &visible,
                        Int32           amin[2],
                        Int32           amax[2],
                        Int32          &bestAxis,
                        Int32          &bestCut);

    /*! \}                                                                 */

    /*==========================  PRIVATE  ================================*/
  private:

};

OSG_END_NAMESPACE

#define OSG_GEOLOADMANAGERHEADER_CVSID "@(#)$Id: OSGGeoLoadManager.h,v 1.2 2002/02/11 17:00:09 marcus Exp $"

#endif /* _GEOLOADMANAGER_H_ */

