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

#include <OpenSG/OSGRasterizer.h>

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
template<UInt16 fragwidth, UInt16 fragheight>
inline void OcclusionZBTest<fragwidth, fragheight>::setAllUnused(void){
	for(Int32 i=0; i<_fr_w*_fr_h; i++){
		_zbuffer[i]._dirty=1;
		_zbuffer[i]._unused=1;
		_zbuffer[i]._reads=0;
	}
};

template<UInt16 fragwidth, UInt16 fragheight>
inline void OcclusionZBTest<fragwidth, fragheight>::setAllDirty(void){
	for(Int32 i=0; i<_fr_w*_fr_h; i++){
		_zbuffer[i]._dirty=1;
		_zbuffer[i]._unused=0;
	}
};

template<UInt16 fragwidth, UInt16 fragheight>
inline Real32& OcclusionZBTest<fragwidth, fragheight>::getPoint(const Int32& x, const Int32& y){
	if(x<0) return(_fr_max);
	if(x>=_w) return(_fr_max);
	if(y<0) return(_fr_max);
	if(y>=_h) return(_fr_max);
	Int32 nx=x;
	Int32 ny=_h-y;
	_zb_pos=nx/fragwidth+(ny/fragheight)*_fr_w;
	if(_zb_pos>_fr_w*_fr_h){
		return(_fr_max);	// FIXME
	}
	if(_zb_pos<0){
		return(_fr_max);	// FIXME
	}
	const Int32 posfr=(nx%fragwidth)+fragwidth*(ny%fragheight);
	if(posfr<0 || posfr>fragwidth*fragheight){
		return(_fr_max);	// FIXME
	}
	if(_zbuffer[_zb_pos]._unused==1){
		_zbuffer[_zb_pos]._unused=0;
		_zbuffer[_zb_pos]._dirty=1;
	 	return(_fr_min);
	}
	if(_zbuffer[_zb_pos]._dirty==1){
		glReadBuffer(GL_BACK);
		Int32 fx=nx-nx%fragwidth;
		Int32 fy=ny-ny%fragheight;
		glReadPixels(fx,fy,fragwidth,fragheight,GL_DEPTH_COMPONENT,GL_FLOAT,_zbuffer[_zb_pos]._buffer);
		_zbuffer[_zb_pos]._dirty=0;
		_zbuffer[_zb_pos]._reads++;
		
		_glReads++;
	}
	return(_zbuffer[_zb_pos]._buffer[posfr]);
};

template<UInt16 fragwidth, UInt16 fragheight>
inline UInt32 OcclusionZBTest<fragwidth, fragheight>::boxIsVisible(void){
	// setAllDirty();
	return(_pixcount);
};

template<UInt16 fragwidth, UInt16 fragheight>
inline UInt32 OcclusionZBTest<fragwidth, fragheight>::boxIsHidden(void){
	return(0);
};

template<UInt16 fragwidth, UInt16 fragheight>
inline bool OcclusionZBTest<fragwidth, fragheight>::visitPoint(const Int16& x, const Int16& y, const Real32& zz){
	/*
	Int32 nx=x;
	Int32 ny=_h-y;
	const Int32 posfr=(nx%fragwidth)+fragwidth*(ny%fragheight);
	_zbuffer[_zb_pos]._buffer[posfr]=zz;
	*/
	if(getPoint(x,y)>zz){
		if(++_pixcount>_maxpix){
			_zbuffer[_zb_pos]._dirty=1;
			return(1);	// visible
		}
	}
	return(0);	// hidden
};

