/*---------------------------------------------------------------------------*\
 *                     OpenSG PLUS OcclusionTest Test                            *
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


#include "OSGConfig.h"

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

/**
 Constructor - does nothing :-)
*/
template<typename octest, typename ocstatistics>
inline
OcclusionTest<octest, ocstatistics>::OcclusionTest(void)
{
};

/**
 Destructor - does nothing :-)
*/
template<typename octest, typename ocstatistics>
inline
OcclusionTest<octest, ocstatistics>::~OcclusionTest(void)
{
};

/*------------------------------ access -----------------------------------*/

/**
 Called at the beginning of every frame before anything is rendered (occlusiion culled?)
*/
template<typename octest, typename ocstatistics>
inline
void OcclusionTest<octest, ocstatistics>::frameInit(void)
{
	_stats.clear();
	_octest.frameInit();
};

/**
 Called at the end of every frame after rendering
*/
template<typename octest, typename ocstatistics>
inline
void OcclusionTest<octest, ocstatistics>::frameExit(void)
{
	_octest.frameExit();
	_stats.print();
};

/**
 Setups the corresponding occlusion culling approach,
 called only once to configure occlusion culling.
*/
template<typename octest, typename ocstatistics>
inline
void OcclusionTest<octest, ocstatistics>::setup(const UInt16& i, Viewport* v, const UInt32 maxpix)
{
	_octest.setup(i,v,maxpix);
};


/**
*/
template<typename octest, typename ocstatistics>
inline
void OcclusionTest<octest, ocstatistics>::init(void)
{
	_octest.init();
};

/**
*/
template<typename octest, typename ocstatistics>
inline
void OcclusionTest<octest, ocstatistics>::perform(const UInt16& i, const OCTestNode* node)
{
	_stats.add(OcclusionStatistics::_tests, 1);
	_octest.perform(i, node);
};

/**
*/
template<typename octest, typename ocstatistics>
inline
UInt32 OcclusionTest<octest, ocstatistics>::result(const UInt16& i)
{
	const UInt16 result=_octest.result(i);
	_stats.add(OcclusionStatistics::_visiblePixels, result);
	_stats.add(OcclusionStatistics::_visibleNodes, result>0?1:0);
	return(result);
};
			
/**
*/
template<typename octest, typename ocstatistics>
inline
void OcclusionTest<octest, ocstatistics>::exit(void)
{
	_octest.exit();
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

