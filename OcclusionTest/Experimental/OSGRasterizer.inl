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

/*------------------------------ access -----------------------------------*/

template<typename T, bool with_depth> Int32 Rasterizer<T,with_depth>::_size=1600;
template<typename T, bool with_depth> Int32 Rasterizer<T,with_depth>::_w=1600;
template<typename T, bool with_depth> Int32 Rasterizer<T,with_depth>::_h=1200;

template<typename T, bool with_depth>
inline bool Rasterizer<T,with_depth>::testTriangle(T* t, const Int16& x1, const Int16& y1, const Real32& z1,
		const Int16& x2, const Int16& y2, const Real32& z2,
		const Int16& x3, const Int16& y3, const Real32& z3){

	register Int16 minidx=0;
	register Int16 maxidx=1;
	register Int16 minidx_x=0;
	register Int16 maxidx_x=1;

	register Real32 z[3];
	if(with_depth){
		z[0]=z1;
		z[1]=z2;
		z[2]=z3;
	}

	register Int16 x[3], y[3];
	x[0]=x1; y[0]=y1;
	x[1]=x2; y[1]=y2;
	x[2]=x3; y[2]=y3;
	
	// sort vertices by y sortieren... 
	for(Int32 i=0; i<3; i++){
		if(y[i]<y[minidx]) minidx=i;
		if(y[i]>y[maxidx]) maxidx=i;
		if(x[i]<x[minidx_x]) minidx_x=i;
		if(x[i]>x[maxidx_x]) maxidx_x=i;
	}
	const Int16 centeridx=(7 ^ ((1 << minidx) | (1 << maxidx))) >> 1;	// :-)

	// vertex top left
	const Int32 xmin=x[minidx];
	const Int32 ymin=y[minidx];
	const Real32 zmin=z[minidx];
	
	// vertex middle
	const Int32 xmitte=x[centeridx];
	const Int32 ymitte=y[centeridx];
	const Real32 zmitte=z[centeridx];
	
	// vertex bottom
	const Int32 xmax=x[maxidx];
	const Int32 ymax=y[maxidx];
	const Real32 zmax=z[maxidx];

	// triangle inside visible viewport?
	if(ymin<0 && ymitte<0 && ymax<0) return(0);
	if(xmin<0 && xmitte<0 && xmax<0) return(0);
	if(ymin>_h && ymitte>_h && ymax>_h) return(0);
	if(xmin>_w && xmitte>_w && xmax>_w) return(0);
	if(zmin>1.0 && zmitte>1.0 && zmax>1.0) return(0);
	if(zmin<0.0 && zmitte<0.0 && zmax<0.0) return(0);

	Int32 dx1=xmax-xmin;
	Int32 dy1=ymax-ymin;
	Real32 dz1;
	if(with_depth){
		if(dy1==0){
			dz1=0;
		}else{
			dz1=(zmax-zmin)/((Real32)dy1);
		}
	}
	
	Int32 f1=1;
	if(dx1<0){
		f1=-1;
		dx1*=f1;
	}

	Int32 xx1=xmin;
	Real32 zz1=zmin;
	Int32 accu1=0;

	//    *
	//   ****
	//  *******
	//     ......
	//        .....
	//           ....
	//              ...
	//                 .
	if(Rasterizer<T,with_depth>::raster(t, accu1, f1, xmin, xmitte, ymin, ymitte, zmin, zmitte, xx1, zz1, dx1, dy1, dz1))
		return(1);

	//    *
	//   ****
	//  *******
	//     ******
	//        *****
	//           ****
	//              ***
	//                 *
	if(Rasterizer<T,with_depth>::raster(t, accu1, f1, xmitte, xmax, ymitte, ymax, zmitte, zmax, xx1, zz1, dx1, dy1, dz1))
		return(1);
	
	return(0);
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

template<typename T, bool with_depth>
inline bool Rasterizer<T,with_depth>::scanline(T* t, Int32& x1, const Real32& z1, Int32& x2, const Real32& z2, const Int32& y){
	register Real32 z=z1;
	register const Real32 dz=(z2-z1)/(x2-x1);
	
	Int32 x=x1;
	if(x<0){
		x=0;
		z+=dz*(-x1);
	}
	Int32 xend=x2;
	if(x2>=_w){
		xend=_w;
	}
	for(; x<xend; x++){
		if(t->visitPoint(x,y,z)) return(1);
		z+=dz;
	}
	return(0);
};

template<typename T, bool with_depth>
inline bool Rasterizer<T,with_depth>::scanline(T* t, Int32& x1, Int32& x2, const Int32& y){
	Int32 x=x1;
	if(x1<0)
		x1=0;
	Int32 xend=x2;
	if(xend>=_w)
		xend=_w;
	for(; x<xend; x++){
		if(t->visitPoint(x,y,0.0)) return(1);
	}
	return(0);
};

template<typename T, bool with_depth>
inline bool Rasterizer<T,with_depth>::raster(T* t, Int32& accu1, Int32& f1,
		const Int32& xmin, const Int32& xmitte, const Int32& ymin, const Int32& ymitte,
		const Real32& zmin, const Real32& zmitte, Int32& x1, Real32& z1,
		int& dx1, Int32& dy1, Real32& dz1){

	register Int32 dx2=xmitte-xmin;
	if(dx2>_size || -dx2>_size) return(1);
	
	register Int32 dy2=ymitte-ymin;
	if(dy2>_size || -dy2>_size) return(1);
	
	register Real32 dz2;
	if(with_depth){
		dz2=(zmitte-zmin)/((Real32)dy2);
	}
	
	register Int32 x2=xmin;
	register Real32 z2;
	if(with_depth){
		z2=zmin;
	}

	register Int32 f2=1;
	if(dx2<0){
		f2=-1;
		dx2*=f2;
	}

	register Int32 accu2=0;
	
	if(ymitte<0) return(0);
	if(ymin>=_h) return(0);
	
	register Int32 y=ymin;

	if(y<0){
		if(with_depth){
			z1+=dz1*(-ymin);
			z2+=dz2*(-ymin);
		}
		accu1-=dx1*ymin;
		accu2-=dx2*ymin;
		Int32 steps1=accu1/dy1;
		if(steps1>0)
			x1+=f1*steps1;
		Int32 steps2=accu2/dy2;
		if(steps2>0)
			x2+=f2*steps2;
		/*
		while(accu1>0){
			x1+=f1;
			accu1-=dy1;
		}
		while(accu2>0){
			x2+=f2;
			accu2-=dy2;
		}
		*/
		y=0;
	}
	register Int32 yende=ymitte>_h?_h:ymitte;
	
	for(; y<yende; y++){
		if(with_depth){
			if(x1>x2){
				if(Rasterizer<T,with_depth>::scanline(t,x2,z2,x1,z1,y)) return(1);
			}else{
				if(Rasterizer<T,with_depth>::scanline(t,x1,z1,x2,z2,y)) return(1);
			}
			z1+=dz1;
			z2+=dz2;
		}else{
			if(x1>x2){
				if(Rasterizer<T,with_depth>::scanline(t,x2,x1,y)) return(1);
			}else{
				if(Rasterizer<T,with_depth>::scanline(t,x1,x2,y)) return(1);
			}
		}
		accu1+=dx1;
		accu2+=dx2;
		if(accu1>0){
			Int32 steps1=accu1/dy1;
			x1+=(f1*steps1++);
			accu1-=(dy1*steps1);
		}
		if(accu2>0){
			Int32 steps2=accu2/dy2;
			x2+=(f2*steps2++);
			accu2-=(dy2*steps2);
		}
		
		/*
		while(accu1>0){
			x1+=f1;
			accu1-=dy1;
		}
		while(accu2>0){
			x2+=f2;
			accu2-=dy2;
		}
		*/
	}
	return(0);
};

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

