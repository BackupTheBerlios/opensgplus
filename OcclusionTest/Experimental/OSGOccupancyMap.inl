/*---------------------------------------------------------------------------*\
 *                     OpenSG PLUS Occlusion Test                            *
 *                                                                           *
 *                                                                           *
 *             Copyright (C) 2002 by the WSI/GRIS Uni Tuebingen              *
 *                                                                           *
 *                            www.opensg.org                                 *
 *                                                                           *
 *   contact: staneker@gris.uni-tuebingen.de                                 *
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

#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGDrawAction.h>
#include <OpenSG/OSGViewport.h>

OSG_BEGIN_NAMESPACE

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

/**************************************************
 * Fragmented Z-Buffer for Occlusion Culling Test *
 **************************************************/
inline bool OccupancyMap::getPoint(const Int32& x, const Int32& y){
	const Int32 pos=(x/BUFFERTYPESIZE)+(_h-y)*_w;
	if(pos>_w*_h)
		return(1);	// FIXME
	if(pos<0)
		return(1);	// FIXME
	return(_buffer[pos] & (1<<(x%BUFFERTYPESIZE)));
};

inline void OccupancyMap::setPoint(const Int32& x, const Int32& y, const bool& v){
	const Int32 pos=(x/BUFFERTYPESIZE)+(_h-y)*_w;
	if(pos>_w*_h)
		return;	// FIXME
	if(pos<0)
		return;	// FIXME
	if(v)
		_buffer[pos]=_buffer[pos] | (1 << (x%BUFFERTYPESIZE));
	else
		_buffer[pos]=_buffer[pos] & (!(1 << (x%BUFFERTYPESIZE)));
};

inline bool OccuancyMap::boxIsVisible(void){
	return(1);
};

inline bool OccupancyMap::boxIsHidden(void){
	return(0);
};

inline void OccupancyMap::renderTriangle(const Int16& x1, const Int16& y1, 
		const Int16& x2, const Int16& y2, 
		const Int16& x3, const Int16& y3){
	const bool v=1;
	Int16 x[3], y[3];
	x[0]=x1; y[0]=y1;
	x[1]=x2; y[1]=y2;
	x[2]=x3; y[2]=y3;

	// y-Sorierung
	for(char i=1; i<3; i++){
		if(y[i-1]>y[i]){
			Int16 tmp;
			tmp=y[i-1]; y[i-1]=y[i]; y[i]=tmp;
			tmp=x[i-1]; x[i-1]=x[i]; x[i]=tmp;
			i=0;
		}
	}
	Real32 dx1=(Real32)(x[1]-x[0])/(Real32)(y[1]-y[0]);
	Real32 dx2=(Real32)(x[2]-x[0])/(Real32)(y[2]-y[0]);
	Real32 xp1=(Real32)x[0];
	Real32 xp2=(Real32)x[0];
	for(Int16 i=y[0]; i<y[1]; i++){
		if(i>0 && i<=_h){
			if(xp1<xp2){
				for(Int16 j=(Int16)xp1; j<(Int16)xp2; j++){
					setPoint(j,i,v);
				}
			}else{
				for(Int16 j=(Int16)xp2; j<(Int16)xp1; j++){
					setPoint(j,i,v);
				}
			}
		}
		xp1+=dx1;
		xp2+=dx2;
	}

	xp1=(Real32)x[1];
	dx1=(Real32)(x[2]-x[1])/(Real32)(y[2]-y[1]);
	for(Int16 i=y[1]; i<y[2]; i++){
		if(i>0 && i<=_h){
			if(xp1<xp2){
				for(Int16 j=(Int16)xp1; j<(Int16)xp2; j++){
					setPoint(j,i,v);
				}
			}else{
				for(Int16 j=(Int16)xp2; j<(Int16)xp1; j++){
					setPoint(j,i,v);
				}
			}
		}
		xp1+=dx1;
		xp2+=dx2;
	}
};

