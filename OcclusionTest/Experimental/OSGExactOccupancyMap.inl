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
template<typename buffertype, UInt16 scaleshift>
inline bool ExactOccupancyMap<buffertype, scaleshift>::getPoint(const Int32& x, const Int32& y){
	if(x<0 || x>=(_w >> scaleshift)) return(1);
	if(y<0 || y>=(_h >> scaleshift)) return(1);
	const Int32 pos=(x/(sizeof(buffertype)*8))+((_h >> scaleshift)-y-1)*_bufferw;
	return(_buffer[pos] & (1<<(x%(sizeof(buffertype)*8))));
};

template<typename buffertype, UInt16 scaleshift>
inline bool ExactOccupancyMap<buffertype, scaleshift>::setPoint(const Int32& x, const Int32& y, const bool& v){
	if(x<0 || x>=(_w >> scaleshift)) return(0);
	if(y<0 || y>=(_h >> scaleshift)) return(0);
	const Int32 pos=(x/(sizeof(buffertype)*8))+((_h >> scaleshift)-y-1)*_bufferw;
	if(v)
		_buffer[pos]=_buffer[pos] | (1 << (x%(sizeof(buffertype)*8)));
	else
		_buffer[pos]=_buffer[pos] & (!(1 << (x%(sizeof(buffertype)*8))));
	return(0);
};

template<typename buffertype, UInt16 scaleshift>
inline bool ExactOccupancyMap<buffertype, scaleshift>::boxIsVisible(void){
	return(1);
};

template<typename buffertype, UInt16 scaleshift>
inline bool ExactOccupancyMap<buffertype, scaleshift>::boxIsHidden(void){
	return(0);
};

template<typename buffertype, UInt16 scaleshift>
inline bool ExactOccupancyMap<buffertype, scaleshift>::visitPoint(const Int32& x, const Int32& y, const bool& v){
	if(getPoint(x,y)==v){
		if(++_pixcount>_maxpix) return(1);	// visible
	}
	return(0);	// hidden
};

