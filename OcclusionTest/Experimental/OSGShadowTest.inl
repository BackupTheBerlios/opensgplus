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

#include<GL/gl.h>

#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGDrawAction.h>
#include <OpenSG/OSGViewport.h>

#include <OVGeoTools.h>

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
ShadowTest::ShadowTest(void):_maxtests(0), _results(NULL), _lastsv(0), _w(0), _h(0)
{
};

inline
ShadowTest::~ShadowTest(void)
{
	for(Int32 i=0; i<_lastsv; i++)
		delete _sv[i];
	if(_results) delete[] _results;
//	if(_sv) delete[] _sv;
};

/*------------------------------ access -----------------------------------*/

inline
void ShadowTest::frameInit(void)
{
};

inline
void ShadowTest::frameExit(void)
{
};

inline
void ShadowTest::setup(const Int32& max, Viewport* port, const Pnt3f& eye)
{
	Int32 w=port->getPixelWidth();
	Int32 h=port->getPixelHeight();
	if(_w!=w || _h!=h){
		_w=w;
		_h=h;
	}

	if(_maxtests!=max){
		if(_results) delete[] _results;
		_results=new bool[max];
		_maxtests=max;
/*
		if(_sv) delete[] _sv;
		_sv=new ShadowVolumePtr[max];
*/
	}
	_eye=eye;
	_eye[0]=_w*0.5f;
	_eye[1]=_h*0.5f;
	_eye[2]=-2000000.0f;
	for(Int32 i=0; i<_lastsv; i++)
		delete _sv[i];
	_lastsv=0;
};

inline
void ShadowTest::init(void)
{
};

inline
void ShadowTest::perform(const Int32& num, const Int16* bb_x, const Int16* bb_y, const Real32* bb_z, const char bb_minidx)
{
	if(_lastsv==0){
		_results[num]=true;
		return;
	}

	Int32 v[4];
	glGetIntegerv(GL_VIEWPORT, v);
	
	for(Int32 i=0; i<_lastsv; i++){
		_results[num]=false;
		for(Int32 j=0; j<8; j++){
			// Pnt3f p((Real32)bb_x[j], (Real32)bb_y[j], bb_z[j]);
			Pnt3f p((Real32)bb_x[j], (Real32)bb_y[j], 0.0f);
			if(_sv[i]->intersect(p)==false){	// visibl
				_results[num]=true;
			/*	
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();
		
				glMatrixMode(GL_PROJECTION);
				glPushMatrix();
				glLoadIdentity();

				glDisable(GL_LIGHTING);
				glDisable(GL_DEPTH_TEST);

				glBegin(GL_LINES);
				glColor3f(0.0,1.0,0.0);
				glVertex2f(2.0*(Real32)bb_x[j]/v[2]-1.0, 1.0-2.0*(Real32)bb_y[j]/v[3]);
				glVertex2f(0.0,0.0);
				glEnd();
				
				glEnable(GL_LIGHTING);
				glEnable(GL_DEPTH_TEST);
		
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
			*/
				break;
			}
			/*
			else{
			
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();
		
				glMatrixMode(GL_PROJECTION);
				glPushMatrix();
				glLoadIdentity();

				glDisable(GL_LIGHTING);
				glDisable(GL_DEPTH_TEST);

				glBegin(GL_LINES);
				glColor3f(0.0,1.0,1.0);
				glVertex2f(2.0*(Real32)bb_x[j]/v[2]-1.0, 1.0-2.0*(Real32)bb_y[j]/v[3]);
				glVertex2f(0.0,0.0);
				glEnd();
				
				glEnable(GL_LIGHTING);
				glEnable(GL_DEPTH_TEST);
		
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
			
			}
			*/
		}
		if(_results[num]==false)
			break;
	}
}

