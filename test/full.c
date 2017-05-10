/**
 * fcontext unit test suite
 *
 *                Copyright Nick Vrvilo 2017.
 *            https://github.com/DaoWen/fcontext/
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE_1_0.txt or copy at
 *           http://www.boost.org/LICENSE_1_0.txt)
 *
 * This test suite is derived from the fcontext API tests in Boost.Context,
 * which includes the following obligatory copyright notice:
 *
 *              Copyright Oliver Kowalke 2009.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE_1_0.txt or copy at
 *           http://www.boost.org/LICENSE_1_0.txt)
 *
 * The original test_fcontext.cpp source file is available on GitHub:
 * https://github.com/boostorg/context/blob/boost-1.64.0/test/test_fcontext.cpp
 *
 * All code in this file should be considered a derivative of the sources
 * listed above, and is distributed under the same license terms.
 */

#include <stdio.h>
#include <assert.h>

static unsigned _total_tests;

#define TEST_RUN(testfn) do { \
    ++_total_tests; \
    fprintf(stderr, ">>> TEST >>> Starting test #%d: %s\n", _total_tests, #testfn); \
    testfn(); \
    fprintf(stderr, ">>> TEST >>> Finished test #%d: %s\n", _total_tests, #testfn); \
} while (0)

#define TEST_SUMMARY() do { \
    fprintf(stderr, ">>> TEST >>> %d tests run successfully!\n", _total_tests); \
} while (0)

#define TEST_CHECK(cond) assert(cond)
#define TEST_CHECK_EQ(lhs, rhs) assert((lhs) == (rhs))

#include "fcontext.h"

int value1;
const char *value2;
double value3;
void *value4;

void f1( fcontext_transfer_t t) {
    ++value1;
    jump_fcontext( t.prev_context, t.data);
}

void f3( fcontext_transfer_t t_) {
    ++value1;
    fcontext_transfer_t t = jump_fcontext( t_.prev_context, 0);
    ++value1;
    jump_fcontext( t.prev_context, t.data);
}

void f4( fcontext_transfer_t t) {
    int i = 7;
    jump_fcontext( t.prev_context, & i);
}

void f5( fcontext_transfer_t t) {
    jump_fcontext( t.prev_context, t.data);
}

void f6( fcontext_transfer_t t_) {
    int *data = t_.data;
    int res = data[0] + data[1];
    fcontext_transfer_t t = jump_fcontext( t_.prev_context, & res);
    data = t_.data;
    res = data[0] + data[1];
    jump_fcontext( t.prev_context, & res);
}

void f7( fcontext_transfer_t t) {
    // TODO - replace with setjmp longjmp?
    //try {
    //    throw std::runtime_error( * ( std::string *) t.data);
    //} catch ( std::runtime_error const& e) {
    //    value2 = e.what();
    value2 = t.data;
    //}
    jump_fcontext( t.prev_context, t.data);
}

void f8( fcontext_transfer_t t) {
    double d = * ( double *) t.data;
    d += 3.45;
    value3 = d;
    jump_fcontext( t.prev_context, 0);
}

void f10( fcontext_transfer_t t) {
    value1 = 3;
    jump_fcontext( t.prev_context, 0);
}

void f9( fcontext_transfer_t t) {
    printf("f1: entered\n");
    fcontext_state_t *state = fcontext_create(f10);
    fcontext_t ctx = state->context;
    jump_fcontext( ctx, 0);
    jump_fcontext( t.prev_context, 0);
}

fcontext_transfer_t f11( fcontext_transfer_t t_) {
    value4 = t_.data;
    fcontext_transfer_t t = { t_.prev_context, t_.data };
    return t;
}

void f12( fcontext_transfer_t t_) {
    fcontext_transfer_t t = jump_fcontext( t_.prev_context, t_.data);
    value1 = * ( int *) t.data;
    jump_fcontext( t.prev_context, t.data);
}

void test_setup() {
    fcontext_state_t *state = fcontext_create(f1);
    fcontext_t ctx = state->context;
    TEST_CHECK( ctx);
    fcontext_destroy(state);
}

