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
#include <OSGRenderNode.h>

OSG_BEGIN_NAMESPACE

class Action;

class OSG_CLUSTERLIB_DLLMAPPING GeoLoadManager
{
    /*==========================  PUBLIC  =================================*/
  public:

    typedef std::vector<GeoLoad>                   GeoLoadLstT;
    typedef std::map<UInt32,GeoLoadLstT::iterator> GeoLoadMapT;
    typedef std::vector<Int32>                     ResultT;
    typedef std::vector<RenderNode>                RenderNodeLstT;

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

    GeoLoadManager(bool useFaceDistribution=false);
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

    void update             (NodePtr           node   );
    void balance            (ViewportPtr       vp,
                             bool              shrink,
                             ResultT          &result );
    void addRenderNode      (const RenderNode &rn,UInt32 id);
    void drawVolumes        (WindowPtr win);

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

    class RegionLoad 
    {
      public:
        RegionLoad         ( GeoLoad *load=NULL           );
        Real32   getCost   ( const RenderNode &renderNode );
        Real32   getCost   ( const RenderNode &renderNode,
                             const Int32 wmin[2],
                             const Int32 wmax[2]          ) const;
        GeoLoad *getLoad   ( void                         );
        void     updateCost( const Int32 wmin[2],
                             const Int32 wmax[2]          );
      private:
        Real32   _visibleFaces;
        Real32   _invisibleFaces;
        Real32   _pixel;
        GeoLoad *_load;
    };
    typedef std::vector<RegionLoad>   RegionLoadVecT;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Fields                                  */
    /*! \{                                                                 */

    GeoLoadLstT           _geoLoad;
    RenderNodeLstT        _renderNode;
    bool                  _useFaceDistribution;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Member                                  */
    /*! \{                                                                 */

    double runFaceBench(GLuint dlist,UInt32 size,Real32 visible);
    void updateSubtree (NodePtr &node,GeoLoadMapT &loadMap);
    void splitRegion   (UInt32          rnFrom,
                        UInt32          rnTo,
                        RegionLoadVecT &visible,
                        Int32           amin[2],
                        Int32           amax[2],
                        ResultT        &result);
    Real32 findBestCut (const RenderNode &renderNodeA,
                        const RenderNode &renderNodeB,
                        RegionLoadVecT &visible,
                        Int32           amin[2],
                        Int32           amax[2],
                        Int32          &bestAxis,
                        Int32          &bestCut);
    double calcFaceRenderingCost(GeoLoad *load,
                                 Int32 amin[2],Int32 amax[2]);

    /*! \}                                                                 */

    /*==========================  PRIVATE  ================================*/
  private:
    friend class RegionLoad;
};

OSG_END_NAMESPACE

#include "OSGGeoLoadManager.inl"

#define OSG_GEOLOADMANAGER_HEADER_CVSID "@(#)$Id: OSGGeoLoadManager.h,v 1.8 2002/06/08 20:33:34 marcus Exp $"

#endif /* _GEOLOADMANAGER_H_ */




