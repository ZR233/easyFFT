//
// Created by zrufo on 2022-05-27.
//
#include "gtest/gtest.h"
#include "easyFFT.h"
#include <vector>
#include <complex>


TEST(TestSuiteName, TestName) {
    FFT_ERROR_CODE err;
    int shape = 4;
    int batch = 2;

    std::vector<std::complex<float>> in(shape * batch, 0);
    std::vector<std::complex<float>> out(in.size());

    auto in_ptr = (ComplexF *)in.data();
    auto out_ptr = (ComplexF *)out.data();


    auto plan = fft_new_plan_float(
            1,
            &shape,
            2,
            FFT_SIGN::FFT_SIGN_FORWARD,
            FFT_DEVICE::FFT_DEVICE_CPU,
            in_ptr, in.size(),
            out_ptr, out.size(),
            &err);

    int i = 0;

    for (; i < shape; ++i) {
        in[i] = {(float )i, (float )-i};
    }
    for (; i < shape * 2; ++i) {
        in[i] = {(float )-i, (float )i};
    }

    fft_execute(plan);

    fft_close_plan(plan);

    EXPECT_EQ(1, 1);
}