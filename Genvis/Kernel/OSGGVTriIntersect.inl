//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//                                License                                     
//                                                                            
//   This library is free software; you can redistribute it and/or modify it 
//   under the terms of the GNU Library General Public License as published  
//   by the Free Software Foundation, version 2.                             
//                                                                             
//   This library is distributed in the hope that it will be useful, but       
//   WITHOUT ANY WARRANTY; without even the implied warranty of                
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         
//   Library General Public License for more details.                          
//                                                                            
//   You should have received a copy of the GNU Library General Public         
//   License along with this library; if not, write to the Free Software       
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.1 $
//   $Date: 2003/09/11 16:20:30 $
//                                                                            
//=============================================================================

// treat values less than LOCAL_EPSILON as zero
#undef  GV_TRITRI_EPSTEST

/*! if GV_TRITRI_EPSTEST is true 
    then we do a check (if |dv|<LOCAL_EPSILON then dv=0.0;) 
    else no check is done (which is less robust, but faster) 
 */	
static const Real LOCAL_EPSILON = 1e-6f;

/*! Sort two arguments a, b, so that a <= b.
 */
#define SORT(a,b)			\
	if(a>b) {			\
		const Real c=a;	\
		a=b;			\
		b=c;			\
	}

/*! Edge (V0,V1) to edge (U0,U1) test based on Franlin Antonio's gem: 
    "Faster Line Segment Intersection", in Graphics Gems III, pp. 199-202.
    Mention this test is performed in the axis-aligned plane given by indices
    i0, i1 and the constants Ax, Ay are required to be Ax = V1[i0] - V0[i0], 
    Ay = V1[i1] - V0[i1].
*/
#define EDGE_EDGE_TEST(V0, U0, U1)						        \
	Bx = U0[i0] - U1[i0];								\
	By = U0[i1] - U1[i1];								\
	Cx = V0[i0] - U0[i0];								\
	Cy = V0[i1] - U0[i1];								\
	f  = Ay*Bx - Ax*By;								\
	d  = By*Cx - Bx*Cy;								\
	if((f>0.0f && d>=0.0f && d<=f) || (f<0.0f && d<=0.0f && d>=f)) {		\
		const Real e=Ax*Cy - Ay*Cx;					        \
		if(f>0.0f) {								\
			if(e>=0.0f && e<=f) return true;				\
		} else {								\
			if(e<=0.0f && e>=f) return true;				\
		}									\
	}

/*! Edge (V0,V1) to triangle (U0,U1,U2) test, uses EDGE_EDGE_TEST three times.
    Mention this test is performed in the axis-aligned plane given by indices
    i0, i1.
*/
#define EDGE_AGAINST_TRI_EDGES(V0, V1, U0, U1, U2)		                        \
{											\
	Real Bx,By,Cx,Cy,d,f;								\
	const Real Ax = V1[i0] - V0[i0];					        \
	const Real Ay = V1[i1] - V0[i1];					        \
	/* test edge U0,U1 against V0,V1 */					        \
	EDGE_EDGE_TEST(V0, U0, U1);							\
	/* test edge U1,U2 against V0,V1 */					        \
	EDGE_EDGE_TEST(V0, U1, U2);							\
	/* test edge U2,U1 against V0,V1 */					        \
	EDGE_EDGE_TEST(V0, U2, U0);							\
}

/*! Point (V0) in triangle (U0,U1,U2) test.
    Mention this test is performed in the axis-aligned plane given by indices
    i0, i1. 
*/
#define POINT_IN_TRI(V0, U0, U1, U2)					                \
{											\
	/* is T1 completly inside T2? */					        \
	/* check if V0 is inside tri(U0,U1,U2) */			                \
	Real a  = U1[i1] - U0[i1];							\
	Real b  = -(U1[i0] - U0[i0]);						        \
	Real c  = -a*U0[i0] - b*U0[i1];					        \
	Real d0 = a*V0[i0] + b*V0[i1] + c;					        \
											\
	a  = U2[i1] - U1[i1];								\
	b  = -(U2[i0] - U1[i0]);							\
	c  = -a*U1[i0] - b*U1[i1];							\
	const Real d1 = a*V0[i0] + b*V0[i1] + c;		 	                \
											\
	a  = U0[i1] - U2[i1];								\
	b  = -(U0[i0] - U2[i0]);							\
	c  = -a*U2[i0] - b*U2[i1];							\
	const Real d2 = a*V0[i0] + b*V0[i1] + c;			                \
	if(d0*d1>0.0f) {							        \
		if(d0*d2>0.0f) return true;						\
	}										\
}

