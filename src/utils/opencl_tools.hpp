//
// Created by zrufo on 2022-06-04.
//

#ifndef EASYFFT_OPENCL_TOOLS_HPP
#define EASYFFT_OPENCL_TOOLS_HPP

#include "Device.hpp"
#include "vkFFT.h"
#include <memory>
#include "include/easyFFT.h"


namespace easyfft {
    namespace opencl {
        struct DeviceInfo {
            std::string name;
            uint32_t max_compute_units{};
        };

        static void handle_cl_err(cl_int err) {
            switch (err) {
                case CL_SUCCESS:
                    return;
                case CL_DEVICE_NOT_FOUND:
                    throw Exception("", FFT_ERROR_CODE::DEVICE_NOT_FOUND);
                default:
                    std::strstream ss;
                    ss << "(" << err << ")";
                    throw Exception(ss.str(), FFT_ERROR_CODE::OPEN_CL);
            }
        }

        static std::shared_ptr<DeviceInfo> getDeviceInfo(cl_device_id device) {
            auto info = std::make_shared<DeviceInfo>();
            char buffer[110];
            auto err = clGetDeviceInfo(device, CL_DEVICE_NAME, 100, buffer, nullptr);
            handle_cl_err(err);
            info->name.assign(buffer);
            cl_uint UnitNum;
            err = clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &UnitNum, nullptr);
            handle_cl_err(err);
            info->max_compute_units = UnitNum;

            return info;
        }

        static cl_int initDevice(easyfft::Device *driver_device, VkFFTConfiguration &configuration) {

            std::vector<std::shared_ptr<DeviceInfo>> device_infos;
            cl_uint platform_num;

            /* Setup OPEN_CL environment. */
            auto err = clGetPlatformIDs(0, nullptr, &platform_num);
            if (platform_num == 0) {
                throw Exception("", DEVICE_NOT_FOUND);
            }
            handle_cl_err(err);

            std::vector<cl_platform_id> platforms(platform_num);
            err = clGetPlatformIDs(platform_num, &platforms[0], nullptr);
            handle_cl_err(err);
            uint32_t max_compute_units = 0;

            for (auto platform: platforms) {
                cl_uint device_num;
                err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, nullptr, &device_num);
                if (err == CL_DEVICE_NOT_FOUND) {
                    continue;
                }
                handle_cl_err(err);

                std::vector<cl_device_id> devices(device_num);
                err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, device_num, &devices[0], nullptr);
                handle_cl_err(err);

                for (auto device_id: devices) {
                    auto info = getDeviceInfo(device_id);
                    device_infos.push_back(info);

                    if (info->max_compute_units > max_compute_units) {
                        driver_device->device_id = device_id;
                        driver_device->device_name = info->name;
                    }
                }
            }

            driver_device->ctx = clCreateContext(nullptr, 1, &driver_device->device_id, nullptr, nullptr, &err);
            handle_cl_err(err);

            driver_device->queue = clCreateCommandQueue(driver_device->ctx, driver_device->device_id, 0, &err);
            handle_cl_err(err);
            //Device management + code submission
            configuration.device = &driver_device->device_id;
            configuration.context = &driver_device->ctx;

            driver_device->buffer = clCreateBuffer(driver_device->ctx, CL_MEM_READ_WRITE, driver_device->buffer_size,
                                                   nullptr, &err);
            handle_cl_err(err);

            configuration.buffer = &driver_device->buffer;
        }

        template<typename T>
        static void
        execute(easyfft::Device *driver_device, VkFFTApplication *vk_app, T *data_in, T *data_out, int inverse) {
            auto err = clEnqueueWriteBuffer(driver_device->queue, driver_device->buffer, CL_TRUE, 0,
                                            driver_device->buffer_size,
                                            data_in, 0, nullptr, NULL);
            handle_cl_err(err);
            VkFFTLaunchParams launchParams = {};
            launchParams.buffer = &driver_device->buffer;

            launchParams.commandQueue = &driver_device->queue;

            auto err2 = VkFFTAppend(vk_app, inverse, &launchParams);
            handle_vkfft_err(err2);

            /* Wait for calculations to be finished. */
            err = clFinish(driver_device->queue);
            handle_cl_err(err);

            /* Fetch results of calculations. */
            err = clEnqueueReadBuffer(driver_device->queue, driver_device->buffer, CL_TRUE, 0,
                                      driver_device->buffer_size, data_out, 0, nullptr, NULL);
            handle_cl_err(err);
        }

        static void closeDevice(easyfft::Device *driver_device) {
            clReleaseMemObject(driver_device->buffer);
            clReleaseCommandQueue(driver_device->queue);
            clReleaseContext(driver_device->ctx);
        }
    }
}

#endif //EASYFFT_OPENCL_TOOLS_HPP
