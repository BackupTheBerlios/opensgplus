/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *             Copyright (C) 2000-2002 by the OpenSG Forum                   *
 *                                                                           *
 *                            www.opensg.org                                 *
 *                                                                           *
 *   contact: dirk@opensg.org, gerrit.voss@vossg.org, jbehr@zgdv.de          *
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


#include <stdlib.h>
#include <stdio.h>

#include <OSGConfig.h>

#include <OSGAction.h>

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

/** \brief Constructor
 */



/** \brief Destructor
 */


/*------------------------------ access -----------------------------------*/

/*---------------------------- properties ---------------------------------*/

inline 
Material *OCRenderAction::getMaterial(void) const
{
    return _pMaterial;
}

inline 
void OCRenderAction::updateTopMatrix(void)
{
    _accMatrix = _camInverse;
    _accMatrix.mult(_currMatrix.second);
}

inline
const Matrix &OCRenderAction::top_matrix(void)
{
// not necessary anymore, is updated as soon as _currMatrix changes
//    _accMatrix = _camInverse;
//    _accMatrix.mult(_currMatrix.second);

    return _accMatrix;
}

inline void OCRenderAction::transformNode(NodePtr& node, Int16* bb_x, Int16* bb_y, Real32* bb_z, char& bb_minidx){
        Pnt3f min,max;
	
        Viewport* port=getViewport();
	const Int16 w=port->getPixelWidth()/2;
	const Int16 h=port->getPixelHeight()/2;

	bb_minidx=0;

        node->getVolume().getBounds( min, max );
        for(int i=0; i<8; i++){
                Pnt3f p1((i&1)?min[0]:max[0],
			(i&2)?min[1]:max[1],
			(i&4)?min[2]:max[2]);
		Pnt3f p3;
		Pnt3f p2;

		// FIXME!
		_currMatrix.second.multFullMatrixPnt(p1,p3);
		_projmatrix.multFullMatrixPnt(p3,p2);
		
                bb_x[i]=(Int16)((p2[0]+1)*w);
                bb_y[i]=(Int16)(port->getPixelHeight()-(p2[1]+1)*h);
		
                bb_z[i]=(p2[2]+1.0)*0.5;
		
                if(bb_z[i]<bb_z[bb_minidx]) bb_minidx=i;
        }
};

inline void OCRenderAction::setSoftTest(const bool v){ 
	 _occltesthp.setSoftTest(v);
	 _occltesthpstats.setSoftTest(v);
	 _occltestnv.setSoftTest(v);
	 _occltestnvstats.setSoftTest(v);
	 _occlteststencil.setSoftTest(v);
	 _occlteststencilstats.setSoftTest(v);
	 _occltestzb.setSoftTest(v);
	 _occltestzbstats.setSoftTest(v);
	 _occltestdummy.setSoftTest(v);
	 _occltestdummystats.setSoftTest(v);
};

inline void OCRenderAction::setPredictionTest(const bool v){
	 _occltesthp.setPredictionTest(v);
	 _occltesthpstats.setPredictionTest(v);
	 _occltestnv.setPredictionTest(v);
	 _occltestnvstats.setPredictionTest(v);
	 _occlteststencil.setPredictionTest(v);
	 _occlteststencilstats.setPredictionTest(v);
	 _occltestzb.setPredictionTest(v);
	 _occltestzbstats.setPredictionTest(v);
	 _occltestdummy.setPredictionTest(v);
	 _occltestdummystats.setPredictionTest(v);
};

inline void OCRenderAction::setShadowTest(const bool v){
	 _occltesthp.setShadowTest(v);
	 _occltesthpstats.setShadowTest(v);
	 _occltestnv.setShadowTest(v);
	 _occltestnvstats.setShadowTest(v);
	 _occlteststencil.setShadowTest(v);
	 _occlteststencilstats.setShadowTest(v);
	 _occltestzb.setShadowTest(v);
	 _occltestzbstats.setShadowTest(v);
	 _occltestdummy.setShadowTest(v);
	 _occltestdummystats.setShadowTest(v);
};

inline void OCRenderAction::setExactOM(const bool v){
	 _occltesthp.setExactOM(v);
	 _occltesthpstats.setExactOM(v);
	 _occltestnv.setExactOM(v);
	 _occltestnvstats.setExactOM(v);
	 _occlteststencil.setExactOM(v);
	 _occlteststencilstats.setExactOM(v);
	 _occltestzb.setExactOM(v);
	 _occltestzbstats.setExactOM(v);
	 _occltestdummy.setExactOM(v);
	 _occltestdummystats.setExactOM(v);
};

inline void OCRenderAction::setMaxPix(const int& mp){
	_maxpix=mp;
};

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

