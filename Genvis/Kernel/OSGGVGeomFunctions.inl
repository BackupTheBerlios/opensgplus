/*! Compute normal to direction in 2-dimensions.
 */
inline VectorClass2
calcNormal (const VectorClass2& v) 
{
   return VectorClass2(v[1], -v[0]);
}
/*! Compute normal to two directions in 3-dimensions.
 */
inline VectorClass3 
calcNormal (const VectorClass3& v,
	    const VectorClass3& dirZ) 
{
   return v.cross(dirZ);
}
/*! Compute a normal to direction in 3-dimensions. The normal is chosen
    in the plane e_argmin(|v[i]|).
 */
inline VectorClass3
calcNormal (const VectorClass3& v)
{
   VectorClass3 result;
   Real xx = stdAbs(v[0]);
   Real yy = stdAbs(v[1]);
   Real zz = stdAbs(v[2]);
   if(xx < yy) {
      if(xx < zz) { result[0]=0; result[1]=-v[2]; result[2]=v[1]; } // use y and z
      else        { result[0]=-v[1]; result[1]=v[0]; result[2]=0; } // use xx and y
   } else {
      if(yy < zz) { result[0]=-v[2]; result[1]=0; result[2]=v[0]; } // use x and z
      else        { result[0]=-v[1]; result[1]=v[0]; result[2]=0; } // use x and y
   }
   result /= result.length();
   return result;
}
/*! Center of gravity of three vectors. */
inline VectorClass3 
calcCenter (const VectorClass3& p0,
	    const VectorClass3& p1,
	    const VectorClass3& p2)
{
   VectorClass3 result(p0+p1+p2);
   result /= result.length(); 
   return result;
}
/*! Center of gravity of five vectors. */
inline VectorClass3
calcCenter (const VectorClass3& p0,
	    const VectorClass3& p1,
	    const VectorClass3& p2, 
	    const VectorClass3& p3,
	    const VectorClass3& p4)
{
   VectorClass3 result(p0+p1+p2+p3+p4);
   result /= result.length(); 
   return result;
}

/*! Circumcircle of triangle rkV0,rkV1,rkV2. */
inline bool calcCircumcircle
(const PointClass3& rkV0, 
 const PointClass3& rkV1,
 const PointClass3& rkV2,
 PointClass3&  center,
 Real&  radius,
 VectorClass3* normal=NULL, 
 VectorClass3* u=NULL,
 VectorClass3* v=NULL)
{
    static FloatingComparator<Real> comp(1e-06);
    VectorClass kE02(rkV0 - rkV2);
    VectorClass kE12(rkV1 - rkV2);
    Real fE02E02 = kE02.dot(kE02);
    Real fE02E12 = kE02.dot(kE12);
    Real fE12E12 = kE12.dot(kE12);
    Real fDet = fE02E02*fE12E12 - fE02E12*fE02E12;
    if ( comp.zero(stdAbs(fDet)) ) {
       // rkV0, rkV1, rkV2 lying on a line
       if (fE02E02 > fE12E12) {
	  center.setValue(VectorClass(rkV0) + VectorClass(rkV2)); 
	  center *= 0.5f;
	  radius  = 0.5f*sqrt(fE02E02);
       } else {
	  center.setValue(VectorClass(rkV1) + VectorClass(rkV2));
	  center *= 0.5f;
	  radius  = 0.5f*sqrt(fE12E12);
       }
       return false;
    }

    Real fHalfInvDet = 0.5f/fDet;
    Real fU0 = fHalfInvDet*fE12E12*(fE02E02 - fE02E12);
    Real fU1 = fHalfInvDet*fE02E02*(fE12E12 - fE02E12);
    VectorClass kTmp(fU0*kE02 + fU1*kE12);

    center.setValue(rkV2 + kTmp);
    radius = kTmp.length();

    if (normal != NULL) {
       normal->setValue(kE02.cross(kE12)); normal->operator/=(normal->length());
       if ( stdAbs(normal->operator[](0)) >= stdAbs(normal->operator[](1))
	    && stdAbs(normal->operator[](0)) >= stdAbs(normal->operator[](2)) ) {
	 u->operator[](0) = -normal->operator[](1);
	 u->operator[](1) = normal->operator[](0);
	 u->operator[](2) = 0.0f;
       } else {
	 u->operator[](0) = 0.0f;
	 u->operator[](1) = normal->operator[](2);
	 u->operator[](2) = -normal->operator[](1);
       }

       u->operator/=(u->length());
       v->setValue(normal->cross(*u));
    }
    return true;
}
