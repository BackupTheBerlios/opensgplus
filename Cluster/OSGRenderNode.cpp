/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *           Copyright (C) 2000,2001,2002 by the OpenSG Forum                *
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

#include <vector>

#include <OSGConfig.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <OSGBaseFunctions.h>
#include <OSGTime.h>
#include <OSGLog.h>

#include "OSGRenderNode.h"

OSG_USING_NAMESPACE
using namespace std;

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static Char8 cvsid_cpp[] = "@(#)$Id:$";
    static Char8 cvsid_hpp[] = OSG_CLUSTERNODE_HEADER_CVSID;
    static Char8 cvsid_inl[] = OSG_CLUSTERNODE_INLINE_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

/** \class osg::RenderNode
 *  \ingroup ClusterLib
 *  \brief cluster node information
 **/

/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

/*! Constructor 
 */
RenderNode::RenderNode(void):
    _visibleFaceCost(.001),
    _invisibleFaceCost(.0008),
    _drawPixelCost(.0001),
    _readPixelCost(.0001),
    _writePixelCost(.0001)
{
}

/*! copy constructor
 */
RenderNode::RenderNode(const RenderNode &source):
    _visibleFaceCost(source._visibleFaceCost),
    _invisibleFaceCost(source._invisibleFaceCost),
    _drawPixelCost(source._drawPixelCost),
    _readPixelCost(source._readPixelCost),
    _writePixelCost(source._writePixelCost)
{
}

/*-------------------------------------------------------------------------*/
/*                             Destructor                                  */

/*! Destructor documentation
 */
RenderNode::~RenderNode(void)
{
}

/*-------------------------------------------------------------------------*/
/*                             Assignment                                  */

/*! assignment
 */
RenderNode& RenderNode::operator = (const RenderNode &source)
{
    if(this == &source)
        return *this;

    _visibleFaceCost    = source._visibleFaceCost;
    _invisibleFaceCost  = source._invisibleFaceCost;
    _drawPixelCost      = source._drawPixelCost;
    _readPixelCost      = source._readPixelCost;
    _writePixelCost     = source._writePixelCost;

    return *this;
}

/*-------------------------------------------------------------------------*/
/*                             Performance analysis                        */


/** get graphics performance
 * 
 * This is a rough estimation of rendering costst for visible faces,
 * faces outside of the viewport and size dependent rendering costs.
 * <pre>
 * // face cost calculation
 * cost = invisible * invisibleFaceCost +
 *        visible * visibleFaceCost + 
 *        pixel * pixelCost
 * </pre>
 *
 **/
void RenderNode::determinePerformance( void )
{
    int c;
    double faces=0;
    double A,B,C;

    SLOG << "Start rendering benchmark" << endl;
    // create display list
    GLuint dList1 = glGenLists(1);
    glNewList(dList1, GL_COMPILE);
    float step = .1;
    int count  = 500;
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
    double t=runFaceBench(dList2,128,1.0);
    count=(int)(count/t);
    glNewList(dList2, GL_COMPILE);
    for(int c=0;c<count;++c)
    {
        glCallList(dList1);
    }        
    glEndList();

    glCallList(dList2);
    glFinish();

    UInt32 aSize=256;
    UInt32 bSize= 32;
    UInt32 cSize=128;
    Real32 aVisible=1.0;
    Real32 bVisible=1.0;
    Real32 cVisible= .2;

    // do the face benchmarks
    do
    {
        A=runFaceBench(dList2,aSize,aVisible)/count;
        B=runFaceBench(dList2,bSize,bVisible)/count;
        C=runFaceBench(dList2,cSize,cVisible)/count;
        // cost = faces(1-visible)*i + vaces*visible*j + size^2 * k;
        // i:  only if invisible
        // j:  only if visible, but not size dependent
        // k:  only size dependent
        
        _drawPixelCost     = 
            (A-B) / 
            (aSize*(double)aSize-bSize*(double)bSize );
        
        _visibleFaceCost   = (A - _drawPixelCost*aSize*aSize)/(double)faces;
        
        _invisibleFaceCost = 
            (C - 
             _visibleFaceCost * cVisible * faces - 
             _drawPixelCost   * cSize*cSize)/((1-cVisible)*faces);
    } while(_invisibleFaceCost<0  ||     // some logical tests
            _visibleFaceCost<0);

    UInt32 width,height;
    GLint view[4];

    // test write performance
    glPixelStorei(GL_PACK_ALIGNMENT,1); 
    glPixelStorei(GL_UNPACK_ALIGNMENT,1); 
    vector<UInt8> pixels;
    glGetIntegerv(GL_VIEWPORT,view);
    width =osgMin(256,view[2]);
    height=osgMin(256,view[3]);
    pixels.resize(width*height*4);
    t=-getSystemTime();
    for(c=0;c<2;++c)
        glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,&pixels[0]);
    glFlush();
    t+=getSystemTime();
    _readPixelCost=t/(c*width*height);

    // test write performance
    glGetIntegerv(GL_VIEWPORT,view);
    width =view[2];
    height=view[3];
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0,view[2],0,view[3]);
    glRasterPos2i(0,0);
    t=-getSystemTime();
    for(c=0;c<2;++c)
        glDrawPixels(width,height,GL_RGB,GL_UNSIGNED_BYTE,&pixels[0]);
    glFlush();
    t+=getSystemTime();
    _writePixelCost=t/(c*width*height);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    SLOG << "End rendering benchmark" << endl;
    
