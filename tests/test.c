
#include <stdio.h>

#include <libmacro/assert.h>
#include <libmacro/slice.h>
#include <libbase/int.h>
#include <libarray/array_char.h>

#include "../vec_int.h"
#include "../vec_ptr_long.h"
#include "../vec_char.h"
#include "../vec_uchar.h"


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
    Vec_ptr_long ys = vec_ptr_long__copy_vec( xs );
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
    vec_int__append_vec( &v, u );
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
    vec_int__append_els( &xs, 60, 70 );
    ASSERT( vec_int__equal_els( xs, 10, 20, 30, 40, 50, 60, 70 ),
            xs.capacity == 7 );
    vec_int__free( &xs );
    ASSERT( xs.e == NULL, xs.length == 0, xs.capacity == 0 );
}


static
void
test_insertion( void )
{
    Vec_int xs = vec_int__new_empty();
    ASSERT( xs.length == 0, xs.e == NULL );
    vec_int__append( &xs, 5 );
    ASSERT( vec_int__equal_els( xs, 5 ) );
    vec_int__append( &xs, 70 );
    ASSERT( vec_int__equal_els( xs, 5, 70 ) );
    vec_int__append( &xs, -1 );
    ASSERT( vec_int__equal_els( xs, 5, 70, -1 ) );
    vec_int__insert( &xs, 1, 88 );
    ASSERT( vec_int__equal_els( xs, 5, 88, 70, -1 ) );
    vec_int__insert( &xs, 0, 9 );
    ASSERT( vec_int__equal_els( xs, 9, 5, 88, 70, -1 ) );
    vec_int__insert_arrayc( &xs, 3, ARRAYC_INT( 14, 3 ) );
    ASSERT( vec_int__equal_els( xs, 9, 5, 88, 14, 3, 70, -1 ) );
    vec_int__free( &xs );
}


static
void
test_argparse( void )
{
    Vec_int xs = { 0 };
    vec_int__argparse( NULL, "123", &xs );
    ASSERT( vec_int__equal_els( xs, 123 ) );
    vec_int__argparse( NULL, "456", &xs );
    ASSERT( vec_int__equal_els( xs, 123, 456 ) );
    vec_int__argparse( NULL, "789", &xs );
    ASSERT( vec_int__equal_els( xs, 123, 456, 789 ) );
    vec_int__free( &xs );
    ASSERT( xs.e == NULL, xs.length == 0, xs.capacity == 0 );
}


static bool int_equals_6( int const x ) { return x == 6; }


