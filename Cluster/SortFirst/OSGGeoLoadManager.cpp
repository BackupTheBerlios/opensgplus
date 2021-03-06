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
#include <OSGGLU.h>

#include <algorithm>
#include <OSGConfig.h>
#include <OSGDrawAction.h>
#include <OSGGeometry.h>
#include <OSGSimpleGeometry.h>

#include "OSGGeoLoadManager.h"

OSG_USING_NAMESPACE

/** \class osg::GeoLoadManager
 *  \ingroup ClusterLib
 *  \brief Manages geometry load
 *
 *
 *
 *
 *
 **/

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static Char8 cvsid_cpp[] = "@(#)$Id: OSGGeoLoadManager.cpp,v 1.14 2002/06/08 20:33:34 marcus Exp $";
    static Char8 cvsid_hpp[] = OSG_GEOLOADMANAGER_HEADER_CVSID;
    static Char8 cvsid_inl[] = OSG_GEOLOADMANAGER_INLINE_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

double sort_t=0;
double sortrow_t=0;
double balance_t=0;
double bestcut_t=0;
double bestcutA_t=0;
double bestcutB_t=0;

/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

/*! Constructor
 */
GeoLoadManager::GeoLoadManager(bool useFaceDistribution):
    _useFaceDistribution(useFaceDistribution)
{
}

/*! Copy Constructor
 */
GeoLoadManager::GeoLoadManager(const GeoLoadManager &source):
    _geoLoad(source._geoLoad),
    _renderNode(source._renderNode),
    _useFaceDistribution(source._useFaceDistribution)
{
}

/*-------------------------------------------------------------------------*/
/*                             Destructor                                  */

/*! Destructor documentation
 */
GeoLoadManager::~GeoLoadManager(void)
{
}

/*-------------------------------------------------------------------------*/
/*                             Assignment                                  */

/*! assignment
 */
GeoLoadManager& GeoLoadManager::operator = (const GeoLoadManager &source)
{
    if(this == &source)
        return *this;
    _geoLoad = source._geoLoad;
    _renderNode = source._renderNode;
    _useFaceDistribution = source._useFaceDistribution;
    return *this;
}


/** Update the load balancing information
 **/
void GeoLoadManager::update(NodePtr node)
{
    GeoLoadMapT loadMap;

    // collect old load obects
    for(GeoLoadLstT::iterator gI=_geoLoad.begin();gI!=_geoLoad.end();++gI)
    {
        loadMap[gI->getNode().getFieldContainerId()] = gI;
    }
    updateSubtree(node,loadMap);
    // remove unused load objects
    for(GeoLoadMapT::iterator mI=loadMap.begin();mI!=loadMap.end();++mI)
    {
        _geoLoad.erase(mI->second);
    }
}

/** load balance
 * 
 *  \param vp        current viewport
 *  \param shrink    shrink to max area of visible objects
 *  \param result    resulting regions
 *
 **/
