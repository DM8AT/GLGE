/**
 * @file MathTestSuite.cpp
 * @author DM8AT
 * @brief a testing suite for the GLGE math system
 * @version 0.1
 * @date 2026-05-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add GLGE
#include "GLGE.h"
//add the testing contract
#include "TestContract.h"

//add stringstreams
#include <sstream>

template <typename vecT, typename Fn>
void vec2Test(vecT a, vecT b, vecT expected, Fn invoke, const char* op, const TestFunctions* fn) {
    //compute the value
    vecT val = invoke(a, b);
    //print an assertion
    TestAssertion ass;
    std::stringstream ex;
    ex << "(x: " << a.x << ", y: " << a.y << ") " << op << " (x: " << b.x << ", y: " << b.y << ") = (x: " << expected.x << ", y: " << expected.y << ")";
    std::string ex_str = ex.str();
    std::stringstream ac;
    ac << "(x: " << a.x << ", y: " << a.y << ") " << op << " (x: " << b.x << ", y: " << b.y << ") = (x: " << val.x << ", y: " << val.y << ")";
    std::string ac_str = ac.str();
    ass.expected = ex_str.c_str();
    ass.actual = ac_str.c_str();
    ass.passed = (expected.x == val.x) && (expected.y == val.y);
    //assert
    (*(fn->assertion))(&ass);
}

template <typename T, typename vecT, typename Fn, typename CmpFn>
void vec2GeomTest(vecT v, T res, Fn invoke, CmpFn cmp, const std::string descr, const TestFunctions* fn) {
    //compute the value
    T val = invoke(v);
    //print an assertion
    TestAssertion ass;
    std::stringstream ex;
    ex << "Calling " << descr << " on vector (x: " << v.x << ", y: " << v.y << ") should result in " << res;
    std::string ex_str = ex.str();
    std::stringstream ac;
    ac << "Calling " << descr << " on vector (x: " << v.x << ", y: " << v.y << ") resulted in " << val;
    std::string ac_str = ac.str();
    ass.expected = ex_str.c_str();
    ass.actual = ac_str.c_str();
    ass.passed = cmp(val, res);
    //assert
    (*(fn->assertion))(&ass);
}

void vec2Test(const TestContext* ctx, TestReport* report, const TestFunctions* fn) {
    //on fail, fail controlled
    report->result = TEST_CONTROLLED_FAIL;

    //message
    TestMessage msg;
    msg.msg = "[INFO] Testing float 2D vectors";
    (*(fn->log))(&msg);

    //test some addition
    vec2Test(GLGE::vec2( 7,  9), GLGE::vec2( 3,  4), GLGE::vec2(10, 13), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::vec2(-2,  5), GLGE::vec2(-5,  2), GLGE::vec2(-7,  7), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::vec2(12,  2), GLGE::vec2(10, -1), GLGE::vec2(22,  1), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::vec2( 0,  0), GLGE::vec2(-4,  7), GLGE::vec2(-4,  7), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::vec2(55, 30), GLGE::vec2(50, 10), GLGE::vec2(105,40), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::vec2( 3, -8), GLGE::vec2( 1, -4), GLGE::vec2( 4,-12), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::vec2(15, 15), GLGE::vec2( 7,  8), GLGE::vec2(22, 23), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::vec2(-6, -6), GLGE::vec2(-3, -3), GLGE::vec2(-9, -9), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::vec2( 8,  1), GLGE::vec2( 8,  0), GLGE::vec2(16,  1), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::vec2(23, 11), GLGE::vec2(12,  5), GLGE::vec2(35, 16), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::vec2( 1,  6), GLGE::vec2(-2,  3), GLGE::vec2(-1,  9), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::vec2(14,  5), GLGE::vec2( 9,  1), GLGE::vec2(23,  6), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::vec2(-1,  9), GLGE::vec2( 4,  5), GLGE::vec2( 3, 14), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::vec2(32, 48), GLGE::vec2(20, 40), GLGE::vec2(52, 88), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a + b;}, "+", fn);

    //test some subtraction
    vec2Test(GLGE::vec2( 7,  9), GLGE::vec2( 3,  4), GLGE::vec2( 4,  5), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::vec2(-2,  5), GLGE::vec2(-5,  2), GLGE::vec2( 3,  3), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::vec2(12,  2), GLGE::vec2(10, -1), GLGE::vec2( 2,  3), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::vec2( 0,  0), GLGE::vec2(-4,  7), GLGE::vec2( 4, -7), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::vec2(55, 30), GLGE::vec2(50, 10), GLGE::vec2( 5, 20), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::vec2( 3, -8), GLGE::vec2( 1, -4), GLGE::vec2( 2, -4), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::vec2(15, 15), GLGE::vec2( 7,  8), GLGE::vec2( 8,  7), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::vec2(-6, -6), GLGE::vec2(-3, -3), GLGE::vec2(-3, -3), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::vec2( 8,  1), GLGE::vec2( 8,  0), GLGE::vec2( 0,  1), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::vec2(23, 11), GLGE::vec2(12,  5), GLGE::vec2(11,  6), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::vec2( 1,  6), GLGE::vec2(-2,  3), GLGE::vec2( 3,  3), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::vec2(14,  5), GLGE::vec2( 9,  1), GLGE::vec2( 5,  4), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::vec2(-1,  9), GLGE::vec2( 4,  5), GLGE::vec2(-5,  4), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::vec2(32, 48), GLGE::vec2(20, 40), GLGE::vec2(12,  8), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a - b;}, "-", fn);

    //test some multiplication
    vec2Test(GLGE::vec2( 3,  4), GLGE::vec2( 2,  5), GLGE::vec2( 6, 20), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a * b;}, "*", fn);
    vec2Test(GLGE::vec2(-5,  2), GLGE::vec2( 3, -3), GLGE::vec2(-15,-6), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a * b;}, "*", fn);
    vec2Test(GLGE::vec2(10, -1), GLGE::vec2( 0,  4), GLGE::vec2( 0, -4), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a * b;}, "*", fn);
    vec2Test(GLGE::vec2( 4,  5), GLGE::vec2( 3,  2), GLGE::vec2(12, 10), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a * b;}, "*", fn);

    //test some division
    vec2Test(GLGE::vec2(12, 20), GLGE::vec2( 3,  4), GLGE::vec2( 4,  5), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a / b;}, "/", fn);
    vec2Test(GLGE::vec2(-15, 9), GLGE::vec2( 5, -3), GLGE::vec2(-3, -3), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a / b;}, "/", fn);
    vec2Test(GLGE::vec2( 8,  0), GLGE::vec2( 2,  1), GLGE::vec2( 4,  0), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a / b;}, "/", fn);
    vec2Test(GLGE::vec2(50, 40), GLGE::vec2(10,  4), GLGE::vec2( 5, 10), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a / b;}, "/", fn);

    //message
    msg.msg = "[INFO] Testing unsigned 32 bit integer 2D vectors";
    (*(fn->log))(&msg);

    //test some addition
    vec2Test(GLGE::uvec2( 7,  9), GLGE::uvec2( 3,  4), GLGE::uvec2(10, 13), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::uvec2(-2,  5), GLGE::uvec2(-5,  2), GLGE::uvec2(-7,  7), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::uvec2(12,  2), GLGE::uvec2(10, -1), GLGE::uvec2(22,  1), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::uvec2( 0,  0), GLGE::uvec2(-4,  7), GLGE::uvec2(-4,  7), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::uvec2(55, 30), GLGE::uvec2(50, 10), GLGE::uvec2(105,40), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::uvec2( 3, -8), GLGE::uvec2( 1, -4), GLGE::uvec2( 4,-12), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::uvec2(15, 15), GLGE::uvec2( 7,  8), GLGE::uvec2(22, 23), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::uvec2(-6, -6), GLGE::uvec2(-3, -3), GLGE::uvec2(-9, -9), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::uvec2( 8,  1), GLGE::uvec2( 8,  0), GLGE::uvec2(16,  1), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::uvec2(23, 11), GLGE::uvec2(12,  5), GLGE::uvec2(35, 16), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::uvec2( 1,  6), GLGE::uvec2(-2,  3), GLGE::uvec2(-1,  9), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::uvec2(14,  5), GLGE::uvec2( 9,  1), GLGE::uvec2(23,  6), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::uvec2(-1,  9), GLGE::uvec2( 4,  5), GLGE::uvec2( 3, 14), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::uvec2(32, 48), GLGE::uvec2(20, 40), GLGE::uvec2(52, 88), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a + b;}, "+", fn);

    //test some subtraction
    vec2Test(GLGE::uvec2( 7,  9), GLGE::uvec2( 3,  4), GLGE::uvec2( 4,  5), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::uvec2(-2,  5), GLGE::uvec2(-5,  2), GLGE::uvec2( 3,  3), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::uvec2(12,  2), GLGE::uvec2(10, -1), GLGE::uvec2( 2,  3), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::uvec2( 0,  0), GLGE::uvec2(-4,  7), GLGE::uvec2( 4, -7), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::uvec2(55, 30), GLGE::uvec2(50, 10), GLGE::uvec2( 5, 20), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::uvec2( 3, -8), GLGE::uvec2( 1, -4), GLGE::uvec2( 2, -4), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::uvec2(15, 15), GLGE::uvec2( 7,  8), GLGE::uvec2( 8,  7), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::uvec2(-6, -6), GLGE::uvec2(-3, -3), GLGE::uvec2(-3, -3), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::uvec2( 8,  1), GLGE::uvec2( 8,  0), GLGE::uvec2( 0,  1), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::uvec2(23, 11), GLGE::uvec2(12,  5), GLGE::uvec2(11,  6), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::uvec2( 1,  6), GLGE::uvec2(-2,  3), GLGE::uvec2( 3,  3), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::uvec2(14,  5), GLGE::uvec2( 9,  1), GLGE::uvec2( 5,  4), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::uvec2(-1,  9), GLGE::uvec2( 4,  5), GLGE::uvec2(-5,  4), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::uvec2(32, 48), GLGE::uvec2(20, 40), GLGE::uvec2(12,  8), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a - b;}, "-", fn);

    //test some multiplication
    vec2Test(GLGE::uvec2( 3,  4), GLGE::uvec2( 2,  5), GLGE::uvec2( 6, 20), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a * b;}, "*", fn);
    vec2Test(GLGE::uvec2(10,  1), GLGE::uvec2( 0,  4), GLGE::uvec2( 0,  4), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a * b;}, "*", fn);
    vec2Test(GLGE::uvec2( 4,  5), GLGE::uvec2( 3,  2), GLGE::uvec2(12, 10), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a * b;}, "*", fn);

    //test some division
    vec2Test(GLGE::uvec2(12, 20), GLGE::uvec2( 3,  4), GLGE::uvec2( 4,  5), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a / b;}, "/", fn);
    vec2Test(GLGE::uvec2( 8,  4), GLGE::uvec2( 2,  1), GLGE::uvec2( 4,  4), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a / b;}, "/", fn);
    vec2Test(GLGE::uvec2(50, 40), GLGE::uvec2(10,  4), GLGE::uvec2( 5, 10), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a / b;}, "/", fn);

    //message
    msg.msg = "[INFO] Testing signed 32 bit integer 2D vectors";
    (*(fn->log))(&msg);

    //test some addition
    vec2Test(GLGE::ivec2( 7,  9), GLGE::ivec2( 3,  4), GLGE::ivec2(10, 13), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::ivec2(-2,  5), GLGE::ivec2(-5,  2), GLGE::ivec2(-7,  7), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::ivec2(12,  2), GLGE::ivec2(10, -1), GLGE::ivec2(22,  1), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::ivec2( 0,  0), GLGE::ivec2(-4,  7), GLGE::ivec2(-4,  7), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::ivec2(55, 30), GLGE::ivec2(50, 10), GLGE::ivec2(105,40), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::ivec2( 3, -8), GLGE::ivec2( 1, -4), GLGE::ivec2( 4,-12), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::ivec2(15, 15), GLGE::ivec2( 7,  8), GLGE::ivec2(22, 23), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::ivec2(-6, -6), GLGE::ivec2(-3, -3), GLGE::ivec2(-9, -9), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::ivec2( 8,  1), GLGE::ivec2( 8,  0), GLGE::ivec2(16,  1), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::ivec2(23, 11), GLGE::ivec2(12,  5), GLGE::ivec2(35, 16), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::ivec2( 1,  6), GLGE::ivec2(-2,  3), GLGE::ivec2(-1,  9), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::ivec2(14,  5), GLGE::ivec2( 9,  1), GLGE::ivec2(23,  6), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::ivec2(-1,  9), GLGE::ivec2( 4,  5), GLGE::ivec2( 3, 14), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a + b;}, "+", fn);
    vec2Test(GLGE::ivec2(32, 48), GLGE::ivec2(20, 40), GLGE::ivec2(52, 88), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a + b;}, "+", fn);

    //test some subtraction
    vec2Test(GLGE::ivec2( 7,  9), GLGE::ivec2( 3,  4), GLGE::ivec2( 4,  5), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::ivec2(-2,  5), GLGE::ivec2(-5,  2), GLGE::ivec2( 3,  3), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::ivec2(12,  2), GLGE::ivec2(10, -1), GLGE::ivec2( 2,  3), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::ivec2( 0,  0), GLGE::ivec2(-4,  7), GLGE::ivec2( 4, -7), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::ivec2(55, 30), GLGE::ivec2(50, 10), GLGE::ivec2( 5, 20), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::ivec2( 3, -8), GLGE::ivec2( 1, -4), GLGE::ivec2( 2, -4), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::ivec2(15, 15), GLGE::ivec2( 7,  8), GLGE::ivec2( 8,  7), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::ivec2(-6, -6), GLGE::ivec2(-3, -3), GLGE::ivec2(-3, -3), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::ivec2( 8,  1), GLGE::ivec2( 8,  0), GLGE::ivec2( 0,  1), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::ivec2(23, 11), GLGE::ivec2(12,  5), GLGE::ivec2(11,  6), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::ivec2( 1,  6), GLGE::ivec2(-2,  3), GLGE::ivec2( 3,  3), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::ivec2(14,  5), GLGE::ivec2( 9,  1), GLGE::ivec2( 5,  4), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::ivec2(-1,  9), GLGE::ivec2( 4,  5), GLGE::ivec2(-5,  4), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a - b;}, "-", fn);
    vec2Test(GLGE::ivec2(32, 48), GLGE::ivec2(20, 40), GLGE::ivec2(12,  8), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a - b;}, "-", fn);

    //test some multiplication
    vec2Test(GLGE::ivec2( 3,  4), GLGE::ivec2( 2,  5), GLGE::ivec2( 6, 20), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a * b;}, "*", fn);
    vec2Test(GLGE::ivec2(-5,  2), GLGE::ivec2( 3, -3), GLGE::ivec2(-15,-6), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a * b;}, "*", fn);
    vec2Test(GLGE::ivec2(10, -1), GLGE::ivec2( 0,  4), GLGE::ivec2( 0, -4), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a * b;}, "*", fn);
    vec2Test(GLGE::ivec2( 4,  5), GLGE::ivec2( 3,  2), GLGE::ivec2(12, 10), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a * b;}, "*", fn);

    //test some division
    vec2Test(GLGE::ivec2(12, 20), GLGE::ivec2( 3,  4), GLGE::ivec2( 4,  5), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a / b;}, "/", fn);
    vec2Test(GLGE::ivec2(-15, 9), GLGE::ivec2( 5, -3), GLGE::ivec2(-3, -3), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a / b;}, "/", fn);
    vec2Test(GLGE::ivec2( 8,  0), GLGE::ivec2( 2,  1), GLGE::ivec2( 4,  0), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a / b;}, "/", fn);
    vec2Test(GLGE::ivec2(50, 40), GLGE::ivec2(10,  4), GLGE::ivec2( 5, 10), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a / b;}, "/", fn);

    //message
    msg.msg = "[INFO] Testing special cases";
    (*(fn->log))(&msg);

    //division by zero for 2D vector
    vec2Test(GLGE::vec2(1, 1), GLGE::vec2(0, 0), GLGE::vec2(1./0.,  1./0.), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a / b;}, "/", fn);
    //test uvec2 underflow (0 - 1 wraps to UINT_MAX)
    vec2Test(GLGE::uvec2(0, 0), GLGE::uvec2(1, 1), GLGE::uvec2(4294967295, 4294967295), [](GLGE::uvec2 a, GLGE::uvec2 b) -> GLGE::uvec2 {return a - b;}, "-", fn);

    //addition overflow (INT_MAX + 1 wraps to INT_MIN)
    vec2Test(GLGE::ivec2(2147483647, 2147483647), GLGE::ivec2(1, 1), GLGE::ivec2(-2147483648, -2147483648), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a + b;}, "+", fn);
    //subtraction underflow (INT_MIN - 1 wraps to INT_MAX)
    vec2Test(GLGE::ivec2(-2147483648, -2147483648), GLGE::ivec2(1, 1), GLGE::ivec2(2147483647, 2147483647), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a - b;}, "-", fn);

    //multiplication by Zero
    vec2Test(GLGE::vec2(0, 0), GLGE::vec2(55, -23), GLGE::vec2(0, 0), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a * b;}, "*", fn);
    //multiplication by One
    vec2Test(GLGE::vec2(1, 1), GLGE::vec2(55, -23), GLGE::vec2(55, -23), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a * b;}, "*", fn);

    //mixed signs and decimals
    vec2Test(GLGE::vec2(-0.5f, 10.5f), GLGE::vec2(0.5f, -10.5f), GLGE::vec2(0.0f, 0.0f), [](GLGE::vec2 a, GLGE::vec2 b) -> GLGE::vec2 {return a + b;}, "+", fn);

    //one component goes up, one goes down
    vec2Test(GLGE::ivec2(100, 100), GLGE::ivec2(100, -100), GLGE::ivec2(200, 0), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a + b;}, "+", fn);
    //zero out only one component via division
    vec2Test(GLGE::ivec2(25, 25), GLGE::ivec2(1, 5), GLGE::ivec2(25, 5), [](GLGE::ivec2 a, GLGE::ivec2 b) -> GLGE::ivec2 {return a / b;}, "/", fn);

    //message
    msg.msg = "[INFO] Testing geometric functions";
    (*(fn->log))(&msg);

    //standard Length
    vec2GeomTest(GLGE::vec2(3.0f, 4.0f), 5.0f, [](GLGE::vec2 v) -> float {return glm::length(v);}, [](float a, float b) -> bool {return a == b;}, "length", fn);
    //normalize standard vector
    vec2GeomTest(GLGE::vec2(0.0f, 5.0f), GLGE::vec2(0.0f, 1.0f), [](GLGE::vec2 v) -> GLGE::vec2 {return glm::normalize(v);}, [](GLGE::vec2 a, GLGE::vec2 b) -> bool {return (a.x == b.x) && (a.y == b.y);}, "normalize", fn);
    //EDGE CASE: Normalize Zero Vector (Prevents crash / handles undefined orientation)
    //expected result: -NaN or NaN
    vec2GeomTest(GLGE::vec2(0.0f, 0.0f), GLGE::vec2(nanf(""), nanf("")), [](GLGE::vec2 v) -> GLGE::vec2 {return glm::normalize(v);}, [](GLGE::vec2 a, GLGE::vec2 b) -> bool {return glm::isnan(a.x) && glm::isnan(a.y);}, "normalize [Zero Vector Case]", fn);
    //EDGE CASE: Large Magnitude Vector (Checks for intermediate square-sum overflow)
    vec2GeomTest<GLGE::vec2, GLGE::vec2>(GLGE::vec2(3e18f, 4e18f), GLGE::vec2(0.6f, 0.8f), [](GLGE::vec2 v) -> GLGE::vec2 {return glm::normalize(v);}, [](GLGE::vec2 a, GLGE::vec2 b) -> bool {return (a.x == b.x) && (a.y == b.y);}, "normalize() [Large Vector Case]", fn);

    //EDGE CASE: Perpendicular vectors (Dot product must be exactly 0.0)
    vec2GeomTest<float, GLGE::vec2>(GLGE::vec2(1.0f, 0.0f), 0.0f, [](GLGE::vec2 v) -> float {return glm::dot(v, GLGE::vec2(0.0f, 1.0f));}, [](float a, float b) -> bool {return a == b;}, "dot [Perpendicular]", fn);
    //EDGE CASE: Opposing directions (Dot product must be exactly -1.0 when normalized)
    vec2GeomTest<float, GLGE::vec2>(GLGE::vec2(1.0f, 0.0f), -1.0f, [](GLGE::vec2 v) -> float {return glm::dot(v, GLGE::vec2(-1.0f, 0.0f));}, [](float a, float b) -> bool {return a == b;}, "dot [Opposite Alignment]", fn);
    //EDGE CASE: Flipped Negative Zero Sign Handling
    //Verifies if multiplying a zero vector components with a negative scalar preserves -0.0f
    vec2GeomTest<GLGE::vec2, GLGE::vec2>(GLGE::vec2(0.0f, 0.0f), GLGE::vec2(-0.0f, -0.0f), [](GLGE::vec2 v) -> GLGE::vec2 {return v * -1.0f;}, [](GLGE::vec2 a, GLGE::vec2 b) -> bool {return (a.x == b.x) && (a.y == b.y);}, "scalar multiplication [Negative Zero Check]", fn);

    //standard Vector * Scalar
    vec2GeomTest<GLGE::vec2, GLGE::vec2>(GLGE::vec2(4.0f, -2.5f), GLGE::vec2(12.0f, -7.5f), [](GLGE::vec2 v) -> GLGE::vec2 {return v * 3.0f;}, [](GLGE::vec2 a, GLGE::vec2 b) -> bool {return (a.x == b.x) && (a.y == b.y);}, "operator*(Vector, Scalar)", fn);
    //commutative Scalar * Vector
    vec2GeomTest<GLGE::vec2, GLGE::vec2>(GLGE::vec2(4.0f, -2.5f), GLGE::vec2(12.0f, -7.5f), [](GLGE::vec2 v) -> GLGE::vec2 { return 3.0f * v; }, [](GLGE::vec2 a, GLGE::vec2 b) -> bool {return (a.x == b.x) && (a.y == b.y);}, "operator*(Scalar, Vector)", fn);

    //success
    report->result = TEST_SUCCESS;
}

template <typename vecT, typename Fn>
void vec3Test(vecT a, vecT b, vecT expected, Fn invoke, const char* op, const TestFunctions* fn) {
    //compute the value
    vecT val = invoke(a, b);
    //print an assertion
    TestAssertion ass;
    std::stringstream ex;
    ex << "(x: " << a.x << ", y: " << a.y << ", z: " << a.z << ") " << op << " (x: " << b.x << ", y: " << b.y << ", " << b.z << ") = (x: " << expected.x << ", y: " << expected.y << ", z: " << expected.z << ")";
    std::string ex_str = ex.str();
    std::stringstream ac;
    ac << "(x: " << a.x << ", y: " << a.y << ", z: " << a.z << ") " << op << " (x: " << b.x << ", y: " << b.y << ", " << b.z << ") = (x: " << val.x << ", y: " << val.y << ", z: " << val.z << ")";
    std::string ac_str = ac.str();
    ass.expected = ex_str.c_str();
    ass.actual = ac_str.c_str();
    ass.passed = (expected.x == val.x) && (expected.y == val.y) && (expected.z == val.z);
    //assert
    (*(fn->assertion))(&ass);
}

template <typename T, typename vecT, typename Fn, typename CmpFn>
void vec3GeomTest(vecT v, T res, Fn invoke, CmpFn cmp, const std::string descr, const TestFunctions* fn) {
    //compute the value
    T val = invoke(v);
    //print an assertion
    TestAssertion ass;
    std::stringstream ex;
    ex << "Calling " << descr << " on vector (x: " << v.x << ", y: " << v.y << ", z: " << v.z << ") should result in " << res;
    std::string ex_str = ex.str();
    std::stringstream ac;
    ac << "Calling " << descr << " on vector (x: " << v.x << ", y: " << v.y << ", z: " << v.z << ") resulted in " << val;
    std::string ac_str = ac.str();
    ass.expected = ex_str.c_str();
    ass.actual = ac_str.c_str();
    ass.passed = cmp(val, res);
    //assert
    (*(fn->assertion))(&ass);
}

void vec3Test(const TestContext* ctx, TestReport* report, const TestFunctions* fn) {
    //on fail, fail controlled
    report->result = TEST_CONTROLLED_FAIL;

    //message
    TestMessage msg;
    msg.msg = "[INFO] Testing float 3D vectors";
    (*(fn->log))(&msg);

    //test some addition
    vec3Test(GLGE::vec3( 7,  9,  2), GLGE::vec3( 3,  4,  5), GLGE::vec3(10, 13,  7), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::vec3(-2,  5,  0), GLGE::vec3(-5,  2, -3), GLGE::vec3(-7,  7, -3), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::vec3(12,  2, -5), GLGE::vec3(10, -1,  4), GLGE::vec3(22,  1, -1), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::vec3( 0,  0,  0), GLGE::vec3(-4,  7,  2), GLGE::vec3(-4,  7,  2), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::vec3(55, 30, 20), GLGE::vec3(50, 10,  5), GLGE::vec3(105,40, 25), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::vec3( 3, -8,  4), GLGE::vec3( 1, -4, -2), GLGE::vec3( 4,-12,  2), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::vec3(15, 15, 15), GLGE::vec3( 7,  8,  9), GLGE::vec3(22, 23, 24), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::vec3(-6, -6, -6), GLGE::vec3(-3, -3, -3), GLGE::vec3(-9, -9, -9), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::vec3( 8,  1,  3), GLGE::vec3( 8,  0, -3), GLGE::vec3(16,  1,  0), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::vec3(23, 11,  5), GLGE::vec3(12,  5,  6), GLGE::vec3(35, 16, 11), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::vec3( 1,  6, -2), GLGE::vec3(-2,  3,  4), GLGE::vec3(-1,  9,  2), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::vec3(14,  5,  8), GLGE::vec3( 9,  1,  2), GLGE::vec3(23,  6, 10), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::vec3(-1,  9,  3), GLGE::vec3( 4,  5,  6), GLGE::vec3( 3, 14,  9), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::vec3(32, 48, 10), GLGE::vec3(20, 40, 30), GLGE::vec3(52, 88, 40), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a + b;}, "+", fn);

    //test some subtraction
    vec3Test(GLGE::vec3( 7,  9,  8), GLGE::vec3( 3,  4,  5), GLGE::vec3( 4,  5,  3), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::vec3(-2,  5,  1), GLGE::vec3(-5,  2, -2), GLGE::vec3( 3,  3,  3), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::vec3(12,  2,  0), GLGE::vec3(10, -1,  4), GLGE::vec3( 2,  3, -4), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::vec3( 0,  0,  0), GLGE::vec3(-4,  7,  5), GLGE::vec3( 4, -7, -5), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::vec3(55, 30, 40), GLGE::vec3(50, 10, 15), GLGE::vec3( 5, 20, 25), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::vec3( 3, -8,  6), GLGE::vec3( 1, -4,  8), GLGE::vec3( 2, -4, -2), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::vec3(15, 15, 15), GLGE::vec3( 7,  8,  9), GLGE::vec3( 8,  7,  6), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::vec3(-6, -6, -6), GLGE::vec3(-3, -3, -3), GLGE::vec3(-3, -3, -3), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::vec3( 8,  1,  2), GLGE::vec3( 8,  0,  2), GLGE::vec3( 0,  1,  0), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::vec3(23, 11,  9), GLGE::vec3(12,  5,  4), GLGE::vec3(11,  6,  5), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::vec3( 1,  6, -3), GLGE::vec3(-2,  3, -6), GLGE::vec3( 3,  3,  3), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::vec3(14,  5,  7), GLGE::vec3( 9,  1,  3), GLGE::vec3( 5,  4,  4), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::vec3(-1,  9,  2), GLGE::vec3( 4,  5,  4), GLGE::vec3(-5,  4, -2), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::vec3(32, 48, 60), GLGE::vec3(20, 40, 50), GLGE::vec3(12,  8, 10), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a - b;}, "-", fn);

    //test some multiplication
    vec3Test(GLGE::vec3( 3,  4,  2), GLGE::vec3( 2,  5,  3), GLGE::vec3( 6, 20,  6), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a * b;}, "*", fn);
    vec3Test(GLGE::vec3(-5,  2, -1), GLGE::vec3( 3, -3,  4), GLGE::vec3(-15,-6, -4), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a * b;}, "*", fn);
    vec3Test(GLGE::vec3(10, -1,  5), GLGE::vec3( 0,  4,  2), GLGE::vec3( 0, -4, 10), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a * b;}, "*", fn);
    vec3Test(GLGE::vec3( 4,  5,  6), GLGE::vec3( 3,  2,  1), GLGE::vec3(12, 10,  6), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a * b;}, "*", fn);

    //test some division
    vec3Test(GLGE::vec3(12, 20, 15), GLGE::vec3( 3,  4,  5), GLGE::vec3( 4,  5,  3), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a / b;}, "/", fn);
    vec3Test(GLGE::vec3(-15, 9, 12), GLGE::vec3( 5, -3, -2), GLGE::vec3(-3, -3, -6), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a / b;}, "/", fn);
    vec3Test(GLGE::vec3( 8,  0,  9), GLGE::vec3( 2,  1,  3), GLGE::vec3( 4,  0,  3), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a / b;}, "/", fn);
    vec3Test(GLGE::vec3(50, 40, 30), GLGE::vec3(10,  4,  3), GLGE::vec3( 5, 10, 10), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a / b;}, "/", fn);

    //message
    msg.msg = "[INFO] Testing unsigned 32 bit integer 3D vectors";
    (*(fn->log))(&msg);

    //test some addition
    vec3Test(GLGE::uvec3( 7,  9,  2), GLGE::uvec3( 3,  4,  5), GLGE::uvec3(10, 13,  7), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::uvec3(-2,  5,  0), GLGE::uvec3(-5,  2, -3), GLGE::uvec3(-7,  7, -3), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::uvec3(12,  2, -5), GLGE::uvec3(10, -1,  4), GLGE::uvec3(22,  1, -1), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::uvec3( 0,  0,  0), GLGE::uvec3(-4,  7,  2), GLGE::uvec3(-4,  7,  2), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::uvec3(55, 30, 20), GLGE::uvec3(50, 10,  5), GLGE::uvec3(105,40, 25), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::uvec3( 3, -8,  4), GLGE::uvec3( 1, -4, -2), GLGE::uvec3( 4,-12,  2), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::uvec3(15, 15, 15), GLGE::uvec3( 7,  8,  9), GLGE::uvec3(22, 23, 24), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::uvec3(-6, -6, -6), GLGE::uvec3(-3, -3, -3), GLGE::uvec3(-9, -9, -9), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::uvec3( 8,  1,  3), GLGE::uvec3( 8,  0, -3), GLGE::uvec3(16,  1,  0), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::uvec3(23, 11,  5), GLGE::uvec3(12,  5,  6), GLGE::uvec3(35, 16, 11), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::uvec3( 1,  6, -2), GLGE::uvec3(-2,  3,  4), GLGE::uvec3(-1,  9,  2), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::uvec3(14,  5,  8), GLGE::uvec3( 9,  1,  2), GLGE::uvec3(23,  6, 10), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::uvec3(-1,  9,  3), GLGE::uvec3( 4,  5,  6), GLGE::uvec3( 3, 14,  9), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::uvec3(32, 48, 10), GLGE::uvec3(20, 40, 30), GLGE::uvec3(52, 88, 40), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a + b;}, "+", fn);

    //test some subtraction
    vec3Test(GLGE::uvec3( 7,  9,  8), GLGE::uvec3( 3,  4,  5), GLGE::uvec3( 4,  5,  3), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::uvec3(-2,  5,  1), GLGE::uvec3(-5,  2, -2), GLGE::uvec3( 3,  3,  3), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::uvec3(12,  2,  0), GLGE::uvec3(10, -1,  4), GLGE::uvec3( 2,  3, -4), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::uvec3( 0,  0,  0), GLGE::uvec3(-4,  7,  5), GLGE::uvec3( 4, -7, -5), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::uvec3(55, 30, 40), GLGE::uvec3(50, 10, 15), GLGE::uvec3( 5, 20, 25), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::uvec3( 3, -8,  6), GLGE::uvec3( 1, -4,  8), GLGE::uvec3( 2, -4, -2), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::uvec3(15, 15, 15), GLGE::uvec3( 7,  8,  9), GLGE::uvec3( 8,  7,  6), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::uvec3(-6, -6, -6), GLGE::uvec3(-3, -3, -3), GLGE::uvec3(-3, -3, -3), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::uvec3( 8,  1,  2), GLGE::uvec3( 8,  0,  2), GLGE::uvec3( 0,  1,  0), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::uvec3(23, 11,  9), GLGE::uvec3(12,  5,  4), GLGE::uvec3(11,  6,  5), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::uvec3( 1,  6, -3), GLGE::uvec3(-2,  3, -6), GLGE::uvec3( 3,  3,  3), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::uvec3(14,  5,  7), GLGE::uvec3( 9,  1,  3), GLGE::uvec3( 5,  4,  4), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::uvec3(-1,  9,  2), GLGE::uvec3( 4,  5,  4), GLGE::uvec3(-5,  4, -2), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::uvec3(32, 48, 60), GLGE::uvec3(20, 40, 50), GLGE::uvec3(12,  8, 10), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a - b;}, "-", fn);

    //test some multiplication
    vec3Test(GLGE::uvec3( 3,  4,  2), GLGE::uvec3( 2,  5,  3), GLGE::uvec3( 6, 20,  6), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a * b;}, "*", fn);
    vec3Test(GLGE::uvec3(10,  1,  5), GLGE::uvec3( 0,  4,  2), GLGE::uvec3( 0,  4, 10), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a * b;}, "*", fn);
    vec3Test(GLGE::uvec3( 4,  5,  6), GLGE::uvec3( 3,  2,  1), GLGE::uvec3(12, 10,  6), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a * b;}, "*", fn);

    //test some division
    vec3Test(GLGE::uvec3(12, 20, 15), GLGE::uvec3( 3,  4,  5), GLGE::uvec3( 4,  5,  3), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a / b;}, "/", fn);
    vec3Test(GLGE::uvec3( 8,  4,  9), GLGE::uvec3( 2,  1,  3), GLGE::uvec3( 4,  4,  3), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a / b;}, "/", fn);
    vec3Test(GLGE::uvec3(50, 40, 30), GLGE::uvec3(10,  4,  3), GLGE::uvec3( 5, 10, 10), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a / b;}, "/", fn);

    //message
    msg.msg = "[INFO] Testing signed 32 bit integer 3D vectors";
    (*(fn->log))(&msg);

    //test some addition
    vec3Test(GLGE::ivec3( 7,  9,  2), GLGE::ivec3( 3,  4,  5), GLGE::ivec3(10, 13,  7), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::ivec3(-2,  5,  0), GLGE::ivec3(-5,  2, -3), GLGE::ivec3(-7,  7, -3), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::ivec3(12,  2, -5), GLGE::ivec3(10, -1,  4), GLGE::ivec3(22,  1, -1), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::ivec3( 0,  0,  0), GLGE::ivec3(-4,  7,  2), GLGE::ivec3(-4,  7,  2), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::ivec3(55, 30, 20), GLGE::ivec3(50, 10,  5), GLGE::ivec3(105,40, 25), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::ivec3( 3, -8,  4), GLGE::ivec3( 1, -4, -2), GLGE::ivec3( 4,-12,  2), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::ivec3(15, 15, 15), GLGE::ivec3( 7,  8,  9), GLGE::ivec3(22, 23, 24), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::ivec3(-6, -6, -6), GLGE::ivec3(-3, -3, -3), GLGE::ivec3(-9, -9, -9), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::ivec3( 8,  1,  3), GLGE::ivec3( 8,  0, -3), GLGE::ivec3(16,  1,  0), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::ivec3(23, 11,  5), GLGE::ivec3(12,  5,  6), GLGE::ivec3(35, 16, 11), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::ivec3( 1,  6, -2), GLGE::ivec3(-2,  3,  4), GLGE::ivec3(-1,  9,  2), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::ivec3(14,  5,  8), GLGE::ivec3( 9,  1,  2), GLGE::ivec3(23,  6, 10), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::ivec3(-1,  9,  3), GLGE::ivec3( 4,  5,  6), GLGE::ivec3( 3, 14,  9), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a + b;}, "+", fn);
    vec3Test(GLGE::ivec3(32, 48, 10), GLGE::ivec3(20, 40, 30), GLGE::ivec3(52, 88, 40), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a + b;}, "+", fn);

    //test some subtraction
    vec3Test(GLGE::ivec3( 7,  9,  8), GLGE::ivec3( 3,  4,  5), GLGE::ivec3( 4,  5,  3), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::ivec3(-2,  5,  1), GLGE::ivec3(-5,  2, -2), GLGE::ivec3( 3,  3,  3), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::ivec3(12,  2,  0), GLGE::ivec3(10, -1,  4), GLGE::ivec3( 2,  3, -4), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::ivec3( 0,  0,  0), GLGE::ivec3(-4,  7,  5), GLGE::ivec3( 4, -7, -5), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::ivec3(55, 30, 40), GLGE::ivec3(50, 10, 15), GLGE::ivec3( 5, 20, 25), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::ivec3( 3, -8,  6), GLGE::ivec3( 1, -4,  8), GLGE::ivec3( 2, -4, -2), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::ivec3(15, 15, 15), GLGE::ivec3( 7,  8,  9), GLGE::ivec3( 8,  7,  6), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::ivec3(-6, -6, -6), GLGE::ivec3(-3, -3, -3), GLGE::ivec3(-3, -3, -3), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::ivec3( 8,  1,  2), GLGE::ivec3( 8,  0,  2), GLGE::ivec3( 0,  1,  0), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::ivec3(23, 11,  9), GLGE::ivec3(12,  5,  4), GLGE::ivec3(11,  6,  5), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::ivec3( 1,  6, -3), GLGE::ivec3(-2,  3, -6), GLGE::ivec3( 3,  3,  3), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::ivec3(14,  5,  7), GLGE::ivec3( 9,  1,  3), GLGE::ivec3( 5,  4,  4), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::ivec3(-1,  9,  2), GLGE::ivec3( 4,  5,  4), GLGE::ivec3(-5,  4, -2), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a - b;}, "-", fn);
    vec3Test(GLGE::ivec3(32, 48, 60), GLGE::ivec3(20, 40, 50), GLGE::ivec3(12,  8, 10), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a - b;}, "-", fn);

    //test some multiplication
    vec3Test(GLGE::ivec3( 3,  4,  2), GLGE::ivec3( 2,  5,  3), GLGE::ivec3( 6, 20,  6), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a * b;}, "*", fn);
    vec3Test(GLGE::ivec3(-5,  2, -1), GLGE::ivec3( 3, -3,  4), GLGE::ivec3(-15,-6, -4), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a * b;}, "*", fn);
    vec3Test(GLGE::ivec3(10, -1,  5), GLGE::ivec3( 0,  4,  2), GLGE::ivec3( 0, -4, 10), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a * b;}, "*", fn);
    vec3Test(GLGE::ivec3( 4,  5,  6), GLGE::ivec3( 3,  2,  1), GLGE::ivec3(12, 10,  6), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a * b;}, "*", fn);

    //test some division
    vec3Test(GLGE::ivec3(12, 20, 15), GLGE::ivec3( 3,  4,  5), GLGE::ivec3( 4,  5,  3), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a / b;}, "/", fn);
    vec3Test(GLGE::ivec3(-15, 9, 12), GLGE::ivec3( 5, -3, -2), GLGE::ivec3(-3, -3, -6), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a / b;}, "/", fn);
    vec3Test(GLGE::ivec3( 8,  0,  9), GLGE::ivec3( 2,  1,  3), GLGE::ivec3( 4,  0,  3), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a / b;}, "/", fn);
    vec3Test(GLGE::ivec3(50, 40, 30), GLGE::ivec3(10,  4,  3), GLGE::ivec3( 5, 10, 10), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a / b;}, "/", fn);

    //message
    msg.msg = "[INFO] Testing special cases";
    (*(fn->log))(&msg);

    //division by zero for 3D vector
    vec3Test(GLGE::vec3(1, 1, 1), GLGE::vec3(0, 0, 0), GLGE::vec3(1./0., 1./0., 1./0.), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a / b;}, "/", fn);
    //test uvec3 underflow (0 - 1 wraps to UINT_MAX)
    vec3Test(GLGE::uvec3(0, 0, 0), GLGE::uvec3(1, 1, 1), GLGE::uvec3(4294967295, 4294967295, 4294967295), [](GLGE::uvec3 a, GLGE::uvec3 b) -> GLGE::uvec3 {return a - b;}, "-", fn);

    //addition overflow (INT_MAX + 1 wraps to INT_MIN)
    vec3Test(GLGE::ivec3(2147483647, 2147483647, 2147483647), GLGE::ivec3(1, 1, 1), GLGE::ivec3(-2147483648, -2147483648, -2147483648), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a + b;}, "+", fn);
    //subtraction underflow (INT_MIN - 1 wraps to INT_MAX)
    vec3Test(GLGE::ivec3(-2147483648, -2147483648, -2147483648), GLGE::ivec3(1, 1, 1), GLGE::ivec3(2147483647, 2147483647, 2147483647), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a - b;}, "-", fn);

    //multiplication by Zero
    vec3Test(GLGE::vec3(0, 0, 0), GLGE::vec3(55, -23, 12), GLGE::vec3(0, 0, 0), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a * b;}, "*", fn);
    //multiplication by One
    vec3Test(GLGE::vec3(1, 1, 1), GLGE::vec3(55, -23, 12), GLGE::vec3(55, -23, 12), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a * b;}, "*", fn);

    //mixed signs and decimals
    vec3Test(GLGE::vec3(-0.5f, 10.5f, -2.25f), GLGE::vec3(0.5f, -10.5f, 2.25f), GLGE::vec3(0.0f, 0.0f, 0.0f), [](GLGE::vec3 a, GLGE::vec3 b) -> GLGE::vec3 {return a + b;}, "+", fn);

    //one component goes up, one goes down
    vec3Test(GLGE::ivec3(100, 100, 100), GLGE::ivec3(100, -100, 0), GLGE::ivec3(200, 0, 100), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a + b;}, "+", fn);
    //zero out only one component via division
    vec3Test(GLGE::ivec3(25, 25, 25), GLGE::ivec3(1, 5, 25), GLGE::ivec3(25, 5, 1), [](GLGE::ivec3 a, GLGE::ivec3 b) -> GLGE::ivec3 {return a / b;}, "/", fn);

    //message
    msg.msg = "[INFO] Testing geometric functions";
    (*(fn->log))(&msg);

    //standard Length (3^2 + 4^2 + 12^2 = 9 + 16 + 144 = 169 -> sqrt(169) = 13)
    vec3GeomTest(GLGE::vec3(3.0f, 4.0f, 12.0f), 13.0f, [](GLGE::vec3 v) -> float {return glm::length(v);}, [](float a, float b) -> bool {return a == b;}, "length", fn);
    //normalize standard vector
    vec3GeomTest(GLGE::vec3(0.0f, 5.0f, 0.0f), GLGE::vec3(0.0f, 1.0f, 0.0f), [](GLGE::vec3 v) -> GLGE::vec3 {return glm::normalize(v);}, [](GLGE::vec3 a, GLGE::vec3 b) -> bool {return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);}, "normalize", fn);
    //EDGE CASE: Normalize Zero Vector (Prevents crash / handles undefined orientation)
    //expected result: -NaN or NaN
    vec3GeomTest(GLGE::vec3(0.0f, 0.0f, 0.0f), GLGE::vec3(nanf(""), nanf(""), nanf("")), [](GLGE::vec3 v) -> GLGE::vec3 {return glm::normalize(v);}, [](GLGE::vec3 a, GLGE::vec3 b) -> bool {return glm::isnan(a.x) && glm::isnan(a.y) && glm::isnan(a.z);}, "normalize [Zero Vector Case]", fn);
    //EDGE CASE: Large Magnitude Vector (Checks for intermediate square-sum overflow)
    vec3GeomTest(GLGE::vec3(3e18f, 4e18f, 0.0f), GLGE::vec3(0.6f, 0.8f, 0.0f), [](GLGE::vec3 v) -> GLGE::vec3 {return glm::normalize(v);}, [](GLGE::vec3 a, GLGE::vec3 b) -> bool {return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);}, "normalize() [Large Vector Case]", fn);

    //EDGE CASE: Perpendicular vectors (Dot product must be exactly 0.0)
    vec3GeomTest(GLGE::vec3(1.0f, 0.0f, 0.0f), 0.0f, [](GLGE::vec3 v) -> float {return glm::dot(v, GLGE::vec3(0.0f, 1.0f, 0.0f));}, [](float a, float b) -> bool {return a == b;}, "dot [Perpendicular]", fn);
    //EDGE CASE: Opposing directions (Dot product must be exactly -1.0 when normalized)
    vec3GeomTest(GLGE::vec3(1.0f, 0.0f, 0.0f), -1.0f, [](GLGE::vec3 v) -> float {return glm::dot(v, GLGE::vec3(-1.0f, 0.0f, 0.0f));}, [](float a, float b) -> bool {return a == b;}, "dot [Opposite Alignment]", fn);
    //EDGE CASE: Flipped Negative Zero Sign Handling
    //Verifies if multiplying a zero vector components with a negative scalar preserves -0.0f
    vec3GeomTest(GLGE::vec3(0.0f, 0.0f, 0.0f), GLGE::vec3(-0.0f, -0.0f, -0.0f), [](GLGE::vec3 v) -> GLGE::vec3 {return v * -1.0f;}, [](GLGE::vec3 a, GLGE::vec3 b) -> bool {return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);}, "scalar multiplication [Negative Zero Check]", fn);

    //standard Vector * Scalar
    vec3GeomTest(GLGE::vec3(4.0f, -2.5f, 1.0f), GLGE::vec3(12.0f, -7.5f, 3.0f), [](GLGE::vec3 v) -> GLGE::vec3 {return v * 3.0f;}, [](GLGE::vec3 a, GLGE::vec3 b) -> bool {return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);}, "operator*(Vector, Scalar)", fn);
    //commutative Scalar * Vector
    vec3GeomTest(GLGE::vec3(4.0f, -2.5f, 1.0f), GLGE::vec3(12.0f, -7.5f, 3.0f), [](GLGE::vec3 v) -> GLGE::vec3 { return 3.0f * v; }, [](GLGE::vec3 a, GLGE::vec3 b) -> bool {return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);}, "operator*(Scalar, Vector)", fn);
    
    //success
    report->result = TEST_SUCCESS;
}

template <typename vecT, typename Fn>
void vec4Test(vecT a, vecT b, vecT expected, Fn invoke, const char* op, const TestFunctions* fn) {
    //compute the value
    vecT val = invoke(a, b);
    //print an assertion
    TestAssertion ass;
    std::stringstream ex;
    ex << "(x: " << a.x << ", y: " << a.y << ", z: " << a.z << ", w: " << a.w << ") " << op 
       << " (x: " << b.x << ", y: " << b.y << ", z: " << b.z << ", w: " << b.w << ") = (x: " 
       << expected.x << ", y: " << expected.y << ", z: " << expected.z << ", w: " << expected.w << ")";
    std::string ex_str = ex.str();
    std::stringstream ac;
    ac << "(x: " << a.x << ", y: " << a.y << ", z: " << a.z << ", w: " << a.w << ") " << op 
       << " (x: " << b.x << ", y: " << b.y << ", z: " << b.z << ", w: " << b.w << ") = (x: " 
       << val.x << ", y: " << val.y << ", z: " << val.z << ", w: " << val.w << ")";
    std::string ac_str = ac.str();
    ass.expected = ex_str.c_str();
    ass.actual = ac_str.c_str();
    ass.passed = (expected.x == val.x) && (expected.y == val.y) && (expected.z == val.z) && (expected.w == val.w);
    //assert
    (*(fn->assertion))(&ass);
}

template <typename T, typename vecT, typename Fn, typename CmpFn>
void vec4GeomTest(vecT v, T res, Fn invoke, CmpFn cmp, const std::string descr, const TestFunctions* fn) {
    //compute the value
    T val = invoke(v);
    //print an assertion
    TestAssertion ass;
    std::stringstream ex;
    ex << "Calling " << descr << " on vector (x: " << v.x << ", y: " << v.y << ", z: " << v.z << ", w: " << v.w << ") should result in " << res;
    std::string ex_str = ex.str();
    std::stringstream ac;
    ac << "Calling " << descr << " on vector (x: " << v.x << ", y: " << v.y << ", z: " << v.z << ", w: " << v.w << ") resulted in " << val;
    std::string ac_str = ac.str();
    ass.expected = ex_str.c_str();
    ass.actual = ac_str.c_str();
    ass.passed = cmp(val, res);
    //assert
    (*(fn->assertion))(&ass);
}

void vec4Test(const TestContext* ctx, TestReport* report, const TestFunctions* fn) {
    //on fail, fail controlled
    report->result = TEST_CONTROLLED_FAIL;

    //message
    TestMessage msg;
    msg.msg = "[INFO] Testing float 4D vectors";
    (*(fn->log))(&msg);

    //test some addition
    vec4Test(GLGE::vec4( 7,  9,  2,  1), GLGE::vec4( 3,  4,  5,  6), GLGE::vec4(10, 13,  7,  7), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::vec4(-2,  5,  0, -1), GLGE::vec4(-5,  2, -3,  4), GLGE::vec4(-7,  7, -3,  3), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::vec4(12,  2, -5,  8), GLGE::vec4(10, -1,  4, -2), GLGE::vec4(22,  1, -1,  6), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::vec4( 0,  0,  0,  0), GLGE::vec4(-4,  7,  2,  1), GLGE::vec4(-4,  7,  2,  1), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::vec4(55, 30, 20, 10), GLGE::vec4(50, 10,  5,  2), GLGE::vec4(105,40, 25, 12), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::vec4( 3, -8,  4, -1), GLGE::vec4( 1, -4, -2, -3), GLGE::vec4( 4,-12,  2, -4), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::vec4(15, 15, 15, 15), GLGE::vec4( 7,  8,  9, 10), GLGE::vec4(22, 23, 24, 25), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::vec4(-6, -6, -6, -6), GLGE::vec4(-3, -3, -3, -3), GLGE::vec4(-9, -9, -9, -9), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::vec4( 8,  1,  3,  4), GLGE::vec4( 8,  0, -3, -4), GLGE::vec4(16,  1,  0,  0), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::vec4(23, 11,  5,  2), GLGE::vec4(12,  5,  6,  7), GLGE::vec4(35, 16, 11,  9), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::vec4( 1,  6, -2,  5), GLGE::vec4(-2,  3,  4, -1), GLGE::vec4(-1,  9,  2,  4), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::vec4(14,  5,  8,  3), GLGE::vec4( 9,  1,  2,  4), GLGE::vec4(23,  6, 10,  7), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::vec4(-1,  9,  3, -2), GLGE::vec4( 4,  5,  6,  1), GLGE::vec4( 3, 14,  9, -1), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::vec4(32, 48, 10,  5), GLGE::vec4(20, 40, 30,  5), GLGE::vec4(52, 88, 40, 10), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a + b;}, "+", fn);

    //test some subtraction
    vec4Test(GLGE::vec4( 7,  9,  8,  6), GLGE::vec4( 3,  4,  5,  2), GLGE::vec4( 4,  5,  3,  4), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::vec4(-2,  5,  1, -3), GLGE::vec4(-5,  2, -2, -6), GLGE::vec4( 3,  3,  3,  3), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::vec4(12,  2,  0,  4), GLGE::vec4(10, -1,  4,  8), GLGE::vec4( 2,  3, -4, -4), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::vec4( 0,  0,  0,  0), GLGE::vec4(-4,  7,  5,  2), GLGE::vec4( 4, -7, -5, -2), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::vec4(55, 30, 40, 50), GLGE::vec4(50, 10, 15, 25), GLGE::vec4( 5, 20, 25, 25), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::vec4( 3, -8,  6,  2), GLGE::vec4( 1, -4,  8,  4), GLGE::vec4( 2, -4, -2, -2), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::vec4(15, 15, 15, 15), GLGE::vec4( 7,  8,  9, 10), GLGE::vec4( 8,  7,  6,  5), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::vec4(-6, -6, -6, -6), GLGE::vec4(-3, -3, -3, -3), GLGE::vec4(-3, -3, -3, -3), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::vec4( 8,  1,  2,  3), GLGE::vec4( 8,  0,  2,  3), GLGE::vec4( 0,  1,  0,  0), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::vec4(23, 11,  9,  4), GLGE::vec4(12,  5,  4,  1), GLGE::vec4(11,  6,  5,  3), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::vec4( 1,  6, -3,  2), GLGE::vec4(-2,  3, -6, -1), GLGE::vec4( 3,  3,  3,  3), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::vec4(14,  5,  7,  9), GLGE::vec4( 9,  1,  3,  5), GLGE::vec4( 5,  4,  4,  4), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::vec4(-1,  9,  2,  0), GLGE::vec4( 4,  5,  4,  2), GLGE::vec4(-5,  4, -2, -2), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::vec4(32, 48, 60, 40), GLGE::vec4(20, 40, 50, 30), GLGE::vec4(12,  8, 10, 10), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a - b;}, "-", fn);

    //test some multiplication
    vec4Test(GLGE::vec4( 3,  4,  2,  5), GLGE::vec4( 2,  5,  3,  2), GLGE::vec4( 6, 20,  6, 10), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a * b;}, "*", fn);
    vec4Test(GLGE::vec4(-5,  2, -1,  3), GLGE::vec4( 3, -3,  4, -2), GLGE::vec4(-15,-6, -4, -6), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a * b;}, "*", fn);
    vec4Test(GLGE::vec4(10, -1,  5,  0), GLGE::vec4( 0,  4,  2,  9), GLGE::vec4( 0, -4, 10,  0), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a * b;}, "*", fn);
    vec4Test(GLGE::vec4( 4,  5,  6,  7), GLGE::vec4( 3,  2,  1,  0), GLGE::vec4(12, 10,  6,  0), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a * b;}, "*", fn);

    //test some division
    vec4Test(GLGE::vec4(12, 20, 15,  8), GLGE::vec4( 3,  4,  5,  2), GLGE::vec4( 4,  5,  3,  4), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a / b;}, "/", fn);
    vec4Test(GLGE::vec4(-15, 9, 12, -4), GLGE::vec4( 5, -3, -2,  2), GLGE::vec4(-3, -3, -6, -2), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a / b;}, "/", fn);
    vec4Test(GLGE::vec4( 8,  0,  9,  6), GLGE::vec4( 2,  1,  3,  2), GLGE::vec4( 4,  0,  3,  3), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a / b;}, "/", fn);
    vec4Test(GLGE::vec4(50, 40, 30, 20), GLGE::vec4(10,  4,  3,  2), GLGE::vec4( 5, 10, 10, 10), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a / b;}, "/", fn);

    //message
    msg.msg = "[INFO] Testing unsigned 32 bit integer 4D vectors";
    (*(fn->log))(&msg);

    //test some addition
    vec4Test(GLGE::uvec4( 7,  9,  2,  1), GLGE::uvec4( 3,  4,  5,  6), GLGE::uvec4(10, 13,  7,  7), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::uvec4(-2,  5,  0, -1), GLGE::uvec4(-5,  2, -3,  4), GLGE::uvec4(-7,  7, -3,  3), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::uvec4(12,  2, -5,  8), GLGE::uvec4(10, -1,  4, -2), GLGE::uvec4(22,  1, -1,  6), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::uvec4( 0,  0,  0,  0), GLGE::uvec4(-4,  7,  2,  1), GLGE::uvec4(-4,  7,  2,  1), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::uvec4(55, 30, 20, 10), GLGE::uvec4(50, 10,  5,  2), GLGE::uvec4(105,40, 25, 12), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::uvec4( 3, -8,  4, -1), GLGE::uvec4( 1, -4, -2, -3), GLGE::uvec4( 4,-12,  2, -4), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::uvec4(15, 15, 15, 15), GLGE::uvec4( 7,  8,  9, 10), GLGE::uvec4(22, 23, 24, 25), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::uvec4(-6, -6, -6, -6), GLGE::uvec4(-3, -3, -3, -3), GLGE::uvec4(-9, -9, -9, -9), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::uvec4( 8,  1,  3,  4), GLGE::uvec4( 8,  0, -3, -4), GLGE::uvec4(16,  1,  0,  0), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::uvec4(23, 11,  5,  2), GLGE::uvec4(12,  5,  6,  7), GLGE::uvec4(35, 16, 11,  9), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::uvec4( 1,  6, -2,  5), GLGE::uvec4(-2,  3,  4, -1), GLGE::uvec4(-1,  9,  2,  4), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::uvec4(14,  5,  8,  3), GLGE::uvec4( 9,  1,  2,  4), GLGE::uvec4(23,  6, 10,  7), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::uvec4(-1,  9,  3, -2), GLGE::uvec4( 4,  5,  6,  1), GLGE::uvec4( 3, 14,  9, -1), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::uvec4(32, 48, 10,  5), GLGE::uvec4(20, 40, 30,  5), GLGE::uvec4(52, 88, 40, 10), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a + b;}, "+", fn);

    //test some subtraction
    vec4Test(GLGE::uvec4( 7,  9,  8,  6), GLGE::uvec4( 3,  4,  5,  2), GLGE::uvec4( 4,  5,  3,  4), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::uvec4(-2,  5,  1, -3), GLGE::uvec4(-5,  2, -2, -6), GLGE::uvec4( 3,  3,  3,  3), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::uvec4(12,  2,  0,  4), GLGE::uvec4(10, -1,  4,  8), GLGE::uvec4( 2,  3, -4, -4), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::uvec4( 0,  0,  0,  0), GLGE::uvec4(-4,  7,  5,  2), GLGE::uvec4( 4, -7, -5, -2), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::uvec4(55, 30, 40, 50), GLGE::uvec4(50, 10, 15, 25), GLGE::uvec4( 5, 20, 25, 25), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::uvec4( 3, -8,  6,  2), GLGE::uvec4( 1, -4,  8,  4), GLGE::uvec4( 2, -4, -2, -2), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::uvec4(15, 15, 15, 15), GLGE::uvec4( 7,  8,  9, 10), GLGE::uvec4( 8,  7,  6,  5), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::uvec4(-6, -6, -6, -6), GLGE::uvec4(-3, -3, -3, -3), GLGE::uvec4(-3, -3, -3, -3), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::uvec4( 8,  1,  2,  3), GLGE::uvec4( 8,  0,  2,  3), GLGE::uvec4( 0,  1,  0,  0), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::uvec4(23, 11,  9,  4), GLGE::uvec4(12,  5,  4,  1), GLGE::uvec4(11,  6,  5,  3), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::uvec4( 1,  6, -3,  2), GLGE::uvec4(-2,  3, -6, -1), GLGE::uvec4( 3,  3,  3,  3), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::uvec4(14,  5,  7,  9), GLGE::uvec4( 9,  1,  3,  5), GLGE::uvec4( 5,  4,  4,  4), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::uvec4(-1,  9,  2,  0), GLGE::uvec4( 4,  5,  4,  2), GLGE::uvec4(-5,  4, -2, -2), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::uvec4(32, 48, 60, 40), GLGE::uvec4(20, 40, 50, 30), GLGE::uvec4(12,  8, 10, 10), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a - b;}, "-", fn);

    //test some multiplication
    vec4Test(GLGE::uvec4( 3,  4,  2,  5), GLGE::uvec4( 2,  5,  3,  2), GLGE::uvec4( 6, 20,  6, 10), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a * b;}, "*", fn);
    vec4Test(GLGE::uvec4(10,  1,  5,  2), GLGE::uvec4( 0,  4,  2,  3), GLGE::uvec4( 0,  4, 10,  6), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a * b;}, "*", fn);
    vec4Test(GLGE::uvec4( 4,  5,  6,  7), GLGE::uvec4( 3,  2,  1,  2), GLGE::uvec4(12, 10,  6, 14), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a * b;}, "*", fn);

    //test some division
    vec4Test(GLGE::uvec4(12, 20, 15,  8), GLGE::uvec4( 3,  4,  5,  2), GLGE::uvec4( 4,  5,  3,  4), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a / b;}, "/", fn);
    vec4Test(GLGE::uvec4( 8,  4,  9,  6), GLGE::uvec4( 2,  1,  3,  2), GLGE::uvec4( 4,  4,  3,  3), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a / b;}, "/", fn);
    vec4Test(GLGE::uvec4(50, 40, 30, 20), GLGE::uvec4(10,  4,  3,  2), GLGE::uvec4( 5, 10, 10, 10), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a / b;}, "/", fn);

    //message
    msg.msg = "[INFO] Testing signed 32 bit integer 4D vectors";
    (*(fn->log))(&msg);

    //test some addition
    vec4Test(GLGE::ivec4( 7,  9,  2,  1), GLGE::ivec4( 3,  4,  5,  6), GLGE::ivec4(10, 13,  7,  7), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::ivec4(-2,  5,  0, -1), GLGE::ivec4(-5,  2, -3,  4), GLGE::ivec4(-7,  7, -3,  3), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::ivec4(12,  2, -5,  8), GLGE::ivec4(10, -1,  4, -2), GLGE::ivec4(22,  1, -1,  6), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::ivec4( 0,  0,  0,  0), GLGE::ivec4(-4,  7,  2,  1), GLGE::ivec4(-4,  7,  2,  1), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::ivec4(55, 30, 20, 10), GLGE::ivec4(50, 10,  5,  2), GLGE::ivec4(105,40, 25, 12), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::ivec4( 3, -8,  4, -1), GLGE::ivec4( 1, -4, -2, -3), GLGE::ivec4( 4,-12,  2, -4), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::ivec4(15, 15, 15, 15), GLGE::ivec4( 7,  8,  9, 10), GLGE::ivec4(22, 23, 24, 25), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::ivec4(-6, -6, -6, -6), GLGE::ivec4(-3, -3, -3, -3), GLGE::ivec4(-9, -9, -9, -9), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::ivec4( 8,  1,  3,  4), GLGE::ivec4( 8,  0, -3, -4), GLGE::ivec4(16,  1,  0,  0), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::ivec4(23, 11,  5,  2), GLGE::ivec4(12,  5,  6,  7), GLGE::ivec4(35, 16, 11,  9), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::ivec4( 1,  6, -2,  5), GLGE::ivec4(-2,  3,  4, -1), GLGE::ivec4(-1,  9,  2,  4), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::ivec4(14,  5,  8,  3), GLGE::ivec4( 9,  1,  2,  4), GLGE::ivec4(23,  6, 10,  7), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::ivec4(-1,  9,  3, -2), GLGE::ivec4( 4,  5,  6,  1), GLGE::ivec4( 3, 14,  9, -1), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a + b;}, "+", fn);
    vec4Test(GLGE::ivec4(32, 48, 10,  5), GLGE::ivec4(20, 40, 30,  5), GLGE::ivec4(52, 88, 40, 10), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a + b;}, "+", fn);

    //test some subtraction
    vec4Test(GLGE::ivec4( 7,  9,  8,  6), GLGE::ivec4( 3,  4,  5,  2), GLGE::ivec4( 4,  5,  3,  4), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::ivec4(-2,  5,  1, -3), GLGE::ivec4(-5,  2, -2, -6), GLGE::ivec4( 3,  3,  3,  3), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::ivec4(12,  2,  0,  4), GLGE::ivec4(10, -1,  4,  8), GLGE::ivec4( 2,  3, -4, -4), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::ivec4( 0,  0,  0,  0), GLGE::ivec4(-4,  7,  5,  2), GLGE::ivec4( 4, -7, -5, -2), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::ivec4(55, 30, 40, 50), GLGE::ivec4(50, 10, 15, 25), GLGE::ivec4( 5, 20, 25, 25), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::ivec4( 3, -8,  6,  2), GLGE::ivec4( 1, -4,  8,  4), GLGE::ivec4( 2, -4, -2, -2), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::ivec4(15, 15, 15, 15), GLGE::ivec4( 7,  8,  9, 10), GLGE::ivec4( 8,  7,  6,  5), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::ivec4(-6, -6, -6, -6), GLGE::ivec4(-3, -3, -3, -3), GLGE::ivec4(-3, -3, -3, -3), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::ivec4( 8,  1,  2,  3), GLGE::ivec4( 8,  0,  2,  3), GLGE::ivec4( 0,  1,  0,  0), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::ivec4(23, 11,  9,  4), GLGE::ivec4(12,  5,  4,  1), GLGE::ivec4(11,  6,  5,  3), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::ivec4( 1,  6, -3,  2), GLGE::ivec4(-2,  3, -6, -1), GLGE::ivec4( 3,  3,  3,  3), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::ivec4(14,  5,  7,  9), GLGE::ivec4( 9,  1,  3,  5), GLGE::ivec4( 5,  4,  4,  4), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::ivec4(-1,  9,  2,  0), GLGE::ivec4( 4,  5,  4,  2), GLGE::ivec4(-5,  4, -2, -2), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a - b;}, "-", fn);
    vec4Test(GLGE::ivec4(32, 48, 60, 40), GLGE::ivec4(20, 40, 50, 30), GLGE::ivec4(12,  8, 10, 10), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a - b;}, "-", fn);

    //test some multiplication
    vec4Test(GLGE::ivec4( 3,  4,  2,  5), GLGE::ivec4( 2,  5,  3,  2), GLGE::ivec4( 6, 20,  6, 10), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a * b;}, "*", fn);
    vec4Test(GLGE::ivec4(-5,  2, -1,  3), GLGE::ivec4( 3, -3,  4, -2), GLGE::ivec4(-15,-6, -4, -6), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a * b;}, "*", fn);
    vec4Test(GLGE::ivec4(10, -1,  5,  0), GLGE::ivec4( 0,  4,  2,  9), GLGE::ivec4( 0, -4, 10,  0), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a * b;}, "*", fn);
    vec4Test(GLGE::ivec4( 4,  5,  6,  7), GLGE::ivec4( 3,  2,  1,  0), GLGE::ivec4(12, 10,  6,  0), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a * b;}, "*", fn);

    //test some division
    vec4Test(GLGE::ivec4(12, 20, 15,  8), GLGE::ivec4( 3,  4,  5,  2), GLGE::ivec4( 4,  5,  3,  4), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a / b;}, "/", fn);
    vec4Test(GLGE::ivec4(-15, 9, 12, -4), GLGE::ivec4( 5, -3, -2,  2), GLGE::ivec4(-3, -3, -6, -2), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a / b;}, "/", fn);
    vec4Test(GLGE::ivec4( 8,  0,  9,  6), GLGE::ivec4( 2,  1,  3,  2), GLGE::ivec4( 4,  0,  3,  3), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a / b;}, "/", fn);
    vec4Test(GLGE::ivec4(50, 40, 30, 20), GLGE::ivec4(10,  4,  3,  2), GLGE::ivec4( 5, 10, 10, 10), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a / b;}, "/", fn);

    //message
    msg.msg = "[INFO] Testing special cases";
    (*(fn->log))(&msg);

    //division by zero for 4D vector
    vec4Test(GLGE::vec4(1, 1, 1, 1), GLGE::vec4(0, 0, 0, 0), GLGE::vec4(1./0., 1./0., 1./0., 1./0.), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a / b;}, "/", fn);
    //test uvec4 underflow (0 - 1 wraps to UINT_MAX)
    vec4Test(GLGE::uvec4(0, 0, 0, 0), GLGE::uvec4(1, 1, 1, 1), GLGE::uvec4(4294967295, 4294967295, 4294967295, 4294967295), [](GLGE::uvec4 a, GLGE::uvec4 b) -> GLGE::uvec4 {return a - b;}, "-", fn);

    //addition overflow (INT_MAX + 1 wraps to INT_MIN)
    vec4Test(GLGE::ivec4(2147483647, 2147483647, 2147483647, 2147483647), GLGE::ivec4(1, 1, 1, 1), GLGE::ivec4(-2147483648, -2147483648, -2147483648, -2147483648), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a + b;}, "+", fn);
    //subtraction underflow (INT_MIN - 1 wraps to INT_MAX)
    vec4Test(GLGE::ivec4(-2147483648, -2147483648, -2147483648, -2147483648), GLGE::ivec4(1, 1, 1, 1), GLGE::ivec4(2147483647, 2147483647, 2147483647, 2147483647), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a - b;}, "-", fn);

    //multiplication by Zero
    vec4Test(GLGE::vec4(0, 0, 0, 0), GLGE::vec4(55, -23, 12, 7), GLGE::vec4(0, 0, 0, 0), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a * b;}, "*", fn);
    //multiplication by One
    vec4Test(GLGE::vec4(1, 1, 1, 1), GLGE::vec4(55, -23, 12, 7), GLGE::vec4(55, -23, 12, 7), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a * b;}, "*", fn);

    //mixed signs and decimals
    vec4Test(GLGE::vec4(-0.5f, 10.5f, -2.25f, 5.0f), GLGE::vec4(0.5f, -10.5f, 2.25f, -5.0f), GLGE::vec4(0.0f, 0.0f, 0.0f, 0.0f), [](GLGE::vec4 a, GLGE::vec4 b) -> GLGE::vec4 {return a + b;}, "+", fn);

    //one component goes up, one goes down
    vec4Test(GLGE::ivec4(100, 100, 100, 100), GLGE::ivec4(100, -100, 0, -50), GLGE::ivec4(200, 0, 100, 50), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a + b;}, "+", fn);
    //zero out only one component via division
    vec4Test(GLGE::ivec4(25, 25, 25, 25), GLGE::ivec4(1, 5, 25, 1), GLGE::ivec4(25, 5, 1, 25), [](GLGE::ivec4 a, GLGE::ivec4 b) -> GLGE::ivec4 {return a / b;}, "/", fn);

    //message
    msg.msg = "[INFO] Testing geometric functions";
    (*(fn->log))(&msg);

    //standard Length (3^2 + 4^2 + 12^2 + 8^2 = 9 + 16 + 144 + 64 = 233 -> sqrt(233) approx 15.264338)
    vec4GeomTest(GLGE::vec4(3.0f, 4.0f, 12.0f, 8.0f), 15.264338f, [](GLGE::vec4 v) -> float {return glm::length(v);}, [](float a, float b) -> bool {return std::abs(a - b) < 1e-5f;}, "length", fn);
    //normalize standard vector
    vec4GeomTest(GLGE::vec4(0.0f, 5.0f, 0.0f, 0.0f), GLGE::vec4(0.0f, 1.0f, 0.0f, 0.0f), [](GLGE::vec4 v) -> GLGE::vec4 {return glm::normalize(v);}, [](GLGE::vec4 a, GLGE::vec4 b) -> bool {return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);}, "normalize", fn);
    //EDGE CASE: Normalize Zero Vector
    vec4GeomTest(GLGE::vec4(0.0f, 0.0f, 0.0f, 0.0f), GLGE::vec4(nanf(""), nanf(""), nanf(""), nanf("")), [](GLGE::vec4 v) -> GLGE::vec4 {return glm::normalize(v);}, [](GLGE::vec4 a, GLGE::vec4 b) -> bool {return glm::isnan(a.x) && glm::isnan(a.y) && glm::isnan(a.z) && glm::isnan(a.w);}, "normalize [Zero Vector Case]", fn);
    //EDGE CASE: Large Magnitude Vector
    vec4GeomTest<GLGE::vec4, GLGE::vec4>(GLGE::vec4(3e18f, 4e18f, 0.0f, 0.0f), GLGE::vec4(0.6f, 0.8f, 0.0f, 0.0f), [](GLGE::vec4 v) -> GLGE::vec4 {return glm::normalize(v);}, [](GLGE::vec4 a, GLGE::vec4 b) -> bool {return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);}, "normalize() [Large Vector Case]", fn);

    //EDGE CASE: Perpendicular vectors (Dot product must be exactly 0.0)
    vec4GeomTest<float, GLGE::vec4>(GLGE::vec4(1.0f, 0.0f, 0.0f, 0.0f), 0.0f, [](GLGE::vec4 v) -> float {return glm::dot(v, GLGE::vec4(0.0f, 1.0f, 0.0f, 0.0f));}, [](float a, float b) -> bool {return a == b;}, "dot [Perpendicular]", fn);
    //EDGE CASE: Opposing directions (Dot product must be exactly -1.0 when normalized)
    vec4GeomTest<float, GLGE::vec4>(GLGE::vec4(1.0f, 0.0f, 0.0f, 0.0f), -1.0f, [](GLGE::vec4 v) -> float {return glm::dot(v, GLGE::vec4(-1.0f, 0.0f, 0.0f, 0.0f));}, [](float a, float b) -> bool {return a == b;}, "dot [Opposite Alignment]", fn);
    //EDGE CASE: Flipped Negative Zero Sign Handling
    vec4GeomTest<GLGE::vec4, GLGE::vec4>(GLGE::vec4(0.0f, 0.0f, 0.0f, 0.0f), GLGE::vec4(-0.0f, -0.0f, -0.0f, -0.0f), [](GLGE::vec4 v) -> GLGE::vec4 {return v * -1.0f;}, [](GLGE::vec4 a, GLGE::vec4 b) -> bool {return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);}, "scalar multiplication [Negative Zero Check]", fn);

    //standard Vector * Scalar
    vec4GeomTest<GLGE::vec4, GLGE::vec4>(GLGE::vec4(4.0f, -2.5f, 1.0f, -0.5f), GLGE::vec4(12.0f, -7.5f, 3.0f, -1.5f), [](GLGE::vec4 v) -> GLGE::vec4 {return v * 3.0f;}, [](GLGE::vec4 a, GLGE::vec4 b) -> bool {return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);}, "operator*(Vector, Scalar)", fn);
    //commutative Scalar * Vector
    vec4GeomTest<GLGE::vec4, GLGE::vec4>(GLGE::vec4(4.0f, -2.5f, 1.0f, -0.5f), GLGE::vec4(12.0f, -7.5f, 3.0f, -1.5f), [](GLGE::vec4 v) -> GLGE::vec4 { return 3.0f * v; }, [](GLGE::vec4 a, GLGE::vec4 b) -> bool {return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);}, "operator*(Scalar, Vector)", fn);
    
    //success
    report->result = TEST_SUCCESS;
}

std::vector<Test> tests {
    Test{
        .header = {
            .sType = TEST_TEST,
            .pNext = nullptr
        },
        .entry = {
            .header = {
                .sType = TEST_ENTRY,
                .pNext = nullptr
            },
            .name = "2D vector test",
            .tags = "math vector suite",
            .description = "Test the functionality of the 2D vector types",
            .timeout = uint64_t(1E4),
            .requirements = 0
        },
        .invoker = &vec2Test
    },
    Test{
        .header = {
            .sType = TEST_TEST,
            .pNext = nullptr
        },
        .entry = {
            .header = {
                .sType = TEST_ENTRY,
                .pNext = nullptr
            },
            .name = "3D vector test",
            .tags = "math vector suite",
            .description = "Test the functionality of the 3D vector types",
            .timeout = uint64_t(1E4),
            .requirements = 0
        },
        .invoker = &vec3Test
    },
    Test{
        .header = {
            .sType = TEST_TEST,
            .pNext = nullptr
        },
        .entry = {
            .header = {
                .sType = TEST_ENTRY,
                .pNext = nullptr
            },
            .name = "4D vector test",
            .tags = "math vector suite",
            .description = "Test the functionality of the 3D vector types",
            .timeout = uint64_t(1E4),
            .requirements = 0
        },
        .invoker = &vec4Test
    }
};

TEST_REGISTER_TEST_FN {
    //store the test count
    if (data == NULL) {
        *size = tests.size();
        return;
    }
    //register all math tests
    memcpy(data, tests.data(), sizeof(Test)* (*size));
}

TEST_GET_ABI_VERSION_FN {
    return 1; //use the initial test ABI version
}