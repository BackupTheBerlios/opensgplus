#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//#include <assert.h>
#include "OSGWMmylimits.h"
#include "OSGWMtinyvector.h"
#include "OSGWMutil.h"   /* for safe_acos() */

static const double ZERO_TOL = 1e-08;

//1e-06;

//#define MIN2(x,y) ((x<y) ? x : y)
double safe_acos(double v)
{
    if(v > 1.)
        v = 1.;
    else if(v < -1.)
        v = -1.;
    return acos(v);
}

/* */
double safe_asin(double v)
{
    if(v > 1.)
        v = 1.;
    else if(v < -1.)
        v = -1.;
    return asin(v);
}

/******************** TinyVectorN ********************/
double &TinyVectorN::operator   [] (int index)
{
    return v_[index];
}
double TinyVectorN::operator    [] (int index) const
{
    return v_[index];
}

/* */
const TinyVectorN &TinyVectorN::operator=(const TinyVectorN &v2)
{
    int i, n;

    n = MIN2(n_, v2.n_);

    for(i = 0; i < n; i++)
    {
        v_[i] = v2.v_[i];
    }

    return *this;
}

/* */
void TinyVectorN::dump(void) const
{
    int i;

    printf("(");
    for(i = 0; i < n_; i++)
        printf("%f ", v_[i]);
    printf(")\n");
}

/* return normalized copy */
TinyVectorN TinyVectorN::normalized(void) const
{
    double  s;
    s = sqrt(v_[0] * v_[0] + v_[1] * v_[1] + v_[2] * v_[2]);

#if 0
    assert(s > ZERO_TOL);
#endif
    if(s > ZERO_TOL)
    {
        return TinyVectorN(4, v_[0] / s, v_[1] / s, v_[2] / s);
    }
    else
        return TinyVectorN(4, 0, 0, 0);
}

/* */
void TinyVectorN::normalize(void)
{
    double  s;
    s = sqrt(v_[0] * v_[0] + v_[1] * v_[1] + v_[2] * v_[2]);

#if 0
    assert(s > ZERO_TOL);
#endif
    if(s > ZERO_TOL)
    {
        v_[0] /= s;
        v_[1] /= s;
        v_[2] /= s;
    }
    else
    {
        v_[0] = 0;
        v_[1] = 0;
        v_[2] = 0;
    }
}

/* */
double dotProduct(const TinyVectorN &v1, const TinyVectorN &v2)
{
    return v1.v_[0] * v2.v_[0] + v1.v_[1] * v2.v_[1] + v1.v_[2] * v2.v_[2];
}

/* */
double tv_abs(const TinyVectorN &v1)
{
    return sqrt(v1.v_[0] * v1.v_[0] + v1.v_[1] * v1.v_[1] + v1.v_[2] * v1.v_[2]);
}

/* */
double fast_abs(const TinyVectorN &v1)
{
    return v1.v_[0] * v1.v_[0] + v1.v_[1] * v1.v_[1] + v1.v_[2] * v1.v_[2];
}

/* cross product */
TinyVectorN TinyVectorN::operator*(const TinyVectorN &v2) const
{
    return TinyVectorN(4, (v_[1] * v2.v_[2] - v_[2] * v2.v_[1]),
                       (v_[2] * v2.v_[0] - v_[0] * v2.v_[2]),
                       (v_[0] * v2.v_[1] - v_[1] * v2.v_[0]));
}

/* */
TinyVectorN TinyVectorN::operator+(const TinyVectorN &v2) const
{
    return TinyVectorN(4, v_[0] + v2.v_[0], v_[1] + v2.v_[1], v_[2] + v2.v_[2]);
}

/* */
TinyVectorN TinyVectorN::operator-(const TinyVectorN &v2) const
{
    return TinyVectorN(4, v_[0] - v2.v_[0], v_[1] - v2.v_[1], v_[2] - v2.v_[2]);
}

/* */
TinyVectorN TinyVectorN::operator*(double f) const
{
    return TinyVectorN(4, v_[0] * f, v_[1] * f, v_[2] * f);
}

