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

#include "algorithm"
#include "OSGConfig.h"

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
    static Char8 cvsid_cpp[] = "@(#)$Id: OSGGeoLoadManager.cpp,v 1.6 2002/02/27 15:36:10 marcus Exp $";
    static Char8 cvsid_hpp[] = OSG_GEOLOADMANAGERHEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

GeoLoadManager::RegionLoad::RegionLoad(GeoLoad *g):
    geometry(g),
    rendering(0)
{
}
void GeoLoadManager::RegionLoad::update(Int32 amin[2],Int32 amax[2])
{
    rendering = geometry->getRenderingLoad(amin,amax);
}

double sort_t=0;
double sortrow_t=0;
double balance_t=0;
double bestcut_t=0;
double bestcutA_t=0;
double bestcutB_t=0;

/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

/*! Constructor documentation
 */
GeoLoadManager::GeoLoadManager(void) 
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

    // free mem alloced by members of 'this'

    // alloc new mem for members

    // copy
    return *this;
}


/** Add all geometry nodes in the given tree
 **/
void GeoLoadManager::add(NodePtr node)
{
    // is nodecore a geometry?
    if(node->getCore() != NullFC &&
       GeometryPtr::dcast(node->getCore()) != NullFC)
    {
        _geoLoad.push_back(GeoLoad(node));
    }
    // handle all childs
    for(MFNodePtr::iterator n=node->getMFChildren()->begin();
        n!=node->getMFChildren()->end();
        ++n)
    {
        add(*n);
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

    vp->getCamera()->getViewing   ( viewing   ,width,height );
    vp->getCamera()->getProjection( projection,width,height );
    visible.reserve(_geoLoad.size());
    for(GeoLoadVecT::iterator l=_geoLoad.begin() ; l!=_geoLoad.end() ; ++l)
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
        vi->update(wmin,wmax);
    }
    if(regions>1)
        splitRegion(regions,visible,wmin,wmax,result);
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
    findBestCut(visible,amin,amax,axis,cut);
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
            if(vi->geometry->getMax()[axis] <= cut)
                visibleA.push_back(*vi);
            else
                if(vi->geometry->getMin()[axis] > cut)
                    visibleB.push_back(*vi);
                else
                {
                    visibleA.push_back(*vi);
                    visibleB.push_back(*vi);
                    visibleA.rbegin()->update(amin,maxA);
                    visibleB.rbegin()->update(minB,amax);
                }
        }
    }
    if(regionsA>1)
        splitRegion(regionsA,visibleA,amin,maxA,result);
    else
    {
        result.insert(result.end(),amin,amin+2);
        result.insert(result.end(),maxA,maxA+2);
/*
        cout << "Cut: " 
             << amin[0] << "," << amin[1] << " "
             << maxA[0] << "," << maxA[1] << " " << load << endl;
*/
    }
    if(regionsB>1)
        splitRegion(regionsB,visibleB,minB,amax,result);
    else
    {
        result.insert(result.end(),minB,minB+2);
        result.insert(result.end(),amax,amax+2);
/*
        cout << "Cut: " 
             << minB[0] << "," << minB[1] << " "
             << amax[0] << "," << amax[1] << " " << load << endl;
*/
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
                if(vi->geometry->getMax()[a] <= newCut)
                    costA+=vi->rendering;
                else
                    if(vi->geometry->getMin()[a] > newCut)
                        costB+=vi->rendering;
                    else
                    {
                        costA+=vi->geometry->getRenderingLoad(amin ,maxA);
                        costB+=vi->geometry->getRenderingLoad(minB ,amax);
                    }
                
            }
            newCost= osgMax( costA, costB );
            if(newCost<bestCost)
            {
                bestCut=newCut;
                bestCost=newCost;
                bestAxis=a;
            }
            // go into direction of inbalance
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
   
