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
    static Char8 cvsid_cpp[] = "@(#)$Id: OSGGeoLoadManager.cpp,v 1.2 2002/02/11 05:45:54 vossg Exp $";
    static Char8 cvsid_hpp[] = OSG_GEOLOADMANAGERHEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

class GeoRegionLoad {
public:
    GeoRegionLoad(GeoLoad *l,Real32 c,Int32 mi,Int32 ma):
        load(l),rendering(c),_min(mi),_max(ma){}
    Real32   rendering;
    GeoLoad *load;
    Int32    _min;
    Int32    _max;
};

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

    int i,a;
    int amin[2]={vp->getPixelWidth()-1, vp->getPixelHeight()-1};
    int amax[2]={0,0};
    Int32 vpMax[2]={vp->getPixelWidth()-1, vp->getPixelHeight()-1};

    sortrow_t-=getSystemTime();
    for(GeoLoadVecT::iterator l=_geoLoad.begin() ; l!=_geoLoad.end() ; ++l)
    {
        // update view dependent values
        l->updateView(vp);
        // sort by min values
        if(l->isVisible())
        {
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
    splitRegion(servers,amin,amax,result);

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
void GeoLoadManager::splitRegion(UInt32  servers,
                                 Int32   amin[2],
                                 Int32   amax[2],
                                 ResultT &result)
{
    if(servers==1)
    {
        result.insert(result.end(),amin,amin+2);
        result.insert(result.end(),amax,amax+2);
        return;
    }
    Int32  x    ,y;
    float  xLoad,yLoad;
    Int32  newMaxA[2];
    Int32  newMaxB[2];
    Int32  newMinA[2];
    Int32  newMinB[2];

    bestcut_t-=getSystemTime();
    // test horizontal cut
    findBestCut(amin,amax,0,1,x,xLoad);
    // test vertical cut
    findBestCut(amin,amax,1,0,y,yLoad);
    bestcut_t+=getSystemTime();

    // create new regions
    if(xLoad < yLoad)
    {
        // Region A
        newMinA[0] = amin[0];
        newMinA[1] = amin[1];
        newMaxA[0] = x;
        newMaxA[1] = amax[1];
        // Region B
        newMinB[0] = x+1;
        newMinB[1] = amin[1];
        newMaxB[0] = amax[0];
        newMaxB[1] = amax[1];
    }
    else
    {
        // Region A
        newMinA[0] = amin[0];
        newMinA[1] = amin[1];
        newMaxA[0] = amax[0];
        newMaxA[1] = y;
        // Region B
        newMinB[0] = amin[0];
        newMinB[1] = y+1;
        newMaxB[0] = amax[0];
        newMaxB[1] = amax[1];
    }
    splitRegion(servers/2            ,newMinA,newMaxA,result);
    splitRegion(servers - (servers/2),newMinB,newMaxB,result);
}

/** Find best cut through the geometries
 **/
void GeoLoadManager::findBestCut(Int32   amin[2],
                                 Int32   amax[2],
                                 UInt32  a,
                                 UInt32  b,
                                 Int32  &bestCut,
                                 Real32 &bestCost)
{
    GeoLoadVecT::iterator l;
    vector<GeoRegionLoad> visible;
    vector<GeoRegionLoad>::iterator v;
    Int32 f,t,newCut;
    Int32 minB[2];
    Int32 maxA[2];
    Real32 costA=0,costB=0;
    Real32 newCost;

    bestCost=1e22;
    // collect all visible geometries
    for(l=_geoLoad.begin();l!=_geoLoad.end();l++)
    {
        if(l->checkRegion(amin,amax))
        {
            visible.push_back( GeoRegionLoad(&(*l),
                                             l->getRenderingLoad(amin ,amax),
                                             amin[a],amax[a]) );
        }
    }
    f=amin[a];
    t=amax[a];
    maxA[b]=amax[b];;
    minB[b]=amin[b];;
    do
    {
        newCut=(f+t)/2;
        maxA[a]=newCut;
        minB[a]=newCut+1;
        costA=costB=0;
        for(v=visible.begin();v!=visible.end();v++)
        {
            if(v->_max <= newCut)
                costA+=v->rendering;
            else
                if(v->_min > newCut)
                    costB+=v->rendering;
                else
                {
                    costA+=v->load->getRenderingLoad(amin ,maxA);
                    costB+=v->load->getRenderingLoad(minB ,amax);
                }

        }
        newCost= osgMax( costA, costB );
        if(newCost<bestCost)
        {
            bestCut =newCut;
            bestCost=newCost;
        }
        // go into direction of inbalance
        if(costA>costB)
            t=newCut;
        else
            f=newCut;
    }
    while(t-f > 2);
}