inline
void ShadowTest::insertBox(const Int16* bb_x, const Int16* bb_y, const Real32* bb_z, const char bb_minidx)
{
	UInt16 result[10];
	const UInt16 res=calcConvHull2D<Int16, Int32, 8>(bb_x, bb_y, result);
	_sv[_lastsv]=new ShadowVolume(res);
	for(Int32 i=1; i<res; i++){
		// Pnt3f p2((Real32)bb_x[result[i-1]], (Real32)bb_y[result[i-1]], bb_z[result[i-1]]);
		// Pnt3f p1((Real32)bb_x[result[i]], (Real32)bb_y[result[i]], bb_z[result[i]]);
		Pnt3f p2((Real32)bb_x[result[i-1]], (Real32)bb_y[result[i-1]], 0.0);
		Pnt3f p1((Real32)bb_x[result[i]], (Real32)bb_y[result[i]], 0.0);


		// _sv[_lastsv]->setPlane(Plane(_eye, p2, p1), i);
		/*
		Real32 a=bb_x[result[i-1]]-bb_x[result[i]];
		Real32 b=bb_y[result[i]]-
		Real32 x=sqrtf();
		Real32 y=sqrtf(1-x*x);
		*/
		Pnt3f p(p1.x(), p1.y(), p1.z()+100.0);
		// Pnt3f p(0.0,0.0,0.0);
		_sv[_lastsv]->setPlane(Plane(p1, p2, p), i);
	
		/*
		Vec3f ntmp(_sv[_lastsv]->getPlanes()[i].getNormal());
		Vec2f n(ntmp.x(), ntmp.y());
		n.normalize();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);

		glBegin(GL_LINES);
		// glColor3f(1.0,(Real32)i/(Real32)res,(Real32)i/(Real32)res);
		glColor3f(1.0,1.0,0.0);
		// glVertex3f(2.0*(Real32)bb_x[result[i-1]]/_w-1.0, 1.0-2.0*(Real32)bb_y[result[i-1]]/_h, bb_z[result[i]]);
		glVertex2f(2.0*(Real32)bb_x[result[i-1]]/_w-1.0, 1.0-2.0*(Real32)bb_y[result[i-1]]/_h);
		glColor3f(0.0,0.0,1.0);
	//	glVertex3f(2.0*_eye.x()/_w-1.0,1.0-2.0*_eye.y()/_h,_eye.z());
		// glVertex3f(2.0*(Real32)bb_x[result[i]]/_w-1.0, 1.0-2.0*(Real32)bb_y[result[i]]/_h, bb_z[result[i+1]]);
		glVertex2f(2.0*(Real32)bb_x[result[i]]/_w-1.0, 1.0-2.0*(Real32)bb_y[result[i]]/_h);
	//	glVertex3f(2.0*_eye.x()/_w-1.0,1.0-2.0*_eye.y()/_h,_eye.z());
		glColor3f(1.0,1.0,0.0);
		glVertex2f(2.0*(Real32)bb_x[result[i-1]]/_w-1.0, 1.0-2.0*(Real32)bb_y[result[i-1]]/_h);
		glColor3f(1.0,1.0,1.0);
		glVertex2f(2.0*(Real32)(bb_x[result[i-1]]+100*n.x())/_w-1.0, 1.0-2.0*(Real32)(bb_y[result[i-1]]+100*n.y())/_h);
		glColor3f(0.0,0.0,1.0);
		glVertex2f(2.0*(Real32)bb_x[result[i]]/_w-1.0, 1.0-2.0*(Real32)bb_y[result[i]]/_h);
		glColor3f(1.0,1.0,1.0);
		glVertex2f(2.0*(Real32)(bb_x[result[i]]+100*n.x())/_w-1.0, 1.0-2.0*(Real32)(bb_y[result[i]]+100*n.y())/_h);
		glEnd();
		
//		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		*/
	}
	_lastsv++;

	// cerr << "i" << _lastsv;
	
	// bb_minidx -> 0...7, weiteste, sichtbare ecke -> "farplane"
	// bb_x[0...7], 8 vektoren fuer die einzelnen ecken
	// bb_maxidx -> naechste, sichtbare ecke
/*	
	UInt16 bb_maxidx=0;
	for(UInt16 i=0; i<8; i++)
		if(bb_z[i]>bb_z[bb_maxidx]) bb_maxidx=i;

	if(bb_minidx>3){
		// Plane 
		if(testTriangle(bb_x[0],bb_y[0],bb_z[0], bb_x[1],bb_y[1],bb_z[1], bb_x[3],bb_y[3],bb_z[3])) return(boxIsVisible());
		if(testTriangle(bb_x[3],bb_y[3],bb_z[3], bb_x[0],bb_y[0],bb_z[0], bb_x[2],bb_y[2],bb_z[2])) return(boxIsVisible());
	}else{
		// if(bb_minidx<4)
		if(testTriangle(bb_x[5],bb_y[5],bb_z[5], bb_x[6],bb_y[6],bb_z[6], bb_x[7],bb_y[7],bb_z[7])) return(boxIsVisible());
		if(testTriangle(bb_x[5],bb_y[5],bb_z[5], bb_x[4],bb_y[4],bb_z[4], bb_x[6],bb_y[6],bb_z[6])) return(boxIsVisible());
	}

	if(bb_minidx==1 || bb_minidx==3 || bb_minidx==5 || bb_minidx==7){
		// ==1,3,5,7
		if(testTriangle(bb_x[0],bb_y[0],bb_z[0], bb_x[2],bb_y[2],bb_z[2], bb_x[4],bb_y[4],bb_z[4])) return(boxIsVisible());
		if(testTriangle(bb_x[2],bb_y[2],bb_z[2], bb_x[4],bb_y[4],bb_z[4], bb_x[6],bb_y[6],bb_z[6])) return(boxIsVisible());
	}else{
		// if(bb_minidx==0 || bb_minidx==2 || bb_minidx==4 || bb_minidx==6){
		// ==0,2,4,6
		if(testTriangle(bb_x[3],bb_y[3],bb_z[3], bb_x[7],bb_y[7],bb_z[7], bb_x[5],bb_y[5],bb_z[5])) return(boxIsVisible());
		if(testTriangle(bb_x[3],bb_y[3],bb_z[3], bb_x[1],bb_y[1],bb_z[1], bb_x[5],bb_y[5],bb_z[5])) return(boxIsVisible());
	}

	if(bb_minidx==2 || bb_minidx==3 || bb_minidx==6 || bb_minidx!=7){
		// ==2,3,6,7
		if(testTriangle(bb_x[0],bb_y[0],bb_z[0], bb_x[5],bb_y[5],bb_z[5], bb_x[4],bb_y[4],bb_z[4])) return(boxIsVisible());
		if(testTriangle(bb_x[0],bb_y[0],bb_z[0], bb_x[1],bb_y[1],bb_z[1], bb_x[5],bb_y[5],bb_z[5])) return(boxIsVisible());
	}else{
		// if(bb_minidx==0 || bb_minidx==1 || bb_minidx==4 || bb_minidx==5){
		// ==0,1,4,5
		if(testTriangle(bb_x[2],bb_y[2],bb_z[2], bb_x[3],bb_y[3],bb_z[3], bb_x[6],bb_y[6],bb_z[6])) return(boxIsVisible());
		if(testTriangle(bb_x[3],bb_y[3],bb_z[3], bb_x[7],bb_y[7],bb_z[7], bb_x[6],bb_y[6],bb_z[6])) return(boxIsVisible());
	}

	return(boxIsHidden());
*/
};

inline
Int32 ShadowTest::result(const Int32& num)
{
	/*
	if(_results[num])
		cerr << "t";
	else
		cerr << "f";
	*/
	if(_results[num]) return(1);
	return(0);
};
			
inline
void ShadowTest::exit(void){
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

