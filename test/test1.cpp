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
#include <chrono>
#include <random>

static void handle_err(struct Result *result){
    if (result->code != FFT_ERROR_CODE::OK){
        std::strstream ss;
        ss << "code: [" << result->code << "] " <<  result->msg;
        throw std::runtime_error(ss.str());
    }
}
std::vector<std::complex<float>> new_data(int nFFT, int batches){
    std::uniform_int_distribution<signed > u(0, 255);
    std::default_random_engine e(clock());
    std::vector<std::complex<float>> data(nFFT * batches);
    for (auto & i : data) {
        auto r = (float )u(e);
        auto imag = (float )u(e);
        i=std::complex<float>( r, imag);
    }

    return data;
}



TEST(TestAccuracy, TestGPUFFT) {
    auto result = fft_new_result();
    int shape = 256;
    int batch = 50000;

    auto sample = new_data(shape, batch);

    std::vector<std::complex<float>> in(shape * batch, 0);
    std::vector<std::complex<float>> out(in.size(), 0);

    std::vector<std::complex<float>> inG(in.size(), 0);
    std::vector<std::complex<float>> outG(in.size(), 0);


    auto in_ptr = (ComplexF *)in.data();
    auto out_ptr = (ComplexF *)out.data();
    auto inG_ptr = (ComplexF *)inG.data();
    auto outG_ptr = (ComplexF *)outG.data();

    auto plan = fft_planf_init(
            FFTPlanConfig{
                    1,
                    &shape,
                    batch,
                    FFT_SIGN::FORWARD,
                    FFT_DEVICE::CPU,
            },
            in_ptr, in.size(),
            out_ptr, out.size(), result);
    handle_err(result);

    auto planG = fft_planf_init(
            FFTPlanConfig{
                    1,
                    &shape,
                    batch,
                    FFT_SIGN::FORWARD,
                    FFT_DEVICE::GPU,
            },
            inG_ptr, in.size(),
            outG_ptr, out.size(), result);

    handle_err(result);


    for (int i = 0; i < sample.size(); ++i) {
        in[i] = sample[i];
    }


    for (int j = 0; j < in.size(); ++j) {
        inG[j] = in[j];
    }

    auto begin = std::chrono::system_clock::now();
    fft_planf_execute(plan, result);
    handle_err(result);
    auto end = std::chrono::system_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    std::string name;
    name.resize(100, '\0');
    fft_planf_device_name(plan, &name[0], 100, result);

    std::cout << name.c_str() << " cost: " << duration.count() << " ms"<< std::endl;

    begin = std::chrono::system_clock::now();
    fft_planf_execute(planG, result);
    end = std::chrono::system_clock::now();
    handle_err(result);

    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    fft_planf_device_name(planG, &name[0], 100, result);
    std::cout <<  name.c_str() << " cost: " << duration.count() << " ms" << std::endl;



    for (int j = 0; j < out.size(); ++j) {
        auto fftw = out[j];
        auto gpu = outG[j];

        auto real_dis = abs(fftw.real()-gpu.real());
        EXPECT_LT(real_dis, 0.01);
        auto imag_dis = abs(fftw.imag()-gpu.imag());
        EXPECT_LT(imag_dis, 0.01);
    }

    fft_close_planf(plan);
    fft_close_planf(planG);

}
//TEST(TestAccuracy, TestGPUIFFT) {
//    auto result = fft_new_result();
//
//    int shape = 4;
//    int batch = 2;
//
//    std::vector<std::complex<float>> in(shape * batch, 0);
//    std::vector<std::complex<float>> out(in.size(), 0);
//
//    std::vector<std::complex<float>> inG(in.size(), 0);
//    std::vector<std::complex<float>> outG(in.size(), 0);
//
//
//    auto in_ptr = (ComplexF *)in.data();
//    auto out_ptr = (ComplexF *)out.data();
//    auto inG_ptr = (ComplexF *)inG.data();
//    auto outG_ptr = (ComplexF *)outG.data();
//
//    auto plan = FFTPlanFloat{
//            FFTPlanConfig{
//                    1,
//                    &shape,
//                    2,
//                    FFT_SIGN::BACKWARD,
//                    FFT_DEVICE::CPU,
//            }
//    };
//    auto planG = FFTPlanFloat{
//            FFTPlanConfig{
//                    1,
//                    &shape,
//                    2,
//                    FFT_SIGN::BACKWARD,
//                    FFT_DEVICE::GPU,
//            }
//    };
//    fft_planf_init(
//            &plan,
//            in_ptr, in.size(),
//            out_ptr, out.size(), result);
//    handle_err(result);
//    fft_planf_init(
//            &planG,
//            inG_ptr, inG.size(),
//            outG_ptr, outG.size(), result);
//    handle_err(result);
//
//    int i = 0;
//
//    for (; i < shape; ++i) {
//        in[i] = {(float )i, (float )-i};
//    }
//    for (; i < shape * 2; ++i) {
//        in[i] = {(float )-i, (float )i};
//    }
//
//    for (int j = 0; j < in.size(); ++j) {
//        inG[j] = in[j];
//    }
//
//
//    fft_planf_execute(&plan, result);
//    handle_err(result);
//    fft_planf_execute(&planG, result);
//    handle_err(result);
//
//    for (int j = 0; j < out.size(); ++j) {
//        auto fftw = out[j];
//        auto gpu = outG[j];
//
//        auto real_dis = abs(fftw.real()-gpu.real());
//        EXPECT_LT(real_dis, 0.01);
//        auto imag_dis = abs(fftw.imag()-gpu.imag());
//        EXPECT_LT(imag_dis, 0.01);
//    }
//
//    fft_close_planf(&plan);
//    fft_close_planf(&planG);
//}
//
//
//
//TEST(TestSuiteName, TestBench) {
//    auto result = fft_new_result();
//
//    int shape = 1024 *512;
//    int batch = 1;
//
//    std::vector<std::complex<float>> in(shape * batch, {1,1});
//    std::vector<std::complex<float>> out(in.size(), 0);
//
//    std::vector<std::complex<float>> inG(in.size(), 0);
//    std::vector<std::complex<float>> outG(in.size(), 0);
//
//    char name[50];
//
//    auto in_ptr = (ComplexF *)in.data();
//    auto out_ptr = (ComplexF *)out.data();
//    auto inG_ptr = (ComplexF *)inG.data();
//    auto outG_ptr = (ComplexF *)outG.data();
//
//    auto plan = FFTPlanFloat{
//            FFTPlanConfig{
//                    1,
//                    &shape,
//                    batch,
//                    FFT_SIGN::FORWARD,
//                    FFT_DEVICE::CPU,
//            }
//    };
//    auto planG = FFTPlanFloat{
//            FFTPlanConfig{
//                    1,
//                    &shape,
//                    batch,
//                    FFT_SIGN::FORWARD,
//                    FFT_DEVICE::GPU,
//            }
//    };
//    fft_planf_init(
//            &plan,
//            in_ptr, in.size(),
//            out_ptr, out.size(), result);
//    handle_err(result);
//    fft_planf_init(
//            &planG,
//            inG_ptr, inG.size(),
//            outG_ptr, outG.size(), result);
//
//    handle_err(result);
//    fft_planf_device_name(&planG, name, 50, result);
//    handle_err(result);
//    std::cout<<"name: "<< name <<std::endl;
//
//    for (int j = 0; j < batch * shape; ++j) {
//            in[j] = {1, -1};
//    }
//
//    for (int j = 0; j < in.size(); ++j) {
//        inG[j] = in[j];
//    }
//
//    auto Time1=clock();
//    fft_planf_execute(&plan, result);
//    handle_err(result);
//    auto Time2=clock();
//    auto Time3 = Time2-Time1;
//    std::cout<<"CPU cost: "<<(double)Time3 / CLOCKS_PER_SEC * 1000.0<<" ms" <<std::endl;
//
//    Time1=clock();
//    fft_planf_execute(&planG, result);
//    handle_err(result);
//    Time2=clock();
//    Time3 = Time2-Time1;
//    std::cout<<"GPU cost: "<<(double)Time3 / CLOCKS_PER_SEC * 1000.0<<" ms" <<std::endl;
//
//    fft_close_planf(&plan);
//    fft_close_planf(&planG);
//    EXPECT_EQ(1, 1);
//}
//
//TEST(TestSuiteName, BenchFFTW) {
//    auto result = fft_new_result();
//
//    int shape = 1024;
//    int batch = 20000;
//
//    std::vector<std::complex<float>> in(shape * batch, {1,1});
//    std::vector<std::complex<float>> out(in.size(), 0);
//
//    std::vector<std::complex<float>> inG(in.size(), 0);
//    std::vector<std::complex<float>> outG(in.size(), 0);
//
//    char name[50];
//
//    auto in_ptr = (ComplexF *)in.data();
//    auto out_ptr = (ComplexF *)out.data();
//    auto inG_ptr = (ComplexF *)inG.data();
//    auto outG_ptr = (ComplexF *)outG.data();
//
//    auto plan = FFTPlanFloat{
//            FFTPlanConfig{
//                    1,
//                    &shape,
//                    batch,
//                    FFT_SIGN::FORWARD,
//                    FFT_DEVICE::CPU,
//            }
//    };
//
//    fft_planf_init(
//            &plan,
//            in_ptr, in.size(),
//            out_ptr, out.size(), result);
//    handle_err(result);
//    std::cout<<"name: "<< name <<std::endl;
//
//    for (int j = 0; j < batch * shape; ++j) {
//        in[j] = {1, -1};
//    }
//
//    for (int j = 0; j < in.size(); ++j) {
//        inG[j] = in[j];
//    }
//
//    auto Time1=clock();
//    fft_planf_execute(&plan, result);
//    handle_err(result);
//    auto Time2=clock();
//    auto Time3 = Time2-Time1;
//    std::cout<<"cost: "<< (double)Time3 / CLOCKS_PER_SEC * 1000.0 << " ms" <<std::endl;
//
//    fft_close_planf(&plan);
//    EXPECT_EQ(1, 1);
//}