/*! Test coplanarity of triangles where n is one normal.
 */
inline bool CoplanarTriTri(const VectorClass& n, 
			   const PointClass& v0, const PointClass& v1, const PointClass& v2, 
			   const PointClass& u0, const PointClass& u1, const PointClass& u2)
{
	Real A[3];
	i16 i0,i1;
	/* first project onto an axis-aligned plane, that maximizes the area */
	/* of the triangles, compute indices: i0,i1. */
	A[0] = stdAbs(n[0]);
	A[1] = stdAbs(n[1]);
	A[2] = stdAbs(n[2]);
	if(A[0]>A[1]) {
		if(A[0]>A[2]) {
			i0=1;      /* A[0] is greatest */
			i1=2;
		} else {
			i0=0;      /* A[2] is greatest */
			i1=1;
		}
	} else  { /* A[0]<=A[1] */
		if(A[2]>A[1]) {
			i0=0;      /* A[2] is greatest */
			i1=1;
		} else {
			i0=0;      /* A[1] is greatest */
			i1=2;
		}
	}

	/* test all edges of triangle 1 against the edges of triangle 2 */
	EDGE_AGAINST_TRI_EDGES(v0, v1, u0, u1, u2);
	EDGE_AGAINST_TRI_EDGES(v1, v2, u0, u1, u2);
	EDGE_AGAINST_TRI_EDGES(v2, v0, u0, u1, u2);

	/* finally, test if tri1 is totally contained in tri2 or vice versa */
	POINT_IN_TRI(v0, u0, u1, u2);
	POINT_IN_TRI(u0, v0, v1, v2);

	return false;
}

/*! Compute interval for triangle (V0,V1,V2) in plane 
    with projection (VV0, VV1, VV2) and distances (D0, D1, D2). 
    Results are A, B, C, X0, X1.
 */
#define NEWCOMPUTE_INTERVALS(VV0, VV1, VV2, D0, D1, D2, D0D1, D0D2, A, B, C, X0, X1)	\
{																						\
	if(D0D1>0.0f) {																					\
		/* here we know that D0D2<=0.0 */												\
		/* that is D0, D1 are on the same side, D2 on the other or on the plane */		\
		A=VV2; B=(VV0 - VV2)*D2; C=(VV1 - VV2)*D2; X0=D2 - D0; X1=D2 - D1;				\
	} else if(D0D2>0.0f) {																					\
		/* here we know that d0d1<=0.0 */												\
		A=VV1; B=(VV0 - VV1)*D1; C=(VV2 - VV1)*D1; X0=D1 - D0; X1=D1 - D2;				\
	} else if(D1*D2>0.0f || D0!=0.0f) {																					\
		/* here we know that d0d1<=0.0 or that D0!=0.0 */								\
		A=VV0; B=(VV1 - VV0)*D0; C=(VV2 - VV0)*D0; X0=D0 - D1; X1=D0 - D2;				\
	} else if(D1!=0.0f) {																					\
		A=VV1; B=(VV0 - VV1)*D1; C=(VV2 - VV1)*D1; X0=D1 - D0; X1=D1 - D2;				\
	} else if(D2!=0.0f) {																					\
		A=VV2; B=(VV0 - VV2)*D2; C=(VV1 - VV2)*D2; X0=D2 - D0; X1=D2 - D1;				\
	} else {																					\
		/* triangles are coplanar */													\
		return CoplanarTriTri(N1, V0, V1, V2, U0, U1, U2);								\
	}																					\
}

/*! Triangle/triangle intersection test routine by Tomas Moller, 1997. 
    See article "A Fast Triangle-Triangle Intersection Test",
    Journal of Graphics Tools, 2(2), 1997.
    \param  V0[in] triangle 0, vertex 0
    \param  V1[in] triangle 0, vertex 1
    \param  V2[in] triangle 0, vertex 2
    \param  U0[in] triangle 1, vertex 0
    \param  U1[in] triangle 1, vertex 1
    \param  U2[in] triangle 1, vertex 2
    \return true if triangles intersect
 */
