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

#include "GL/gl.h"
#include "GL/glu.h"
#include "OSGConfig.h"
#include "OSGBaseFunctions.h"
#include "OSGImageComposition.h"
#include "OSGImageFileHandler.h"
#include "OSGImageFileType.h"
#include "OSGRLECoder.h"

OSG_USING_NAMESPACE

namespace 
{
    char cvsid_cpp[] = "@(#)$Id:$";
    char cvsid_hpp[] = OSG_IMAGECOMPOSITIONHEADER_CVSID;
}

/***************************************************************************\
 *                               Types                                     *
\***************************************************************************/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*\
 -  protected                                                              -
\*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/



/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/

/*------------- constructors & destructors --------------------------------*/

/** \brief Constructor
 */

ImageComposition::ImageComposition(void)
{
    clearStatistics();
}

/** \brief Destructor
 */

ImageComposition::~ImageComposition(void)
{
}

/*------------------------------ access -----------------------------------*/

/*---------------------------- properties ---------------------------------*/

/*-------------------------- your_category---------------------------------*/

/** Read image tiles from rendering server
 *
 * @param connection     connection to server
 * @param width          window width
 * @param height         window height
 *
 **/

void ImageComposition::recv(Connection &connection,
                            UInt32     width,
                            UInt32     height)
{
    UInt32 tx,ty,tw,th;
    UInt32 missing=connection.getChannelCount();
    Image image;
    BufferT data;
    BufferT imageData;
    UInt32 dataSize;
    UInt8 finish;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0,width,0,height);
    glDisable(GL_DEPTH_TEST);  
    glPixelStorei(GL_UNPACK_ALIGNMENT,1); 

    // we expect tiles form all connected servers
    while(missing)
    {
        connection.selectChannel();
        connection.getUInt8(finish);
        if(finish)
        {
            missing--;
        }
        else
        {
            // read data
            connection.getUInt32(tx);
            connection.getUInt32(ty);
            connection.getUInt32(dataSize);
            data.resize(dataSize);
            if(dataSize>0)
            {
                connection.get(&data[0],dataSize);
                _statBytesRead       +=dataSize+3*sizeof(UInt32);
            }
            else
            {
                connection.getUInt32(tw);
                connection.getUInt32(th);
                if(imageData.size()<(tw*th*3))
                {
                    imageData.resize(tw*th*3);
                }
                // set image size
                image.set(Image::OSG_RGB_PF,
                          tw,th,1,
                          1,1,0.0,
                          (UChar8*)&imageData[0],false);
                connection.get(image.getData(),tw*th*3);
                _statBytesRead       +=tw*th*3+5*sizeof(UInt32);
            }

            // umcompress
            if(dataSize>0)
            {
                ImageFileType::restore(image,(UChar8*)&data[0],dataSize);
            }

            // draw
            glRasterPos2i(tx,ty);
            glDrawPixels(image.getWidth(),image.getHeight(),
                         GL_RGB,GL_UNSIGNED_BYTE,image.getData());

            // statistics
            _statBytesRead       +=dataSize+3*sizeof(UInt32);
            _statBytesCompress   +=dataSize;
            _statBytesUncompress +=image.getSize();
            _statPixelWrite      +=image.getWidth()*image.getHeight();
        }
    }

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);  
}

/** Send image tiles to the rendering client
 *
 * @param connection     connection to client
 * @param dstX           x position in client window
 * @param dstY           y position in client window
 * @param width          window width
 * @param height         window height
 *
 **/

void ImageComposition::send(Connection &connection,
                            string     mimeType,
                            UInt32     subTileSize,
                            UInt32     dstX,
                            UInt32     dstY,
                            UInt32     width,
                            UInt32     height)
{
    UInt32 tx,ty,tw,th;
    Image image;
    BufferT data;
    BufferT imageData;
    UInt32 dataSize;
    ImageFileType *imageFileType=NULL;

    if(!mimeType.empty())
        imageFileType=ImageFileHandler::the().getFileType(mimeType.c_str());

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0,width,0,height);
    glPixelStorei(GL_PACK_ALIGNMENT,1); 

    data.resize(subTileSize * subTileSize * 4 * 2);
    imageData.resize(subTileSize * subTileSize * 4);
    for(ty=0;ty<height;ty+=subTileSize)
    {
        for(tx=0;tx<width;tx+=subTileSize)
        {
            tw = osgMin(subTileSize,width-tx);
            th = osgMin(subTileSize,height-ty);

            connection.putUInt8(false);
            // set image size
            image.set(Image::OSG_RGB_PF,
                      tw,th,1,
                      1,1,0.0,
                      (UChar8*)&imageData[0],false);
            // read image data
            glReadPixels(tx,ty,tw,th,
                         GL_RGB,GL_UNSIGNED_BYTE,
                         image.getData());
            // compress image
            if(imageFileType)
            {
                dataSize=imageFileType->store(image,(UChar8*)&data[0],
                                              data.size());
                _statBytesCompress  +=dataSize;
            }

            // send data
            connection.putUInt32(tx+dstX);
            connection.putUInt32(ty+dstY);
            if(imageFileType)
            {
                connection.putUInt32(dataSize);
                connection.put(&data[0],dataSize);
                _statBytesWrite     +=dataSize+3*sizeof(UInt32);
            }
            else
            {
                connection.putUInt32(0);
                connection.putUInt32(tw);
                connection.putUInt32(th);
                connection.put(image.getData(),tw*th*3);
                _statBytesWrite     +=tw*th*3+5*sizeof(UInt32);
            }
            connection.flush();
                
            // statistics
            _statPixelRead      +=tw*th;
            _statBytesUncompress+=image.getSize();
        }
    }
    connection.putUInt8(true);
    connection.flush();

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}
 
/*-------------------------- assignment -----------------------------------*/

/** \brief assignment
 */

/*-------------------------- comparison -----------------------------------*/

/** \brief assignment
 */

/** \brief equal
 */

/** \brief unequal
 */

/*-------------------------------------------------------------------------*\
 -  protected                                                              -
\*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/

void ImageComposition::clearStatistics()
{
    _statBytesRead=0;
    _statBytesWrite=0;
    _statBytesCompress=0;
    _statBytesUncompress=0;
    _statPixelRead=0;
    _statPixelWrite=0;
}

void ImageComposition::printStatistics()
{
    printf("Bytes read .............%10d\n",  _statBytesRead);
    printf("Bytes write ............%10d\n",  _statBytesWrite);
    printf("Pixel read .............%10d\n",  _statPixelRead);
    printf("Pixel write ............%10d\n",  _statPixelWrite);
    printf("Compress ...............%10.5f\n",
           (float)_statBytesUncompress/_statBytesCompress);
}