template<UInt16 fragwidth, UInt16 fragheight>
inline UInt32 OcclusionZBTest<fragwidth, fragheight>::testBox(const Int16* bb_x, const Int16* bb_y, const Real32* bb_z, const char bb_minidx){
	_pixcount=0;

	Rasterizer<OcclusionZBTest<fragwidth, fragheight> >::setSize(100000, _w, _h);
	
	if(bb_minidx<4){
		if(Rasterizer<OcclusionZBTest<fragwidth, fragheight> >::testTriangle(this, bb_x[0],bb_y[0],bb_z[0], bb_x[1],bb_y[1],bb_z[1], bb_x[3],bb_y[3],bb_z[3])) return(boxIsVisible());
		if(Rasterizer<OcclusionZBTest<fragwidth, fragheight> >::testTriangle(this, bb_x[3],bb_y[3],bb_z[3], bb_x[0],bb_y[0],bb_z[0], bb_x[2],bb_y[2],bb_z[2])) return(boxIsVisible());
	}else{
		if(Rasterizer<OcclusionZBTest<fragwidth, fragheight> >::testTriangle(this, bb_x[5],bb_y[5],bb_z[5], bb_x[6],bb_y[6],bb_z[6], bb_x[7],bb_y[7],bb_z[7])) return(boxIsVisible());
		if(Rasterizer<OcclusionZBTest<fragwidth, fragheight> >::testTriangle(this, bb_x[5],bb_y[5],bb_z[5], bb_x[4],bb_y[4],bb_z[4], bb_x[6],bb_y[6],bb_z[6])) return(boxIsVisible());
	}
	
	if(bb_minidx==0 || bb_minidx==1 || bb_minidx==4 || bb_minidx==5){
		if(Rasterizer<OcclusionZBTest<fragwidth, fragheight> >::testTriangle(this, bb_x[0],bb_y[0],bb_z[0], bb_x[5],bb_y[5],bb_z[5], bb_x[4],bb_y[4],bb_z[4])) return(boxIsVisible());
		if(Rasterizer<OcclusionZBTest<fragwidth, fragheight> >::testTriangle(this, bb_x[0],bb_y[0],bb_z[0], bb_x[1],bb_y[1],bb_z[1], bb_x[5],bb_y[5],bb_z[5])) return(boxIsVisible());
	}else{
		if(Rasterizer<OcclusionZBTest<fragwidth, fragheight> >::testTriangle(this, bb_x[2],bb_y[2],bb_z[2], bb_x[3],bb_y[3],bb_z[3], bb_x[6],bb_y[6],bb_z[6])) return(boxIsVisible());
		if(Rasterizer<OcclusionZBTest<fragwidth, fragheight> >::testTriangle(this, bb_x[3],bb_y[3],bb_z[3], bb_x[7],bb_y[7],bb_z[7], bb_x[6],bb_y[6],bb_z[6])) return(boxIsVisible());
	}

	if(bb_minidx==0 || bb_minidx==2 || bb_minidx==4 || bb_minidx==6){
		if(Rasterizer<OcclusionZBTest<fragwidth, fragheight> >::testTriangle(this, bb_x[0],bb_y[0],bb_z[0], bb_x[2],bb_y[2],bb_z[2], bb_x[4],bb_y[4],bb_z[4])) return(boxIsVisible());
		if(Rasterizer<OcclusionZBTest<fragwidth, fragheight> >::testTriangle(this, bb_x[2],bb_y[2],bb_z[2], bb_x[4],bb_y[4],bb_z[4], bb_x[6],bb_y[6],bb_z[6])) return(boxIsVisible());
	}else{
		if(Rasterizer<OcclusionZBTest<fragwidth, fragheight> >::testTriangle(this, bb_x[3],bb_y[3],bb_z[3], bb_x[7],bb_y[7],bb_z[7], bb_x[5],bb_y[5],bb_z[5])) return(boxIsVisible());
		if(Rasterizer<OcclusionZBTest<fragwidth, fragheight> >::testTriangle(this, bb_x[3],bb_y[3],bb_z[3], bb_x[1],bb_y[1],bb_z[1], bb_x[5],bb_y[5],bb_z[5])) return(boxIsVisible());
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

template<UInt16 fragwidth, UInt16 fragheight>
inline
OcclusionZBTest<fragwidth, fragheight>::OcclusionZBTest<fragwidth, fragheight>(void):_maxtests(0), _results(NULL), _zbuffer(NULL), _w(0), _h(0), _fr_min(0.0), _fr_max(1.0)
{
};

template<UInt16 fragwidth, UInt16 fragheight>
inline
OcclusionZBTest<fragwidth, fragheight>::~OcclusionZBTest<fragwidth, fragheight>(void)
{
	if(_zbuffer) delete _zbuffer;
};

/*------------------------------ access -----------------------------------*/

template<UInt16 fragwidth, UInt16 fragheight>
inline
void OcclusionZBTest<fragwidth, fragheight>::frameInit(void)
{
};

template<UInt16 fragwidth, UInt16 fragheight>
inline
void OcclusionZBTest<fragwidth, fragheight>::frameExit(void)
{
	// visualize();
};

template<UInt16 fragwidth, UInt16 fragheight>
inline
void OcclusionZBTest<fragwidth, fragheight>::setup(const Int32& max, Viewport* port, const UInt32 maxpix)
{
	Int32 w=port->getPixelWidth();
	Int32 h=port->getPixelHeight();
	if(_w!=w || _h!=h){
		_w=w;
		_h=h;
		_fr_w=1+_w/fragwidth;	// FIXME
		_fr_h=1+_h/fragheight;	// FIXME
		if(_zbuffer) delete[] _zbuffer;		// UGLY!
		_zbuffer=new _fragment[_fr_w*_fr_h];
	}
	setAllUnused();

	_maxpix=maxpix;
        if(_maxtests!=max){
                if(_results) delete[] _results;
		_results=new UInt32[max];
		_maxtests=max;
	}

	_glReads=0;
};

template<UInt16 fragwidth, UInt16 fragheight>
inline
void OcclusionZBTest<fragwidth, fragheight>::init(void)
{
};

template<UInt16 fragwidth, UInt16 fragheight>
inline
void OcclusionZBTest<fragwidth, fragheight>::perform(const Int32& num, const OCTestNode* node)
{
	const Int16* bb_x=node->_x;
	const Int16* bb_y=node->_y;
	const Real32* bb_z=node->_z;
	const char bb_minidx=node->_index;
	_results[num]=testBox(bb_x,bb_y,bb_z,bb_minidx);
};

template<UInt16 fragwidth, UInt16 fragheight>
inline
UInt32 OcclusionZBTest<fragwidth, fragheight>::result(const Int32& num)
{
	return(_results[num]);
};
			
template<UInt16 fragwidth, UInt16 fragheight>
inline
void OcclusionZBTest<fragwidth, fragheight>::exit(void){
	// std::cerr << "glReadPixels: " << _glReads << std::endl;
};

template<UInt16 fragwidth, UInt16 fragheight>
inline
void OcclusionZBTest<fragwidth, fragheight>::visualize(void){
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
			const Int32 nx=x;
			const Int32 ny=_h-y;
			const Int32 pos=nx/fragwidth+(ny/fragheight)*_fr_w;
			const Int32 posfr=(nx%fragwidth)+fragwidth*(ny%fragheight);
			const Real32 c=(Real32)(_zbuffer[pos]._reads-1)/5.0;
			if(_zbuffer[pos]._unused==1){
				glColor3f(0.0,1.0,0.0);
			}else if(_zbuffer[pos]._dirty==1){
				const Real32 z=_zbuffer[pos]._buffer[posfr];
				glColor3f(1.0,z,c);
			}else{
				const Real32 z=_zbuffer[pos]._buffer[posfr];
				glColor3f(z,z,c);
			}
			glVertex2i(x,ny);
			if(_zbuffer[pos]._reads>1 && x==0 && y==0)
				std::cerr << " " << _zbuffer[pos]._reads;
		}
	}
	std::cerr << std::endl;
	glEnd();
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

