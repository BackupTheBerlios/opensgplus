/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *             Copyright (C) 2000,2001 by the OpenSG Forum                   *
 *                                                                           *
 *                            www.opensg.org                                 *
 *                                                                           *
 *   contact: dirk@opensg.org, gerrit.voss@vossg.org, mroth@zgdv.de          *
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

//-------------------------------
// 	Includes 					 			    
//-------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "OSGConfig.h"

#include <iostream>
#include <fstream>

#include <OSGLog.h>

#include "OSGRLEImageFileType.h"

OSG_USING_NAMESPACE

/**
 *  if there is a limits implementation in all stl libs, then this
 *  can be removed
 **/
template <class T> inline T maxValue(T)                
{ return 0;         }
template <class T> inline T minValue(T)                
{ return 0;         }
template <> inline signed char maxValue<signed char>(signed char) 
{ return SCHAR_MAX; }
template <> inline signed char minValue<signed char>(signed char) 
{ return SCHAR_MIN; }
template <> inline int         maxValue<int>(int)         
{ return INT_MAX;   }
template <> inline int         minValue<int>(int)         
{ return INT_MIN;   }
template <> inline short       maxValue<short>(short)       
{ return SHRT_MAX;  }
template <> inline short       minValue<short>(short)       
{ return SHRT_MIN;  }

/**
 *  Run length compression
 *
 *  @author Marcus Roth
 *
 *  @param src      source data
 *  @param srcEnd   behind source data
 *  @param dst      destination data
 *
 *  @result behind of destination data
 *
 *  Resulting data structure:
 *  
 *  Equal Values       cnt value              Copy value cnt+1 times
 *  Not equal         -cnt value1 ... valueN  N is -cnt + 1 
 *
 *  In th worst case the result is 1/3 larger then the source.
 *  To be save, the resulting buffer must hold size+size/3 elements
 *
 *  AABAABAABAAB This is the pattern for the worst case 
 *
 *  The template param 'offset' is used to compress data with gaps.
 *  E.g. to compress each color component of an RGBA value you can 
 *  use an offset 4 and call RLEEncode 4 times.
 *  
 *  buffer=RLEEncode<signed char,4>(&img[0],&img[800*600*4  ],buffer);
 *  buffer=RLEEncode<signed char,4>(&img[1],&img[800*600*4+1],buffer);
 *  buffer=RLEEncode<signed char,4>(&img[2],&img[800*600*4+2],buffer);
 *
 **/
template <class T,int offset>
class RLE
{
public:
    static T *encode(const T *src,const T *srcEnd,T *dst)
    {
        T *cntPtr=dst++;
        T cnt=0;
        T curr;
        T last=*src;
        *dst++=*src;
        src+=offset;
        
        while(src != srcEnd)
        {
            curr=*src;
            src+=offset;
            if(curr == last)
            {
                if(cnt >= 0)
                {
                    if( cnt < maxValue(curr) )
                    {
                        cnt++;
                    }
                    else
                    {
                        // start a run becasue the current one is full
                        *cntPtr=cnt;
                        cntPtr=dst++;
                        cnt=0;
                        *dst++=curr;
                    }
                }
                else
                {
                    // current is equal last and cnt < 0. So before this
                    // is a run with unequal elements. The last of this
                    // elements is equal current. So we can remove last
                    // from unequal elements and add 1 to current equal
                    // elements
                    *cntPtr=cnt+1;   
                    cntPtr=dst-1;
                    cnt=1;
                    *dst++=curr;
                }
            }
            else
            {
                if(cnt <= 0 && cnt > minValue(curr) )
                {
                    // unequal elements are counted as negativ numbers
                    cnt--;
                    *dst++=curr;
                }
                else
                {
                    // start a run becasue the current one is full or 
                    // it is not a run for unequal elemts
                    *cntPtr=cnt;
                    cntPtr=dst++;
                    cnt=0;
                    *dst++=curr;
                }
            }
            last=curr;
        }
        *cntPtr=cnt;
        return dst;
    }

