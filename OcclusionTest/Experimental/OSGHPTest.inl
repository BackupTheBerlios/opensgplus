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
#include <OSGWindow.h>
#include <OSGWindowBase.h>

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
HPTest::HPTest(void):_maxtests(0), _results(NULL)
{
	_hpExtension = Window::registerExtension("GL_HP_occlusion_test");
};

inline
HPTest::~HPTest(void)
{
	if(_results) delete[] _results;
};

/*------------------------------ access -----------------------------------*/

inline
void HPTest::frameInit(void)
{
};

inline
void HPTest::frameExit(void)
{
};

inline
void HPTest::setup(const UInt16& max, Viewport* vp, const UInt32 maxpix)
{
	WindowPtr win=vp->getParent();
	if(win->hasExtension(_hpExtension)){
		_extworks=true;
	}else{
		_extworks=false;
	}
	
	_vispix=maxpix+1;

	if(_maxtests!=max){
		if(_results) delete[] _results;
		_results=new GLboolean[max];
		_maxtests=max;
	}
};

inline
void HPTest::init(void)
{
	glDepthMask(GL_FALSE);
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
};

inline
void HPTest::perform(const UInt16& num, const OCTestNode* node)
{
	const DynamicVolume& vol=node->_node->getVolume();
#ifdef GL_OCCLUSION_TEST_HP
	if(_extworks){
		glEnable(GL_OCCLUSION_TEST_HP);

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

		glDisable(GL_OCCLUSION_TEST_HP);
		
		_results[num]=0;
		glGetBooleanv(GL_OCCLUSION_TEST_RESULT_HP, &(_results[num]));
	}else{
		_results[num]=1;
	}
#else
	_results[num]=1;
#endif
};

inline
UInt32 HPTest::result(const UInt16& num)
{
	return(_results[num]?_vispix:0);
};
			
inline
void HPTest::exit(void){
	glDepthMask(GL_TRUE);
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
};

/**
*/
inline
void HPTest::visualize(void)
{
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

