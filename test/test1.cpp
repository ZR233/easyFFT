//
// Created by zrufo on 2022-05-27.
//
#include "gtest/gtest.h"
#include "easyFFT.h"
#include <vector>
#include <complex>
#include <ctime>
#include <iostream>
#include <strstream>



static void handle_err(struct Result *result){
    if (result->code != FFT_ERROR_CODE::OK){
        std::strstream ss;
        ss << "code: [" << result->code << "] " <<  result->msg;
        throw std::runtime_error(ss.str());
    }
}




TEST(TestAccuracy, TestGPUFFT) {
    auto result = fft_new_result();
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
                FFT_SIGN::FORWARD,
                FFT_DEVICE::CPU,
        }
    };
    auto planG = FFTPlanFloat{
            FFTPlanConfig{
                    1,
                    &shape,
                    2,
                    FFT_SIGN::FORWARD,
                    FFT_DEVICE::GPU,
            }
    };
    fft_planf_init(
            &plan,
            in_ptr, in.size(),
            out_ptr, out.size(), result);
    handle_err(result);

    fft_planf_init(
            &planG,
            inG_ptr, inG.size(),
            outG_ptr, outG.size(), result);

    handle_err(result);

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


    fft_planf_execute(&plan, result);
    handle_err(result);

    fft_planf_execute(&planG, result);
    handle_err(result);

    for (int j = 0; j < out.size(); ++j) {
        auto fftw = out[j];
        auto gpu = outG[j];

        auto real_dis = abs(fftw.real()-gpu.real());
        EXPECT_LT(real_dis, 0.01);
        auto imag_dis = abs(fftw.imag()-gpu.imag());
        EXPECT_LT(imag_dis, 0.01);
    }

    fft_close_planf(&plan);
    fft_close_planf(&planG);
}
TEST(TestAccuracy, TestGPUIFFT) {
    auto result = fft_new_result();

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
                    FFT_SIGN::BACKWARD,
                    FFT_DEVICE::CPU,
            }
    };
    auto planG = FFTPlanFloat{
            FFTPlanConfig{
                    1,
                    &shape,
                    2,
                    FFT_SIGN::BACKWARD,
                    FFT_DEVICE::GPU,
            }
    };
    fft_planf_init(
            &plan,
            in_ptr, in.size(),
            out_ptr, out.size(), result);
    handle_err(result);
    fft_planf_init(
            &planG,
            inG_ptr, inG.size(),
            outG_ptr, outG.size(), result);
    handle_err(result);

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


    fft_planf_execute(&plan, result);
    handle_err(result);
    fft_planf_execute(&planG, result);
    handle_err(result);

    for (int j = 0; j < out.size(); ++j) {
        auto fftw = out[j];
        auto gpu = outG[j];

        auto real_dis = abs(fftw.real()-gpu.real());
        EXPECT_LT(real_dis, 0.01);
        auto imag_dis = abs(fftw.imag()-gpu.imag());
        EXPECT_LT(imag_dis, 0.01);
    }

    fft_close_planf(&plan);
    fft_close_planf(&planG);
}



TEST(TestSuiteName, TestBench) {
    auto result = fft_new_result();

    int shape = 4096;
    int batch = 20000;

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
                    FFT_SIGN::FORWARD,
                    FFT_DEVICE::CPU,
            }
    };
    auto planG = FFTPlanFloat{
            FFTPlanConfig{
                    1,
                    &shape,
                    batch,
                    FFT_SIGN::FORWARD,
                    FFT_DEVICE::GPU,
            }
    };
    fft_planf_init(
            &plan,
            in_ptr, in.size(),
            out_ptr, out.size(), result);
    handle_err(result);
    fft_planf_init(
            &planG,
            inG_ptr, inG.size(),
            outG_ptr, outG.size(), result);

    handle_err(result);
    fft_planf_device_name(&planG, name, 50, result);
    handle_err(result);
    std::cout<<"name: "<< name <<std::endl;

    for (int j = 0; j < batch * shape; ++j) {
            in[j] = {1, -1};
    }

    for (int j = 0; j < in.size(); ++j) {
        inG[j] = in[j];
    }

    auto Time1=clock();
    fft_planf_execute(&plan, result);
    handle_err(result);
    auto Time2=clock();
    auto Time3 = Time2-Time1;
    std::cout<<"CPU cost: "<< Time3 <<std::endl;

    Time1=clock();
    fft_planf_execute(&planG, result);
    handle_err(result);
    Time2=clock();
    Time3 = Time2-Time1;
    std::cout<<"GPU cost: "<<Time3<<std::endl;

    fft_close_planf(&plan);
    fft_close_planf(&planG);
    EXPECT_EQ(1, 1);
}

TEST(TestSuiteName, BenchFFTW) {
    auto result = fft_new_result();

    int shape = 4096;
    int batch = 20000;

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
                    FFT_SIGN::FORWARD,
                    FFT_DEVICE::CPU,
            }
    };

    fft_planf_init(
            &plan,
            in_ptr, in.size(),
            out_ptr, out.size(), result);
    handle_err(result);
    std::cout<<"name: "<< name <<std::endl;

    for (int j = 0; j < batch * shape; ++j) {
        in[j] = {1, -1};
    }

    for (int j = 0; j < in.size(); ++j) {
        inG[j] = in[j];
    }

    auto Time1=clock();
    fft_planf_execute(&plan, result);
    handle_err(result);
    auto Time2=clock();
    auto Time3 = Time2-Time1;
    std::cout<<"cost: "<< Time3 <<std::endl;

    fft_close_planf(&plan);
    EXPECT_EQ(1, 1);
}