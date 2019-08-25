/*  Subroutine to generate a B-spline curve using an uniform open knot vector

	C code for An Introduction to NURBS
	by David F. Rogers. Copyright (C) 2000 David F. Rogers,
	All rights reserved.
	
	Name: bspline.c
	Language: C
	Subroutines called: knot.c, basis.c, fmtmul.c
	Book reference: Section 3.5, Ex. 3.4, Alg. p. 281

    i_knot       = array containing the defining polygon vertices
                  b[1] contains the x-component of the vertex
                  b[2] contains the y-component of the vertex
                  b[3] contains the z-component of the vertex
    so_knot     = order of the \bsp basis function
    spline      = array containing the curve points
                  p[1] contains the x-component of the point
                  p[2] contains the y-component of the point
                  p[3] contains the z-component of the point
    so_spline   = number of points to be calculated on the curve
    k           = spline order

JB#07/08/2006 Takes the sources control
JB#24/08/2019 Refactoring points:  minimize client side, full naming review, interface still c-compatible
*/

#ifndef __NURBS_inc
#define __NURBS_inc

typedef enum NurbsNumerology
{
    NurbsOrderBound = 15,
    LengthOfInterpolation = 200,
    NurbsKnotBound  = 100,

    NurbsLengthOutOfBounds = -4000,
    NurbsOrderOutOfBounds,
    NurbsKnotsOutOfBounds
}NurbsNumerology;

typedef struct NurbsDataT
{
    float x[ LengthOfInterpolation ];
    float y[ LengthOfInterpolation ];
    float z[ LengthOfInterpolation ];
}NurbsDataT;

typedef struct
{
    int noKnots, soInterpolation, order;
    NurbsDataT data;
}NurbsDTO;

int nurbs_generator( NurbsDTO* );

#endif
