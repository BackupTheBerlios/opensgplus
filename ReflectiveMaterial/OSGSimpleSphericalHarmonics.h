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

#ifndef _OSGSIMPLESPHERICALHARMONICS_H_
#define _OSGSIMPLESPHERICALHARMONICS_H_
#ifdef __sgi
#pragma once
#endif

#include <OSGVector.h>
#include <OSGMatrix.h> 
/*  #include <OSGBaseTypes.h> */
/*  #include <iostream> */

OSG_BEGIN_NAMESPACE
class SimpleSphericalHarmonics
{
public:
  /*! \}                                                                 */
  /*---------------------------------------------------------------------*/
  /*! \name                      Constructors                            */
  /*! \{                                                                 */
  SimpleSphericalHarmonics(const SimpleSphericalHarmonics &source);
  SimpleSphericalHarmonics( int _Lmax=4 );

  /*! \}                                                                 */
  /*---------------------------------------------------------------------*/
  /*! \name                       Destructors                            */
  /*! \{                                                                 */

    ~SimpleSphericalHarmonics(void){};

  /*---------------------------------------------------------------------*/
  /*! \name                      Class Specific                          */
  /*! \{                                                                 */

  void computeYlmArray( const Vec3f &dir  );
  
  /* reset all coefficients */
  inline void resetCoeffs()
    {
      for( int k = 0; k < numBasis; k++ ){
	coefficients[k] = Vec3f( 0.0, 0.0, 0.0 );
      }
    }
    
  /* Add a sample, update coefficients accordingly */
  inline void add( const Vec3f &dir, Vec3f radiance )
    {
      computeYlmArray( dir );
      
      for( int k = 0; k < numBasis; k++ )
	coefficients[k] += radiance * YlmTemp[k];
    }
  
  /* Scale all the coefficients with f */
  inline void scale( float f )
    {
      for( int k = 0; k < numBasis; k++ )
	coefficients[k] *= f;
    }
  
  /*  Compute radiance from coefficients for given direction */
  inline Vec3f getRadiance( const Vec3f &dir )
    {
      Vec3f radiance( 0.0, 0.0, 0.0 );
      
      computeYlmArray( dir );
      
      for( int k = 0; k < numBasis; k++ )
	radiance += coefficients[k] * YlmTemp[k];
      
      return radiance;
    }
  
  /* Rotation stuff */
  /*  Rotate coefficients according to rotation matrix */
  void rotate( const Matrix4f &rot );
  
  /*  Descrease Lmax to a smaller value */
  void decreaseLmax( int newLmax ) {
    assert( newLmax < Lmax );
      Lmax = newLmax;
  }

  /*! \}                                                                 */
  /*---------------------------------------------------------------------*/
  /*! \name                           Get                                */
  /*! \{                                                                 */
  inline int getLmax() { return Lmax; }
  
  /*  Get array of coefficients */
  inline Vec3f *getCoefficients() { return coefficients; }


  /*! \}                                                                 */
  /*---------------------------------------------------------------------*/
  /*! \name                        Operators                             */
  /*! \{                                                                 */
  
  /*  Copy */
  inline void operator=( const SimpleSphericalHarmonics &sh )
    {
      if( this == &sh ) return; // copy a onto a, eh?
      
      // copy variables over
      Lmax = sh.Lmax;
      numBasis = sh.numBasis;
      
      // copy coefficients over
      for( int i = 0; i < numBasis; i++ ) {
	coefficients[i] = sh.coefficients[i];
	YlmTemp[i] = sh.YlmTemp[i];
      }
    }
  
  /*  Equal? */
  inline bool operator==(const SimpleSphericalHarmonics &other) const {
    
    for( int i=0; i<numBasis; i++ ){
      if(coefficients[i]!=other.coefficients[i]) return false;
    }
    return true; 
  }

protected:
  inline void init()
    {
      Lmax = 0;
      numBasis = 0;
    }


private:
  int Lmax;
  int numBasis;           /* basically (Lmax+1)*(Lmax+1) */
  
  Vec3f coefficients[25];
  float YlmTemp[25];        /* temp storage for Ylm basis function */
};


/*  template <class ValueTypeT> inline */
/*  std::ostream &operator <<(      std::ostream       &outStream, */
/*                            const Color3<ValueTypeT> &color); */

/*  template <class ValueTypeT> inline */
/*  std::ostream &operator <<(      std::ostream       &outStream, */
/*                            const Color4<ValueTypeT> &color); */

/*  #if !defined(OSG_DO_DOC) || defined(OSG_DOC_FIELD_TYPEDEFS)  */

/*  typedef SField<SimpleSphericalHarmonics> SFSimpleSphericalHarmonics; */
/*  //typedef MField<SimpleSphericalHarmonics> MFSimpleSphericalHarmonics; */
/*  #endif */


/*  #ifndef OSG_COMPILEFIELDINST */
/*  OSG_DLLEXPORT_DECL1(SField, SimpleSphericalHarmonics, OSG_BASE_DLLTMPLMAPPING) */
/*  #endif */


OSG_END_NAMESPACE

#include <OSGSimpleSphericalHarmonics.inl>

#define OSG_SIMPLESPHERICALHARMONICS_HEADER_CVSID "@(#)$Id: OSGSimpleSphericalHarmonics.h,v 1.1 2003/11/03 19:13:00 jnkautz Exp $"

#endif /* _OSG_SIMPLE_SPHERICAL_HARMONICS_H_ */


