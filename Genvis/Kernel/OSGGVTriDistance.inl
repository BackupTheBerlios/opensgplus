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

/*! Closest points between two edges.
    Implemented from an algorithm described in Vladimir J. Lumelsky,
    On fast computation of distance between line segments.
    In Information Processing Letters, no. 21, pages 55-61, 1985.   
    @param VEC[out] direction between closest points
    @param X[out]   closest point on edge 0
    @param Y[out]   closest point on edge 1
    @param P[in]    origin of edge 0
    @param A[in]    direction of edge 0
    @param Q[in]    origin of edge 1
    @param B[in]    direction of edge 1
 */
inline void edgeEdgePoints (VectorClass& VEC, 
			    PointClass& X, PointClass& Y,
			    const PointClass& P, const VectorClass& A, 
			    const PointClass& Q, const VectorClass& B)
{
  Real A_dot_A, B_dot_B, A_dot_B, A_dot_T, B_dot_T;
  VectorClass T;
  VectorClass TMP;

  T = Q - P;
  A_dot_A = A.dot(A);
  B_dot_B = B.dot(B);
  A_dot_B = A.dot(B);
  A_dot_T = A.dot(T);
  B_dot_T = B.dot(T);

  // t parameterizes ray P,A 
  // u parameterizes ray Q,B 

  Real t, u;

  // compute t for the closest point on ray P,A to
  // ray Q,B

  Real denom = A_dot_A*B_dot_B - A_dot_B*A_dot_B;

  t = (A_dot_T*B_dot_B - B_dot_T*A_dot_B) / denom;

  // clamp result so t is on the edge P,A

  if (t < 0) 
    t = 0; 
  else if (t > 1) 
    t = 1; 
  else if (!(t >= 0 && t <= 1)) 
    t = 0; // for NAN

  // find u for point on ray Q,B closest to point at t

  u = (t*A_dot_B - B_dot_T) / B_dot_B;

  // if u is on edge Q,B, then t and u correspond to 
  // closest points, otherwise, clamp u, recompute and
  // clamp t 

  if (u <= 0) {

    Y = Q;

    t = A_dot_T / A_dot_A;

    if (t <= 0) {
      X = P;
      VEC = Q - P;
    } else if (t >= 1) {
      X = P + A;
      VEC = Q - X;
    } else if (!(t > 0 && t < 1)) { // for NAN
      X = P;
      VEC = Q - P;
    } else {
      X = P + t*A;
      TMP = T.cross(A);
      VEC = A.cross(TMP);
    }
  } else if (u >= 1) {

    Y = Q + B;

    t = (A_dot_B + A_dot_T) / A_dot_A;

    if (t <= 0) {
      X = P;
      VEC = Y - P;
    } else if (t >= 1) {
      X = P + A;
      VEC = Y - X;
    } else if (!(t > 0 && t < 1)) { // for NAN
      X = P;
      VEC = Y - P;
    } else {
      X = P + t*A;
      T = Y - P;
      TMP = T.cross(A);
      VEC = A.cross(TMP);
    }
  } else if (!(u > 0 && u < 1)) { // for NAN

    Y = Q;

    t = A_dot_T / A_dot_A;

    if (t <= 0) {
      X = P;
      VEC = Q - P;
    } else if (t >= 1) {
      X = P + A;
      VEC = Q - X;
    } else if (!(t > 0 && t < 1)) { // for NAN
      X = P;
      VEC = Q - P;
    } else {
      X = P + t*A;
      TMP = T.cross(A);
      VEC = A.cross(TMP);
    }
  } else {

    Y = Q + u*B;

    if (t <= 0) {
      X = P;
      TMP = T.cross(B);
      VEC = B.cross(TMP);
    } else if (t >= 1) {
      X = P + A;
      T = Q - X;
      TMP = T.cross(B);
      VEC = B.cross(TMP);
    } else if (!(t > 0 && t < 1)) { // for NAN
      X = P;
      TMP = T.cross(B);
      VEC = B.cross(TMP);
    } else {
      X = P + t*A;
      VEC = A.cross(B);
      if (VEC.dot(T) < 0) {
	VEC *= -1;
      }
    }
  }
}

/*! Computes the closest points on two triangles, and returns the 
    distance between them.
    If the triangles are disjoint, P and Q give the closest points of 
    S and T respectively. However, if the triangles overlap, P and Q 
    are basically a random pair of points from the triangles, not 
    coincident points on the intersection of the triangles, as might 
    be expected.
    @param S[in]  points of triangle 0
    @param T[in]  points of triangle 1
    @param P[out] closest point on triangle 0
    @param Q[out] closest point on triangle 1
    @return distance between the triangles.
 */