/* */
TinyVectorN TinyVectorN::operator/(double f) const
{
    //if (f>ZERO_TOL)
    return TinyVectorN(4, v_[0] / f, v_[1] / f, v_[2] / f);
}
#if 0
/* copied streams impelementation from blindly from MAM/VRS :-< */
ostream &operator<<(ostream &s, const TinyVectorN &v1)
{
    int i;

    s << "(" << v1.v_[0] << "," << v1.v_[1] << "," << v1.v_[2] << ")";
    return s;
}

/* */
istream &operator>>(istream &s, TinyVectorN &v1)
{
    char    ch;

    s >> ch;
    if(ch != '(')
    {
        s.clear(ios::badbit);
        return s;
    }

    s >> v1.v_[0];
    if(!s.good())
    {
        return s;
    }

    s >> ch;
    if(ch != ',')
    {
        s.clear(ios::badbit);
        return s;
    }

    s >> v1.v_[1];
    if(!s.good())
    {
        return s;
    }

    s >> ch;
    if(ch != ',')
    {
        s.clear(ios::badbit);
        return s;
    }

    s >> v1.v_[2];
    if(!s.good())
    {
        return s;
    }

    s >> ch;
    if(ch != ')')
    {
        s.clear(ios::badbit);
        return s;
    }

    return s;
}
#endif
/******************** TinyVector ********************/
double &TinyVector::operator    [] (int index)
{
    return v_[index];
}
double TinyVector::operator [] (int index) const
{
    return v_[index];
}

/* */
const TinyVector &TinyVector::operator=(const TinyVector &v2)
{
    memcpy(v_, v2.v_, sizeof(v_));
    return *this;
}

/* */
void TinyVector::dump(void)
{
    int i;

    printf("(");
    for(i = 0; i < 4; i++)
        printf("%f ", v_[i]);
    printf(")\n");
}

/* return normalized copy */
TinyVector TinyVector::normalized(void) const
{
    double  s;
    s = sqrt(v_[0] * v_[0] + v_[1] * v_[1] + v_[2] * v_[2]);

#if 0
    assert(s > ZERO_TOL_VEC_LENGTH);
#endif
    if(s > ZERO_TOL_VEC_LENGTH)
    {
        return TinyVector(v_[0] / s, v_[1] / s, v_[2] / s);
    }
    else
        return TinyVector(0, 0, 0);
}

/* */
void TinyVector::normalize(void)
{
    double  s;
    s = sqrt(v_[0] * v_[0] + v_[1] * v_[1] + v_[2] * v_[2]);

#if 0
    assert(s > ZERO_TOL);
#endif
    if(s > ZERO_TOL)
    {
        v_[0] /= s;
        v_[1] /= s;
        v_[2] /= s;
    }
    else
    {
        v_[0] = 0;
        v_[1] = 0;
        v_[2] = 0;
    }
}

/* */
TinyVector TinyVector::perpendicularNormal(void)
{
    int         i;
    TinyVector  normal;
    TinyVector  vNormalized;
    int         numZeroEntries;
    int         zeroEntryIndices[3];
    int         index;

    numZeroEntries = 0;

    vNormalized[0] = v_[0];
    vNormalized[1] = v_[1];
    vNormalized[2] = v_[2];
    vNormalized.normalize();

    for(i = 0; i < 3; i++)
    {
        if(fabs(vNormalized[i]) < ZERO_TOL)
        {
            zeroEntryIndices[numZeroEntries] = i;
            numZeroEntries++;
        }
    }

    if(numZeroEntries == 2)
    {
        normal[zeroEntryIndices[0]] = 1;
    }
    else if(numZeroEntries == 1)
    {
        if(zeroEntryIndices[0] == 0)
        {
            normal[1] = vNormalized[2];
            normal[2] = -vNormalized[1];
        }
        else if(zeroEntryIndices[0] == 1)
        {
            normal[0] = vNormalized[2];
            normal[2] = -vNormalized[0];
        }
        else
        {
            normal[0] = vNormalized[1];
            normal[1] = -vNormalized[0];
        }
    }
    else
    {
        normal[0] = vNormalized[1];
        normal[1] = -vNormalized[0];
        normal[2] = 0;
    }

    normal.normalize();

    return normal;
}

