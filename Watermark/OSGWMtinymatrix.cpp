#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
#include <stdio.h>
#include <math.h>                               /* for PI */
#include <string.h>                             /* for memmove() */
#include <assert.h>                             /* for assert() */

#include "OSGWMtinymatrix.h"
#include "OSGWMutil.h"

static const double pi = PI;                    //3.14159265358979323846
#define SWAP(a, b, t)   (t = a, a = b, b = t)
#define ABS(x)          (x >= 0 ? x : -(x))
#define MIN2(x, y)      ((x < y) ? x : y)

static TinyMatrix   id(4, 4, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
                       1.0, 0.0, 0.0, 0.0, 0.0, 1.0);

static TinyMatrix   zero(4, 4, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                         0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

static TinyMatrixNM idNM(4, 4, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0,
                         0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);

static TinyMatrixNM zeroNM(4, 4, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                           0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

/* */

void TinyMatrixNM::identity(void)
{
    int i, j, n;

    n = MIN2(n_, m_);                           /* if not nxn set quadratic submatrix */

    for(i = 0; i < n_; i++)
    {
        for(j = 0; j < n_; j++)
        {
            if(i == j)
                ma_[i][j] = 1.;
            else
                ma_[i][j] = 0.;
        }
    }
}

/* */
void TinyMatrixNM::zero(void)
{
    int i, j;

    for(i = 0; i < n_; i++)
        for(j = 0; j < m_; j++)
            ma_[i][j] = 0.;
}

double *TinyMatrixNM::operator  [] (int line)
{
    // perform bounds checking here
    return &ma_[line][0];
}

/* no enlarging of matrix !! */
TinyMatrixNM &TinyMatrixNM::operator=(const TinyMatrixNM &m2)
{
    int n, m, i, j;

    n = MIN2(norig_, m2.n_);
    m = MIN2(morig_, m2.m_);

    for(i = 0; i < n; i++)
    {
        for(j = 0; j < m; j++)
            ma_[i][j] = m2.ma_[i][j];
    }

    n_ = m2.n_;
    m_ = m2.m_;

    return *this;
}

/* */
int TinyMatrixNM::setIthLine(int i, TinyVector &v)
{
    assert((i >= 0) && (i < n_));

    int ne = MIN2(m_, 3);

    for(int j = 0; j < ne; j++)
        ma_[i][j] = v[j];

    return 0;
}

/* */
int TinyMatrixNM::setIthLine(int i, TinyVectorN &v)
{
    assert((i >= 0) && (i < n_));

    int ne = MIN2(m_, 3);

    for(int j = 0; j < ne; j++)
        ma_[i][j] = v[j];

    return 0;
}

/* */
TinyVector TinyMatrixNM::operator*(const TinyVector &v) const
{
    int     i, j, n;
    double  product;

    /* m (matrix) and n (vector) must match ! */
    assert(m_ == 4);

    //n=MIN2(m_,3);
    n = m_;

    TinyVector  result;

    for(i = 0; i < n; i++)
    {
        product = 0.;
        for(j = 0; j < m_; j++)
        {
            product += ma_[i][j] * v[j];
        }

        result[i] = product;
    }

    return TinyVector(result);
}

/* */
TinyVectorN TinyMatrixNM::operator*(const TinyVectorN &v) const
{
    int     i, j;
    double  product;

    /* m (matrix) and n (vector) must match ! */
    assert(m_ == v.n_);

    TinyVectorN result(m_);

    for(i = 0; i < n_; i++)
    {
        product = 0.;
        for(j = 0; j < m_; j++)
        {
            product += ma_[i][j] * v[j];
        }

        result[i] = product;
    }

    return TinyVectorN(result);
}

/* */
TinyMatrixNM TinyMatrixNM::operator*(TinyMatrixNM &m2) const
{
    int     i, j, k;
    double  product;

    /* m (matrix) and n (matrix2) must match ! */
    assert(m_ == m2.n_);

    TinyMatrixNM    result(n_, m2.m_);

    for(i = 0; i < n_; i++)
    {
        for(j = 0; j < m2.m_; j++)
        {
            product = 0.;
            for(k = 0; k < m2.n_; k++)
            {
                product += ma_[i][k] * m2.ma_[k][j];
            }

            result.ma_[i][j] = product;
        }
    }

    return TinyMatrixNM(result);
}

/* */
TinyMatrixNM TinyMatrixNM::operator*(const double &value) const
{
    int             i, j;

    TinyMatrixNM    result(n_, m_);

    for(i = 0; i < n_; i++)
    {
        for(j = 0; j < m_; j++)
        {
            result.ma_[i][j] = value * ma_[i][j];
        }
    }

    return TinyMatrixNM(result);
}

/* */
void TinyMatrixNM::shear(double sh1, double sh2, int axis)
{
    this->identity();

    if(axis == 0)
    {                                           /* YZ shear */
        ma_[1][0] = sh1;
        ma_[2][0] = sh2;
    }
    else if(axis == 1)
    {                                           /* XZ shear */
        ma_[0][1] = sh1;
        ma_[2][1] = sh2;
    }
    else
    {                                           /* XY shear */
        ma_[0][2] = sh1;
        ma_[1][2] = sh2;
    }
}

/* */
void TinyMatrixNM::trans(double dx, double dy, double dz)
{
    this->identity();

    ma_[0][3] = dx;
    ma_[1][3] = dy;
    ma_[2][3] = dz;
}

/* */
void TinyMatrixNM::scale(double sx, double sy, double sz)
{
    this->identity();

    ma_[0][0] = sx;
    ma_[1][1] = sy;
    ma_[2][2] = sz;
}

/* */
void TinyMatrixNM::rot(double angle, int axis)
{
    double  rad;
    double  cosa, sina;

    rad = RADIANS(angle);                       //(angle / 180.)*pi;
    cosa = cos(rad);
    sina = sin(rad);

    this->identity();

    if(axis == 0)
    {
        ma_[1][1] = cosa;
        ma_[1][2] = -sina;
        ma_[2][1] = sina;
        ma_[2][2] = cosa;
    }
    else if(axis == 1)
    {
        ma_[0][0] = cosa;
        ma_[0][2] = sina;
        ma_[2][0] = -sina;
        ma_[2][2] = cosa;
    }
    else if(axis == 2)
    {
        ma_[0][0] = cosa;
        ma_[0][1] = -sina;
        ma_[1][0] = sina;
        ma_[1][1] = cosa;
    }
}

/* */
TinyMatrixNM TinyMatrixNM::transpose(void)
{
    TinyMatrixNM    result(m_, n_);
    int             i, j;

    for(i = 0; i < n_; i++)
    {
        for(j = 0; j < m_; j++)
            result.ma_[j][i] = ma_[i][j];
    }

    return TinyMatrixNM(result);
}

/* */
void TinyMatrixNM::dump(void) const
{
    int i, j;

    printf("%d,%d -------------------------\n", n_, m_);
    for(i = 0; i < n_; i++)
    {
        for(j = 0; j < m_; j++)
            printf("%f ", ma_[i][j]);
        printf("\n");
    }
}

#if 0

/* "CONVERSION" NEEDS TO BE COMPLETED */
/* original LineAlg matrix inversion */
/* most general (Gauss Jordan) matrix inverse */
/* matrix to be inverted must be:
   - square
   - non-singular
   - "well-conditioned"
*/
#define REAL    double

/* */

int TinyMatrixNM::inverse(TinyMatrixNM &result) const
{
    //is_valid();
    if(n_ != m_)
        return -1;

    int             nrows = ncols = n_;

    double          determinant = 1;
    const double    singularity_tolerance = 1e-35;

    // Locations of pivots (indices start with 0)
    struct Pivot
    {
        int row, col;
    }

    *const pivots = (Pivot *) alloca(ncols * sizeof(Pivot));

    bool *const was_pivoted = (bool *) alloca(nrows * sizeof(bool));
    memset(was_pivoted, false, nrows * sizeof(bool));

    for(register Pivot * pivotp = &pivots[0]; pivotp < &pivots[ncols]; pivotp++)
    {
        const REAL  *old_ppos = 0;              // Location of a pivot to be
        int         prow = 0, pcol = 0;         // Pivot's row and column indices
        {                                       // Look through all non-pivoted cols
            REAL                max_value = 0;  // (and rows) for a pivot (max elem)
            register const REAL *cp = elements; // column pointer
            for(register int j = 0; j < ncols; j++)
            {
                if(!was_pivoted[j])
                {                               // the following loop would increment
                    REAL    curr_value = 0;     // cp by nrows
                    for(register int k = 0; k < nrows; k++, cp++)
                    {
                        if(!was_pivoted[k] &&
                           (curr_value = abs(*cp)) > max_value)
                        {
                            max_value = curr_value, prow = k, pcol = j, old_ppos = cp;
                        }
                    }
                }
                else
                    cp += nrows;                // and this branch would too
            }

            if(max_value < singularity_tolerance)
            {
                if(!is_dummy_det_ref(determ_ref))
                {
                    determ_ref = 0;
                    return *this;
                }
                else
                {
                    _error("Matrix turns out to be singular: can't invert");
                }
            }

            pivotp->row = prow;
            pivotp->col = pcol;
        }

        REAL *const old_colp = const_cast < REAL * > (old_ppos) - prow;
        REAL *const new_colp =
            (
                prow == pcol ? old_colp : elements +
                prow *
                nrows
            );
        if(prow != pcol)                        // Swap prow-th and pcol-th columns to
        {   // bring the pivot to the diagonal
            register REAL   *cr = new_colp;
            register REAL   *cc = old_colp;
            for(register int k = 0; k < nrows; k++)
            {
                REAL    temp = *cr;
                *cr++ = *cc;
                *cc++ = temp;
            }
        }

        was_pivoted[prow] = true;
        {   // Normalize the pivot column and
            register REAL   *pivot_cp = new_colp;
            const double    pivot_val = pivot_cp[prow]; // pivot is at the diagonal
            determinant *= pivot_val;                   // correct the determinant
            pivot_cp[prow] = 1;
            for(register int k = 0; k < nrows; k++)
                *pivot_cp++ /= pivot_val;
        }
        {   // Perform eliminations
            register REAL   *pivot_rp = elements + prow;        // pivot row
            register REAL   *ep = elements;                     // sweeps all matrix' columns
            for(register int k = 0; k < ncols; k++, pivot_rp += nrows)
            {
                if(k != prow)
                {
                    const double    temp = *pivot_rp;
                    *pivot_rp = 0;

                    register const REAL *pivot_cp = new_colp;   // pivot column
                    for(register int l = 0; l < nrows; l++)
                        *ep++ -= temp **pivot_cp++;
                }
                else
                    ep += nrows;
            }
        }
    }

    int no_swaps = 0;                           // Swap exchanged *rows* back in place
    for(register const Pivot * pvp = &pivots[ncols - 1]; pvp >= &pivots[0];
        pvp--)
    {
        if(pvp->row != pvp->col)
        {
            no_swaps++;

            register REAL   *rp = elements + pvp->row;
            register REAL   *cp = elements + pvp->col;
            for(register int k = 0; k < ncols; k++, rp += nrows, cp += nrows)
            {
                const REAL  temp = *rp;
                *rp = *cp;
                *cp = temp;
            }
        }
    }

    if(!is_dummy_det_ref(determ_ref))
    {
        determ_ref = (no_swaps & 1 ? -determinant : determinant);
    }

    return *this;
}
#endif
#if 1

/* original MAM/VRS matrix inversion */

/* changed to calculate inverses of nxn matrices */
int TinyMatrixNM::inverse(TinyMatrixNM &result) const
{
    register int    i, j, k, n;
    double          temp;

    /* matrix must be of nxn type */
    assert(m_ == n_);
    n = n_;

    TinyMatrixNM    m4tmp(*this);
    TinyMatrixNM    inverse(n, n);

    for(i = 0; i < n; i++)
        inverse[i][i] = 1;

    for(i = 0; i < n; i++)
    {
        /* Look for row with largest pivot and swap rows */
        temp = 0;
        j = -1;
        for(k = i; k < n; k++)
        {
            if(ABS(m4tmp.ma_[k][i]) > temp)
            {
                temp = ABS(m4tmp.ma_[k][i]);
                j = k;
            }
        }

        if(j == -1)                             /* No replacing row -> no inverse */
        {
            return -1;
        }

        if(j != i)
        {
            for(k = 0; k < n; k++)
            {
                SWAP(m4tmp.ma_[i][k], m4tmp.ma_[j][k], temp);
                SWAP(inverse.ma_[i][k], inverse.ma_[j][k], temp);
            }
        }

        temp = m4tmp.ma_[i][i];
        for(k = 0; k < n; k++)
        {
            m4tmp.ma_[i][k] /= temp;
            inverse.ma_[i][k] /= temp;
        }

        for(j = 0; j < n; j++)
        {
            if(j != i)
            {
                temp = m4tmp.ma_[j][i];
                for(k = 0; k < n; k++)
                {
                    m4tmp.ma_[j][k] -= m4tmp.ma_[i][k] * temp;
                    inverse.ma_[j][k] -= inverse.ma_[i][k] * temp;
                }
            }
        }
    }

    result = inverse;

    return 0;
}
#endif

/* */
int TinyMatrixNM::isSymmetric(void)
{
    int symmetric = 1;

    if(!isQuadratic())
        return -1;

    int i, j;
    i = 1;
    while((i < n_) && (symmetric))
    {
        j = 0;
        while((j < i) && (symmetric))
        {
            if(ma_[i][j] != ma_[j][i])
                symmetric = 0;
            j++;
        }

        i++;
    }

    return symmetric;
}

/* */
int TinyMatrixNM::det2(double *determinant)
{
    assert(MIN2(m_, n_) >= 2);

    *determinant = ma_[0][0] * ma_[1][1] - ma_[1][0] * ma_[0][1];

    return 0;
}

/* */
int TinyMatrixNM::det3(double *determinant)
{
    assert(MIN2(m_, n_) >= 3);

    /* apply sarrus rule */
    double  hd, nd;

    hd = 0;
    nd = 0;

    hd += ma_[0][0] * ma_[1][1] * ma_[2][2];
    hd += ma_[1][0] * ma_[2][1] * ma_[0][2];
    hd += ma_[2][0] * ma_[0][1] * ma_[1][2];

    nd += ma_[0][2] * ma_[1][1] * ma_[2][0];
    nd += ma_[1][2] * ma_[2][1] * ma_[0][0];
    nd += ma_[2][2] * ma_[0][1] * ma_[1][0];

    *determinant = hd - nd;

    return 0;
}

/* */
int TinyMatrixNM::isSymmPositiveDefinite(void)
{
    /* check criteria of Hurwitz:

  symmetric nxn Matrix M=mij 1<=i,j<=n) is positive definit <=>
  
  det M=mij (1<=i,j<=k)> 0 for k=1,..,n
  */
    assert(MIN2(n_, m_) >= 3);

    double  determinant;

    if(ma_[0][0] <= 0)
        return 0;
    det2(&determinant);
    if(determinant <= 0)
        return 0;
    det3(&determinant);
    if(determinant <= 0)
        return 0;
    return 1;
}

/* */
int TinyMatrixNM::frobeniusNorm(double *normValue)
{
    int     i, j;
    double  dtmp, norm = 0;

    assert((n_ == m_) && (n_ >= 3));

    for(i = 0; i < n_; i++)
    {
        for(j = 0; j < n_; j++)
        {
            dtmp = fabs(ma_[i][j]);
            norm += (dtmp * dtmp);
        }
    }

    norm = sqrt(norm);

    *normValue = norm;

    return 0;
}

/* matrix implementation that uses static memory */
void TinyMatrix::identity(void)
{
    int i, j, n;

    n = MIN2(n_, m_);                           /* if not nxn set quadratic submatrix */

    for(i = 0; i < n_; i++)
    {
        for(j = 0; j < n_; j++)
        {
            if(i == j)
                ma_[i][j] = 1.;
            else
                ma_[i][j] = 0.;
        }
    }
}

/* */
void TinyMatrix::zero(void)
{
    int i, j;

    for(i = 0; i < n_; i++)
        for(j = 0; j < m_; j++)
            ma_[i][j] = 0.;
}

/* */
void TinyMatrix::zeroWithOne(void)
{
    int i, j;

    for(i = 0; i < n_; i++)
        for(j = 0; j < m_; j++)
            ma_[i][j] = 0.;
    ma_[n_ - 1][m_ - 1] = 1.;
}

double *TinyMatrix::operator    [] (int line)
{
    // perform bounds checking here
    return &ma_[line][0];
}

/* no enlarging of matrix !! */
TinyMatrix &TinyMatrix::operator=(const TinyMatrix &m2)
{
    int n, m, i, j;

    n = MIN2(norig_, m2.n_);
    m = MIN2(morig_, m2.m_);

    for(i = 0; i < n; i++)
    {
        for(j = 0; j < m; j++)
            ma_[i][j] = m2.ma_[i][j];
    }

    n_ = m2.n_;
    m_ = m2.m_;

    return *this;
}

/* */
TinyMatrix &TinyMatrix::operator=(const TinyMatrixNM &m2)
{
    int n, m, i, j;

    n = MIN2(norig_, m2.n_);
    m = MIN2(morig_, m2.m_);

    for(i = 0; i < n; i++)
    {
        for(j = 0; j < m; j++)
            ma_[i][j] = m2.ma_[i][j];
    }

    n_ = m2.n_;
    m_ = m2.m_;

    return *this;
}

/* */
int TinyMatrix::setIthLine(int i, TinyVector &v)
{
    assert((i >= 0) && (i < n_));

    int ne = MIN2(m_, 3);

    for(int j = 0; j < ne; j++)
        ma_[i][j] = v[j];

    return 0;
}

/* */
int TinyMatrix::setIthLine(int i, TinyVectorN &v)
{
    assert((i >= 0) && (i < n_));

    int ne = MIN2(m_, 3);

    for(int j = 0; j < ne; j++)
        ma_[i][j] = v[j];

    return 0;
}

/* */
TinyVector TinyMatrix::operator*(const TinyVector &v) const
{
    int     i, j, n;
    double  product;

    /* m (matrix) and n (vector) must match ! */
    //assert(m_==4);
    assert(m_ <= 4);

    //n=MIN2(m_,3);
    n = m_;

    TinyVector  result;

    for(i = 0; i < n; i++)
    {
        product = 0.;
        for(j = 0; j < m_; j++)
        {
            product += ma_[i][j] * v[j];
        }

        result[i] = product;
    }

    //return TinyVector(result);
    return result;
}

/* */
TinyVectorN TinyMatrix::operator*(const TinyVectorN &v) const
{
    int     i, j;
    double  product;

    /* m (matrix) and n (vector) must match ! */
    assert(m_ == v.n_);

    TinyVectorN result(m_);

    for(i = 0; i < n_; i++)
    {
        product = 0.;
        for(j = 0; j < m_; j++)
        {
            product += ma_[i][j] * v[j];
        }

        result[i] = product;
    }

    //return TinyVectorN(result);
    return result;
}

/* */
TinyMatrix TinyMatrix::operator*(TinyMatrix &m2) const
{
    int     i, j, k;
    double  product;

    /* m (matrix) and n (matrix2) must match ! */
    assert(m_ == m2.n_);

    TinyMatrix  result(n_, m2.m_);

    for(i = 0; i < n_; i++)
    {
        for(j = 0; j < m2.m_; j++)
        {
            product = 0.;
            for(k = 0; k < m2.n_; k++)
            {
                product += ma_[i][k] * m2.ma_[k][j];
            }

            result.ma_[i][j] = product;
        }
    }

    //return TinyMatrix(result);
    return result;
}

/* */
TinyMatrix TinyMatrix::operator*(const double &value) const
{
    int         i, j;

    TinyMatrix  result(n_, m_);

    for(i = 0; i < n_; i++)
    {
        for(j = 0; j < m_; j++)
        {
            result.ma_[i][j] = value * ma_[i][j];
        }
    }

    //return TinyMatrix(result);
    return result;
}

/* */
void TinyMatrix::shear(double sh1, double sh2, int axis)
{
    this->identity();

    if(axis == 0)
    {                                           /* YZ shear */
        ma_[1][0] = sh1;
        ma_[2][0] = sh2;
    }
    else if(axis == 1)
    {                                           /* XZ shear */
        ma_[0][1] = sh1;
        ma_[2][1] = sh2;
    }
    else
    {                                           /* XY shear */
        ma_[0][2] = sh1;
        ma_[1][2] = sh2;
    }
}

/* */
void TinyMatrix::trans(double dx, double dy, double dz)
{
    this->identity();

    ma_[0][3] = dx;
    ma_[1][3] = dy;
    ma_[2][3] = dz;
}

/* */
void TinyMatrix::scale(double sx, double sy, double sz)
{
    this->identity();

    ma_[0][0] = sx;
    ma_[1][1] = sy;
    ma_[2][2] = sz;
}

/* */
void TinyMatrix::rot(double angle, int axis)
{
    double  rad;
    double  cosa, sina;

    rad = RADIANS(angle);                       //(angle / 180.)*pi;
    cosa = cos(rad);
    sina = sin(rad);

    this->identity();

    if(axis == 0)                               /* X */
    {
        ma_[1][1] = cosa;
        ma_[1][2] = -sina;
        ma_[2][1] = sina;
        ma_[2][2] = cosa;
    }
    else if(axis == 1)                          /* Y */
    {
        ma_[0][0] = cosa;
        ma_[0][2] = sina;
        ma_[2][0] = -sina;
        ma_[2][2] = cosa;
    }
    else if(axis == 2)                          /* Z */
    {
        ma_[0][0] = cosa;
        ma_[0][1] = -sina;
        ma_[1][0] = sina;
        ma_[1][1] = cosa;
    }
}

/* */
TinyMatrix TinyMatrix::transpose(void)
{
    TinyMatrix  result(m_, n_);
    int         i, j;

    for(i = 0; i < n_; i++)
    {
        for(j = 0; j < m_; j++)
            result.ma_[j][i] = ma_[i][j];
    }

    return TinyMatrix(result);
}

/* */
void TinyMatrix::dump(void) const
{
    int i, j;

    printf("%d,%d -------------------------\n", n_, m_);
    for(i = 0; i < n_; i++)
    {
        for(j = 0; j < m_; j++)
            printf("%f ", ma_[i][j]);
        printf("\n");
    }
}

#if 0

/* "CONVERSION" NEEDS TO BE COMPLETED */
/* original LineAlg matrix inversion */
/* most general (Gauss Jordan) matrix inverse */
/* matrix to be inverted must be:
   - square
   - non-singular
   - "well-conditioned"
*/
#define REAL    double

/* */

int TinyMatrix::inverse(TinyMatrix &result) const
{
    //is_valid();
    if(n_ != m_)
        return -1;

    int             nrows = ncols = n_;

    double          determinant = 1;
    const double    singularity_tolerance = 1e-35;

    // Locations of pivots (indices start with 0)
    struct Pivot
    {
        int row, col;
    }

    *const pivots = (Pivot *) alloca(ncols * sizeof(Pivot));

    bool *const was_pivoted = (bool *) alloca(nrows * sizeof(bool));
    memset(was_pivoted, false, nrows * sizeof(bool));

    for(register Pivot * pivotp = &pivots[0]; pivotp < &pivots[ncols]; pivotp++)
    {
        const REAL  *old_ppos = 0;              // Location of a pivot to be
        int         prow = 0, pcol = 0;         // Pivot's row and column indices
        {                                       // Look through all non-pivoted cols
            REAL                max_value = 0;  // (and rows) for a pivot (max elem)
            register const REAL *cp = elements; // column pointer
            for(register int j = 0; j < ncols; j++)
            {
                if(!was_pivoted[j])
                {                               // the following loop would increment
                    REAL    curr_value = 0;     // cp by nrows
                    for(register int k = 0; k < nrows; k++, cp++)
                    {
                        if(!was_pivoted[k] &&
                           (curr_value = abs(*cp)) > max_value)
                        {
                            max_value = curr_value, prow = k, pcol = j, old_ppos = cp;
                        }
                    }
                }
                else
                    cp += nrows;                // and this branch would too
            }

            if(max_value < singularity_tolerance)
            {
                if(!is_dummy_det_ref(determ_ref))
                {
                    determ_ref = 0;
                    return *this;
                }
                else
                {
                    _error("Matrix turns out to be singular: can't invert");
                }
            }

            pivotp->row = prow;
            pivotp->col = pcol;
        }

        REAL *const old_colp = const_cast < REAL * > (old_ppos) - prow;
        REAL *const new_colp =
            (
                prow == pcol ? old_colp : elements +
                prow *
                nrows
            );
        if(prow != pcol)                        // Swap prow-th and pcol-th columns to
        {   // bring the pivot to the diagonal
            register REAL   *cr = new_colp;
            register REAL   *cc = old_colp;
            for(register int k = 0; k < nrows; k++)
            {
                REAL    temp = *cr;
                *cr++ = *cc;
                *cc++ = temp;
            }
        }

        was_pivoted[prow] = true;
        {   // Normalize the pivot column and
            register REAL   *pivot_cp = new_colp;
            const double    pivot_val = pivot_cp[prow]; // pivot is at the diagonal
            determinant *= pivot_val;                   // correct the determinant
            pivot_cp[prow] = 1;
            for(register int k = 0; k < nrows; k++)
                *pivot_cp++ /= pivot_val;
        }
        {   // Perform eliminations
            register REAL   *pivot_rp = elements + prow;        // pivot row
            register REAL   *ep = elements;                     // sweeps all matrix' columns
            for(register int k = 0; k < ncols; k++, pivot_rp += nrows)
            {
                if(k != prow)
                {
                    const double    temp = *pivot_rp;
                    *pivot_rp = 0;

                    register const REAL *pivot_cp = new_colp;   // pivot column
                    for(register int l = 0; l < nrows; l++)
                        *ep++ -= temp **pivot_cp++;
                }
                else
                    ep += nrows;
            }
        }
    }

    int no_swaps = 0;   // Swap exchanged *rows* back in place
    for(register const Pivot * pvp = &pivots[ncols - 1]; pvp >= &pivots[0];
        pvp--)
    {
        if(pvp->row != pvp->col)
        {
            no_swaps++;

            register REAL   *rp = elements + pvp->row;
            register REAL   *cp = elements + pvp->col;
            for(register int k = 0; k < ncols; k++, rp += nrows, cp += nrows)
            {
                const REAL  temp = *rp;
                *rp = *cp;
                *cp = temp;
            }
        }
    }

    if(!is_dummy_det_ref(determ_ref))
    {
        determ_ref = (no_swaps & 1 ? -determinant : determinant);
    }

    return *this;
}
#endif
#if 1

/* original MAM/VRS matrix inversion */

/* changed to calculate inverses of nxn matrices */
int TinyMatrix::inverse(TinyMatrix &result) const
{
    register int    i, j, k, n;
    double          temp;

    /* matrix must be of nxn type */
    assert(m_ == n_);
    n = n_;

    TinyMatrix  m4tmp(*this);
    TinyMatrix  inverse(n, n);

    for(i = 0; i < n; i++)
        inverse[i][i] = 1;

    for(i = 0; i < n; i++)
    {
        /* Look for row with largest pivot and swap rows */
        temp = 0;
        j = -1;
        for(k = i; k < n; k++)
        {
            if(ABS(m4tmp.ma_[k][i]) > temp)
            {
                temp = ABS(m4tmp.ma_[k][i]);
                j = k;
            }
        }

        if(j == -1)     /* No replacing row -> no inverse */
        {
            return -1;
        }

        if(j != i)
        {
            for(k = 0; k < n; k++)
            {
                SWAP(m4tmp.ma_[i][k], m4tmp.ma_[j][k], temp);
                SWAP(inverse.ma_[i][k], inverse.ma_[j][k], temp);
            }
        }

        temp = m4tmp.ma_[i][i];
        for(k = 0; k < n; k++)
        {
            m4tmp.ma_[i][k] /= temp;
            inverse.ma_[i][k] /= temp;
        }

        for(j = 0; j < n; j++)
        {
            if(j != i)
            {
                temp = m4tmp.ma_[j][i];
                for(k = 0; k < n; k++)
                {
                    m4tmp.ma_[j][k] -= m4tmp.ma_[i][k] * temp;
                    inverse.ma_[j][k] -= inverse.ma_[i][k] * temp;
                }
            }
        }
    }

    result = inverse;

    return 0;
}
#endif

/* */
int TinyMatrix::isSymmetric(void)
{
    int symmetric = 1;

    if(!isQuadratic())
        return -1;

    int i, j;
    i = 1;
    while((i < n_) && (symmetric))
    {
        j = 0;
        while((j < i) && (symmetric))
        {
            if(ma_[i][j] != ma_[j][i])
                symmetric = 0;
            j++;
        }

        i++;
    }

    return symmetric;
}

/* */
int TinyMatrix::det2(double *determinant)
{
    assert(MIN2(m_, n_) >= 2);

    *determinant = ma_[0][0] * ma_[1][1] - ma_[1][0] * ma_[0][1];

    return 0;
}

/* */
int TinyMatrix::det3(double *determinant)
{
    assert(MIN2(m_, n_) >= 3);

    /* apply sarrus rule */
    double  hd, nd;

    hd = 0;
    nd = 0;

    hd += ma_[0][0] * ma_[1][1] * ma_[2][2];
    hd += ma_[1][0] * ma_[2][1] * ma_[0][2];
    hd += ma_[2][0] * ma_[0][1] * ma_[1][2];

    nd += ma_[0][2] * ma_[1][1] * ma_[2][0];
    nd += ma_[1][2] * ma_[2][1] * ma_[0][0];
    nd += ma_[2][2] * ma_[0][1] * ma_[1][0];

    *determinant = hd - nd;

    return 0;
}

/* */
int TinyMatrix::det4(double *determinant)
{
    //assert(MIN2(m_,n_) >=3);
    TinyMatrix  matrix;
    int         i, j;
    BOOL        sign;
    int         line;
    double      detTmp, detVal;

    /* i,j e [1..4] 

  	 we calculate cofactor for last column:

     cofactor aij=(-1)^(i+j) (minor of aij)

     minor aij=det (Mij), Mij is matrix with i-th line and j-th
	                      column removed	
   */
    sign = TRUE;
    detVal = 0;
    for(i = 0; i < 4; i++)
    {
        /* remove (i+1)-th line, 4-th column */
        line = 0;
        for(j = 0; j < 4; j++)
        {
            if(j != i)
            {
                matrix[line][0] = ma_[j][0];
                matrix[line][1] = ma_[j][1];
                matrix[line][2] = ma_[j][2];
                line++;
            }
        }

        matrix.det3(&detTmp);
        if(sign)
            detVal -= detTmp;
        else
            detVal += detTmp;

        if(sign)
            sign = FALSE;
        else
            sign = TRUE;
    }

    *determinant = detVal;

    return 0;
}

/* */
int TinyMatrix::isSymmPositiveDefinite(void)
{
    /* check criteria of Hurwitz:

  symmetric nxn Matrix M=mij 1<=i,j<=n) is positive definit <=>
  
  det M=mij (1<=i,j<=k)> 0 for k=1,..,n
  */
    assert(MIN2(n_, m_) >= 3);

    double  determinant;

    if(ma_[0][0] <= 0)
        return 0;
    det2(&determinant);
    if(determinant <= 0)
        return 0;
    det3(&determinant);
    if(determinant <= 0)
        return 0;
    return 1;
}

/* */
int TinyMatrix::frobeniusNorm(double *normValue)
{
    int     i, j;
    double  dtmp, norm = 0;

    assert((n_ == m_) && (n_ >= 3));

    for(i = 0; i < n_; i++)
    {
        for(j = 0; j < n_; j++)
        {
            dtmp = fabs(ma_[i][j]);
            norm += (dtmp * dtmp);
        }
    }

    norm = sqrt(norm);

    *normValue = norm;

    return 0;
}

/* */
int TinyMatrix::setSize(int rows, int columns)
{
    n_ = rows;
    m_ = columns;
    return 0;
}
#endif
