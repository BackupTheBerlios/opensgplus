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


/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/

/*------------- constructors & destructors --------------------------------*/

inline
NVTestGeo::NVTestGeo(void):_maxtests(0), _results(NULL)
{
};

inline
NVTestGeo::~NVTestGeo(void)
{
	if(_results) delete[] _results;
};

/*------------------------------ access -----------------------------------*/

inline
void NVTestGeo::frameInit(void)
{
};

inline
void NVTestGeo::frameExit(void)
{
};

inline
void NVTestGeo::setup(const Int32& max)
{
	if(_maxtests!=max){
		if(_results) delete[] _results;
		_results=new GLuint[max];
		_maxtests=max;
	}
};

inline
void NVTestGeo::init(void)
{
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
#ifdef GL_NV_occlusion_query
	// cerr << "_maxtest=" << _maxtests << endl;
        glGenOcclusionQueriesNV(_maxtests, _results);
#endif
};

inline
void NVTestGeo::start(const Int32& num)
{
#ifdef GL_NV_occlusion_query
	glBeginOcclusionQueryNV(_results[num]);
#endif
}
	
inline
void NVTestGeo::end(void)
{
#ifdef GL_NV_occlusion_query
	glEndOcclusionQueryNV();
#endif
};

inline
int NVTestGeo::result(const Int32& num)
{
#ifdef GL_NV_occlusion_query
	GLuint pixelCount;
	glGetOcclusionQueryuivNV(_results[num], GL_PIXEL_COUNT_NV, &pixelCount);
#else
	GLuint pixelCount=100000;
#endif
	return(pixelCount);
};
			
inline
void NVTestGeo::exit(void){
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

