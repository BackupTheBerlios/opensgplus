#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
#ifndef _TINYMATRIX_H_
#define _TINYMATRIX_H_

#include "OSGWMcommontypes.h"
#include "OSGWMtinyvector.h"

/*
  Re-Implementation of MAM/VRS class Matrix.
  Mimics interface but no source code of implementation used.

  NEED TO REPLACE MAM/VRS MATRIX INVERSION ROUTINE ! 

  Functions relate to 4x4 matrices in general.
  
  PARTIAL SUPPORT FOR NXM MATRICES:
  
  - MATRIX*MATRIX, MATRIX*VECTOR, transpose(), identity(), zero()  support nxm matrices,
    inverse() supports nxn matrices.
  - There is no resizing of matrices. Assigning a matrix nxm a smaller one n'xm' with
    n'<=n,m'<=m is ok.
*/
#define ROT_X       0
#define ROT_Y       1
#define ROT_Z       2

#define SHEAR_YZ    0
#define SHEAR_XZ    1
#define SHEAR_XY    2

class   TinyMatrixNM
{
    friend class    TinyMatrix;                                 /* for assignment TinyMatrix:=TinyMatrixNM */

    int             norig_, morig_;                             /* matrix size when allocated */
    int             n_, m_;                                     /* current matrix size */
    double          **ma_;                                      /* first is line, second is row */
public:

    TinyMatrixNM(int n = 4, int m = 4, double m00 = 1, double m01 = 0,
                 double m02 = 0, double m03 = 0, double m10 = 0, double m11 = 1,
                 double m12 = 0, double m13 = 0, double m20 = 0, double m21 = 0,
                 double m22 = 1, double m23 = 0, double m30 = 0, double m31 = 0,
                 double m32 = 0, double m33 = 1)
    {
        int i, j;

        norig_ = n_ = n;
        morig_ = m_ = m;
        ma_ = (double **) malloc(sizeof(double *) * n_);        /* allocate mem for pointers
                                                to n_ lines */
        for(i = 0; i < n_; i++)
        {
            ma_[i] = (double *) malloc(sizeof(double) * m_);
        }

        if((n == 4) && (m == 4))
        {
            ma_[0][0] = m00;
            ma_[0][1] = m01;
            ma_[0][2] = m02;
            ma_[0][3] = m03;
            ma_[1][0] = m10;
            ma_[1][1] = m11;
            ma_[1][2] = m12;
            ma_[1][3] = m13;
            ma_[2][0] = m20;
            ma_[2][1] = m21;
            ma_[2][2] = m22;
            ma_[2][3] = m23;
            ma_[3][0] = m30;
            ma_[3][1] = m31;
            ma_[3][2] = m32;
            ma_[3][3] = m33;
        }
        else
        {
            for(i = 0; i < n_; i++)
                for(j = 0; j < m_; j++)
                    ma_[i][j] = 0;
        }
    }

    TinyMatrixNM(const TinyMatrixNM &m2)
    {
        int i, j;

        norig_ = n_ = m2.n_;
        morig_ = m_ = m2.m_;

        ma_ = (double **) malloc(sizeof(double *) * n_);        /* allocate mem for pointers
                                                to n_ lines */
        for(i = 0; i < n_; i++)
        {
            ma_[i] = (double *) malloc(sizeof(double) * m_);
            for(j = 0; j < m_; j++)
                ma_[i][j] = m2.ma_[i][j];
        }
    }

    ~TinyMatrixNM(void)
    {
        int i;

        for(i = 0; i < norig_; i++)
        {
            GEOMARK_FREE(ma_[i]);
            ma_[i] = NULL;
        }
        GEOMARK_FREE(ma_);
        ma_ = NULL;
    }
    double *operator        [] (int);                           /* yields i-th line */
    int                     setIthLine(int i, TinyVector &v);   /* sets i-th line */
    int                     setIthLine(int i, TinyVectorN &v);  /* sets i-th line */

    TinyMatrixNM &operator  =(const TinyMatrixNM &);

    void                    identity(void);
    void                    zero(void);

    void                    shear(double sh1, double sh2, int axis);    /* 0=YZ shear, 1=XZ shear,
                                                 2=XY shear */
    void                    trans(double dx, double dy, double dz);
    void                    scale(double sx, double sy, double sz);
    void                    rot(double angle, int axis);                /* counter-clockwise rotation,
                                     0=x-axis,1=y,2=z,
                                     angle in degrees */
    TinyMatrixNM            transpose(void);
    int                     inverse(TinyMatrixNM &result) const;        /* invert matrix and store result in 'result' */
    TinyMatrixNM operator *(TinyMatrixNM &) const;
    TinyMatrixNM operator *(const double &) const;

