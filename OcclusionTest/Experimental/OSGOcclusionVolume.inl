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

template<UInt16 volwidth, UInt16 volheight, UInt16 voldepth>
void OcclusionVolume<volwidth, volheight, voldepth>::generateOcclusionVolume(NodePtr){
	osgWin->frameInit();    // frame-cleanup
	
	cam_trans->getSFMatrix()->setValue(m1);

	osgWin->drawAllViewports(ract);   // draw the viewports
	osgWin->swap();
	osgWin->frameExit();    // frame-cleanup
};

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

template<UInt16 volwidth, UInt16 volheight, UInt16 voldepth>
inline
OcclusionVolume<volwidth, volheight, voldepth>::OcclusionVolume<volwidth, volheight, voldepth>(void):_maxtests(0), _results(NULL), _w(0), _h(0)
{
};

template<UInt16 volwidth, UInt16 volheight, UInt16 voldepth>
inline
OcclusionVolume<volwidth, volheight, voldepth>::~OcclusionVolume<volwidth, volheight, voldepth>(void)
{
};

/*------------------------------ access -----------------------------------*/

template<UInt16 volwidth, UInt16 volheight, UInt16 voldepth>
inline
void OcclusionVolume<volwidth, volheight, voldepth>::frameInit(void)
{
};

template<UInt16 volwidth, UInt16 volheight, UInt16 voldepth>
inline
void OcclusionVolume<volwidth, volheight, voldepth>::frameExit(void)
{
	// visualize();
};

template<UInt16 volwidth, UInt16 volheight, UInt16 voldepth>
inline
void OcclusionVolume<volwidth, volheight, voldepth>::setup(const Int32& max, Viewport* port, const UInt32 maxpix)
{
	Int32 w=port->getPixelWidth();
	Int32 h=port->getPixelHeight();
	if(_w!=w || _h!=h){
		_w=w;
		_h=h;
	}
	setAllUnused();

	_maxpix=maxpix;
        if(_maxtests!=max){
                if(_results) delete[] _results;
		_results=new UInt32[max];
		_maxtests=max;
	}
};

template<UInt16 volwidth, UInt16 volheight, UInt16 voldepth>
inline
void OcclusionVolume<volwidth, volheight, voldepth>::init(void)
{
};

template<UInt16 volwidth, UInt16 volheight, UInt16 voldepth>
inline
void OcclusionVolume<volwidth, volheight, voldepth>::perform(const Int32& num, const OCTestNode* node)
{
	_results[num]=testBox(bb_x,bb_y,bb_z,bb_minidx);
};

template<UInt16 volwidth, UInt16 volheight, UInt16 voldepth>
inline
UInt32 OcclusionVolume<volwidth, volheight, voldepth>::result(const Int32& num)
{
	return(_results[num]);
};
			
template<UInt16 volwidth, UInt16 volheight, UInt16 voldepth>
inline
void OcclusionVolume<volwidth, volheight, voldepth>::exit(void){
};

template<UInt16 volwidth, UInt16 volheight, UInt16 voldepth>
inline
void OcclusionVolume<volwidth, volheight, voldepth>::visualize(void){
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