inline bool OccupancyMap::testTriangle(const Int16& x1, const Int16& y1,
		const Int16& x2, const Int16& y2,
		const Int16& x3, const Int16& y3){
	const bool v=0;
	Int16 x[3], y[3];
	x[0]=x1; y[0]=y1;
	x[1]=x2; y[1]=y2;
	x[2]=x3; y[2]=y3;

	// y-Sorierung
	for(char i=1; i<3; i++){
		if(y[i-1]>y[i]){
			Int16 tmp;
			tmp=y[i-1]; y[i-1]=y[i]; y[i]=tmp;
			tmp=x[i-1]; x[i-1]=x[i]; x[i]=tmp;
			i=0;
		}
	}
	Real32 dx1=(Real32)(x[1]-x[0])/(Real32)(y[1]-y[0]);
	Real32 dx2=(Real32)(x[2]-x[0])/(Real32)(y[2]-y[0]);
	Real32 xp1=(Real32)x[0];
	Real32 xp2=(Real32)x[0];
	for(Int16 i=y[0]; i<y[1]; i++){
		if(i>0 && i<=_h){
			if(xp1<xp2){
				for(Int16 j=(Int16)xp1; j<(Int16)xp2; j++){
					if(visitPoint(getPoint(j,i),v)) return(1);	// Triangle is visible -> early out...
				}
			}else{
				for(Int16 j=(Int16)xp2; j<(Int16)xp1; j++){
					if(visitPoint(getPoint(j,i),v)) return(1);	// Triangle is visible -> early out...
				}
			}
		}
		xp1+=dx1;
		xp2+=dx2;
	}

	xp1=(Real32)x[1];
	dx1=(Real32)(x[2]-x[1])/(Real32)(y[2]-y[1]);
	for(Int16 i=y[1]; i<y[2]; i++){
		if(i>0 && i<=_h){
			if(xp1<xp2){
				for(Int16 j=(Int16)xp1; j<(Int16)xp2; j++){
					if(visitPoint(getPoint(j,i),v)) return(1);	// Triangle is visible -> early out...
				}
			}else{
				for(Int16 j=(Int16)xp2; j<(Int16)xp1; j++){
					if(visitPoint(getPoint(j,i),v)) return(1);	// Triangle is visible -> early out...
				}
			}
		}
		xp1+=dx1;
		xp2+=dx2;
	}
	return(0);
};

inline bool OccupancyMap::visitPoint(const bool& v1, const bool& v2){
	if(v1==v2){
		if(++_pixcount>_maxpix) return(1);	// visible
	}
	return(0);	// hidden
};

inline bool OccupancyMap::testBox(const Int16* bb_x, const Int16* bb_y, const Real32* bb_z, const char bb_minidx){
	_pixcount=0;
	if(bb_minidx>3){
		if(testTriangle(bb_x[0],bb_y[0], bb_x[1],bb_y[1], bb_x[3],bb_y[3])) return(boxIsVisible());
		if(testTriangle(bb_x[3],bb_y[3], bb_x[0],bb_y[0], bb_x[2],bb_y[2])) return(boxIsVisible());
	}
	
	if(bb_minidx!=0 && bb_minidx!=1 && bb_minidx!=4 && bb_minidx!=5){
		if(testTriangle(bb_x[0],bb_y[0], bb_x[5],bb_y[5], bb_x[4],bb_y[4])) return(boxIsVisible());
		if(testTriangle(bb_x[0],bb_y[0], bb_x[1],bb_y[1], bb_x[5],bb_y[5])) return(boxIsVisible());
	}

	if(bb_minidx!=0 && bb_minidx!=2 && bb_minidx!=4 && bb_minidx!=6){
		if(testTriangle(bb_x[0],bb_y[0], bb_x[2],bb_y[2], bb_x[4],bb_y[4])) return(boxIsVisible());
		if(testTriangle(bb_x[2],bb_y[2], bb_x[4],bb_y[4], bb_x[6],bb_y[6])) return(boxIsVisible());
	}

	if(bb_minidx!=2 && bb_minidx!=3 && bb_minidx!=6 && bb_minidx!=7){
		if(testTriangle(bb_x[2],bb_y[2], bb_x[3],bb_y[3], bb_x[6],bb_y[6])) return(boxIsVisible());
		if(testTriangle(bb_x[3],bb_y[3], bb_x[7],bb_y[7], bb_x[6],bb_y[6])) return(boxIsVisible());
	}

	if(bb_minidx!=1 && bb_minidx!=3 && bb_minidx!=5 && bb_minidx!=7){
		if(testTriangle(bb_x[3],bb_y[3], bb_x[7],bb_y[7], bb_x[5],bb_y[5])) return(boxIsVisible());
		if(testTriangle(bb_x[3],bb_y[3], bb_x[1],bb_y[1], bb_x[5],bb_y[5])) return(boxIsVisible());
	}

	if(bb_minidx<4){
		if(testTriangle(bb_x[5],bb_y[5], bb_x[6],bb_y[6], bb_x[7],bb_y[7])) return(boxIsVisible());
		if(testTriangle(bb_x[5],bb_y[5], bb_x[4],bb_y[4], bb_x[6],bb_y[6])) return(boxIsVisible());
	}
	return(boxIsHidden());
};

/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/

/*------------- constructors & destructors --------------------------------*/