void GeoLoadManager::balance(ViewportPtr    vp,
                             bool           shrink,
                             ResultT       &result)
{
    sort_t=0;
    sortrow_t=0;
    balance_t=0;
    bestcut_t=0;
    bestcutA_t=0;
    bestcutB_t=0;
    balance_t-=getSystemTime();

    Matrix                   projection,viewing;
    RegionLoadVecT           visible;
    RegionLoadVecT::iterator vi;
    Int32                    width =vp->getPixelWidth();
    Int32                    height=vp->getPixelHeight();
    Int32                    wmin[2]={width,height};
    Int32                    wmax[2]={0    ,0     };
    Real32                   rNear=vp->getCamera()->getNear();

    sortrow_t-=getSystemTime();

    result.clear();
    vp->getCamera()->getViewing   ( viewing   ,width,height );
    vp->getCamera()->getProjection( projection,width,height );
    visible.reserve(_geoLoad.size());
    for(GeoLoadLstT::iterator l=_geoLoad.begin() ; l!=_geoLoad.end() ; ++l)
    {
        // update view dependent values
        l->updateView(viewing,
                      projection,
                      rNear,
                      width,
                      height);
        // sort by min values
        if(l->isVisible())
        {
            // collect visible geometries
            visible.push_back(RegionLoad(&(*l)));
            if(shrink)
            {
                if(l->getMin()[0] < wmin[0]) 
                    wmin[0]=l->getMin()[0];
                if(l->getMin()[1] < wmin[1]) 
                    wmin[1]=l->getMin()[1];
                if(l->getMax()[0] > wmax[0]) 
                    wmax[0]=l->getMax()[0];
                if(l->getMax()[1] > wmax[1]) 
                    wmax[1]=l->getMax()[1];
            }
        }
    }
    sortrow_t+=getSystemTime();
    if(shrink)
    {
        // handle invisible area
        if(wmax[0]<wmin[0] || 
           wmax[1]<wmin[1] )
            wmin[0]=wmax[0]=wmin[1]=wmax[1]=0;
        // clamp to viewable area
        if(wmin[0]<0) 
            wmin[0]=0;
        if(wmin[1]<0) 
            wmin[1]=0;
        if(wmax[0]>=width ) 
            wmax[0]=width -1;
        if(wmax[1]>=height) 
            wmax[1]=height-1;
    }
    else
    {
        wmin[0]=wmin[1]=0;
        wmax[0]=width-1;
        wmax[1]=height-1;
    }
    // calculate region cost
    for(vi=visible.begin();vi!=visible.end();vi++)
    {
        vi->updateCost(wmin,wmax);
    }
    if(_renderNode.size()>1)
    {
        splitRegion(0,_renderNode.size()-1,
                    visible,
                    wmin,
                    wmax,
                    result);
    }
    else
    {
        result.insert(result.end(),wmin,wmin+2);
        result.insert(result.end(),wmax,wmax+2);
    }
    balance_t+=getSystemTime();
    printf("Balance:   %10.8f\n",balance_t);
    printf("Sort:      %10.8f\n",sort_t);
    printf("Sort row:  %10.8f\n",sortrow_t);
    printf("Best cut:  %10.8f\n",bestcut_t);
    printf("Best cutA: %10.8f\n",bestcutA_t);
    printf("Best cutB: %10.8f\n",bestcutB_t);
    printf("\n");
}

/** Add new render node
 * 
 **/
void GeoLoadManager::addRenderNode(const RenderNode &rn,UInt32 id)
{
    while(_renderNode.size()<=id)
        _renderNode.push_back(RenderNode());
    _renderNode[id]=rn;
}

/** Draw recangular volume projection
 * 
 **/
