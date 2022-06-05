//
// Created by zrufo on 2022-06-04.
//

#ifndef EASYFFT_DEVICE_HPP
#define EASYFFT_DEVICE_HPP
#include "vkFFT.h"
#include <string>
#include "../Exception.hpp"

namespace easyfft{
    struct Device{
#if(VKFFT_BACKEND==0)
        VkInstance instance;//a connection between the application and the Vulkan library
        VkPhysicalDevice physicalDevice;//a handle for the graphics card used in the application
        VkPhysicalDeviceProperties physicalDeviceProperties;//bastic device properties
        VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;//bastic memory properties of the device
        VkDevice device;//a logical device, interacting with physical device
        VkDebugUtilsMessengerEXT debugMessenger;//extension for debugging
        uint64_t queueFamilyIndex;//if multiple queues are available, specify the used one
        VkQueue queue;//a place, where all operations are submitted
        VkCommandPool commandPool;//an opaque objects that command buffer memory is allocated from
        VkFence fence;//a vkGPU->fence used to synchronize dispatches
        std::vector<const char*> enabledDeviceExtensions;
        uint64_t enableValidationLayers;
        uint64_t device_id{};//an id of a device, reported by Vulkan device list
        VkBuffer buffer = {};
        VkDeviceMemory bufferDeviceMemory = {};
#elif(VKFFT_BACKEND==1)
        CUdevice device;
	CUcontext context;
#elif(VKFFT_BACKEND==2)
	hipDevice_t device;
	hipCtx_t context;
#elif(VKFFT_BACKEND==3)
    cl_mem buffer= nullptr;
    cl_context ctx = nullptr;
    cl_command_queue queue = nullptr;
    cl_device_id device_id = nullptr;

#elif(VKFFT_BACKEND==4)
	ze_driver_handle_t driver;
	ze_device_handle_t device;
	ze_context_handle_t context;
	ze_command_queue_handle_t commandQueue;
	uint32_t commandQueueID;
#endif
        uint64_t buffer_size = 0;
        std::string device_name;

    };
}



#endif //EASYFFT_DEVICE_HPP
