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
 *  1. Face rendering cost
 *     
 *      a + b - (a*b)
 *
 *      2x-2x^2
 *
 *
 *
 **/

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static Char8 cvsid_cpp[] = "@(#)$Id: OSGGeoLoadManager.cpp,v 1.9 2002/04/25 07:22:13 vossg Exp $";
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
GeoLoadManager::GeoLoadManager(void) 
{
}

/*! Copy Constructor
 */
GeoLoadManager::GeoLoadManager(const GeoLoadManager &source):
    _geoLoad(source._geoLoad),
    _faceCostInvisible(source._faceCostInvisible),
    _faceCostVisible(source._faceCostVisible),
    _faceCostVisibleArea(source._faceCostVisibleArea)
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
    _faceCostInvisible = source._faceCostInvisible;
    _faceCostVisible = source._faceCostVisible;
    _faceCostVisibleArea = source._faceCostVisibleArea;
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
 *  \param regions   number of resulting regions
 *  \param shrink    shrink to max area of visible objects
 *  \param result    resulting regions
 *
 **/
void GeoLoadManager::balance(ViewportPtr    vp,
                             UInt32         regions,
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
        vi->setCost(
            calcFaceRenderingCost(
                vi->getLoad(),wmin,wmax));
    }
    if(regions>1)
    {
        splitRegion(regions,visible,wmin,wmax,result);
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


/** check face rendering performance
 * 
 * This method will render a display list with vertices in x,y range [0.1].
 * size specifies the viewport size. The vertices will be scaled to fit
 * exactly in to the viewport. The parameter visible defines which share
 * of the geometry should be visible. 1: all is visible, 0.5 half of the
 * geometry is visible.
 *
 * \param dlist     display list
 * \param size      viewport size
 * \param visible   visible portion of the geometry
 *
 **/
double GeoLoadManager::runFaceBench(GLuint dlist,UInt32 size,Real32 visible)
{
    glViewport(0, 0, size, size);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(1.0-visible,0,0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glPushMatrix();
    gluOrtho2D(0,1,0,1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glFinish();
    // start bench
    Time t=-getSystemTime();
    glCallList(dlist);
    glFinish();
    t+=getSystemTime();
    // end bench
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    return t;
}

void GeoLoadManager::estimatePerformace()
{
    double faces=0;

    // create display list
    GLuint dList1 = glGenLists(1);
    glNewList(dList1, GL_COMPILE);
    float step = .1;
    int count  = 500;
    int c;

    glBegin(GL_TRIANGLE_STRIP);
    for(float y=0;y<1;y+=step)
    {        
        glVertex3f(0,y     ,-1);
        glVertex3f(0,y+step,-1);
        for(float x=0;x<1;x+=step)
        {
            glVertex3f(x,y     ,-1);
            glVertex3f(x,y+step,-1);
            faces+=2;
        }
    }
    glEnd();
    glEndList();
    glFinish();
    GLuint dList2 = glGenLists(1);
    glNewList(dList2, GL_COMPILE);
    for(c=0;c<count;++c)
    {
        glCallList(dList1);
    }        
    glEndList();
    glFlush();
    double t=runFaceBench(dList2,256,1.0);
    count=(int)(count/t);
    glNewList(dList2, GL_COMPILE);
    for(c=0;c<count;++c)
    {
        glCallList(dList1);
    }        
    glEndList();

    faces*=count;
    glCallList(dList2);
    glFinish();

    UInt32 aSize=256;
    UInt32 bSize= 16;
    Real32 aVisible=1.0;
    Real32 cVisible= .1;
    
    // do the benchmarks
    double A=runFaceBench(dList2,aSize,aVisible)/faces;
    double B=runFaceBench(dList2,bSize,aVisible)/faces;
    double C=runFaceBench(dList2,aSize,cVisible)/faces;

    // cost = (1-visible)*i + (j + size^2 * k)*visible;
    // i:  only if invisible
    // j:  only if visible, but not size dependent
    // k:  only if visible, and size dependent

    _faceCostVisibleArea = 
        (A-B) / 
        (aSize*(double)aSize-bSize*(double)bSize );
    
    _faceCostVisible     = A - _faceCostVisibleArea*aSize*aSize;
    
    _faceCostInvisible       = 
        (C - 
         _faceCostVisible    *cVisible - 
         _faceCostVisibleArea*cVisible*aSize*aSize)/(1-cVisible);
    
#if 0
    cout << _faceCostInvisible << endl;
    cout << _faceCostVisible << endl;
    cout << _faceCostVisibleArea << endl;

    printf("Max invisible faces         : %12.2f\n",
           1.0/_faceCostInvisible);
    printf("Max visible faces           : %12.2f\n",
           1.0/_faceCostVisible);

    printf("A  %12.2f\n",1.0/A);
    printf("A  %12.2f\n",1.0/calcFaceRenderingCost(1,aSize*aSize,1.0));
   
    printf("B  %12.2f\n",1.0/B);
    printf("B  %12.2f\n",1.0/calcFaceRenderingCost(1,bSize*bSize,1.0));

    printf("C  %12.2f\n",1.0/C);
    printf("C  %12.2f\n",1.0/calcFaceRenderingCost(1,aSize*aSize,cVisible));
#endif
    glDeleteLists(dList2,1);
    glDeleteLists(dList1,1);
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
            _geoLoad.push_back(GeoLoad(node));
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
void GeoLoadManager::splitRegion(UInt32          regions,
                                 RegionLoadVecT &visible,
                                 Int32           amin[2],
                                 Int32           amax[2],
                                 ResultT        &result)
{
    Int32  axis,cut;
    Int32  maxA[2];
    Int32  minB[2];
    Int32  regionsA,regionsB;
    RegionLoadVecT visibleA;
    RegionLoadVecT visibleB;
    RegionLoadVecT::iterator vi;

    // search for best cut
    Real32 cost=findBestCut(visible,amin,amax,axis,cut);
    // create new regions
    maxA[axis  ]=cut;
    maxA[axis^1]=amax[axis^1];
    minB[axis  ]=cut+1;
    minB[axis^1]=amin[axis^1];
    visibleA.reserve(visible.size());
    visibleB.reserve(visible.size());
    // split regions
    regionsA=regions/2;
    regionsB=regions-regionsA;

    if(regionsA>1 || regionsB>1)
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
                    visibleA.rbegin()->setCost(
                        calcFaceRenderingCost(
                            visibleA.rbegin()->getLoad(),amin,maxA));
                    visibleB.rbegin()->setCost(
                        calcFaceRenderingCost(
                            visibleA.rbegin()->getLoad(),minB,amax));
                }
        }
    }
    if(regionsA>1)
        splitRegion(regionsA,visibleA,amin,maxA,result);
    else
    {
        result.insert(result.end(),amin,amin+2);
        result.insert(result.end(),maxA,maxA+2);
    }
    if(regionsB>1)
        splitRegion(regionsB,visibleB,minB,amax,result);
    else
    {
        result.insert(result.end(),minB,minB+2);
        result.insert(result.end(),amax,amax+2);
    }
}

/** Find best cut through the geometries
 **/
Real32 GeoLoadManager::findBestCut (RegionLoadVecT &visible,
                                    Int32           amin[2],
                                    Int32           amax[2],
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
        f=amin[a];
        t=amax[a];
        maxA[0]=amax[0];;
        maxA[1]=amax[1];;
        minB[0]=amin[0];;
        minB[1]=amin[1];;
        do
        {
            newCut=(f+t)/2;
            maxA[a]=newCut;
            minB[a]=newCut+1;
            costA=costB=0;
            for(vi=visible.begin();vi!=visible.end();vi++)
            {
                if(vi->getLoad()->getMax()[a] <= newCut)
                    costA+=vi->getCost();
                else
                    if(vi->getLoad()->getMin()[a] > newCut)
                        costB+=vi->getCost();
                    else
                    {
                        costA+=calcFaceRenderingCost(
                            vi->getLoad(),amin,maxA);
                        costB+=calcFaceRenderingCost(
                            vi->getLoad(),minB,amax);
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

double GeoLoadManager::calcFaceRenderingCost(GeoLoad *load,
                                             Int32 wmin[2],Int32 wmax[2])
{
    Real64 area=0;

    Real64 visible=load->getVisibleFraction(wmin,wmax);
    if(visible)
    {
        area =
            (load->getMax()[0] - load->getMin()[0] + 1)*
            (load->getMax()[1] - load->getMin()[1] + 1);
        UInt32 faces  =load->getFaces();
        return 
            (double)faces*( 
                (1-visible)*_faceCostInvisible +
                visible*(_faceCostVisible +
                         area*_faceCostVisibleArea));
    }
    else
    {
        // should be culled by scenegraph renderer
        return 0;
    }
}

