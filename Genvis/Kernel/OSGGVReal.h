// ---------------------------------------------------------------
// File:    $Id: OSGGVReal.h,v 1.3 2004/12/20 15:58:23 fuenfzig Exp $
// Author:  Gordon Mueller, <gordon@vrroom.de>
// ---------------------------------------------------------------

#ifndef OSGGVREAL_H
#define OSGGVREAL_H

#include <math.h>
#include "OSGGVBase.h"

BEGIN_GENVIS_NAMESPACE

const float  PRECISION_DEFAULT = 1e-6;
const float  PRECISION_FLOAT   = 1e-6;
const double PRECISION_DOUBLE  = 1e-11;

template <class REAL>
inline int sign_pm (REAL a) 
{ 
   return ( a < REAL(0) ? -1 : 1 ); 
}

template <class REAL>
class FloatingComparator
{
 public:
   FloatingComparator(REAL precision = REAL(PRECISION_DEFAULT));

   void        setPrecision (const REAL& value);
   const REAL& getPrecision () const;

   bool leq      (const REAL& f1, const REAL& f2) const;
   bool geq      (const REAL& f1, const REAL& f2) const;
   bool less     (const REAL& f1, const REAL& f2) const;
   bool greater  (const REAL& f1, const REAL& f2) const;
   bool equal    (const REAL& f1, const REAL& f2) const;
   bool positive (const REAL& f) const;
   bool negative (const REAL& f) const;
   bool positive0 (const REAL& f) const;
   bool negative0 (const REAL& f) const;
   bool zero     (const REAL& f) const;

   bool leq      (const REAL& f1, const REAL& f2, const REAL& eps) const;
   bool geq      (const REAL& f1, const REAL& f2, const REAL& eps) const;
   bool less     (const REAL& f1, const REAL& f2, const REAL& eps) const;
   bool greater  (const REAL& f1, const REAL& f2, const REAL& eps) const;
   bool equal    (const REAL& f1, const REAL& f2, const REAL& eps) const;
   bool positive (const REAL& f, const REAL& eps) const;
   bool negative (const REAL& f, const REAL& eps) const;
   bool positive0 (const REAL& f, const REAL& eps) const;
   bool negative0 (const REAL& f, const REAL& eps) const;
   bool zero     (const REAL& f, const REAL& eps) const;

 private:
   REAL m_precision;
};

typedef FloatingComparator<float>  FloatComparator;
typedef FloatingComparator<double> DoubleComparator;

template <class REAL>
inline FloatingComparator<REAL>::FloatingComparator(REAL precision)
  : m_precision(precision) 
{
}
template <class REAL>
inline const REAL& FloatingComparator<REAL>::getPrecision () const
{
   return m_precision;   
}
template <class REAL>
inline void FloatingComparator<REAL>::setPrecision (const REAL& value)
{
   m_precision = value;   
}


template <class REAL>
inline bool FloatingComparator<REAL>::less     (const REAL& f1, const REAL& f2, const REAL& eps) const
{
  return (f2-f1) > eps;   
}
template <class REAL>
inline bool FloatingComparator<REAL>::greater  (const REAL& f1, const REAL& f2, const REAL& eps) const
{
  return (f1-f2) > eps;   
} 
template <class REAL>
inline bool FloatingComparator<REAL>::leq     (const REAL& f1, const REAL& f2, const REAL& eps) const
{
  return (f2-f1) > -eps;   
}
template <class REAL>
inline bool FloatingComparator<REAL>::geq  (const REAL& f1, const REAL& f2, const REAL& eps) const
{
  return (f1-f2) > -eps;   
} 
template <class REAL>
inline bool FloatingComparator<REAL>::equal    (const REAL& f1, const REAL& f2, const REAL& eps) const
{
  return !(less(f1,f2,eps) || greater(f1,f2,eps));   
}
template <class REAL>
inline bool FloatingComparator<REAL>::positive (const REAL& f, const REAL& eps) const
{
  return f > eps;   
}
template <class REAL>
inline bool FloatingComparator<REAL>::negative (const REAL& f, const REAL& eps) const
{
  return f < -eps;   
}
template <class REAL>
inline bool FloatingComparator<REAL>::positive0 (const REAL& f, const REAL& eps) const
{
  return f > -eps;   
}
template <class REAL>
inline bool FloatingComparator<REAL>::negative0 (const REAL& f, const REAL& eps) const
{
  return f < eps;   
}
template <class REAL>
inline bool FloatingComparator<REAL>::zero     (const REAL& f, const REAL& eps) const
{
  return !(positive(f,eps) || negative(f,eps));   
}

template <class REAL>
inline bool FloatingComparator<REAL>::less     (const REAL& f1, const REAL& f2) const
{
  return less(f1, f2, getPrecision());   
}
template <class REAL>
inline bool FloatingComparator<REAL>::greater  (const REAL& f1, const REAL& f2) const
{
  return greater(f1, f2, getPrecision());   
} 
template <class REAL>
inline bool FloatingComparator<REAL>::leq     (const REAL& f1, const REAL& f2) const
{
  return leq(f1, f2, getPrecision());   
}
template <class REAL>
inline bool FloatingComparator<REAL>::geq  (const REAL& f1, const REAL& f2) const
{
  return geq(f1, f2, getPrecision());   
} 
template <class REAL>
inline bool FloatingComparator<REAL>::equal    (const REAL& f1, const REAL& f2) const
{
  return equal(f1, f2, getPrecision());   
}
template <class REAL>
inline bool FloatingComparator<REAL>::positive (const REAL& f) const
{
  return positive(f, getPrecision());   
}
template <class REAL>
inline bool FloatingComparator<REAL>::negative (const REAL& f) const
{
  return negative(f, getPrecision());   
}
template <class REAL>
inline bool FloatingComparator<REAL>::positive0 (const REAL& f) const
{
  return positive0(f, getPrecision());   
}
template <class REAL>
inline bool FloatingComparator<REAL>::negative0 (const REAL& f) const
{
  return negative0(f, getPrecision());   
}
template <class REAL>
inline bool FloatingComparator<REAL>::zero     (const REAL& f) const
{
  return zero(f, getPrecision());   
}

END_GENVIS_NAMESPACE
#endif

