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
#include "GL/gl.h"
#include "GL/glu.h"
#include "OSGBaseFunctions.h"
#include "OSGLog.h"
#include "OSGViewBufferHandler.h"
#include "OSGImageFileHandler.h"
#include "OSGImageFileType.h"
#include "OSGConnection.h"

OSG_USING_NAMESPACE

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static Char8 cvsid_cpp[] = "@(#)$Id:$";
    static Char8 cvsid_hpp[] = OSG_VIEWBUFFERHANDLER_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

/** \class osg::ViewBufferHandler
 *  \ingroup ClusterLib
 *  \brief Viewbuffer functions
 *
 * This class provides an interface to an OpenGL view buffer.
 * The Buffer contents can be transfered over a Connection.
 * All Image types can be used for image compression. The
 * ViewBufferHandler can access RGBA, Stencil and Z-Buffer values.
 * 
 * The whole imagebuffer is divided into subtiles. Each subtile
 * is read from the buffer, compressed and send over the nertwork.
 * In most cases, the buffer read and the network send of the previous
 * tile is done in parallel.
 * 
 * \todo currently ViewBufferHandler works on the activated window.
 *       Is this a good idea. Better WindowPtr as parameter and then
 *       call activate before send,recv? MR
 **/

/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

/*! Constructor documentation
 *
 * Initialize a ViewBufferHandler. By default, no image compression
 * is set and the subtile size is set to 32
 */
ViewBufferHandler::ViewBufferHandler(void):
    _imgTransType(NULL),
    _subTileSize(32)
{
}

/*-------------------------------------------------------------------------*/
/*                             Destructor                                  */

/*! Destructor documentation
 */
ViewBufferHandler::~ViewBufferHandler(void)
{
}

/** \brief Receive image data from a network connection
 *
 * Receive image data from all channels of a conneciton. The receive is
 * finished, when the last channel signals a transmission end.
 *
 * \param connection   Connection from which read is done
 */
void ViewBufferHandler::recv(Connection &connection)
{
    UInt32             tx,ty,tw,th;
    UInt32             missing=connection.getChannelCount();
    Image              image;
    BufferT            data;
    BufferT            imageData;
    UInt32             dataSize;
    UInt32             component;
    GLenum             glformat;
    int                componentCnt;

    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0,getBufferWidth(),
               0,getBufferHeight());
    glDisable(GL_DEPTH_TEST);  
    glPixelStorei(GL_UNPACK_ALIGNMENT,1); 

    // we expect tiles form all connected servers
    while(missing)
    {
        connection.selectChannel();
        connection.getUInt32(component);
        if(!component)
        {
            missing--;
            continue;
        }
        // get dimension
        connection.getUInt32(tx);
        connection.getUInt32(ty);
        connection.getUInt32(tw);
        connection.getUInt32(th);
        glRasterPos2i(tx,ty);
        // =========== recv stencil =====================================
        if(component & STENCIL)
        {
            data.resize(tw*th);
            connection.get(&data[0],tw*th);
            glDrawPixels(tw,th,GL_STENCIL_INDEX,GL_UNSIGNED_BYTE,&data[0]);
            glEnable(GL_STENCIL_TEST);  
        }

        // =========== recv depth =======================================
        if(component & DEPTH)
        {
            glEnable(GL_DEPTH_TEST);  
            glEnable(GL_STENCIL_TEST);  
            glStencilFunc(GL_ALWAYS,1,1);
            glStencilOp(GL_KEEP,GL_ZERO,GL_REPLACE);
            data.resize(tw*th*sizeof(float));
            connection.get(&data[0],tw*th*sizeof(float));
            glDrawPixels(tw,th,GL_DEPTH_COMPONENT,GL_FLOAT,&data[0]);
            glDisable(GL_DEPTH_TEST);  
        }

        // =========== recv RGBA ========================================
        if(component & RGBA)
        {
            if(component & (DEPTH | STENCIL))
            {
                glStencilFunc(GL_EQUAL,1,1);
            }
            switch(component&RGBA)
            {
                case RGB: 
                    glformat     = GL_RGB; 
                    componentCnt = 3;
                    break;
                case RGBA:
                    glformat     = GL_RGBA;
                    componentCnt = 4;
                    break;
                default:
                    SFATAL << "Component combination not supported" << endl;
                    return;
            }
            connection.getUInt32(dataSize);
            // compression ?
            if(dataSize>0)
            {
                data.resize(dataSize);
                connection.get(&data[0],dataSize);
                imageData.resize(tw*th*componentCnt);
                ImageFileType::restore(image,(UChar8*)&data[0],dataSize);
                glDrawPixels(tw,th,glformat,GL_UNSIGNED_BYTE,image.getData());
            }
            else
            {
                data.resize(tw*th*componentCnt);
                connection.get(&data[0],tw*th*componentCnt);
                glDrawPixels(tw,th,glformat,GL_UNSIGNED_BYTE,&data[0]);
            }
        }

        if(component & (DEPTH | STENCIL))
        {
            glDisable(GL_STENCIL_TEST);  
        }
    }
    connection.putUInt32(0);
    connection.flush();
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);  
}

/** \brief Send parts of a view buffer to a Connection
 *
 * \param connection   send to this connection
 * \param component    Component to transfer
 * \param x1           left begin of rectangle to be transfered
 * \param y1           bottom begin of rectangle to be transfered
 * \param x2           right. The right pixel is included
 * \param x2           top. The top pixel is included
 * \param toX          copy to this x position on destination buffer
 * \param toY          copy to this y position on destination buffer
 */