    /**
     *  Run length uncompression
     **/
    static const T *decode(const T *src,T *dst,const T *dstEnd)
    {
        T value;
        T cnt;
        while(dst != dstEnd)
        {
            cnt = *src++;
            if(cnt>=0)
            {
                value=*src++;
                while((cnt--)>=0)
                {
                    *dst=value;
                    dst+=offset;
                }
            }
            else
            {
                while((cnt++)<=0)
                {
                    *dst=*src++;
                    dst+=offset;
                }
            }
        }
        return src;
    }
};

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::RLEImageFileType

Fast run lentgh image encoding

*/

/***************************************************************************\
 *                               Types                                     *
\***************************************************************************/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

// Static Class Varible implementations: 
static const Char8 *suffixArray[] = 
{
	"rle"
};

RLEImageFileType RLEImageFileType::_the ( 	suffixArray,
                                            sizeof(suffixArray) );


/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/

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

RLEImageFileType::RLEImageFileType ( const Char8 *suffixArray[], 
                                     UInt16 suffixByteCount )
	: ImageFileType ( suffixArray, suffixByteCount )
{
	return;
}

/** \brief Copy Constructor
 */

RLEImageFileType::RLEImageFileType (const RLEImageFileType &obj )
	: ImageFileType(obj)
{
	return;
}

/** \brief Destructor
 */

RLEImageFileType::~RLEImageFileType (void )
{
	return;
}

/*------------------------------ access -----------------------------------*/

/*------------------------------- dump ----------------------------------*/

/*------------------------------- dump ----------------------------------*/

/*-------------------------------------------------------------------------*\
 -  protected                                                              -
\*-------------------------------------------------------------------------*/

UInt64 RLEImageFileType::restoreData ( Image &image, const UChar8 *buffer,
                                       Int32 /* memSize !!! */)
{
    UInt32 size=image.getSize();
    const Int8 *s = (const Int8*)buffer;
    Int8 *d       = (Int8*)image.getData();

    switch(image.getBpp())
    {
        case 1:
            s=RLE<Int8,1>::decode(s ,&d[0],&d[size  ]);
            break;
        case 3:
            s=RLE<Int8,3>::decode(s ,&d[0],&d[size  ]);
            s=RLE<Int8,3>::decode(s ,&d[1],&d[size+1]);
            s=RLE<Int8,3>::decode(s ,&d[2],&d[size+2]);
            break;
        case 4:
            s=RLE<Int8,3>::decode(s ,&d[0],&d[size  ]);
            s=RLE<Int8,3>::decode(s ,&d[1],&d[size+1]);
            s=RLE<Int8,3>::decode(s ,&d[2],&d[size+2]);
            s=RLE<Int8,3>::decode(s ,&d[3],&d[size+3]);
            break;
        default:
            SFATAL << "Unsupported format" << endl;
    }
    return true;
}

UInt64 RLEImageFileType::storeData ( const Image &image, 
                                     UChar8 *buffer,
                                     Int32 /*memSize*/ )
{
    UInt32          resultSize=0;
    Int8           *d=(Int8*)buffer;
    const Int8     *s=(const Int8*)image.getData();
    UInt32          size=image.getSize();

    switch(image.getBpp())
    {
        case 1:
            d=RLE<Int8,1>::encode(&s[0],&s[size  ],d);
            resultSize= d - (Int8*)buffer;
        case 3:
            d=RLE<Int8,3>::encode(&s[0],&s[size  ],d);
            d=RLE<Int8,3>::encode(&s[1],&s[size+1],d);
            d=RLE<Int8,3>::encode(&s[2],&s[size+2],d);
            resultSize= d - (Int8*)buffer;
            break;
        case 4:
            d=RLE<Int8,4>::encode(&s[0],&s[size  ],d);
            d=RLE<Int8,4>::encode(&s[1],&s[size+1],d);
            d=RLE<Int8,4>::encode(&s[2],&s[size+2],d);
            d=RLE<Int8,4>::encode(&s[3],&s[size+3],d);
            resultSize= d - (Int8*)buffer;
            break;
        default:
            SFATAL << "Unsupported number of components" << endl;
    }
    return resultSize;
} 

bool RLEImageFileType::read (Image & /*image*/, const Char8 */*fileName*/ )
{
  bool retCode = false;
  return retCode;
}

bool RLEImageFileType::write ( const Image &/*image*/, 
                               const Char8 */*fileName*/ )
{	
  bool retCode = false;
  return retCode;
}

/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/