    TinyVectorN operator *(const TinyVectorN &) const;
    TinyVector operator *(const TinyVector &) const;

    int getNumLines(void)
    {
        return n_;
    };
    int getNumColumns(void)
    {
        return m_;
    };

    int isQuadratic(void)
    {
        return n_ == m_;
    }                           /* is matrix of nxn type */

    int     isSymmetric(void);  /* is matrix symmetric ? */
    int     isSymmPositiveDefinite(void);                               /* is symmetric 3x3 (sub-)matrix positive definite ? */

    int     det2(double *determinant);
    int     det3(double *determinant);

    int     frobeniusNorm(double *normValue);

    void    dump(void) const;
};

class   TinyMatrix
{
    int     norig_, morig_;                                             /* matrix size when allocated */
    int     n_, m_;                                                     /* current matrix size */
    double  ma_[4][4];                                                  /* first is line, second is column */
public:

    TinyMatrix(int n = 4, int m = 4, double m00 = 1, double m01 = 0,
               double m02 = 0, double m03 = 0, double m10 = 0, double m11 = 1,
               double m12 = 0, double m13 = 0, double m20 = 0, double m21 = 0,
               double m22 = 1, double m23 = 0, double m30 = 0, double m31 = 0,
               double m32 = 0, double m33 = 1)
    {
        int i, j;

        norig_ = n_ = n;
        morig_ = m_ = m;

        ma_[0][0] = m00;
        ma_[0][1] = m01;
        ma_[0][2] = m02;
        ma_[0][3] = m03;
        ma_[1][0] = m10;
        ma_[1][1] = m11;
        ma_[1][2] = m12;
        ma_[1][3] = m13;
        ma_[2][0] = m20;
        ma_[2][1] = m21;
        ma_[2][2] = m22;
        ma_[2][3] = m23;
        ma_[3][0] = m30;
        ma_[3][1] = m31;
        ma_[3][2] = m32;
        ma_[3][3] = m33;
    }

    TinyMatrix(const TinyMatrix &m2)
    {
        int i, j;

        norig_ = n_ = m2.n_;
        morig_ = m_ = m2.m_;

        for(i = 0; i < n_; i++)
        {
            for(j = 0; j < m_; j++)
                ma_[i][j] = m2.ma_[i][j];
        }
    }

    ~TinyMatrix(void)
    { }
    double *operator        [] (int);                                   /* yields i-th line */
    int                     setIthLine(int i, TinyVector &v);           /* sets i-th line */
    int                     setIthLine(int i, TinyVectorN &v);          /* sets i-th line */

    TinyMatrix &operator    =(const TinyMatrix &);
    TinyMatrix &operator    =(const TinyMatrixNM &);

    void                    identity(void);
    void                    zero(void);
    void                    zeroWithOne(void);                          /* element (n_,m_) is 1 rest is zero */

    void                    shear(double sh1, double sh2, int axis);    /* 0=YZ shear, 1=XZ shear,
                                                 2=XY shear */
    void                    trans(double dx, double dy, double dz);
    void                    scale(double sx, double sy, double sz);
    void                    rot(double angle, int axis);                /* counter-clockwise rotation,
                                     0=x-axis,1=y,2=z,
                                     angle in degrees */
    TinyMatrix              transpose(void);
    int                     inverse(TinyMatrix &result) const;          /* invert matrix and store result in 'result' */
    TinyMatrix operator *(TinyMatrix &) const;
    TinyMatrix operator *(const double &) const;

    TinyVectorN operator *(const TinyVectorN &) const;
    TinyVector operator *(const TinyVector &) const;

    int getNumLines(void)
    {
        return n_;
    };
    int getNumColumns(void)
    {
        return m_;
    };

    int isQuadratic(void)
    {
        return n_ == m_;
    }                           /* is matrix of nxn type */

    int     isSymmetric(void);  /* is matrix symmetric ? */
    int     isSymmPositiveDefinite(void);   /* is symmetric 3x3 (sub-)matrix positive definite ? */

    int     det2(double *determinant);
    int     det3(double *determinant);
    int     det4(double *determinant);

    int     frobeniusNorm(double *normValue);

    int     setSize(int rows, int columns);

    void    dump(void) const;
};
#endif
#endif