inline bool triTriOverlap (const PointClass& V0, const PointClass& V1, const PointClass& V2, 
			   const PointClass& U0, const PointClass& U1, const PointClass& U2)
{
	// Compute plane equation of triangle(V0,V1,V2)
	VectorClass E1(V1 - V0);
	VectorClass E2(V2 - V0);
	const VectorClass N1(E1.cross(E2));
	const Real d1 = -N1.dot(V0);
	// Plane equation 1: N1.X+d1=0

	// Put U0,U1,U2 into plane equation 1 to compute signed distances to the plane
	Real du0 = N1.dot(U0) + d1;
	Real du1 = N1.dot(U1) + d1;
	Real du2 = N1.dot(U2) + d1;

	// Coplanarity robustness check
#ifdef GV_TRITRI_EPSTEST
	if (stdAbs(du0)<LOCAL_EPSILON) du0 = 0.0f;
	if (stdAbs(du1)<LOCAL_EPSILON) du1 = 0.0f;
	if (stdAbs(du2)<LOCAL_EPSILON) du2 = 0.0f;
#endif
	const Real du0du1 = du0 * du1;
	const Real du0du2 = du0 * du2;

	if (du0du1>0.0f && du0du2>0.0f)	// same sign on all of them + not equal 0 ?
		return false;					// no intersection occurs

	// Compute plane of triangle (U0,U1,U2)
	E1.setValue(U1 - U0);
	E2.setValue(U2 - U0);
	const VectorClass N2(E1.cross(E2));
	const Real d2 = -N2.dot(U0);
	// plane equation 2: N2.X+d2=0

	// put V0,V1,V2 into plane equation 2
	Real dv0 = N2.dot(V0) + d2;
	Real dv1 = N2.dot(V1) + d2;
	Real dv2 = N2.dot(V2) + d2;

#ifdef GV_TRITRI_EPSTEST
	if (stdAbs(dv0)<LOCAL_EPSILON) dv0 = 0.0f;
	if (stdAbs(dv1)<LOCAL_EPSILON) dv1 = 0.0f;
	if (stdAbs(dv2)<LOCAL_EPSILON) dv2 = 0.0f;
#endif

	const Real dv0dv1 = dv0 * dv1;
	const Real dv0dv2 = dv0 * dv2;

	if (dv0dv1>0.0f && dv0dv2>0.0f)	// same sign on all of them + not equal 0 ?
		return false;					// no intersection occurs

	// Compute direction of intersection line
	const VectorClass D(N1.cross(N2));

	// Compute and index to the largest component of D
	Real max = stdAbs(D[0]);
	i16 index = 0;
	Real bb = stdAbs(D[1]);
	Real cc = stdAbs(D[2]);
	if (bb>max) max=bb,index=1;
	if (cc>max) max=cc,index=2;

	// This is the simplified projection onto L
	const Real vp0 = V0[index];
	const Real vp1 = V1[index];
	const Real vp2 = V2[index];

	const Real up0 = U0[index];
	const Real up1 = U1[index];
	const Real up2 = U2[index];

	// Compute interval for triangle 1
	Real a,b,c,x0,x1;
	NEWCOMPUTE_INTERVALS(vp0,vp1,vp2,dv0,dv1,dv2,dv0dv1,dv0dv2,a,b,c,x0,x1);

	// Compute interval for triangle 2
	Real d,e,f,y0,y1;
	NEWCOMPUTE_INTERVALS(up0,up1,up2,du0,du1,du2,du0du1,du0du2,d,e,f,y0,y1);

	const Real xx=x0*x1;
	const Real yy=y0*y1;
	const Real xxyy=xx*yy;

	Real isect1[2], isect2[2];

	Real tmp = a*xxyy;
	isect1[0] = tmp+b*x1*yy;
	isect1[1] = tmp+c*x0*yy;

	tmp = d*xxyy;
	isect2[0] = tmp+e*xx*y1;
	isect2[1] = tmp+f*xx*y0;

	SORT(isect1[0], isect1[1]);
	SORT(isect2[0], isect2[1]);

	if (isect1[1]<isect2[0] || isect2[1]<isect1[0]) {
	   return false;
	}
	return true;
}

/*! Triangle/triangle intersection test routine by Tomas Moller, 1997. 
    See article "A Fast Triangle-Triangle Intersection Test",
    Journal of Graphics Tools, 2(2), 1997.
    This version (\see triTriOverlap) additionally computes an intersection 
    point.
    \param  P[out] an intersection point on both triangles
    \param  V0[in] triangle 0, vertex 0
    \param  V1[in] triangle 0, vertex 1
    \param  V2[in] triangle 0, vertex 2
    \param  U0[in] triangle 1, vertex 0
    \param  U1[in] triangle 1, vertex 1
    \param  U2[in] triangle 1, vertex 2
    \return true if triangles intersect
 */
