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

#ifndef OSGRLECODER_H
#define OSGRLECODER_H

#include <limits.h>

/**
 *  if there is a limits implementation in all stl libs, then this
 *  can be removed
 **/
template <class T> inline T maxValue()                 { return 0;         }
template <class T> inline T minValue()                 { return 0;         }
template <> inline signed char maxValue<signed char>() { return SCHAR_MAX; }
template <> inline signed char minValue<signed char>() { return SCHAR_MIN; }
template <> inline int         maxValue<int>()         { return INT_MAX;   }
template <> inline int         minValue<int>()         { return INT_MIN;   }
template <> inline short       maxValue<short>()       { return SHRT_MAX;  }
template <> inline short       minValue<short>()       { return SHRT_MIN;  }

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
T *RLEEncode(const T *src,const T *srcEnd,T *dst)
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
                if( cnt < maxValue<T>() )
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
            if(cnt <= 0 && cnt > minValue<T>() )
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
template <class T,int offset>
const T *RLEDecode(const T *src,T *dst,const T *dstEnd)
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

#endif


