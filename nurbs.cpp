#include <string.h>
#include "nurbs.h"

static float nbasis [ 300 ];
static float temp   [ 300 ];
static int   x      [ 750 ];
static float knots3D  [ NurbsKnotBound *3 ];
static float interpolation3D [ LengthOfInterpolation *3 ];

static int basis   ( int c, float t, int npts, int *x, float *n );
static int knot    ( int n, int c, int *x );
static int bspline (int so_knot, int k, int so_spline, float *knots3D, float *interpolation3D );


/*  Subroutine to generate a B-spline curve using an uniform open knot vector

	C code for An Introduction to NURBS
	by David F. Rogers. Copyright (C) 2000 David F. Rogers,
	All rights reserved.
	
	Name: bspline.c
	Language: C
	Subroutines called: knot.c, basis.c, fmtmul.c
	Book reference: Section 3.5, Ex. 3.4, Alg. p. 281

    .- input args: 
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

    .- internal stuff: 
    t           = parameter value 0 <= t <= 1
    x[]         = array containing the knot vector
    nbasis      = array containing the basis functions for a single value of t
    nplusc      = number of knot values
    npts        = number of defining polygon vertices
    p[]         = array containing the curve points
*/

static int bspline( int so_knot, int k, int so_spline, float *knots, float *spline )
{
    float step, t;
    int no_knots = so_knot + k;

    /*  zero and redimension the knot vector and the basis array */
    memset( nbasis, 0, sizeof( float ) * so_knot );
    memset( x, 0, sizeof( float ) * no_knots );

    /* generate the uniform open knot vector */
    knot( so_knot, k, x );

    int n = 0;

    /* calculate the points on the bspline curve */
    t = 0;
    step = (( float ) x[ no_knots ] ) / ( ( float )( so_spline - 1 ));

    for( int s = 1; s <= so_spline; s++ )
    {
        if ( ( ( ( float )x[ no_knots ] ) - t ) < 5e-6)
            t = ( float )( x[ no_knots ] );

        /* generate the basis function for this value of t */
        basis( k, t, so_knot, x, nbasis );

        /* generate a point on the curve */
        for( int j = 1, m = 0; j <= 3; j++ )
        {
            m = j;
            spline[ n + j ] = 0.;

            /* Do local matrix multiplication */
            for( int i = 1; i <= so_knot; i++ )
            {
                spline[ n + j ] = spline[ n + j ] + ( nbasis[ i ] * knots[ m ] );
                m = m + 3;
            }
        }
        n = n + 3;
        t = t + step;
    }
    return 0;

}


/*  Subroutine to generate B-spline basis functions for open knot vectors

	C code for An Introduction to NURBS
	by David F. Rogers. Copyright (C) 2000 David F. Rogers,
	All rights reserved.
	
	Name: basis.c
	Language: C
	Subroutines called: none
	Book reference: p. 279

    c        = order of the B-spline basis function
    d        = first term of the basis function recursion relation
    e        = second term of the basis function recursion relation
    npts     = number of defining polygon vertices
    n[]      = array containing the basis functions
               n[1] contains the basis function associated with B1 etc.
    nplusc   = constant -- npts + c -- maximum number of knot values
    t        = parameter value
    temp[]   = temporary array
    x[]      = knot vector
*/	

static int basis( int c, float t, int no_vertex, int *x, float *n )
{
    int no_knots = no_vertex + c;
    /* calculate the first order basis functions n[i][1]	*/
    for( int i = 1; i <= no_knots-1; i++ )
        temp[ i ] = ( ( t >= x[ i ] ) && ( t < x[ i + 1 ] ) ) ? 1 : 0;

    /* calculate the higher order basis functions */
    for( int k = 2; k <= c; k++ )
    {
        for( int i = 1; i <= ( no_knots - k ); i++ )
        {
            /* if the lower order basis function is zero skip the calculation */
            float d = 0;
            if( temp[ i ] != 0 )
                d = ( ( t - x[ i ] ) * temp[ i ] ) / ( x [ i + k - 1 ] - x[ i ] );

            /* if the lower order basis function is zero skip the calculation */
            float e = 0;
            if( temp[ i + 1 ] != 0 )
                e = ( ( x[ i + k ] - t ) * temp[ i + 1 ] ) /( x[ i + k ] - x[ i + 1 ] );
            temp[ i ] = d + e;
        }
    }

    /*    pick up last point	*/
    if( t == ( ( float )( x[ no_knots ] ) ) )
        temp[ no_vertex ] = 1;

    /* put in n array	*/
    for( int i = 1; i <= no_vertex; i++)
        n[ i ] = temp[ i ];

    return 0;

}


/*
    Subroutine to generate a B-spline open knot vector with multiplicity
    equal to the order at the ends.
	
    c            = order of the basis function
    n            = the number of defining polygon vertices
    nplus2       = index of x() for the first occurence of the maximum knot vector value
    nplusc       = maximum value of the knot vector -- $n + c$
    x()          = array containing the knot vector
*/

static int knot( int n, int c, int *x )
{
    int nplusc, nplus2;

    nplusc = n + c;
    nplus2 = n + 2;

    x[ 1 ] = 0;
    for( int i = 2; i <= nplusc; i++ ){
        if( ( i > c ) && ( i < nplus2 ) )
            x[ i ] = x[ i -1 ] + 1;
        else
            x[ i ] = x[ i -1 ];
    }
    return( 1 );

}


int nurbs_generator( t_nurbs_iface *iface )
{

    if( ( iface->soInterpolation < 1 )||(  iface->soInterpolation >  LengthOfInterpolation) )
        return  NurbsLengthOutOfBounds;
    if( ( iface->order < 1 )||( iface->order > NurbsOrderBound ) )
        return NurbsOrderOutOfBounds;
    if( iface->noKnots > NurbsKnotBound )
        return NurbsKnotsOutOfBounds;


    for( int i = 0, k = 0; i < iface->noKnots; i++ )
    {
        k = ( 3 *i ) +1;
        knots3D[ k    ] = iface->data.x[ i ];
        knots3D[ k +1 ] = iface->data.y[ i ];
        knots3D[ k +2 ] = iface->data.z[ i ];
    }

    bspline( iface->noKnots, iface->order, iface->soInterpolation, knots3D, interpolation3D );

    for( int j = 0, i = 1; i <= ( 3 * iface->soInterpolation ); i = i + 3, j++ )
    {
        iface->data.x[ j ] = interpolation3D[ i    ];
        iface->data.y[ j ] = interpolation3D[ i +1 ];
        iface->data.z[ j ] = interpolation3D[ i +2 ];
    }
    return 0;

}
