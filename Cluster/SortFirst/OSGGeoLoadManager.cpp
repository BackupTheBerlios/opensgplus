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
    static Char8 cvsid_cpp[] = "@(#)$Id: OSGGeoLoadManager.cpp,v 1.3 2002/02/11 17:00:09 marcus Exp $";
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

/*! assignment
 */
void GeoLoadManager::balance(ViewportPtr    vp,
                             UInt32         servers,
                             ResultT       &result)
{
    sort_t=0;
    sortrow_t=0;
    balance_t=0;
    bestcut_t=0;
    bestcutA_t=0;
    bestcutB_t=0;
    balance_t-=getSystemTime();

    int a;
    int amin[2]={vp->getPixelWidth()-1, vp->getPixelHeight()-1};
    int amax[2]={0,0};
    Int32 vpMax[2]={vp->getPixelWidth()-1, vp->getPixelHeight()-1};
    RegionLoadVecT visible;
    RegionLoadVecT::iterator vi;

    sortrow_t-=getSystemTime();
    visible.reserve(_geoLoad.size());
    for(GeoLoadVecT::iterator l=_geoLoad.begin() ; l!=_geoLoad.end() ; ++l)
    {
        // update view dependent values
        l->updateView(vp);
        // sort by min values
        if(l->isVisible())
        {
            // collect visible geometries
            visible.push_back(RegionLoad(&(*l)));
            for(a=0;a<=1;++a)
            {
                // shrink min, max to visible geometries
                if(l->getMin()[a] <   amin[a] &&
                   l->getMax()[a] >= 0)
                    if(l->getMin()[a]<0)
                        amin[a]=0;
                    else
                        amin[a]=l->getMin()[a];
                if(l->getMax()[a] >   amax[a] &&
                   l->getMin()[a] <= vpMax[a])
                    if(l->getMax()[a]>vpMax[a])
                        amax[a]=vpMax[a];
                    else
                        amax[a]=l->getMax()[a];
            }
        }
    }
    sortrow_t+=getSystemTime();
    if(amax[0]<amin[0] ||
       amax[1]<amin[1])
       amax[0]=amin[0]=amax[1]=amin[1]=0;
    // calculate region cost
    for(vi=visible.begin();vi!=visible.end();vi++)
    {
        vi->update(amin,amax);
    }
    splitRegion(servers,visible,amin,amax,result);

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
void GeoLoadManager::splitRegion(UInt32          servers,
                                 RegionLoadVecT &visible,
                                 Int32           amin[2],
                                 Int32           amax[2],
                                 ResultT        &result)
{
    Int32  axis,cut;
    Int32  maxA[2];
    Int32  minB[2];
    Int32  serversA,serversB;
    RegionLoadVecT visibleA;
    RegionLoadVecT visibleB;
    RegionLoadVecT::iterator vi;
    Real32 load;

    // search for best cut
    load=findBestCut(visible,amin,amax,axis,cut);
    // create new regions
    maxA[axis  ]=cut;
    maxA[axis^1]=amax[axis^1];
    minB[axis  ]=cut+1;
    minB[axis^1]=amin[axis^1];
    visibleA.reserve(visible.size());
    visibleB.reserve(visible.size());
    // split servers
    serversA=servers/2;
    serversB=servers-serversA;

    if(serversA>1 || serversB>1)
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
    if(serversA>1)
        splitRegion(serversA,visibleA,amin,maxA,result);
    else
    {
        result.insert(result.end(),amin,amin+2);
        result.insert(result.end(),maxA,maxA+2);
        cout << "Cut: " 
             << amin[0] << "," << amin[1] << " "
             << maxA[0] << "," << maxA[1] << " " << load << endl;
    }
    if(serversB>1)
        splitRegion(serversB,visibleB,minB,amax,result);
    else
    {
        result.insert(result.end(),minB,minB+2);
        result.insert(result.end(),amax,amax+2);
        cout << "Cut: " 
             << minB[0] << "," << minB[1] << " "
             << amax[0] << "," << amax[1] << " " << load << endl;
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
   




