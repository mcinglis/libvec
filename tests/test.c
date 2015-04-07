
#include <stdio.h>

#include <libmacro/assert.h>
#include <libbase/int.h>

#include "../vec-int.h"
#include "../vec-ptr-long.h"


static
void
test_vec_int( void )
{
    Vec_int xs = vec_int__new( ( int[] ){ 1, 2, 3, 4, 5 },
                               5, 10 );
    Vec_int ys = vec_int__new( ( int[] ){ 1, 2, 3, 4, 5, 999 },
                               5, 10 );

    ASSERT( vec_int__is_valid( xs ),
            vec_int__is_valid( ys ),
            xs.length == 5,
            xs.length == ys.length,
            xs.e[ 0 ] == 1, xs.e[ 1 ] == 2, xs.e[ 2 ] == 3,
            xs.e[ 3 ] == 4, xs.e[ 4 ] == 5,
            vec_int__equal( xs, ys ),
            vec_int__all( xs, int__is_positive ) );

    vec_int__free( &xs );
    vec_int__free( &ys );

    ASSERT( vec_int__is_valid( xs ),
            vec_int__is_valid( ys ),
            xs.length == 0, xs.e == NULL,
            ys.length == 0, ys.e == NULL );
}



static
bool
ptr_long_equal_val( long const * const x,
                    long const * const y )
{
    ASSERT( x != NULL, y != NULL );

    return *x == *y;
}


static
void
test_vec_ptr_long( void )
{
    Vec_ptr_long xs = vec_ptr_long__new(
                          ( long const * [] ){
                              &( long ){ 1 }, &( long ){ 2 },
                              &( long ){ 4 }, &( long ){ 8 } },
                          4, 4 );
    Vec_ptr_long ys = vec_ptr_long__copy( xs );
    Vec_ptr_long zs = vec_ptr_long__new(
                          ( long const * [] ){
                              &( long ){ 1 }, &( long ){ 2 },
                              &( long ){ 4 }, &( long ){ 8 } },
                          4, 4 );
    ASSERT( xs.length == 4,
            !vec_ptr_long__same( xs, ys ),
            !vec_ptr_long__same( xs, zs ),
            vec_ptr_long__equal( xs, ys ),
            !vec_ptr_long__equal( xs, zs ),
            vec_ptr_long__equal_by( xs, ys, ptr_long_equal_val ),
            vec_ptr_long__equal_by( xs, zs, ptr_long_equal_val ) );
}


static
void
test_capacity_growth( void )
{
    Vec_int v = { 0 };
    ASSERT( v.capacity == 0 );
    vec_int__ensure_capacity( &v, 32 );
    ASSERT( vec_int__equal( v, ( Vec_int ){ 0 } ), v.capacity == 32 );
    Vec_int u = VEC_INT( 123, 345, 678 );
    vec_int__extend_vec( &v, u );
    ASSERT( vec_int__equal( v, u ), v.capacity == 32 );
    vec_int__grow_capacity_by( &v, 5 );
    ASSERT( vec_int__equal( v, u ), v.capacity == 37 );
    vec_int__ensure_capacity( &v, 32 );
    ASSERT( vec_int__equal( v, u ), v.capacity == 37 );
    vec_int__free( &v );
}


static
void
test_filtering( void )
{
    Vec_int a = VEC_INT( 0, 1, 2, 3, 4 );
    int * const a_els = a.e;
    ASSERT( a.length == 5, a.capacity == 5 );
    vec_int__filter( &a, int__is_odd );
    ASSERT( vec_int__equal( a, ( Vec_int ) VEC_INT( 1, 3 ) ),
            a.e == a_els );

    Vec_int b = VEC_INT( 5, 6, 7, 8, 9 );
    ASSERT( b.length == 5, b.capacity == 5 );
    Vec_int bf = vec_int__filtered( b, int__is_even );
    ASSERT( vec_int__equal( b, ( Vec_int ) VEC_INT( 5, 6, 7, 8, 9 ) ),
            vec_int__equal( bf, ( Vec_int ) VEC_INT( 6, 8 ) ),
            bf.e != b.e );
    vec_int__free( &bf );
}


static
void
test_els_funcs( void )
{
    Vec_int xs = vec_int__new_els( 10, 20, 30, 40 );
    ASSERT( vec_int__equal_els( xs, 10, 20, 30, 40 ) );
    vec_int__grow_capacity_by( &xs, 3 );
    ASSERT( vec_int__equal_els( xs, 10, 20, 30, 40 ),
            xs.capacity == 7 );
    vec_int__append( &xs, 50 );
    ASSERT( vec_int__equal_els( xs, 10, 20, 30, 40, 50 ),
            xs.capacity == 7 );
    vec_int__extend_els( &xs, 60, 70 );
    ASSERT( vec_int__equal_els( xs, 10, 20, 30, 40, 50, 60, 70 ),
            xs.capacity == 7 );
    vec_int__free( &xs );
    ASSERT( xs.e == NULL, xs.length == 0, xs.capacity == 0 );
}


static
void
test_arg_parse( void )
{
    Vec_int xs = { 0 };
    vec_int__arg_parse( NULL, "123", &xs );
    ASSERT( vec_int__equal_els( xs, 123 ) );
    vec_int__arg_parse( NULL, "456", &xs );
    ASSERT( vec_int__equal_els( xs, 123, 456 ) );
    vec_int__arg_parse( NULL, "789", &xs );
    ASSERT( vec_int__equal_els( xs, 123, 456, 789 ) );
    vec_int__free( &xs );
    ASSERT( xs.e == NULL, xs.length == 0, xs.capacity == 0 );
}


static
void
test_replace( void )
{
    Vec_int const xs = VEC_INT( 3, 4, -8, 4, 4, 5, 4, -1 );

    Vec_int const ys = vec_int__replaced( xs, 4, 6 );
    ASSERT( vec_int__equal( ys,
                ( Vec_int ) VEC_INT( 3, 6, -8, 6, 6, 5, 6, -1 ) ) );

    vec_int__replace( ys, 5, 0 );
    ASSERT( vec_int__equal( ys,
                ( Vec_int ) VEC_INT( 3, 6, -8, 6, 6, 0, 6, -1 ) ) );

    vec_int__replacef( ys, int__is_negative, 7 );
    ASSERT( vec_int__equal( ys,
                ( Vec_int ) VEC_INT( 3, 6, 7, 6, 6, 0, 6, 7 ) ) );

    Vec_int const zs = vec_int__replacedf( ys, int__is_zero, 1 );
    ASSERT( vec_int__equal( zs,
                ( Vec_int ) VEC_INT( 3, 6, 7, 6, 6, 1, 6, 7 ) ) );

    vec_int__freev( ys );
    vec_int__freev( zs );
}


int
main( void )
{
    printf( "Running tests...\n" );
    test_vec_int();
    printf( "  Vec_int tests passed\n" );
    test_vec_ptr_long();
    printf( "  Vec_ptr_long tests passed\n" );
    test_capacity_growth();
    printf( "  capacity growth tests passed\n" );
    test_filtering();
    printf( "  filtering tests passed\n" );
    test_els_funcs();
    printf( "  els function tests passed\n" );
    test_arg_parse();
    printf( "  arg parse tests passed\n" );
    test_replace();
    printf( "  replace tests passed\n" );
    printf( "All tests passed!\n" );
}