static
void
test_replace( void )
{
    Vec_int const xs = VEC_INT( 3, 4, -8, 4, 4, 5, 4, -1 );

    Vec_int ys = vec_int__replaced( xs, 4, 6 );
    ASSERT( vec_int__equal( ys,
                ( Vec_int ) VEC_INT( 3, 6, -8, 6, 6, 5, 6, -1 ) ) );

    vec_int__replace( ys, 5, 0 );
    ASSERT( vec_int__equal( ys,
                ( Vec_int ) VEC_INT( 3, 6, -8, 6, 6, 0, 6, -1 ) ) );

    vec_int__replacef( ys, int__is_negative, 7 );
    ASSERT( vec_int__equal_els( ys,
                3, 6, 7, 6, 6, 0, 6, 7 ) );

    vec_int__replacef_arrayc( &ys, int_equals_6, ARRAYC_INT( -2, -3 ) );
    ASSERT( vec_int__equal_els( ys,
                3, -2, -3, 7, -2, -3, -2, -3, 0, -2, -3, 7 ) );

    Vec_int zs = vec_int__replacedf( ys, int__is_zero, 1 );
    ASSERT( vec_int__equal_els( zs,
                3, -2, -3, 7, -2, -3, -2, -3, 1, -2, -3, 7 ) );

    vec_int__free( &ys );
    vec_int__free( &zs );

    {
        Vec_int v = vec_int__new_empty();
        size_t const n = vec_int__replacef_arrayc(
                             &v, int__is_zero, ARRAYC_INT( 1, 2, 3 ) );
        ASSERT( n == 0, vec_int__is_empty( v ), v.e == NULL );
    } {
        Vec_int v = vec_int__new_els( 87 );
        size_t const n = vec_int__replacef_arraym(
                             &v, int__is_positive, ARRAYM_INT( 4, -4 ) );
        ASSERT( n == 1, vec_int__equal_els( v, 4, -4 ) );
        vec_int__free( &v );
    } {
        Vec_int v = vec_int__new_els( 87 );
        size_t const n = vec_int__replacef_arraym(
                             &v, int__is_negative, ARRAYM_INT( 4, -4 ) );
        ASSERT( n == 0, vec_int__equal_els( v, 87 ) );
        vec_int__free( &v );
    } {
        Vec_int v = vec_int__new_els( 87, -1, 3 );
        size_t const n = vec_int__replacef_arraym(
                             &v, int__is_negative, ARRAYM_INT( 4, -4 ) );
        ASSERT( n == 1, vec_int__equal_els( v, 87, 4, -4, 3 ) );
        vec_int__free( &v );
    } {
        Vec_int v = vec_int__new_els( 87, 3, -1 );
        size_t const n = vec_int__replacef_arraym(
                             &v, int__is_negative, ARRAYM_INT( 4, -4 ) );
        ASSERT( n == 1, vec_int__equal_els( v, 87, 3, 4, -4 ) );
        vec_int__free( &v );
    } {
        Vec_int v = vec_int__new_els( 87, 3, -1 );
        size_t const n1 = vec_int__replacef_arraym(
                              &v, int__is_negative, ARRAYM_INT( 4, -4 ) );
        ASSERT( n1 == 1, vec_int__equal_els( v, 87, 3, 4, -4 ) );
        size_t const n2 = vec_int__replacef_arraym(
                              &v, int__is_negative, ARRAYM_INT( 4, -4 ) );
        ASSERT( n2 == 1, vec_int__equal_els( v, 87, 3, 4, 4, -4 ) );
        vec_int__free( &v );
    } {
        Vec_int v = vec_int__new_els( 87, 3, -1 );
        size_t const n = vec_int__replacef_arraym(
                             &v, int__is_positive, ARRAYM_INT( 4, -4 ) );
        ASSERT( n == 2, vec_int__equal_els( v, 4, -4, 4, -4, -1 ) );
        vec_int__free( &v );
    } {
        Vec_char v = vec_char__copy_str( "hello $" );
        size_t const n = vec_char__replace_arrayc(
                             &v, '$', arrayc_char__view_buf( "world" ) );
        ASSERT( n == 1, vec_char__equal_str( v, "hello world" ) );
        vec_char__free( &v );
    } {
        Vec_char v = vec_char__copy_str( "hello X; hello!" );
        size_t const n = vec_char__replace_arrayc(
                             &v, 'X', arrayc_char__view_buf( "world" ) );
        ASSERT( n == 1, vec_char__equal_str( v, "hello world; hello!" ) );
        vec_char__free( &v );
    } {
        Vec_char v = vec_char__copy_str( "hello X; hello X" );
        size_t const n = vec_char__replace_arrayc(
                             &v, 'X', arrayc_char__view_buf( "world" ) );
        ASSERT( n == 2, vec_char__equal_str( v, "hello world; hello world" ) );
        vec_char__free( &v );
    } {
        Vec_char v = vec_char__copy_str( "x1x3x5x" );
        size_t const n = vec_char__replace_arrayc(
                             &v, 'x', arrayc_char__view_buf( NULL ) );
        ASSERT( n == 4, vec_char__equal_str( v, "135" ) );
        vec_char__free( &v );
    } {
        Vec_char v = vec_char__copy_str( "x1x3x5x" );
        size_t const n = vec_char__replace_arrayc(
                             &v, 'x', arrayc_char__view_buf( "x" ) );
        ASSERT( n == 4, vec_char__equal_str( v, "x1x3x5x" ) );
        vec_char__free( &v );
    }
}


