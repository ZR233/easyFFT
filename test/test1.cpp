//
// Created by zrufo on 2022-05-27.
//
#include "gtest/gtest.h"
#include "easyFFT.h"
#include <vector>
#include <complex>
#include <ctime>
#include <iostream>

TEST(TestSuiteName, TestName) {
    FFT_ERROR_CODE err;
    int shape = 4;
    int batch = 2;

    std::vector<std::complex<float>> in(shape * batch, 0);
    std::vector<std::complex<float>> out(in.size(), 0);

    std::vector<std::complex<float>> inG(in.size(), 0);
    std::vector<std::complex<float>> outG(in.size(), 0);


    auto in_ptr = (ComplexF *)in.data();
    auto out_ptr = (ComplexF *)out.data();
    auto inG_ptr = (ComplexF *)inG.data();
    auto outG_ptr = (ComplexF *)outG.data();

    auto plan = FFTPlanFloat{
        FFTPlanConfig{
                1,
                &shape,
                2,
                FFT_SIGN::FFT_SIGN_FORWARD,
                FFT_DEVICE::FFT_DEVICE_CPU,
        }
    };
    auto planG = FFTPlanFloat{
            FFTPlanConfig{
                    1,
                    &shape,
                    2,
                    FFT_SIGN::FFT_SIGN_FORWARD,
                    FFT_DEVICE::FFT_DEVICE_GPU,
            }
    };
    err = fft_plan_init(
            &plan,
            in_ptr, in.size(),
            out_ptr, out.size());

    err = fft_plan_init(
            &planG,
            inG_ptr, inG.size(),
            outG_ptr, outG.size());
    int i = 0;

    for (; i < shape; ++i) {
        in[i] = {(float )i, (float )-i};
    }
    for (; i < shape * 2; ++i) {
        in[i] = {(float )-i, (float )i};
    }

    for (int j = 0; j < in.size(); ++j) {
        inG[j] = in[j];
    }


    fft_execute(&plan);
    fft_execute(&planG);
    fft_close_plan(&plan);
    fft_close_plan(&planG);
    EXPECT_EQ(1, 1);
}




TEST(TestSuiteName, TestBench) {
    FFT_ERROR_CODE err;
    int shape = 4096;
    int batch = 10;

    std::vector<std::complex<float>> in(shape * batch, {1,1});
    std::vector<std::complex<float>> out(in.size(), 0);

    std::vector<std::complex<float>> inG(in.size(), 0);
    std::vector<std::complex<float>> outG(in.size(), 0);

    char name[50];


    auto in_ptr = (ComplexF *)in.data();
    auto out_ptr = (ComplexF *)out.data();
    auto inG_ptr = (ComplexF *)inG.data();
    auto outG_ptr = (ComplexF *)outG.data();

    auto plan = FFTPlanFloat{
            FFTPlanConfig{
                    1,
                    &shape,
                    batch,
                    FFT_SIGN::FFT_SIGN_FORWARD,
                    FFT_DEVICE::FFT_DEVICE_CPU,
            }
    };
    auto planG = FFTPlanFloat{
            FFTPlanConfig{
                    1,
                    &shape,
                    batch,
                    FFT_SIGN::FFT_SIGN_FORWARD,
                    FFT_DEVICE::FFT_DEVICE_GPU,
            }
    };
    err = fft_plan_init(
            &plan,
            in_ptr, in.size(),
            out_ptr, out.size());

    err = fft_plan_init(
            &planG,
            inG_ptr, inG.size(),
            outG_ptr, outG.size());
    int i = 0;

    fft_plan_device_name(&planG, name, 50);

    std::cout<<"name: "<< name <<std::endl;

    for (int j = 0; j < batch * shape; ++j) {
            in[j] = {1, -1};
    }




    for (int j = 0; j < in.size(); ++j) {
        inG[j] = in[j];
    }

    auto Time1=clock();
    fft_execute(&plan);
    auto Time2=clock();
    auto Time3 = Time2-Time1;
    std::cout<<"cost 1:"<< Time3 <<std::endl;

    Time1=clock();
    fft_execute(&planG);
    Time2=clock();
    Time3 = Time2-Time1;
    std::cout<<"cost 2:"<<Time3<<std::endl;

    fft_close_plan(&plan);
    fft_close_plan(&planG);
    EXPECT_EQ(1, 1);
}