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

#include <OSGConfig.h>
#include <OSGDrawAction.h>
#include <OSGViewport.h>

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
template<UInt16 scaleshift>
inline Real32& SoftZBTest<scaleshift>::getPoint(const Int32& x, const Int32& y){
	if(x<0 || x>=(_w >> scaleshift)) return(_fr_max);
	if(y<0 || y>=(_h >> scaleshift)) return(_fr_max);
	const Int32 pos=x+((_h >> scaleshift)-y-1)*(_w >> scaleshift);
	return(_zbuffer[pos]);
};

template<UInt16 scaleshift>
inline bool SoftZBTest<scaleshift>::setPoint(const Int32& x, const Int32& y, const Real32& z){
	if(x<0 || x>=(_w >> scaleshift)) return(0);
	if(y<0 || y>=(_h >> scaleshift)) return(0);
	const Int32 pos=x+((_h >> scaleshift)-y-1)*(_w >> scaleshift);
	if(_zbuffer[pos]>z){
		_zbuffer[pos]=z<0?-z:z;
	}
	return(0);
};

template<UInt16 scaleshift>
inline UInt32 SoftZBTest<scaleshift>::boxIsVisible(void){
	return(_pixcount);
};

template<UInt16 scaleshift>
inline UInt32 SoftZBTest<scaleshift>::boxIsHidden(void){
	return(0);
};

template<UInt16 scaleshift>
inline bool SoftZBTest<scaleshift>::visitPoint(const Int16& x, const Int16& y, const Real32& zz){
	if(getPoint(x,y)>zz){
		if(++_pixcount>_maxpix) return(1);	// visible
	}
	return(0);	// hidden
};