static
void
test_pop( void )
{
    Vec_int xs = VEC_INT( 1, 2, 3, 4, 5 );

    int const a = vec_int__pop( &xs );
    ASSERT( a == 5, vec_int__equal( xs, ( Vec_int ) VEC_INT( 1, 2, 3, 4 ) ) );
    int const b = vec_int__pop( &xs );
    ASSERT( b == 4, vec_int__equal( xs, ( Vec_int ) VEC_INT( 1, 2, 3 ) ) );
    int const c = vec_int__pop( &xs );
    ASSERT( c == 3, vec_int__equal( xs, ( Vec_int ) VEC_INT( 1, 2 ) ) );
    int const d = vec_int__pop( &xs );
    ASSERT( d == 2, vec_int__equal( xs, ( Vec_int ) VEC_INT( 1 ) ) );
    int const e = vec_int__pop( &xs );
    ASSERT( e == 1, vec_int__is_empty( xs ) );
    vec_int__append( &xs, 10 );
    ASSERT( vec_int__equal( xs, ( Vec_int ) VEC_INT( 10 ) ) );
    int const f = vec_int__pop( &xs );
    ASSERT( f == 10, vec_int__is_empty( xs ) );
}


static
void
test_elem_or_append( void )
{
    Vec_int xs = vec_int__new_els( 1, 2, 3 );
    int * const one = vec_int__elem_or_append( &xs, 1 );
    ASSERT( one != NULL, *one == 1, one == vec_int__getp( xs, 0 ),
            vec_int__equal( xs, ( Vec_int ) VEC_INT( 1, 2, 3 ) ) );
    int * const two = vec_int__elem_or_append( &xs, 2 );
    ASSERT( two != NULL, *two == 2, two == vec_int__getp( xs, 1 ),
            vec_int__equal( xs, ( Vec_int ) VEC_INT( 1, 2, 3 ) ) );
    int * const four = vec_int__elem_or_append( &xs, 4 );
    ASSERT( four != NULL, *four == 4, four == vec_int__getp( xs, 3 ),
            vec_int__equal( xs, ( Vec_int ) VEC_INT( 1, 2, 3, 4 ) ) );
    int * const four2 = vec_int__elem_or_append( &xs, 4 );
    ASSERT( four2 != NULL, *four2 == 4, four2 == vec_int__getp( xs, 3 ),
            vec_int__equal( xs, ( Vec_int ) VEC_INT( 1, 2, 3, 4 ) ) );
    int * const five = vec_int__elem_or_append( &xs, 5 );
    ASSERT( five != NULL, *five == 5, five == vec_int__getp( xs, 4 ),
            vec_int__equal( xs, ( Vec_int ) VEC_INT( 1, 2, 3, 4, 5 ) ) );
    vec_int__free( &xs );
}


static
void
test_into( void )
{
    Vec_int xs = vec_int__new_els( 1, 2, 3 );
    Vec_int ys = vec_int__new_alloc( 0 );
    vec_int__copy_vec_into( &ys, xs );
    ASSERT( vec_int__equal( xs, ys ), xs.capacity == ys.capacity );
    vec_int__grow_capacity_by( &ys, 5 );
    vec_int__copy_vec_into( &ys, xs );
    ASSERT( vec_int__equal( xs, ys ), xs.capacity < ys.capacity );
    vec_int__append( &ys, 999 );
    ASSERT( vec_int__equal( ys, ( Vec_int ) VEC_INT( 1, 2, 3, 999 ) ) );
    vec_int__free( &xs );
    vec_int__free( &ys );
}


static
void
test_null( void )
{
    {
        Vec_int const v = vec_int__view_buf( NULL );
        ASSERT( vec_int__is_empty( v ) );
    } {
        Vec_int const v = vec_int__view_buf0( NULL );
        ASSERT( vec_int__is_empty( v ) );
    } {
        int buf[] = { 1, 2, 3, 0 };
        Vec_int const v = vec_int__view_buf( buf );
        ASSERT( vec_int__equal_els( v, 1, 2, 3 ) );
    } {
        int buf[] = { 0 };
        Vec_int const v = vec_int__view_buf( buf );
        ASSERT( vec_int__is_empty( v ) );
    } {
        int buf[] = { 1, 2, 3, 0 };
        Vec_int const v = vec_int__view_buf0( buf );
        ASSERT( vec_int__equal_els( v, 1, 2, 3, 0 ) );
    } {
        int buf[] = { 0 };
        Vec_int const v = vec_int__view_buf0( buf );
        ASSERT( vec_int__equal_els( v, 0 ) );
    }
}


