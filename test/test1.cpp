//
// Created by zrufo on 2022-05-27.
//
#include "gtest/gtest.h"
#include "easyFFT.h"
#include <vector>
#include <complex>

TEST(TestSuiteName, TestName) {
    FFT_ErrorCode err;
    int shape = 4;

    std::vector<std::complex<float>> in = {
            (1,-1), (2,-2), (3,-3) ,(4,-4),
            (-1,1), (-2,2), (-3,3) ,(-4,4),
    };
    std::vector<std::complex<float>> out(in.size());


    auto plan = fft_new_plan_float(
            1,
            &shape,
            2,
            FFT_SIGN::FFT_SIGN_FORWARD,
            FFT_DEVICE::FFT_DEVICE_CPU,
            (float *)in.data(), in.size() * 2,
            (float *)out.data(), out.size() * 2,
            &err);

    fft_close_plan(plan);

    EXPECT_EQ(1, 1);
}