template<UInt16 scaleshift>
inline UInt32 SoftZBTest<scaleshift>::testBox(const Int16* bb_x, const Int16* bb_y, const Real32* bb_z, const char bb_minidx){
	_pixcount=0;
/*
	for(int i=0; i<8; i++){
		// std::cerr << bb_x[i] << " | " << bb_y[i] << " | " << bb_z[i] << "\n";
		if(bb_x[i]<-50 || bb_x[i]>_w+50){
			_pixcount=100000;
			return(boxIsVisible());
		}
		if(bb_y[i]<-50 || bb_y[i]>_h+50){
			_pixcount=100000;
			return(boxIsVisible());
		}
		if(bb_z[i]<0.0 || bb_z[i]>1.0){
			_pixcount=100000;
			return(boxIsVisible());
		}
	}
*/	
	Rasterizer<renderPoint>::setSize(20000, _w, _h);

	if(bb_minidx<4){
		if(Rasterizer<testPoint>::testTriangle(&_testPoint, (bb_x[0] >> scaleshift),(bb_y[0] >> scaleshift),bb_z[0], (bb_x[1] >> scaleshift),(bb_y[1] >> scaleshift),bb_z[1], (bb_x[3] >> scaleshift),(bb_y[3] >> scaleshift),bb_z[3])) return(boxIsVisible());
		if(Rasterizer<testPoint>::testTriangle(&_testPoint, (bb_x[3] >> scaleshift),(bb_y[3] >> scaleshift),bb_z[3], (bb_x[0] >> scaleshift),(bb_y[0] >> scaleshift),bb_z[0], (bb_x[2] >> scaleshift),(bb_y[2] >> scaleshift),bb_z[2])) return(boxIsVisible());
	}else{
		if(Rasterizer<testPoint>::testTriangle(&_testPoint, (bb_x[5] >> scaleshift),(bb_y[5] >> scaleshift),bb_z[5], (bb_x[6] >> scaleshift),(bb_y[6] >> scaleshift),bb_z[6], (bb_x[7] >> scaleshift),(bb_y[7] >> scaleshift),bb_z[7])) return(boxIsVisible());
		if(Rasterizer<testPoint>::testTriangle(&_testPoint, (bb_x[5] >> scaleshift),(bb_y[5] >> scaleshift),bb_z[5], (bb_x[4] >> scaleshift),(bb_y[4] >> scaleshift),bb_z[4], (bb_x[6] >> scaleshift),(bb_y[6] >> scaleshift),bb_z[6])) return(boxIsVisible());
	}
	
	if(bb_minidx==0 || bb_minidx==1 || bb_minidx==4 || bb_minidx==5){
		if(Rasterizer<testPoint>::testTriangle(&_testPoint, (bb_x[0] >> scaleshift),(bb_y[0] >> scaleshift),bb_z[0], (bb_x[5] >> scaleshift),(bb_y[5] >> scaleshift),bb_z[5], (bb_x[4] >> scaleshift),(bb_y[4] >> scaleshift),bb_z[4])) return(boxIsVisible());
		if(Rasterizer<testPoint>::testTriangle(&_testPoint, (bb_x[0] >> scaleshift),(bb_y[0] >> scaleshift),bb_z[0], (bb_x[1] >> scaleshift),(bb_y[1] >> scaleshift),bb_z[1], (bb_x[5] >> scaleshift),(bb_y[5] >> scaleshift),bb_z[5])) return(boxIsVisible());
	}else{
		if(Rasterizer<testPoint>::testTriangle(&_testPoint, (bb_x[2] >> scaleshift),(bb_y[2] >> scaleshift),bb_z[2], (bb_x[3] >> scaleshift),(bb_y[3] >> scaleshift),bb_z[3], (bb_x[6] >> scaleshift),(bb_y[6] >> scaleshift),bb_z[6])) return(boxIsVisible());
		if(Rasterizer<testPoint>::testTriangle(&_testPoint, (bb_x[3] >> scaleshift),(bb_y[3] >> scaleshift),bb_z[3], (bb_x[7] >> scaleshift),(bb_y[7] >> scaleshift),bb_z[7], (bb_x[6] >> scaleshift),(bb_y[6] >> scaleshift),bb_z[6])) return(boxIsVisible());
	}

	if(bb_minidx==0 || bb_minidx==2 || bb_minidx==4 || bb_minidx==6){
		if(Rasterizer<testPoint>::testTriangle(&_testPoint, (bb_x[0] >> scaleshift),(bb_y[0] >> scaleshift),bb_z[0], (bb_x[2] >> scaleshift),(bb_y[2] >> scaleshift),bb_z[2], (bb_x[4] >> scaleshift),(bb_y[4] >> scaleshift),bb_z[4])) return(boxIsVisible());
		if(Rasterizer<testPoint>::testTriangle(&_testPoint, (bb_x[2] >> scaleshift),(bb_y[2] >> scaleshift),bb_z[2], (bb_x[4] >> scaleshift),(bb_y[4] >> scaleshift),bb_z[4], (bb_x[6] >> scaleshift),(bb_y[6] >> scaleshift),bb_z[6])) return(boxIsVisible());
	}else{
		if(Rasterizer<testPoint>::testTriangle(&_testPoint, (bb_x[3] >> scaleshift),(bb_y[3] >> scaleshift),bb_z[3], (bb_x[7] >> scaleshift),(bb_y[7] >> scaleshift),bb_z[7], (bb_x[5] >> scaleshift),(bb_y[5] >> scaleshift),bb_z[5])) return(boxIsVisible());
		if(Rasterizer<testPoint>::testTriangle(&_testPoint, (bb_x[3] >> scaleshift),(bb_y[3] >> scaleshift),bb_z[3], (bb_x[1] >> scaleshift),(bb_y[1] >> scaleshift),bb_z[1], (bb_x[5] >> scaleshift),(bb_y[5] >> scaleshift),bb_z[5])) return(boxIsVisible());
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

template<UInt16 scaleshift>
inline
SoftZBTest<scaleshift>::SoftZBTest(void):_renderPoint(this),_testPoint(this),
_maxtests(0), _results(NULL), _zbuffer(NULL), _w(0), _h(0), _fr_min(0.0), _fr_max(1.0), _bufdirty(true)
{
};

template<UInt16 scaleshift>
inline
SoftZBTest<scaleshift>::~SoftZBTest(void)
{
	if(_zbuffer) delete _zbuffer;
};

/*------------------------------ access -----------------------------------*/

template<UInt16 scaleshift>
inline
void SoftZBTest<scaleshift>::frameInit(void)
{
};

template<UInt16 scaleshift>
inline
void SoftZBTest<scaleshift>::frameExit(void)
{
	// visualize();
};

template<UInt16 scaleshift>
inline
void SoftZBTest<scaleshift>::setup(const Int32& max, Viewport* port, const Int32 maxpix)
{
	Int32 w=port->getPixelWidth();
	Int32 h=port->getPixelHeight();
	if(_w!=w || _h!=h){
		_w=w;
		_h=h;
		if(_zbuffer) delete[] _zbuffer;		// UGLY!
		_zbuffer=new Real32[(_w*_h) >> scaleshift];
		
		_bufdirty=true;
	}

	clearZBuffer();
	_maxpix=maxpix;
        if(_maxtests!=max){
                if(_results) delete[] _results;
		_results=new UInt32[max];
		_maxtests=max;
	}
};

template<UInt16 scaleshift>
inline
void SoftZBTest<scaleshift>::init(void)
{
};

template<UInt16 scaleshift>
inline void SoftZBTest<scaleshift>::clearZBuffer(void)
{
	if(_bufdirty==false) return;
	
	for(Int32 i=0; i<(_w*_h) >> scaleshift; i++){
		_zbuffer[i]=_fr_max;
	}
	_bufdirty=false;
};

template<UInt16 scaleshift>
inline
void SoftZBTest<scaleshift>::perform(const Int32& num, const Int16* bb_x, const Int16* bb_y, const Real32* bb_z, const char bb_minidx)
{
	_results[num]=testBox(bb_x,bb_y,bb_z,bb_minidx);
};

template<UInt16 scaleshift>
inline
void SoftZBTest<scaleshift>::insertBox(const Int16* bb_x, const Int16* bb_y, const Real32* bb_z, const char bb_minidx)
{
	Rasterizer<renderPoint>::setSize(20000, _w, _h);
	_bufdirty=true;
	
	if(bb_minidx<4){
		Rasterizer<renderPoint>::testTriangle(&_renderPoint,(bb_x[0] >> scaleshift),(bb_y[0] >> scaleshift),bb_z[0], (bb_x[1] >> scaleshift),(bb_y[1] >> scaleshift),bb_z[1], (bb_x[3] >> scaleshift),(bb_y[3] >> scaleshift),bb_z[3]);
		Rasterizer<renderPoint>::testTriangle(&_renderPoint,(bb_x[3] >> scaleshift),(bb_y[3] >> scaleshift),bb_z[3], (bb_x[0] >> scaleshift),(bb_y[0] >> scaleshift),bb_z[0], (bb_x[2] >> scaleshift),(bb_y[2] >> scaleshift),bb_z[2]);
	}else{
		Rasterizer<renderPoint>::testTriangle(&_renderPoint,(bb_x[5] >> scaleshift),(bb_y[5] >> scaleshift),bb_z[5], (bb_x[6] >> scaleshift),(bb_y[6] >> scaleshift),bb_z[6], (bb_x[7] >> scaleshift),(bb_y[7] >> scaleshift),bb_z[7]);
		Rasterizer<renderPoint>::testTriangle(&_renderPoint,(bb_x[5] >> scaleshift),(bb_y[5] >> scaleshift),bb_z[5], (bb_x[4] >> scaleshift),(bb_y[4] >> scaleshift),bb_z[4], (bb_x[6] >> scaleshift),(bb_y[6] >> scaleshift),bb_z[6]);
	}
	
	if(bb_minidx==0 || bb_minidx==1 || bb_minidx==4 || bb_minidx==5){
		Rasterizer<renderPoint>::testTriangle(&_renderPoint,(bb_x[0] >> scaleshift),(bb_y[0] >> scaleshift),bb_z[0], (bb_x[5] >> scaleshift),(bb_y[5] >> scaleshift),bb_z[5], (bb_x[4] >> scaleshift),(bb_y[4] >> scaleshift),bb_z[4]);
		Rasterizer<renderPoint>::testTriangle(&_renderPoint,(bb_x[0] >> scaleshift),(bb_y[0] >> scaleshift),bb_z[0], (bb_x[1] >> scaleshift),(bb_y[1] >> scaleshift),bb_z[1], (bb_x[5] >> scaleshift),(bb_y[5] >> scaleshift),bb_z[5]);
	}else{
		Rasterizer<renderPoint>::testTriangle(&_renderPoint,(bb_x[2] >> scaleshift),(bb_y[2] >> scaleshift),bb_z[2], (bb_x[3] >> scaleshift),(bb_y[3] >> scaleshift),bb_z[3], (bb_x[6] >> scaleshift),(bb_y[6] >> scaleshift),bb_z[6]);
		Rasterizer<renderPoint>::testTriangle(&_renderPoint,(bb_x[3] >> scaleshift),(bb_y[3] >> scaleshift),bb_z[3], (bb_x[7] >> scaleshift),(bb_y[7] >> scaleshift),bb_z[7], (bb_x[6] >> scaleshift),(bb_y[6] >> scaleshift),bb_z[6]);
	}

	if(bb_minidx==0 || bb_minidx==2 || bb_minidx==4 || bb_minidx==6){
		Rasterizer<renderPoint>::testTriangle(&_renderPoint,(bb_x[0] >> scaleshift),(bb_y[0] >> scaleshift),bb_z[0], (bb_x[2] >> scaleshift),(bb_y[2] >> scaleshift),bb_z[2], (bb_x[4] >> scaleshift),(bb_y[4] >> scaleshift),bb_z[4]);
		Rasterizer<renderPoint>::testTriangle(&_renderPoint,(bb_x[2] >> scaleshift),(bb_y[2] >> scaleshift),bb_z[2], (bb_x[4] >> scaleshift),(bb_y[4] >> scaleshift),bb_z[4], (bb_x[6] >> scaleshift),(bb_y[6] >> scaleshift),bb_z[6]);
	}else{
		Rasterizer<renderPoint>::testTriangle(&_renderPoint,(bb_x[3] >> scaleshift),(bb_y[3] >> scaleshift),bb_z[3], (bb_x[7] >> scaleshift),(bb_y[7] >> scaleshift),bb_z[7], (bb_x[5] >> scaleshift),(bb_y[5] >> scaleshift),bb_z[5]);
		Rasterizer<renderPoint>::testTriangle(&_renderPoint,(bb_x[3] >> scaleshift),(bb_y[3] >> scaleshift),bb_z[3], (bb_x[1] >> scaleshift),(bb_y[1] >> scaleshift),bb_z[1], (bb_x[5] >> scaleshift),(bb_y[5] >> scaleshift),bb_z[5]);
	}
};

template<UInt16 scaleshift>
inline
UInt32 SoftZBTest<scaleshift>::result(const Int32& num)
{
	return(_results[num]);
};
			
template<UInt16 scaleshift>
inline
void SoftZBTest<scaleshift>::exit(void){
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

template<UInt16 scaleshift>
inline
void SoftZBTest<scaleshift>::visualize(void){
	glViewport(0,0,_w,_h);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0,(Real32)_w, 0.0,(Real32)_h, 0.0,1.0);

	glShadeModel(GL_FLAT);

	glBegin(GL_POINTS);
	for(Int32 y=0; y<_h; y++){
		for(Int32 x=0; x<_w; x++){
			const Real32 c=getPoint(x,y);
			glColor3f(c,c,c);
			glVertex2i(x,y);
		}
	}
	glEnd();
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
