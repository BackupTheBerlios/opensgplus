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
void StencilTest::setup(const UInt16& max, Viewport* port, const UInt32 maxpix)
{
	_port=port;
	UInt16 w=_port->getPixelWidth();
	UInt16 h=_port->getPixelHeight();
	if(_w!=w || _h!=h || _stencilbuf==NULL){
		_w=w;
		_h=h;
		if(_stencilbuf) delete _stencilbuf;
		_stencilbuf=new GLuint[_w*_h];
	}
	if(_maxtests!=max){
		if(_results) delete[] _results;
		_results=new UInt32[max];
		_maxtests=max;
	}
	_maxpix=maxpix;
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
void StencilTest::perform(const UInt16& num, const OCTestNode* node)
{
	const DynamicVolume& vol=node->_node->getVolume();
	/*
	Matrix m,v;
	_port->getCamera()->getProjection(m, _w, _h);
	_port->getCamera()->getViewing(v, _w, _h);
	m.mult(v);
	*/

	// FIXME
	// UGLY, very slow implementation... :-(
        Pnt3f min,max;
	vol.getBounds( min, max );

	Matrix transmatr,modelv;
	GLfloat proj_f[16];
	GLfloat modelv_f[16];

	glGetFloatv(GL_MODELVIEW_MATRIX,modelv_f);
	glGetFloatv(GL_PROJECTION_MATRIX,proj_f);
	transmatr.setValue(proj_f);
	modelv.setValue(modelv_f);
	transmatr.mult(modelv);

	short minx=_w, maxx=0;
	short miny=_h, maxy=0;
        for(UInt16 i=0; i<8; i++){
		Pnt3f p1,p2;
		p1[0]=(i&1)?min[0]:max[0];
		p1[1]=(i&2)?min[1]:max[1];
		p1[2]=(i&4)?min[2]:max[2];

		transmatr.multFullMatrixPnt(p1,p2);

		const short bb_x=(short)((p2[0]+1)*_w*0.5);
		const short bb_y=(short)(_h-(p2[1]+1)*_h*0.5);
		if(bb_x<minx) minx=bb_x;
		if(bb_x>maxx) maxx=bb_x+1;
		if(bb_y<miny) miny=bb_y;
		if(bb_y>maxy) maxy=bb_y+1;
        }

	if(_count>15){		// FIXME
		glClearStencil(0);
		glClear(GL_STENCIL_BUFFER_BIT);
		_count=1;
	}else
		_count++;

	glStencilFunc(GL_ALWAYS, _count, (GLuint)(-1));

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
				if(_results[num]>_maxpix)
					y=maxy;
			}
		}
	}
};

inline
UInt32 StencilTest::result(const UInt16& num)
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

