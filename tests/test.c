
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


int
main( void )
{
    printf( "Running tests...\n" );
    test_vec_int();
    printf( "  Vec_int tests passed.\n" );
    test_vec_ptr_long();
    printf( "  Vec_ptr_long tests passed.\n" );
    test_capacity_growth();
    printf( "  capacity growth tests passed.\n" );
    printf( "All tests passed!\n" );
}