#if 0
    cout << A << endl;
    cout << 
        faces*(1-aVisible) * invisibleFaceCost +
        faces*aVisible * visibleFaceCost + 
        aSize*aSize * pixelCost
         << endl;

    cout << B << endl;
    cout << 
        faces*(1-aVisible) * invisibleFaceCost +
        faces*aVisible * visibleFaceCost + 
        bSize*bSize * pixelCost
         << endl;

    cout << C << endl;
    cout << 
        faces*(1-cVisible) * invisibleFaceCost +
        faces*cVisible * visibleFaceCost + 
        cSize*cSize * pixelCost
         << endl;
#endif

#if 0
    double v,s;
    for(Real32 v=.1;v<=1.01;v+=.1)
    {
        for(Int32 s=10;s<=200;s+=10)
        {
            printf("%d %f %f\n",s*s,v*100,
                   
                   100-(faces*(1-v) * invisibleFaceCost +
                        faces*v * visibleFaceCost + 
                        s*s * pixelCost) * 
                   100 / (runFaceBench(dList2,s,v)/count));
        }
//        printf("\n");
    }
#endif

    glDeleteLists(dList2,1);
    glDeleteLists(dList1,1);
}

/** Write class contents to the given data stream
 *
 **/

void RenderNode::copyToBin(BinaryDataHandler &handle)
{
    handle.putUInt32( (UInt32)(1.0/_invisibleFaceCost) );
    handle.putUInt32( (UInt32)(1.0/_visibleFaceCost) );
    handle.putUInt32( (UInt32)(1.0/_drawPixelCost) );
    handle.putUInt32( (UInt32)(1.0/_readPixelCost) );
    handle.putUInt32( (UInt32)(1.0/_writePixelCost) );
}

/** Read class contents from the given data stream
 *
 **/

void RenderNode::copyFromBin(BinaryDataHandler &handle)
{
    UInt32 count;
    
    handle.getUInt32(count);
    _invisibleFaceCost = 1.0/count;
    handle.getUInt32(count);
    _visibleFaceCost = 1.0/count;
    handle.getUInt32(count);
    _drawPixelCost = 1.0/count;
    handle.getUInt32(count);
    _readPixelCost = 1.0/count;
    handle.getUInt32(count);
    _writePixelCost = 1.0/count;
}

/** Set render performance values for a group of render nodes
 **/
void RenderNode::setGroup(const RenderNode *begin,const RenderNode *end)
{
    const RenderNode *i;
    Real32 invisibleFaces=0;
    Real32 visibleFaces=0;
    Real32 drawPixels=0;
    Real32 readPixels=0;
    Real32 writePixels=0;

    for(i=begin;i!=end;++i)
    {
        invisibleFaces+=1.0/i->_invisibleFaceCost;
        visibleFaces  +=1.0/i->_visibleFaceCost;
        drawPixels    +=1.0/i->_drawPixelCost;
        readPixels    +=1.0/i->_readPixelCost;
        writePixels   +=1.0/i->_writePixelCost;
    }
    _invisibleFaceCost=(1.0/invisibleFaces);
    _visibleFaceCost  =(1.0/visibleFaces);
    _drawPixelCost    =(1.0/drawPixels);
    _readPixelCost    =(1.0/readPixels);
    _writePixelCost   =(1.0/writePixels);
}

/** Dump class contents
 **/

void RenderNode::dump(void) const
{
    FLOG(("Clipped Faces/s  : %20.5f\n",1.0/_invisibleFaceCost));
    FLOG(("Faces/s          : %20.5f\n",1.0/_visibleFaceCost));
    FLOG(("Pixel/s          : %20.5f\n",1.0/_drawPixelCost));
    FLOG(("Read pixel/s     : %20.5f\n",1.0/_readPixelCost));
    FLOG(("Write pixel/s    : %20.5f\n",1.0/_writePixelCost));
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
double RenderNode::runFaceBench(UInt32 dlist,UInt32 size,Real32 visible)
{
    glViewport(0, 0, size, size);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glScalef(1.0/visible,1.0,1.0);
//    glTranslatef(1.0-visible,0,0);
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

