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


/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/

/*------------- constructors & destructors --------------------------------*/

inline
StencilTest::StencilTest(void):_stencilbuf(NULL), _w(0), _h(0), _count(1), _maxtests(0), _results(NULL)
{
};

inline
StencilTest::~StencilTest(void)
{
	if(_stencilbuf) delete[] _stencilbuf;
	if(_results) delete[] _results;
};

/*------------------------------ access -----------------------------------*/

inline
void StencilTest::frameInit(void)
{
};

inline
void StencilTest::frameExit(void)
{
};

inline
void StencilTest::setup(Viewport* port, const int& max)
{
	int w=port->getPixelWidth();
	int h=port->getPixelHeight();
	if(_w!=w || _h!=h || _stencilbuf==NULL){
		_w=w;
		_h=h;
		if(_stencilbuf) delete _stencilbuf;
		_stencilbuf=new GLuint[_w*_h];
	}
	if(_maxtests!=max){
		if(_results) delete[] _results;
		_results=new unsigned int[max];
		_maxtests=max;
	}
};

inline
void StencilTest::init(void)
{
	glDepthMask(GL_FALSE);
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glEnable(GL_STENCIL_TEST);
	glReadBuffer(GL_BACK);
};

inline
void StencilTest::perform(const int& num, const DynamicVolume& vol)
{
	if(_count>15){		// FIXME
		glClearStencil(0);
		glClear(GL_STENCIL_BUFFER_BIT);
		_count=1;
	}else
		_count++;
	glStencilFunc(GL_ALWAYS, _count, (GLuint)(-1));
	Pnt3f min,max;
	vol.getBounds(min, max);
	glBegin( GL_TRIANGLE_STRIP);
	glVertex3f( min[0], min[1], max[2]);
	glVertex3f( max[0], min[1], max[2]);
	glVertex3f( min[0], max[1], max[2]);
	glVertex3f( max[0], max[1], max[2]);
	glVertex3f( min[0], max[1], min[2]);
	glVertex3f( max[0], max[1], min[2]);
	glVertex3f( min[0], min[1], min[2]);
	glVertex3f( max[0], min[1], min[2]);
	glEnd();

	glBegin( GL_TRIANGLE_STRIP);
	glVertex3f( max[0], max[1], min[2]);
	glVertex3f( max[0], max[1], max[2]);
	glVertex3f( max[0], min[1], min[2]);
	glVertex3f( max[0], min[1], max[2]);
	glVertex3f( min[0], min[1], min[2]);
	glVertex3f( min[0], min[1], max[2]);
	glVertex3f( min[0], max[1], min[2]);
	glVertex3f( min[0], max[1], max[2]);
	glEnd();
		
	_results[num]=0;

	short minx=_w, maxx=0;
	short miny=_h, maxy=0;

	short bb_x[8];
	short bb_y[8];
	short b_z[8];
	for(char j=0; j<8; j++){
		if(bb_x[j]<minx) minx=bb_x[j];
		if(bb_x[j]>maxx) maxx=bb_x[j]+1;
		if(bb_y[j]<miny) miny=bb_y[j];
		if(bb_y[j]>maxy) maxy=bb_y[j]+1;
	}

	// FIXME
	short tmp=miny;
	miny=_h-maxy;
	maxy=_h-tmp;
		
	short sx=minx-minx%32;
	short sw=maxx-minx;
	sw+=sw%32;
	for(short y=miny; y<maxy; y++){
		glReadPixels(sx,y,sw,1,GL_STENCIL_INDEX,GL_INT,_stencilbuf+sx);
		for(short x=minx; x<maxx; x++){
			if(_stencilbuf[x]==_count){
				_results[num]++;
				y=maxy;
			}
		}
	}
};

inline
int StencilTest::result(const int& num)
{
	return(_results[num]);
};
			
inline
void StencilTest::exit(void){
	glDisable(GL_STENCIL_TEST);
	glDepthMask(GL_TRUE);
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
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

