#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
#ifndef _TINYVECTOR_H_
#define _TINYVECTOR_H_

#ifdef WIN32

//#include <fstream.h>
#include <fstream>
#include <iostream>
#else
//#include <stream.h>
#endif
#include <stdlib.h>
#include <stdio.h>

using namespace std;                                        /* does not work, used std::ofstream instead */

#include "OSGWMcommontypes.h"

double          safe_acos(double v);                        /* ensures acos() is calculated for values >= -1 and <= 1 */
double          safe_asin(double v);

/*
  Re-Implementation of MAM/VRS class MVector.
  Mimics interface but no source code of implementation used.

  TinyVector:	4-element vectors (calculations ignore anything above 3rd element)
  TinyVectorN:	N-element vectors (N>=2, calculations ignore anything above ")

  => only access-method [] and matrix multiplication
     "takes notice of" 4th and higher elements  

  PLEASE NOTICE:
  
  In case of TinyVectorN class, the space for holding the
  elements is dynamically allocated, so TinyVectorN objects
  CANNOT be stored in object containers defined in array.cpp !
*/
/* to get the fastest abs() possible */
#define TINYVECTOR_FAST_ABS(V) \
        (V.v_[0] * V.v_[0] + V.v_[1] * V.v_[1] + V.v_[2] * V.v_[2])

class   TinyVectorN
{
public:

    int     n_;                                             /* public because of performance reasons */
    double  *v_;

    /* " */
    TinyVectorN(int n = 4, double v0 = 0., double v1 = 0., double v2 = 0.,
                double v3 = 1.)
    {
        //printf("*n=%d\n",n);
        n_ = n;

        v_ = (double *) malloc(sizeof(double) * n_);

        //printf("ADR=%d\n",v_);
        v_[0] = v0;
        v_[1] = v1;
        if(n > 2)
            v_[2] = v2;
        if(n > 3)
            v_[3] = v3;
    }

    TinyVectorN(const TinyVectorN &v)
    {
        n_ = v.n_;
        v_ = (double *) malloc(sizeof(double) * n_);
        for(int i = 0; i < n_; i++)
            v_[i] = v.v_[i];
    }

    ~TinyVectorN(void)
    {
        //printf("Destructor !\n");
        GEOMARK_FREE(v_);
    }
    void zero(void)
    {
        for(int i = 0; i < n_; i++)
            v_[i] = 0.;
    }

    void zeroWithOne(void)
    {
        for(int i = 0; i < n_ - 1; i++)
            v_[i] = 0.;
        v_[n_ - 1] = 1.;
    }

    double &operator    [] (int index);
    double operator             [] (int index) const;
    const TinyVectorN &operator =(const TinyVectorN &);
    void                        dump(void) const;
    TinyVectorN                 normalized(void) const;
    void                        normalize(void);
    TinyVectorN operator *(const TinyVectorN &v2) const;    /* cross product */
    TinyVectorN operator+(const TinyVectorN &v2) const;
    TinyVectorN operator-(const TinyVectorN &v2) const;

    TinyVectorN operator *(double f) const;
    TinyVectorN operator/(double f) const;

    int size(void)
    {
        return n_;
    }

    friend double   tv_abs(const TinyVectorN &);
    friend double   fast_abs(const TinyVectorN &);
    friend double   dotProduct(const TinyVectorN &, const TinyVectorN &);
    //friend std::ostream & operator <<(std::ostream &, const TinyVectorN &);
    //friend std::istream & operator >>(std::istream &, TinyVectorN &);
};

class   TinyVector
{
    //private:
public:

    double  v_[4];

    /* public because of performance reasons */
    TinyVector(double v0 = 0., double v1 = 0., double v2 = 0., double v3 = 1.)
    {
        v_[0] = v0;
        v_[1] = v1;
        v_[2] = v2;
        v_[3] = v3;
    }

    TinyVector(const TinyVector &v)
    {
        for(int i = 0; i < 4; i++)
            v_[i] = v.v_[i];
    }

    ~TinyVector(void)
    { }
    void zero(void)
    {
        v_[0] = 0.;
        v_[1] = 0.;
        v_[2] = 0.;
        v_[3] = 0.;
    }

    void zeroWithOne(void)
    {
        v_[0] = 0.;
        v_[1] = 0.;
        v_[2] = 0.;
        v_[3] = 1.;
    }

    double &operator    [] (int index);
    double operator             [] (int index) const;
    const TinyVector &operator  =(const TinyVector &);
    void                        dump(void);
    TinyVector                  normalized(void) const;
    void                        normalize(void);

    /* returns (arbitrary) normal perpendicular to 
    actual vector (normalized) */
    TinyVector                  perpendicularNormal(void);

    friend void                 initTinyVectorArray(TinyVector *va,
                                                    int numElements);

    BOOL isZero(void)
    {
        return (v_[0] == 0) && (v_[1] == 0) && (v_[2] == 0);
    }

    double  angle(TinyVector &v2);                          /* output in radians */
    double  fast_angle(TinyVector &v2);                     /* just returns cos value */

    void    randomize(BOOL allPositive = FALSE);            /* random vector of length 1, 
										  if allPositive==TRUE all
					                      coordinates >= 0 */

    TinyVector operator *(const TinyVector &v2) const;      /* cross product */
    TinyVector operator+(const TinyVector &v2) const;
    TinyVector operator-(const TinyVector &v2) const;

    TinyVector operator *(double f) const;
    TinyVector operator/(double f) const;

    friend double tv_abs(const TinyVector &);

    /* next one does not calc the square root */
    friend double fast_abs(const TinyVector &);
    friend double dotProduct(const TinyVector &, const TinyVector &);
    //friend std::ostream & operator <<(std::ostream &, const TinyVector &);
    //friend std::istream & operator >>(std::istream &, TinyVector &);
};
#endif
#endif