inline bool triTriIntersect (PointClass& P,
			     const PointClass& V0, const PointClass& V1, const PointClass& V2, 
			     const PointClass& U0, const PointClass& U1, const PointClass& U2)
{
	// Compute plane equation of triangle(V0,V1,V2)
	VectorClass E1(V1 - V0);
	VectorClass E2(V2 - V0);
	const VectorClass N1(E1.cross(E2));
	const Real d1 = -N1.dot(V0);
	// Plane equation 1: N1.X+d1=0

	// Put U0,U1,U2 into plane equation 1 to compute signed distances to the plane
	Real du0 = N1.dot(U0) + d1;
	Real du1 = N1.dot(U1) + d1;
	Real du2 = N1.dot(U2) + d1;

	// Coplanarity robustness check
#ifdef GV_TRITRI_EPSTEST
	if (stdAbs(du0)<LOCAL_EPSILON) du0 = 0.0f;
	if (stdAbs(du1)<LOCAL_EPSILON) du1 = 0.0f;
	if (stdAbs(du2)<LOCAL_EPSILON) du2 = 0.0f;
#endif
	const Real du0du1 = du0 * du1;
	const Real du0du2 = du0 * du2;

	if (du0du1>0.0f && du0du2>0.0f)	{ // same sign on all of them + not equal 0 ?
	   return false;		  // no intersection occurs
	}

	// Compute plane of triangle (U0,U1,U2)
	E1.setValue(U1 - U0);
	E2.setValue(U2 - U0);
	const VectorClass N2(E1.cross(E2));
	const Real d2 = -N2.dot(U0);
	// plane equation 2: N2.X+d2=0

	// put V0,V1,V2 into plane equation 2
	Real dv0 = N2.dot(V0) + d2;
	Real dv1 = N2.dot(V1) + d2;
	Real dv2 = N2.dot(V2) + d2;

#ifdef GV_TRITRI_EPSTEST
	if (stdAbs(dv0)<LOCAL_EPSILON) dv0 = 0.0f;
	if (stdAbs(dv1)<LOCAL_EPSILON) dv1 = 0.0f;
	if (stdAbs(dv2)<LOCAL_EPSILON) dv2 = 0.0f;
#endif

	const Real dv0dv1 = dv0 * dv1;
	const Real dv0dv2 = dv0 * dv2;

	if (dv0dv1>0.0f && dv0dv2>0.0f)	{ // same sign on all of them + not equal 0 ?
	   return false;		  // no intersection occurs
	}

	// Compute direction of intersection line
	const VectorClass D(N1.cross(N2));

	// Compute and index to the largest component of D
	Real max = stdAbs(D[0]);
	i16 index = 0;
	Real bb = stdAbs(D[1]);
	Real cc = stdAbs(D[2]);
	if (bb>max) max=bb,index=1;
	if (cc>max) max=cc,index=2;

	// This is the simplified projection onto L
	const Real vp0 = V0[index];
	const Real vp1 = V1[index];
	const Real vp2 = V2[index];

	const Real up0 = U0[index];
	const Real up1 = U1[index];
	const Real up2 = U2[index];

	// Compute interval for triangle 1
	Real a,b,c,x0,x1;
	NEWCOMPUTE_INTERVALS(vp0,vp1,vp2,dv0,dv1,dv2,dv0dv1,dv0dv2,a,b,c,x0,x1);

	// Compute interval for triangle 2
	Real d,e,f,y0,y1;
	NEWCOMPUTE_INTERVALS(up0,up1,up2,du0,du1,du2,du0du1,du0du2,d,e,f,y0,y1);

	const Real xx=x0*x1;
	const Real yy=y0*y1;
	const Real xxyy=xx*yy;

	Real isect1[2], isect2[2];

	Real tmp = a*xxyy;
	isect1[0] = tmp+b*x1*yy;
	isect1[1] = tmp+c*x0*yy;

	tmp = d*xxyy;
	isect2[0] = tmp+e*xx*y1;
	isect2[1] = tmp+f*xx*y0;

	SORT(isect1[0],isect1[1]);
	SORT(isect2[0],isect2[1]);

	if (isect1[1]<isect2[0] || isect2[1]<isect1[0]) {
	   return false;
	}

	// compute intersection point
	P.setValue(isect1[1]*D);
	return true;
}
