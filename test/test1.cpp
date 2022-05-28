//
// Created by zrufo on 2022-05-27.
//
#include "gtest/gtest.h"
#include "easyFFT.h"
#include <vector>
#include <complex>

template<typename T>
std::vector<T> array2vec(T* ptr, size_t size){
    std::vector<T> vec;
    vec.resize(size);
    for (int i = 0; i < size; ++i) {
        vec[i]=ptr[i];
    }
    return vec;
}

TEST(TestSuiteName, TestName) {
    FFT_ErrorCode err;
    int shape = 4;
    int batch = 2;

    std::vector<std::complex<float>> in(shape * batch, 0);
    std::vector<std::complex<float>> out(in.size());

    auto in_ptr = (float *)in.data();
    auto out_ptr = (float *)out.data();

    auto show_in = array2vec(in_ptr, in.size() * 2);
    auto show_out = array2vec(out_ptr, out.size() * 2);

    auto plan = fft_new_plan_float(
            1,
            &shape,
            2,
            FFT_SIGN::FFT_SIGN_FORWARD,
            FFT_DEVICE::FFT_DEVICE_CPU,
            in_ptr, in.size() * 2,
            out_ptr, out.size() * 2,
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