void ViewBufferHandler::send(Connection &connection,
                             UInt32     component,
                             UInt32     x1,
                             UInt32     y1,
                             UInt32     x2,
                             UInt32     y2,
                             UInt32     toX,
                             UInt32     toY)
{
    UInt32             tx,ty,tw,th;
    Image              image;
    BufferT            data;
    BufferT            imageData;
    UInt32             dataSize;
    GLenum             glformat;
    Image::PixelFormat imgformat;
    int                componentCnt;
    int                imgtranssize=0;

    switch(component&RGBA)
    {
        case RGB: 
            glformat     = GL_RGB; 
            imgformat    = Image::OSG_RGB_PF;
            componentCnt = 3;
            break;
        case RGBA:
            glformat     = GL_RGBA;
            imgformat    = Image::OSG_RGBA_PF; 
            componentCnt = 4;
            break;
        default:
            SFATAL << "Component combination not supported" << endl;
            return;
    }
    // resize image buffer
    imageData.resize(_subTileSize*_subTileSize*componentCnt);

    glPixelStorei(GL_PACK_ALIGNMENT,1); 

    for(ty=y1;ty<=y2;ty+=_subTileSize)
    {
        for(tx=x1;tx<=x2;tx+=_subTileSize)
        {
            tw = osgMin(_subTileSize,x2+1-tx);
            th = osgMin(_subTileSize,y2+1-ty);

            connection.putUInt32(component);
            connection.putUInt32(tx+toX);
            connection.putUInt32(ty+toY);
            connection.putUInt32(tw);
            connection.putUInt32(th);

            // =========== send STENCIL =======================================
            if(component & STENCIL)
            {
                // read stencil buffer
                data.resize(tw*th);
                glReadPixels(tx,ty,tw,th,
                             GL_STENCIL_INDEX,GL_UNSIGNED_BYTE,
                             &data[0]);
                connection.put(&data[0],tw*th);
            }

            // =========== send DEPTH =======================================
            if(component & DEPTH)
            {
                // read stencil buffer
                data.resize(tw*th*sizeof(float));
                glReadPixels(tx,ty,tw,th,
                             GL_DEPTH_COMPONENT,GL_FLOAT,
                             &data[0]);
                connection.put(&data[0],tw*th*sizeof(float));
            }

            // =========== send RGBA ========================================
            if(component & RGBA)
            {
                // use compression ?
                if(_imgTransType)
                {
                    // set image size
                    image.set(imgformat,
                              tw,th,1,
                              1,1,0.0,
                              (UChar8*)&imageData[0],false);
                    // read buffer data into image
                    glReadPixels(tx,ty,tw,th,
                                 glformat,GL_UNSIGNED_BYTE,
                                 image.getData());
                    // bug maxsize is not big enugh
                    data.resize(_imgTransType->maxBufferSize(image)+1000);
                    dataSize=_imgTransType->store(image,
                                                  (UChar8*)&data[0],
                                                  data.size());
                    connection.putUInt32(dataSize);
                    connection.put(&data[0],dataSize);
                    imgtranssize+=dataSize;
                }
                else
                {
                    data.resize(tw*th*componentCnt);
                    // read buffer data
                    glReadPixels(tx,ty,tw,th,
                                 glformat,GL_UNSIGNED_BYTE,
                                 &data[0]);
                    connection.putUInt32(0);
                    connection.put(&data[0],tw*th*componentCnt);
                    imgtranssize+=tw*th*componentCnt;
                }
            }

            connection.flush();
        }
    }
    connection.putUInt32(0);
    connection.flush();
    connection.selectChannel();
    connection.getUInt32(component);
    cout << "IMG size" << imgtranssize << endl;

}

/** \brief Send parts of a view buffer to a Connection
 *
 * \param connection   send to this connection
 * \param component    Component to transfer
 * \param toX          copy to this x position on destination buffer
 * \param toY          copy to this y position on destination buffer
 */
void ViewBufferHandler::send(Connection &connection,
                             UInt32     component,
                             UInt32     toX,
                             UInt32     toY)
{
    send(connection,component,0,0,getBufferWidth(),getBufferHeight(),toX,toY);
}

/** \brief Set compression type
 *
 * By default, no compression is used for image transmission. 
 * The given mime type identifies an ImageType e.g. "JPEG".
 * This image type is used for compression. 
 *
 * \param mimeType   Image mime type or NULL for no compression.
 */
void ViewBufferHandler::setImgTransType(const char *mimeType)
{
    if(mimeType==NULL)
    {
        _imgTransType=NULL;
    }
    else
    {
        _imgTransType=ImageFileHandler::the().getFileType(mimeType);
    }
}

/** \brief Set subtile size
 *
 * The whole buffer is transfered as small subtiles. Increasing or 
 * decreasing the subtile size will result in changes to the performance.
 * The best size depends on network package size and the ration 
 * between network performance and buffer read/write performance.
 *
 * \param size   Subtile size
 */
void ViewBufferHandler::setSubtileSize(UInt32 size)
{
    _subTileSize=size;
}

/** \brief Get buffer width
 *
 * \return current buffer width
 */
UInt32 ViewBufferHandler::getBufferWidth()
{
    GLint view[4];
    glGetIntegerv(GL_VIEWPORT,view);
    return view[2];
}

/** \brief Get buffer width
 *
 * \return current buffer height
 */
UInt32 ViewBufferHandler::getBufferHeight()
{
    GLint view[4];
    glGetIntegerv(GL_VIEWPORT,view);
    return view[3];
}

