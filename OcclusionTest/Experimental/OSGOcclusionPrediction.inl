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

#include<stdio.h>

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
OcclusionPrediction::OcclusionPrediction(void):
_frame(0),
_maxtests(0),
_results(NULL)
{
};

inline
OcclusionPrediction::~OcclusionPrediction(void)
{
};

/*------------------------------ access -----------------------------------*/

inline
void OcclusionPrediction::frameInit(void)
{
	_fdiff=2;
};

inline
void OcclusionPrediction::frameExit(void)
{
	_frame++;
	if(_frame>31)
		_frame=0;
};

inline
void OcclusionPrediction::setup(const Int32& max, Viewport* port, const UInt32& x)
{
	// _frame=0 ? Hmmmm... FIXME
        if(_maxtests!=max){
                if(_results) delete[] _results;
		_results=new Int16[max];
		_maxtests=max;
	}

	_cam=port->getCamera();
};

inline
void OcclusionPrediction::init(void)
{
};

inline
void OcclusionPrediction::perform(const Int32& index, const OCTestNode* ocnode)
{
	TraceMap::iterator pos=_visibilityTraces.find(ocnode->_node.getCPtr());

	if(pos==_visibilityTraces.end()){
		VisibilityTrace* trace=new VisibilityTrace();
		trace->_frame=0;
		trace->_visibility=-1;
		_visibilityTraces[ocnode->_node.getCPtr()]=trace;
		_results[index]=0;	// unknown
	}else{
		_results[index]=0;
		VisibilityTrace* trace=pos->second;
		Int16 d;
		if(_frame>=(trace->_frame))
			d=_frame-(trace->_frame);
		else
			d=31-(trace->_frame)+_frame;
	
		UInt32 x=1 << trace->_frame;
		if(trace->_visibility & x){
			if(d<_fdiff){
				_results[index]=1;
			}
		}else{
			if(d<2){
				_results[index]=-1;
			}
		}
	}
	_fdiff++;
	if(_fdiff>4)
		_fdiff=2;
}

inline
void OcclusionPrediction::update(const OCTestNode* ocnode, const Int32 visi)
{
	TraceMap::iterator pos=_visibilityTraces.find(ocnode->_node.getCPtr());
	
	if(pos==_visibilityTraces.end())
		return;
	
	VisibilityTrace* trace=pos->second;
	trace->_frame=_frame+1;
	const UInt32 x=1 << _frame+1;
	if(visi<0){
		trace->_visibility&=!x;
	}else{
		trace->_visibility|=x;
	}
};

inline
Int32 OcclusionPrediction::result(const Int32& num)
{
	return(_results[num]);
};
			
inline
void OcclusionPrediction::exit(void){
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