void GeoLoadManager::drawVolumes(WindowPtr win)
{
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glViewport(0,0,win->getWidth(),win->getHeight());
    gluOrtho2D(0,win->getWidth(),
               0,win->getHeight());
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    for(GeoLoadLstT::iterator l=_geoLoad.begin() ; l!=_geoLoad.end() ; ++l)
    {
        if(l->isVisible())
        {
            glBegin(GL_LINE_LOOP);
            glColor3f(0, 1, 0);
            glVertex3f(l->getMin()[0],l->getMin()[1],0);
            glVertex3f(l->getMax()[0],l->getMin()[1],0);
            glVertex3f(l->getMax()[0],l->getMax()[1],0);
            glVertex3f(l->getMin()[0],l->getMax()[1],0);
            glEnd();
        }
    }
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

/** Add all geometry nodes in the given tree
 **/
void GeoLoadManager::updateSubtree(NodePtr &node,GeoLoadMapT &loadMap)
{
    // is nodecore a geometry?
    if(node->getCore() != NullFC &&
       GeometryPtr::dcast(node->getCore()) != NullFC)
    {
        // is this a new node
        GeoLoadMapT::iterator mI=loadMap.find(node.getFieldContainerId());
        if(mI==loadMap.end())
        {
            _geoLoad.push_back(GeoLoad(node,_useFaceDistribution));
        }
        else
        {
            loadMap.erase(mI);
        }
    }
    // handle all childs
    for(MFNodePtr::iterator n=node->getMFChildren()->begin();
        n!=node->getMFChildren()->end();
        ++n)
    {
        updateSubtree(*n,loadMap);
    }
}

/** Splitupt region
 **/
void GeoLoadManager::splitRegion(UInt32          rnFrom,
                                 UInt32          rnTo,
                                 RegionLoadVecT &visible,
                                 Int32           wmin[2],
                                 Int32           wmax[2],
                                 ResultT        &result)
{
    UInt32 i;
    Int32  axis,cut;
    Int32  maxA[2];
    Int32  minB[2];
    RegionLoadVecT visibleA;
    RegionLoadVecT visibleB;
    RegionLoadVecT::iterator vi;
    UInt32 rnToA,rnFromB;
    RenderNode groupRegionA,groupRegionB;
    RenderNode *renderNodeA,*renderNodeB;

    // create group render node if neccessary
    rnToA  =(rnTo+rnFrom)>>1;
    rnFromB=rnToA+1;
    if(rnFrom != rnToA)
    {
        groupRegionA.setGroup(&_renderNode[rnFrom],&_renderNode[rnToA+1]);
        renderNodeA=&groupRegionA;
    }
    else
    {
        renderNodeA=&_renderNode[rnFrom];
    }
    if(rnFromB != rnTo)
    {
        groupRegionB.setGroup(&_renderNode[rnFromB],&_renderNode[rnTo+1]);
        renderNodeB=&groupRegionB;
    }
    else
    {
        renderNodeB=&_renderNode[rnFromB];
    }

    // search for best cut
    Real32 cost=findBestCut(*renderNodeA,*renderNodeB,
                            visible,wmin,wmax,axis,cut);
    // create new regions
    maxA[axis  ]=cut;
    maxA[axis^1]=wmax[axis^1];
    minB[axis  ]=cut+1;
    minB[axis^1]=wmin[axis^1];
    visibleA.reserve(visible.size());
    visibleB.reserve(visible.size());

    // split regions
    if(rnFrom != rnToA || rnFromB != rnTo)
    {
        // split visible regions
        for(vi=visible.begin();vi!=visible.end();vi++)
        {
            if(vi->getLoad()->getMax()[axis] <= cut)
                visibleA.push_back(*vi);
            else
                if(vi->getLoad()->getMin()[axis] > cut)
                    visibleB.push_back(*vi);
                else
                {
                    visibleA.push_back(*vi);
                    visibleB.push_back(*vi);
                    visibleA.rbegin()->updateCost(wmin,maxA);
                    visibleB.rbegin()->updateCost(minB,wmax);
                }
        }
    }
    if(rnFrom != rnToA)
        splitRegion(rnFrom,rnToA,visibleA,wmin,maxA,result);
    else
    {
        result.insert(result.end(),wmin,wmin+2);
        result.insert(result.end(),maxA,maxA+2);
    }
    if(rnFromB != rnTo)
        splitRegion(rnFromB,rnTo,visibleB,minB,wmax,result);
    else
    {
        result.insert(result.end(),minB,minB+2);
        result.insert(result.end(),wmax,wmax+2);
    }
}

/** Find best cut through the geometries
 **/
Real32 GeoLoadManager::findBestCut (const RenderNode &renderNodeA,
                                    const RenderNode &renderNodeB,
                                    RegionLoadVecT &visible,
                                    Int32           wmin[2],
                                    Int32           wmax[2],
                                    Int32          &bestAxis,
                                    Int32          &bestCut)
{
    bestcut_t-=getSystemTime();
    RegionLoadVecT::iterator vi;
    Int32 a,f,t,newCut;
    Int32 minB[2];
    Int32 maxA[2];
    Real32 costA=0,costB=0;
    Real32 newCost;
    Real32 bestCost;
    
    bestCost=1e22;
    for(a=0;a<=1;++a)
    {
        f=wmin[a];
        t=wmax[a];
        maxA[0]=wmax[0];;
        maxA[1]=wmax[1];;
        minB[0]=wmin[0];;
        minB[1]=wmin[1];;
        do
        {
            newCut=(f+t)/2;
            maxA[a]=newCut;
            minB[a]=newCut+1;
            costA=costB=0;
            for(vi=visible.begin();vi!=visible.end();vi++)
            {
                if(vi->getLoad()->getMax()[a] <= newCut)
                    costA+=vi->getCost(renderNodeA);
                else
                    if(vi->getLoad()->getMin()[a] > newCut)
                        costB+=vi->getCost(renderNodeB);
                    else
                    {
                        costA+=vi->getCost(renderNodeA,wmin,maxA);
                        costB+=vi->getCost(renderNodeB,minB,wmax);
                    }
                
            }
            newCost= osgMax( costA, costB );
            if(newCost<bestCost)
            {
                bestCut=newCut;
                bestCost=newCost;
                bestAxis=a;
            }
            // walk into direction of inbalance
            if(costA>costB)
                t=newCut;
            else
                f=newCut;
        }
        while(t-f > 2);
    }
    bestcut_t+=getSystemTime();
    return bestCost;
}    