template<typename buffertype, UInt16 scaleshift>
inline bool ExactOccupancyMap<buffertype, scaleshift>::testBox(const Int16* bb_x, const Int16* bb_y, const Real32* bb_z, const char bb_minidx){
	_pixcount=0;
	
	if(bb_minidx<4){
		if(Rasterizer<testPoint,0>::testTriangle(&_testPoint, (bb_x[0] >> scaleshift),(bb_y[0] >> scaleshift),bb_z[0], (bb_x[1] >> scaleshift),(bb_y[1] >> scaleshift),bb_z[1], (bb_x[3] >> scaleshift),(bb_y[3] >> scaleshift),bb_z[3])) return(boxIsVisible());
		if(Rasterizer<testPoint,0>::testTriangle(&_testPoint, (bb_x[3] >> scaleshift),(bb_y[3] >> scaleshift),bb_z[3], (bb_x[0] >> scaleshift),(bb_y[0] >> scaleshift),bb_z[0], (bb_x[2] >> scaleshift),(bb_y[2] >> scaleshift),bb_z[2])) return(boxIsVisible());
	}else{
		if(Rasterizer<testPoint,0>::testTriangle(&_testPoint, (bb_x[5] >> scaleshift),(bb_y[5] >> scaleshift),bb_z[5], (bb_x[6] >> scaleshift),(bb_y[6] >> scaleshift),bb_z[6], (bb_x[7] >> scaleshift),(bb_y[7] >> scaleshift),bb_z[7])) return(boxIsVisible());
		if(Rasterizer<testPoint,0>::testTriangle(&_testPoint, (bb_x[5] >> scaleshift),(bb_y[5] >> scaleshift),bb_z[5], (bb_x[4] >> scaleshift),(bb_y[4] >> scaleshift),bb_z[4], (bb_x[6] >> scaleshift),(bb_y[6] >> scaleshift),bb_z[6])) return(boxIsVisible());
	}
	
	if(bb_minidx==0 || bb_minidx==1 || bb_minidx==4 || bb_minidx==5){
		if(Rasterizer<testPoint,0>::testTriangle(&_testPoint, (bb_x[0] >> scaleshift),(bb_y[0] >> scaleshift),bb_z[0], (bb_x[5] >> scaleshift),(bb_y[5] >> scaleshift),bb_z[5], (bb_x[4] >> scaleshift),(bb_y[4] >> scaleshift),bb_z[4])) return(boxIsVisible());
		if(Rasterizer<testPoint,0>::testTriangle(&_testPoint, (bb_x[0] >> scaleshift),(bb_y[0] >> scaleshift),bb_z[0], (bb_x[1] >> scaleshift),(bb_y[1] >> scaleshift),bb_z[1], (bb_x[5] >> scaleshift),(bb_y[5] >> scaleshift),bb_z[5])) return(boxIsVisible());
	}else{
		if(Rasterizer<testPoint,0>::testTriangle(&_testPoint, (bb_x[2] >> scaleshift),(bb_y[2] >> scaleshift),bb_z[2], (bb_x[3] >> scaleshift),(bb_y[3] >> scaleshift),bb_z[3], (bb_x[6] >> scaleshift),(bb_y[6] >> scaleshift),bb_z[6])) return(boxIsVisible());
		if(Rasterizer<testPoint,0>::testTriangle(&_testPoint, (bb_x[3] >> scaleshift),(bb_y[3] >> scaleshift),bb_z[3], (bb_x[7] >> scaleshift),(bb_y[7] >> scaleshift),bb_z[7], (bb_x[6] >> scaleshift),(bb_y[6] >> scaleshift),bb_z[6])) return(boxIsVisible());
	}

	if(bb_minidx==0 || bb_minidx==2 || bb_minidx==4 || bb_minidx==6){
		if(Rasterizer<testPoint,0>::testTriangle(&_testPoint, (bb_x[0] >> scaleshift),(bb_y[0] >> scaleshift),bb_z[0], (bb_x[2] >> scaleshift),(bb_y[2] >> scaleshift),bb_z[2], (bb_x[4] >> scaleshift),(bb_y[4] >> scaleshift),bb_z[4])) return(boxIsVisible());
		if(Rasterizer<testPoint,0>::testTriangle(&_testPoint, (bb_x[2] >> scaleshift),(bb_y[2] >> scaleshift),bb_z[2], (bb_x[4] >> scaleshift),(bb_y[4] >> scaleshift),bb_z[4], (bb_x[6] >> scaleshift),(bb_y[6] >> scaleshift),bb_z[6])) return(boxIsVisible());
	}else{
		if(Rasterizer<testPoint,0>::testTriangle(&_testPoint, (bb_x[3] >> scaleshift),(bb_y[3] >> scaleshift),bb_z[3], (bb_x[7] >> scaleshift),(bb_y[7] >> scaleshift),bb_z[7], (bb_x[5] >> scaleshift),(bb_y[5] >> scaleshift),bb_z[5])) return(boxIsVisible());
		if(Rasterizer<testPoint,0>::testTriangle(&_testPoint, (bb_x[3] >> scaleshift),(bb_y[3] >> scaleshift),bb_z[3], (bb_x[1] >> scaleshift),(bb_y[1] >> scaleshift),bb_z[1], (bb_x[5] >> scaleshift),(bb_y[5] >> scaleshift),bb_z[5])) return(boxIsVisible());
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

template<typename buffertype, UInt16 scaleshift>
inline
ExactOccupancyMap<buffertype, scaleshift>::ExactOccupancyMap<buffertype, scaleshift>(void):_renderPoint(this),_testPoint(this),_maxtests(0), _results(NULL), _buffer(NULL), _w(0), _bufferw(0), _h(0)
{
};

template<typename buffertype, UInt16 scaleshift>
inline
ExactOccupancyMap<buffertype, scaleshift>::~ExactOccupancyMap<buffertype, scaleshift>(void)
{
	if(_buffer) delete _buffer;
};

/*------------------------------ access -----------------------------------*/

template<typename buffertype, UInt16 scaleshift>
inline
void ExactOccupancyMap<buffertype, scaleshift>::frameInit(void)
{
};

template<typename buffertype, UInt16 scaleshift>
inline
void ExactOccupancyMap<buffertype, scaleshift>::frameExit(void)
{
//	visualize();
};

template<typename buffertype, UInt16 scaleshift>
inline
void ExactOccupancyMap<buffertype, scaleshift>::setup(const Int32& max, const UInt16& w, const UInt16& h)
{
	if(_w!=w || _h!=h){
		_w=w;
		_h=h;
		_bufferw=((_w >> scaleshift)+(sizeof(buffertype)*8-(_w >> scaleshift)%(sizeof(buffertype)*8)))/(sizeof(buffertype)*8);
		if(_buffer) delete[] _buffer;		// UGLY!
		_buffer=new buffertype[_bufferw*((_h >> scaleshift)+1)];
	}
	clearBuffer();

	_maxpix=0;
        if(_maxtests!=max){
                if(_results) delete[] _results;
		_results=new UInt32[max];
		_maxtests=max;
	}
};

template<typename buffertype, UInt16 scaleshift>
inline
void ExactOccupancyMap<buffertype, scaleshift>::setup(const Int32& max, Viewport* port)
{
	const UInt16 w=port->getPixelWidth();
	const UInt16 h=port->getPixelHeight();
	setup(max, w, h);
};

template<typename buffertype, UInt16 scaleshift>
inline
void ExactOccupancyMap<buffertype, scaleshift>::init(void)
{
};

template<typename buffertype, UInt16 scaleshift>
inline void ExactOccupancyMap<buffertype, scaleshift>::clearBuffer(void)
{
	for(Int32 i=0; i<_bufferw*((_h >> scaleshift)+1); i++)
		_buffer[i]=0;
};

template<typename buffertype, UInt16 scaleshift>
inline
void ExactOccupancyMap<buffertype, scaleshift>::perform(const Int32& num, const Int16* bb_x, const Int16* bb_y, const Real32* bb_z, const char bb_minidx)
{
	_results[num]=testBox(bb_x,bb_y,bb_z,bb_minidx);
};

template<typename buffertype, UInt16 scaleshift>
inline
void ExactOccupancyMap<buffertype, scaleshift>::insertBox(const Int16* bb_x, const Int16* bb_y, const Real32* bb_z, const char bb_minidx)
{
	if(bb_minidx<4){
		Rasterizer<renderPoint,0>::testTriangle(&_renderPoint,(bb_x[0] >> scaleshift),(bb_y[0] >> scaleshift),bb_z[0], (bb_x[1] >> scaleshift),(bb_y[1] >> scaleshift),bb_z[1], (bb_x[3] >> scaleshift),(bb_y[3] >> scaleshift),bb_z[3]);
		Rasterizer<renderPoint,0>::testTriangle(&_renderPoint,(bb_x[3] >> scaleshift),(bb_y[3] >> scaleshift),bb_z[3], (bb_x[0] >> scaleshift),(bb_y[0] >> scaleshift),bb_z[0], (bb_x[2] >> scaleshift),(bb_y[2] >> scaleshift),bb_z[2]);
	}else{
		Rasterizer<renderPoint,0>::testTriangle(&_renderPoint,(bb_x[5] >> scaleshift),(bb_y[5] >> scaleshift),bb_z[5], (bb_x[6] >> scaleshift),(bb_y[6] >> scaleshift),bb_z[6], (bb_x[7] >> scaleshift),(bb_y[7] >> scaleshift),bb_z[7]);
		Rasterizer<renderPoint,0>::testTriangle(&_renderPoint,(bb_x[5] >> scaleshift),(bb_y[5] >> scaleshift),bb_z[5], (bb_x[4] >> scaleshift),(bb_y[4] >> scaleshift),bb_z[4], (bb_x[6] >> scaleshift),(bb_y[6] >> scaleshift),bb_z[6]);
	}
	
	if(bb_minidx==0 || bb_minidx==1 || bb_minidx==4 || bb_minidx==5){
		Rasterizer<renderPoint,0>::testTriangle(&_renderPoint,(bb_x[0] >> scaleshift),(bb_y[0] >> scaleshift),bb_z[0], (bb_x[5] >> scaleshift),(bb_y[5] >> scaleshift),bb_z[5], (bb_x[4] >> scaleshift),(bb_y[4] >> scaleshift),bb_z[4]);
		Rasterizer<renderPoint,0>::testTriangle(&_renderPoint,(bb_x[0] >> scaleshift),(bb_y[0] >> scaleshift),bb_z[0], (bb_x[1] >> scaleshift),(bb_y[1] >> scaleshift),bb_z[1], (bb_x[5] >> scaleshift),(bb_y[5] >> scaleshift),bb_z[5]);
	}else{
		Rasterizer<renderPoint,0>::testTriangle(&_renderPoint,(bb_x[2] >> scaleshift),(bb_y[2] >> scaleshift),bb_z[2], (bb_x[3] >> scaleshift),(bb_y[3] >> scaleshift),bb_z[3], (bb_x[6] >> scaleshift),(bb_y[6] >> scaleshift),bb_z[6]);
		Rasterizer<renderPoint,0>::testTriangle(&_renderPoint,(bb_x[3] >> scaleshift),(bb_y[3] >> scaleshift),bb_z[3], (bb_x[7] >> scaleshift),(bb_y[7] >> scaleshift),bb_z[7], (bb_x[6] >> scaleshift),(bb_y[6] >> scaleshift),bb_z[6]);
	}

	if(bb_minidx==0 || bb_minidx==2 || bb_minidx==4 || bb_minidx==6){
		Rasterizer<renderPoint,0>::testTriangle(&_renderPoint,(bb_x[0] >> scaleshift),(bb_y[0] >> scaleshift),bb_z[0], (bb_x[2] >> scaleshift),(bb_y[2] >> scaleshift),bb_z[2], (bb_x[4] >> scaleshift),(bb_y[4] >> scaleshift),bb_z[4]);
		Rasterizer<renderPoint,0>::testTriangle(&_renderPoint,(bb_x[2] >> scaleshift),(bb_y[2] >> scaleshift),bb_z[2], (bb_x[4] >> scaleshift),(bb_y[4] >> scaleshift),bb_z[4], (bb_x[6] >> scaleshift),(bb_y[6] >> scaleshift),bb_z[6]);
	}else{
		Rasterizer<renderPoint,0>::testTriangle(&_renderPoint,(bb_x[3] >> scaleshift),(bb_y[3] >> scaleshift),bb_z[3], (bb_x[7] >> scaleshift),(bb_y[7] >> scaleshift),bb_z[7], (bb_x[5] >> scaleshift),(bb_y[5] >> scaleshift),bb_z[5]);
		Rasterizer<renderPoint,0>::testTriangle(&_renderPoint,(bb_x[3] >> scaleshift),(bb_y[3] >> scaleshift),bb_z[3], (bb_x[1] >> scaleshift),(bb_y[1] >> scaleshift),bb_z[1], (bb_x[5] >> scaleshift),(bb_y[5] >> scaleshift),bb_z[5]);
	}
};

template<typename buffertype, UInt16 scaleshift>
inline
Int32 ExactOccupancyMap<buffertype, scaleshift>::result(const Int32& num)
{
	return(_results[num]);
};
			
template<typename buffertype, UInt16 scaleshift>
inline
void ExactOccupancyMap<buffertype, scaleshift>::exit(void){
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

template<typename buffertype, UInt16 scaleshift>
inline
void ExactOccupancyMap<buffertype, scaleshift>::visualize(void){
	glViewport(0,0,_w,_h);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	// glClearColor(0.0,0.0,0.0,0.0);
	// glClear(GL_COLOR_BUFFER_BIT);
	// glColorMask(GL_TRUE,GL_FALSE,GL_FALSE,GL_FALSE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0,(Real32)_w, 0.0,(Real32)_h, 0.0,1.0);

	glShadeModel(GL_FLAT);

	glBegin(GL_POINTS);
	for(Int32 y=0; y<_h; y++){
		for(Int32 x=0; x<_w; x++){
			const Real32 c=getPoint(x >> scaleshift,y >> scaleshift)?1.0:0.0;
			glColor3f(c,0.0,0.0);
			glVertex2i(x,_h-y-1);
		}
	}
	glEnd();
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
};

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