inline
OccupancyMap::OccupancyMap(void):_maxtests(0), _results(NULL), _buffer(NULL), _w(0), _h(0)
{
};

inline
OccupancyMap::~OccupancyMap(void)
{
	if(_buffer) delete _buffer;
};

/*------------------------------ access -----------------------------------*/

inline
void OccupancyMap::frameInit(void)
{
};

inline
void OccupancyMap::frameExit(void)
{
};

inline
void OccupancyMap::setup(const Int32& max, const UInt16& w, const UInt16& h)
{
	if(_w!=w || _h!=h){
		_w=w/BUFFERTYPESIZE;
		_h=h;
		if(_buffer) delete[] _buffer;		// UGLY!
		_buffer=new BUFFERTYPE[_w*_h];
	}
	clearBuffer();

	_maxpix=0;
        if(_maxtests!=max){
                if(_results) delete[] _results;
		_results=new UInt32[max];
		_maxtests=max;
	}
};

inline
void OccupancyMap::setup(const Int32& max, Viewport* port)
{
	const UInt16 w=port->getPixelWidth();
	const UInt16 h=port->getPixelHeight();
	setup(max, w, h);
};

inline
void OccupancyMap::init(void)
{
};

inline void OccupancyMap::clearBuffer(void)
{
	for(Int32 i=0; i<_w*_h; i++)
		_buffer[i]=0;
};

inline
void OccupancyMap::perform(const Int32& num, const Int16* bb_x, const Int16* bb_y, const Real32* bb_z, const char bb_minidx)
{
	_results[num]=testBox(bb_x,bb_y,bb_z,bb_minidx);
};

inline
void OccupancyMap::insertBox(const Int16* bb_x, const Int16* bb_y, const Real32* bb_z, const char bb_minidx)
{
	if(bb_minidx>3){
		renderTriangle(bb_x[0],bb_y[0], bb_x[1],bb_y[1], bb_x[3],bb_y[3]);
		renderTriangle(bb_x[3],bb_y[3], bb_x[0],bb_y[0], bb_x[2],bb_y[2]);
	}
	
	if(bb_minidx!=0 && bb_minidx!=1 && bb_minidx!=4 && bb_minidx!=5){
		renderTriangle(bb_x[0],bb_y[0], bb_x[5],bb_y[5], bb_x[4],bb_y[4]);
		renderTriangle(bb_x[0],bb_y[0], bb_x[1],bb_y[1], bb_x[5],bb_y[5]);
	}

	if(bb_minidx!=0 && bb_minidx!=2 && bb_minidx!=4 && bb_minidx!=6){
		renderTriangle(bb_x[0],bb_y[0], bb_x[2],bb_y[2], bb_x[4],bb_y[4]);
		renderTriangle(bb_x[2],bb_y[2], bb_x[4],bb_y[4], bb_x[6],bb_y[6]);
	}

	if(bb_minidx!=2 && bb_minidx!=3 && bb_minidx!=6 && bb_minidx!=7){
		renderTriangle(bb_x[2],bb_y[2], bb_x[3],bb_y[3], bb_x[6],bb_y[6]);
		renderTriangle(bb_x[3],bb_y[3], bb_x[7],bb_y[7], bb_x[6],bb_y[6]);
	}

	if(bb_minidx!=1 && bb_minidx!=3 && bb_minidx!=5 && bb_minidx!=7){
		renderTriangle(bb_x[3],bb_y[3], bb_x[7],bb_y[7], bb_x[5],bb_y[5]);
		renderTriangle(bb_x[3],bb_y[3], bb_x[1],bb_y[1], bb_x[5],bb_y[5]);
	}

	if(bb_minidx<4){
		renderTriangle(bb_x[5],bb_y[5], bb_x[6],bb_y[6], bb_x[7],bb_y[7]);
		renderTriangle(bb_x[5],bb_y[5], bb_x[4],bb_y[4], bb_x[6],bb_y[6]);
	}
};

inline
Int32 OccupancyMap::result(const Int32& num)
{
	return(_results[num]);
};
			
inline
void OccupancyMap::exit(void){
};

/*---------------------------- properties ---------------------------------*/

/*-------------------------- your_category---------------------------------*/

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

OSG_END_NAMESPACE

///---------------------------------------------------------------------------
///  FUNCTION:
///---------------------------------------------------------------------------
//:  Example for the head comment of a function
///---------------------------------------------------------------------------
///
//p: Paramaters:
//p:
///
//g: GlobalVars:
//g:
///
//r: Return:
//r:
///
//c: Caution:
//c:
///
//a: Assumptions:
//a:
///
//d: Description:
//d:
///
//s: SeeAlso:
//s:
///---------------------------------------------------------------------------