void test_start() {
    value1 = 0;
    fcontext_state_t *state = fcontext_create(f1);
    fcontext_t ctx = state->context;
    TEST_CHECK( ctx);

    TEST_CHECK_EQ( 0, value1);
    jump_fcontext( ctx, 0);
    TEST_CHECK_EQ( 1, value1);
    fcontext_destroy(state);
}

void test_jump() {
    value1 = 0;
    fcontext_state_t *state = fcontext_create(f3);
    fcontext_t ctx = state->context;
    TEST_CHECK( ctx);
    TEST_CHECK_EQ( 0, value1);
    fcontext_transfer_t t = jump_fcontext( ctx, 0);
    TEST_CHECK_EQ( 1, value1);
    jump_fcontext( t.prev_context, 0);
    TEST_CHECK_EQ( 2, value1);
    fcontext_destroy(state);
}

void test_result() {
    fcontext_state_t *state = fcontext_create(f4);
    fcontext_t ctx = state->context;
    TEST_CHECK( ctx);
    fcontext_transfer_t t = jump_fcontext( ctx, 0);
    int result = * ( int *) t.data;
    TEST_CHECK_EQ( 7, result);
    fcontext_destroy(state);
}

void test_arg() {
    int i = 7;
    fcontext_state_t *state = fcontext_create(f5);
    fcontext_t ctx = state->context;
    TEST_CHECK( ctx);
    fcontext_transfer_t t = jump_fcontext( ctx, & i);
    int result = * ( int *) t.data;
    TEST_CHECK_EQ( i, result);
    fcontext_destroy(state);
}

void test_transfer() {
    int data[] = { 3, 7 };
    fcontext_state_t *state = fcontext_create(f6);
    fcontext_t ctx = state->context;
    TEST_CHECK( ctx);
    fcontext_transfer_t t = jump_fcontext( ctx, data);
    int result = * ( int *) t.data;
    TEST_CHECK_EQ( 10, result);
    data[0] = 7; data[1] = 7;
    t = jump_fcontext( t.prev_context, data);
    result = * ( int *) t.data;
    TEST_CHECK_EQ( 14, result);
    fcontext_destroy(state);
}

void test_exception() {
    char what[] = "hello world";
    fcontext_state_t *state = fcontext_create(f7);
    fcontext_t ctx = state->context;
    TEST_CHECK( ctx);
    jump_fcontext( ctx, what);
    TEST_CHECK_EQ( what, value2);
    fcontext_destroy(state);
}

void test_fp() {
    double d = 7.13;
    fcontext_state_t *state = fcontext_create(f8);
    fcontext_t ctx = state->context;
    TEST_CHECK( ctx);
    jump_fcontext( ctx, & d);
    TEST_CHECK_EQ( 10.58, value3);
    fcontext_destroy(state);
}

void test_stacked() {
    value1 = 0;
    fcontext_state_t *state = fcontext_create(f9);
    fcontext_t ctx = state->context;
    jump_fcontext( ctx, 0);
    TEST_CHECK_EQ( 3, value1);
    fcontext_destroy(state);
}

void test_ontop() {
    value1 = 0;
    value4 = 0;
    fcontext_state_t *state = fcontext_create(f12);
    fcontext_t ctx = state->context;
    TEST_CHECK( ctx);
    fcontext_transfer_t t = jump_fcontext( ctx, 0);
    TEST_CHECK_EQ( 0, value1);
    TEST_CHECK( 0 == value4);
    int i = -3;
    t = ontop_fcontext( t.prev_context, & i, f11);
    TEST_CHECK_EQ( -3, value1);
    TEST_CHECK_EQ( & i, value4);
    TEST_CHECK_EQ( -3, * ( int *) t.data);
    TEST_CHECK_EQ( & i, ( int *) t.data);
    fcontext_destroy(state);
}

int main(int argc, char* argv[]) {
    TEST_RUN(test_setup);
    TEST_RUN(test_start);
    TEST_RUN(test_jump);
    TEST_RUN(test_result);
    TEST_RUN(test_arg);
    TEST_RUN(test_transfer);
    TEST_RUN(test_exception);
    TEST_RUN(test_fp);
    TEST_RUN(test_stacked);
    TEST_RUN(test_ontop);

    TEST_SUMMARY();
    return 0;
}