static
void
test_char( void )
{
    {
        char const * const str = "hello";
        Vec_uchar v = vec_uchar__copy_str( str );
        ASSERT( vec_uchar__equal_els( v, SLICE( str, 0, 5 ) ),
                vec_uchar__equal_str( v, str ),
                !vec_uchar__equal_str0( v, str ) );
        vec_uchar__free( &v );
    } {
        char const * const str = "";
        Vec_uchar v = vec_uchar__copy_str( str );
        ASSERT( vec_uchar__is_empty( v ),
                vec_uchar__equal_str( v, str ),
                !vec_uchar__equal_str0( v, str ) );
        vec_uchar__free( &v );
    } {
        char const * const str = NULL;
        Vec_uchar v = vec_uchar__copy_str( str );
        ASSERT( vec_uchar__is_empty( v ),
                vec_uchar__equal_str( v, str ),
                vec_uchar__equal_str0( v, str ) );
        vec_uchar__free( &v );
    } {
        char const * const str = "hello";
        Vec_uchar v = vec_uchar__copy_str0( str );
        ASSERT( vec_uchar__equal_els( v, SLICE( str, 0, 6 ) ),
                !vec_uchar__equal_str( v, str ),
                vec_uchar__equal_str0( v, str ) );
        vec_uchar__free( &v );
    } {
        char const * const str = "";
        Vec_uchar v = vec_uchar__copy_str0( str );
        ASSERT( vec_uchar__equal_els( v, '\0' ),
                !vec_uchar__equal_str( v, str ),
                vec_uchar__equal_str0( v, str ) );
        vec_uchar__free( &v );
    } {
        char const * const str = NULL;
        Vec_uchar v = vec_uchar__copy_str0( str );
        ASSERT( vec_uchar__is_empty( v ),
                vec_uchar__equal_str( v, str ),
                vec_uchar__equal_str0( v, str ) );
        vec_uchar__free( &v );
    } {
        Vec_uchar v = vec_uchar__new_fmt( "blah %d test %s", 5, "hi" );
        ASSERT( vec_uchar__equal_str( v, "blah 5 test hi" ) );
        vec_uchar__free( &v );
    } {
        Vec_uchar v = vec_uchar__new_fmt( "%x %d %s", 0xf9, -3, "cool" );
        ASSERT( vec_uchar__equal_str( v, "f9 -3 cool" ) );
        vec_uchar__fmt_into( &v, "%s replacement", "some" );
        ASSERT( vec_uchar__equal_str( v, "some replacement" ) );
        vec_uchar__fmt_into( &v, "blah" );
        ASSERT( vec_uchar__equal_str( v, "blah" ) );
        vec_uchar__append_fmt( &v, " %d %d %d", 1, 2, 3 );
        ASSERT( vec_uchar__equal_str( v, "blah 1 2 3" ) );
        vec_uchar__append_fmt( &v, " %d", 4 );
        ASSERT( vec_uchar__equal_str( v, "blah 1 2 3 4" ) );
        vec_uchar__free( &v );
    }
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
    test_insertion();
    printf( "  insertion tests passed\n" );
    test_argparse();
    printf( "  arg parse tests passed\n" );
    test_replace();
    printf( "  replace tests passed\n" );
    test_pop();
    printf( "  pop tests passed\n" );
    test_elem_or_append();
    printf( "  elem_or_append() tests passed\n" );
    test_into();
    printf( "  into() tests passed\n" );

    test_null();        puts( "  NULL typeclass tests passed" );
    test_char();        puts( "  CHAR typeclass tests passed" );
    printf( "All tests passed!\n" );
}