/* */
void initTinyVectorArray(TinyVector *va, int numElements)
{
    int i;
    for(i = 0; i < numElements; i++)
        va[i].zeroWithOne();
}

/* srand must be set from the outside */
void TinyVector::randomize(BOOL allPositive)
{
    int         i;
    TinyVector  v;
    int         signVal;

    for(i = 0; i < 3; i++)
    {
        v_[i] = ((double) rand()) / ((double) RAND_MAX);
        if(!allPositive)
        {
            signVal = rand() % 2;
            if(signVal == 0)
                signVal = -1;
            v_[i] = v_[i] * signVal;
        }
    }

    normalize();    /* gives zero vector if vector could not be normalized */
}

/* */
double TinyVector::angle(TinyVector &v2)
{
    double      alpha;
    TinyVector  a, b;

    a = *this;
    b = v2;

    //a.normalize();b.normalize();
    alpha = safe_acos(dotProduct(a, b));
    return alpha;
}

/* */
double TinyVector::fast_angle(TinyVector &v2)
{
    double      alpha;
    TinyVector  a, b;

    a = *this;
    b = v2;

    //a.normalize();b.normalize();
    alpha = dotProduct(a, b);
    return alpha;
}

/* */
double dotProduct(const TinyVector &v1, const TinyVector &v2)
{
    return v1.v_[0] * v2.v_[0] + v1.v_[1] * v2.v_[1] + v1.v_[2] * v2.v_[2];
}

/* */
double tv_abs(const TinyVector &v1)
{
    return sqrt(v1.v_[0] * v1.v_[0] + v1.v_[1] * v1.v_[1] + v1.v_[2] * v1.v_[2]);
}

/* */
double fast_abs(const TinyVector &v1)
{
    return v1.v_[0] * v1.v_[0] + v1.v_[1] * v1.v_[1] + v1.v_[2] * v1.v_[2];
}

/* cross product */
TinyVector TinyVector::operator*(const TinyVector &v2) const
{
    return TinyVector((v_[1] * v2.v_[2] - v_[2] * v2.v_[1]),
                      (v_[2] * v2.v_[0] - v_[0] * v2.v_[2]),
                      (v_[0] * v2.v_[1] - v_[1] * v2.v_[0]));
}

/* */
TinyVector TinyVector::operator+(const TinyVector &v2) const
{
    return TinyVector(v_[0] + v2.v_[0], v_[1] + v2.v_[1], v_[2] + v2.v_[2]);
}

/* */
TinyVector TinyVector::operator-(const TinyVector &v2) const
{
    return TinyVector(v_[0] - v2.v_[0], v_[1] - v2.v_[1], v_[2] - v2.v_[2]);
}

/* */
TinyVector TinyVector::operator*(double f) const
{
    return TinyVector(v_[0] * f, v_[1] * f, v_[2] * f);
}

/* */
TinyVector TinyVector::operator/(double f) const
{
    //if (f>ZERO_TOL)
    return TinyVector(v_[0] / f, v_[1] / f, v_[2] / f);
}
#if 0
/* copied streams impelementation from blindly from MAM/VRS :-< */
ostream &operator<<(ostream &s, const TinyVector &v1)
{
    int i;

    s << "(" << v1.v_[0] << "," << v1.v_[1] << "," << v1.v_[2] << ")";
    return s;
}

/* */
istream &operator>>(istream &s, TinyVector &v1)
{
    char    ch;

    s >> ch;
    if(ch != '(')
    {
        s.clear(ios::badbit);
        return s;
    }

    s >> v1.v_[0];
    if(!s.good())
    {
        return s;
    }

    s >> ch;
    if(ch != ',')
    {
        s.clear(ios::badbit);
        return s;
    }

    s >> v1.v_[1];
    if(!s.good())
    {
        return s;
    }

    s >> ch;
    if(ch != ',')
    {
        s.clear(ios::badbit);
        return s;
    }

    s >> v1.v_[2];
    if(!s.good())
    {
        return s;
    }

    s >> ch;
    if(ch != ')')
    {
        s.clear(ios::badbit);
        return s;
    }

    return s;
}
#endif
#endif