inline Real triTriDistance (PointClass& P, PointClass& Q,
			    const PointClass S[3], const PointClass T[3])  
{
  // Compute vectors along the 6 sides

  VectorClass Sv[3], Tv[3];
  VectorClass VEC;

  Sv[0] = S[1] - S[0];
  Sv[1] = S[2] - S[1];
  Sv[2] = S[0] - S[2];

  Tv[0] = T[1] - T[0];
  Tv[1] = T[2] - T[1];
  Tv[2] = T[0] - T[2];

  // For each edge pair, the vector connecting the closest points 
  // of the edges defines a slab (parallel planes at head and tail
  // enclose the slab). If we can show that the off-edge vertex of 
  // each triangle is outside of the slab, then the closest points
  // of the edges are the closest points for the triangles.
  // Even if these tests fail, it may be helpful to know the closest
  // points found, and whether the triangles were shown disjoint

  VectorClass V;
  VectorClass Z;
  PointClass minP, minQ;
  Real mindd;
  i32 shown_disjoint = 0;

  mindd = S[0].dist2(T[0]) + 1;  // Set first minimum safely high

  for (i32 i = 0; i < 3; i++) {
    for (i32 j = 0; j < 3; j++) {
      // Find closest points on edges i & j, plus the 
      // vector (and distance squared) between these points

      edgeEdgePoints(VEC, P, Q, S[i], Sv[i], T[j], Tv[j]);
      
      V = Q - P;
      Real dd = V.dot(V);

      // Verify this closest point pair only if the distance 
      // squared is less than the minimum found thus far.

      if (dd <= mindd) {
        minP = P;
        minQ = Q;
        mindd = dd;

        Z = S[(i+2)%3] - P;
        Real a = Z.dot(VEC);
        Z = T[(j+2)%3] - Q;
        Real b = Z.dot(VEC);

	if ((a <= 0) && (b >= 0)) 
	  return sqrt(dd);

	Real p = V.dot(VEC);

        if (a < 0) 
	  a = 0;
        if (b > 0) 
	  b = 0;
        if ((p - a + b) > 0) 
	  shown_disjoint = 1;	
      }
    }
  }

  // No edge pairs contained the closest points.  
  // either:
  // 1. one of the closest points is a vertex, and the
  //    other point is interior to a face.
  // 2. the triangles are overlapping.
  // 3. an edge of one triangle is parallel to the other's face. If
  //    cases 1 and 2 are not true, then the closest points from the 9
  //    edge pairs checks above can be taken as closest points for the
  //    triangles.
  // 4. possibly, the triangles were degenerate.  When the 
  //    triangle points are nearly colinear or coincident, one 
  //    of above tests might fail even though the edges tested
  //    contain the closest points.

  // First check for case 1
  VectorClass Sn;
  Real Snl;       
  Sn = Sv[0].cross(Sv[1]); // Compute normal to S triangle
  Snl = Sn.dot(Sn);      // Compute square of length of normal
  
  // If cross product is long enough,
  if (Snl > 1e-15) {
    // Get projection lengths of T points

    Real Tp[3]; 

    V = S[0] - T[0];
    Tp[0] = V.dot(Sn);

    V = S[0] - T[1];
    Tp[1] = V.dot(Sn);

    V = S[0] - T[2];
    Tp[2] = V.dot(Sn);

    // If Sn is a separating direction,
    // find point with smallest projection

    i32 point = -1;
    if ((Tp[0] > 0) && (Tp[1] > 0) && (Tp[2] > 0)) {
      if (Tp[0] < Tp[1]) 
	point = 0; 
      else point = 1;
      if (Tp[2] < Tp[point]) 
	point = 2;
    } else if ((Tp[0] < 0) && (Tp[1] < 0) && (Tp[2] < 0)) {
      if (Tp[0] > Tp[1]) 
	point = 0; 
      else point = 1;
      if (Tp[2] > Tp[point]) 
	point = 2;
    }

    // If Sn is a separating direction, 

    if (point >= 0) {
      shown_disjoint = 1;

      // Test whether the point found, when projected onto the 
      // other triangle, lies within the face.
    
      V = T[point] - S[0];
      Z = Sn.cross(Sv[0]);
      if (V.dot(Z) > 0) {
        V = T[point] - S[1];
        Z = Sn.cross(Sv[1]);
        if (V.dot(Z) > 0) {
          V = T[point] - S[2];
          Z = Sn.cross(Sv[2]);
          if (V.dot(Z) > 0) {
            // T[point] passed the test - it's a closest point for 
            // the T triangle; the other point is on the face of S

            P = T[point] + (Tp[point]/Snl)*Sn;
            Q = T[point];
            return P.dist(Q);
          }
        }
      }
    }
  }

  VectorClass Tn;
  Real Tnl;       
  Tn  = Tv[0].cross(Tv[1]); 
  Tnl = Tn.dot(Tn);      
  
  if (Tnl > 1e-15) {
    Real Sp[3]; 

    V = T[0] - S[0];
    Sp[0] = V.dot(Tn);

    V = T[0] - S[1];
    Sp[1] = V.dot(Tn);

    V = T[0] - S[2];
    Sp[2] = V.dot(Tn);

    i32 point = -1;
    if ((Sp[0] > 0) && (Sp[1] > 0) && (Sp[2] > 0)) {
      if (Sp[0] < Sp[1]) 
	point = 0; 
      else point = 1;
      if (Sp[2] < Sp[point]) 
	point = 2;
    } else if ((Sp[0] < 0) && (Sp[1] < 0) && (Sp[2] < 0)) {
      if (Sp[0] > Sp[1]) 
	point = 0; 
      else point = 1;
      if (Sp[2] > Sp[point]) 
	point = 2;
    }

    if (point >= 0) { 
      shown_disjoint = 1;

      V = S[point] - T[0];
      Z = Tn.cross(Tv[0]);
      if (V.dot(Z) > 0) {
        V = S[point] - T[1];
        Z = Tn.cross(Tv[1]);
        if (V.dot(Z) > 0) {
          V = S[point] - T[2];
          Z = Tn.cross(Tv[2]);
          if (V.dot(Z) > 0) {
            P = S[point];
            Q = S[point] + (Sp[point]/Tnl)*Tn;
            return P.dist(Q);
          }
        }
      }
    }
  }

  // Case 1 can't be shown.
  // If one of these tests showed the triangles disjoint,
  // we assume case 3 or 4, otherwise we conclude case 2, 
  // that the triangles overlap.  
  if (shown_disjoint) {
    P = minP;
    Q = minQ;
    return sqrt(mindd);
  } else {
    return 0;
  }
}
