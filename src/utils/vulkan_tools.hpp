//
// Created by zrufo on 2022-06-04.
//
#pragma once
#ifndef EASYFFT_VULKAN_TOOLS_HPP
#define EASYFFT_VULKAN_TOOLS_HPP

#include "vulkan/vulkan.h"
#include "glslang_c_interface.h"
#include "../include/easyFFT.h"
#include "Device.hpp"
#include <vector>

namespace easyfft {
    namespace vulcan {

        static VkResult checkValidationLayerSupport() {
            //check if validation layers are supported when an instance is created
            uint32_t layerCount;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

            std::vector<VkLayerProperties> availableLayers(layerCount);

//    VkLayerProperties* availableLayers = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * layerCount);
//    if (!availableLayers) return VK_INCOMPLETE;
            vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

            for (uint64_t i = 0; i < layerCount; i++) {
                if (strcmp("VK_LAYER_KHRONOS_validation", availableLayers[i].layerName) == 0) {

                    return VK_SUCCESS;
                }
            }


            return VK_ERROR_LAYER_NOT_PRESENT;
        }

        static std::vector<const char *> getRequiredExtensions(easyfft::Device *vkGPU) {
            std::vector<const char *> extensions;

            if (vkGPU->enableValidationLayers) {
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }
//    switch (sample_id) {
//#if (VK_API_VERSION>10)
//        case 2: case 102:
//		extensions.push_back("VK_KHR_get_physical_device_properties2");
//		break;
//#endif
//        default:
//            break;
//    }


            return extensions;
        }

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                            VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                            void *pUserData) {
            printf("validation layer: %s\n", pCallbackData->pMessage);
            return VK_FALSE;
        }


        static VkResult createInstance(easyfft::Device *vkGPU) {
            //create instance - a connection between the application and the Vulkan library
            VkResult res = VK_SUCCESS;
            //check if validation layers are supported
            if (vkGPU->enableValidationLayers == 1) {
                res = checkValidationLayerSupport();
                if (res != VK_SUCCESS) return res;
            }

            VkApplicationInfo applicationInfo = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
            applicationInfo.pApplicationName = "VkFFT";
            applicationInfo.applicationVersion = (uint32_t) VkFFTGetVersion();
            applicationInfo.pEngineName = "VkFFT";
            applicationInfo.engineVersion = 1;
#if (VK_API_VERSION >= 12)
            applicationInfo.apiVersion = VK_API_VERSION_1_2;
#elif (VK_API_VERSION == 11)
            applicationInfo.apiVersion = VK_API_VERSION_1_1;
#else
            applicationInfo.apiVersion = VK_API_VERSION_1_0;
#endif

            VkInstanceCreateInfo createInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
            createInfo.flags = 0;
            createInfo.pApplicationInfo = &applicationInfo;

            auto extensions = getRequiredExtensions(vkGPU);
            createInfo.enabledExtensionCount = (uint32_t) (extensions.size());
            createInfo.ppEnabledExtensionNames = extensions.data();

            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {
                    VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
            if (vkGPU->enableValidationLayers) {
                //query for the validation layer support in the instance
                createInfo.enabledLayerCount = 1;
                const char *validationLayers = "VK_LAYER_KHRONOS_validation";
                createInfo.ppEnabledLayerNames = &validationLayers;
                debugCreateInfo.messageSeverity =
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
                debugCreateInfo.messageType =
                        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
                debugCreateInfo.pfnUserCallback = debugCallback;
                createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
            } else {
                createInfo.enabledLayerCount = 0;

                createInfo.pNext = nullptr;
            }

            res = vkCreateInstance(&createInfo, nullptr, &vkGPU->instance);
            if (res != VK_SUCCESS) return res;

            return res;
        }

        static VkResult
        CreateDebugUtilsMessengerEXT(easyfft::Device *vkGPU, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                     const VkAllocationCallbacks *pAllocator,
                                     VkDebugUtilsMessengerEXT *pDebugMessenger) {
            //pointer to the function, as it is not part of the core. Function creates debugging messenger
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vkGPU->instance,
                                                                                   "vkCreateDebugUtilsMessengerEXT");
            if (func != nullptr) {
                return func(vkGPU->instance, pCreateInfo, pAllocator, pDebugMessenger);
            } else {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }

        static VkResult setupDebugMessenger(easyfft::Device *vkGPU) {
            //function that sets up the debugging messenger
            if (vkGPU->enableValidationLayers == 0) return VK_SUCCESS;

            VkDebugUtilsMessengerCreateInfoEXT createInfo = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
            createInfo.messageSeverity =
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            createInfo.messageType =
                    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            createInfo.pfnUserCallback = debugCallback;

            if (CreateDebugUtilsMessengerEXT(vkGPU, &createInfo, nullptr, &vkGPU->debugMessenger) != VK_SUCCESS) {
                return VK_ERROR_INITIALIZATION_FAILED;
            }
            return VK_SUCCESS;
        }


        static VkResult findPhysicalDevice(easyfft::Device *vkGPU) {
            //check if there are GPUs that support Vulkan and select one

            uint32_t deviceCount;
            auto res = vkEnumeratePhysicalDevices(vkGPU->instance, &deviceCount, nullptr);
            if (res != VK_SUCCESS) return res;
            if (deviceCount == 0) {
                return VK_ERROR_DEVICE_LOST;
            }

            std::vector<VkPhysicalDevice> devices(deviceCount);

            res = vkEnumeratePhysicalDevices(vkGPU->instance, &deviceCount, devices.data());
            if (res != VK_SUCCESS) return res;

            vkGPU->physicalDevice = devices[0];

            return VK_SUCCESS;
        }

        static VkResult getComputeQueueFamilyIndex(easyfft::Device *vkGPU) {
            //find a queue family for a selected GPU, select the first available for use
            uint32_t queueFamilyCount;
            vkGetPhysicalDeviceQueueFamilyProperties(vkGPU->physicalDevice, &queueFamilyCount, nullptr);

            auto *queueFamilies = (VkQueueFamilyProperties *) malloc(
                    sizeof(VkQueueFamilyProperties) * queueFamilyCount);
            if (!queueFamilies) return VK_INCOMPLETE;
            if (queueFamilies) {
                vkGetPhysicalDeviceQueueFamilyProperties(vkGPU->physicalDevice, &queueFamilyCount, queueFamilies);
                uint64_t i = 0;
                for (; i < queueFamilyCount; i++) {
                    VkQueueFamilyProperties props = queueFamilies[i];

                    if (props.queueCount > 0 && (props.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
                        break;
                    }
                }
                free(queueFamilies);
                if (i == queueFamilyCount) {
                    return VK_ERROR_INITIALIZATION_FAILED;
                }
                vkGPU->queueFamilyIndex = i;
                return VK_SUCCESS;
            } else
                return VK_INCOMPLETE;
        }

        static VkResult createDevice(easyfft::Device *vkGPU) {
            //create logical device representation
            VkResult res = VK_SUCCESS;
            VkDeviceQueueCreateInfo queueCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
            res = getComputeQueueFamilyIndex(vkGPU);
            if (res != VK_SUCCESS) return res;
            queueCreateInfo.queueFamilyIndex = (uint32_t) vkGPU->queueFamilyIndex;
            queueCreateInfo.queueCount = 1;
            float queuePriorities = 1.0;
            queueCreateInfo.pQueuePriorities = &queuePriorities;
            VkDeviceCreateInfo deviceCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
            VkPhysicalDeviceFeatures deviceFeatures = {};


            deviceCreateInfo.enabledExtensionCount = (uint32_t) vkGPU->enabledDeviceExtensions.size();
            deviceCreateInfo.ppEnabledExtensionNames = vkGPU->enabledDeviceExtensions.data();
            deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
            deviceCreateInfo.queueCreateInfoCount = 1;
            deviceCreateInfo.pEnabledFeatures = nullptr;
            deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
            res = vkCreateDevice(vkGPU->physicalDevice, &deviceCreateInfo, nullptr, &vkGPU->device);
            if (res != VK_SUCCESS) return res;
            vkGetDeviceQueue(vkGPU->device, (uint32_t) vkGPU->queueFamilyIndex, 0, &vkGPU->queue);


            return res;
        }

        static VkResult createFence(easyfft::Device *vkGPU) {
            //create fence for synchronization
            VkResult res = VK_SUCCESS;
            VkFenceCreateInfo fenceCreateInfo = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
            fenceCreateInfo.flags = 0;
            res = vkCreateFence(vkGPU->device, &fenceCreateInfo, nullptr, &vkGPU->fence);
            return res;
        }

        static VkResult createCommandPool(easyfft::Device *vkGPU) {
            //create a place, command buffer memory is allocated from
            VkResult res = VK_SUCCESS;
            VkCommandPoolCreateInfo commandPoolCreateInfo = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
            commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            commandPoolCreateInfo.queueFamilyIndex = (uint32_t) vkGPU->queueFamilyIndex;
            res = vkCreateCommandPool(vkGPU->device, &commandPoolCreateInfo, nullptr, &vkGPU->commandPool);
            return res;
        }

        static void findMemoryType(
                easyfft::Device *vkGPU,
                uint64_t memoryTypeBits,
                uint64_t memorySize,
                VkMemoryPropertyFlags properties,
                uint32_t *memoryTypeIndex) {

            VkPhysicalDeviceMemoryProperties memoryProperties = {0};

            vkGetPhysicalDeviceMemoryProperties(vkGPU->physicalDevice, &memoryProperties);

            for (uint64_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
                if ((memoryTypeBits & ((uint64_t)1 << i))
                && ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
                   && (memoryProperties.memoryHeaps[memoryProperties.memoryTypes[i].heapIndex].size >= memorySize))
                {
                    memoryTypeIndex[0] = (uint32_t) i;
                    return;
                }
            }
            throw Exception("VKFFT_ERROR_FAILED_TO_FIND_MEMORY", FFT_ERROR_CODE::VKFFT);
        }

        static void allocateBuffer(
                easyfft::Device *vkGPU,
                VkBuffer* buffer,
                VkDeviceMemory* deviceMemory,
                VkBufferUsageFlags usageFlags,
                VkMemoryPropertyFlags propertyFlags,
                uint64_t size) {

            //allocate the buffer used by the GPU with specified properties
            VkFFTResult resFFT = VKFFT_SUCCESS;
            VkResult res = VK_SUCCESS;
            uint32_t queueFamilyIndices;
            VkBufferCreateInfo bufferCreateInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
            bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            bufferCreateInfo.queueFamilyIndexCount = 1;
            bufferCreateInfo.pQueueFamilyIndices = &queueFamilyIndices;
            bufferCreateInfo.size = size;
            bufferCreateInfo.usage = usageFlags;
            res = vkCreateBuffer(vkGPU->device, &bufferCreateInfo, nullptr, buffer);
            handle_vk_err(res);

            VkMemoryRequirements memoryRequirements = {0};
            vkGetBufferMemoryRequirements(vkGPU->device, buffer[0], &memoryRequirements);
            VkMemoryAllocateInfo memoryAllocateInfo = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
            memoryAllocateInfo.allocationSize = memoryRequirements.size;

            findMemoryType(vkGPU, memoryRequirements.memoryTypeBits, memoryRequirements.size, propertyFlags,
                                    &memoryAllocateInfo.memoryTypeIndex);
            handle_vk_err(res);
            res = vkAllocateMemory(vkGPU->device, &memoryAllocateInfo, nullptr, deviceMemory);
            handle_vk_err(res);
            res = vkBindBufferMemory(vkGPU->device, buffer[0], deviceMemory[0], 0);
            handle_vk_err(res);
        }

        static VkResult initDevice(easyfft::Device *driver_device, VkFFTConfiguration &configuration) {
            //create instance - a connection between the application and the Vulkan library
            auto res = createInstance(driver_device);
            if (res != VK_SUCCESS) return res;
            res = setupDebugMessenger(driver_device);
            if (res != VK_SUCCESS) return res;
            res = findPhysicalDevice(driver_device);
            if (res != VK_SUCCESS) return res;
            res = createDevice(driver_device);
            if (res != VK_SUCCESS) return res;
            //create fence for synchronization
            res = createFence(driver_device);
            if (res != VK_SUCCESS) return res;
            //create a place, command buffer memory is allocated from
            res = createCommandPool(driver_device);
            if (res != VK_SUCCESS) return res;
            vkGetPhysicalDeviceProperties(driver_device->physicalDevice, &driver_device->physicalDeviceProperties);
            vkGetPhysicalDeviceMemoryProperties(driver_device->physicalDevice,
                                                &driver_device->physicalDeviceMemoryProperties);

            glslang_initialize_process();//compiler can be initialized before VkFFT

            configuration.device = &driver_device->device;
            configuration.queue = &driver_device->queue;
            configuration.fence = &driver_device->fence;
            configuration.commandPool = &driver_device->commandPool;
            configuration.physicalDevice = &driver_device->physicalDevice;
            configuration.isCompilerInitialized = 1;
//glslang compiler can be initialized before VkFFT plan creation. if not, VkFFT will create and destroy one after initialization

            allocateBuffer(driver_device,
                           &driver_device->buffer,
                           &driver_device->bufferDeviceMemory,
                           VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                           VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                           VK_MEMORY_HEAP_DEVICE_LOCAL_BIT,
                           driver_device->buffer_size);


            configuration.buffer = &driver_device->buffer;

            return VK_SUCCESS;
        }
        static void transferDataFromCPU(easyfft::Device* vkGPU, void* arr, VkBuffer* buffer, uint64_t bufferSize) {
            //a function that transfers data from the CPU to the GPU using staging buffer, because the GPU memory is not host-coherent
            VkFFTResult resFFT = VKFFT_SUCCESS;
            VkResult res = VK_SUCCESS;
            uint64_t stagingBufferSize = bufferSize;
            VkBuffer stagingBuffer = {nullptr };
            VkDeviceMemory stagingBufferMemory = {nullptr };

            allocateBuffer(vkGPU,
                           &stagingBuffer,
                           &stagingBufferMemory,
                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBufferSize);


            handle_vk_err(res);
            void* data;
            res = vkMapMemory(vkGPU->device, stagingBufferMemory, 0, stagingBufferSize, 0, &data);
            handle_vk_err(res);
            memcpy(data, arr, stagingBufferSize);
            vkUnmapMemory(vkGPU->device, stagingBufferMemory);
            VkCommandBufferAllocateInfo commandBufferAllocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
            commandBufferAllocateInfo.commandPool = vkGPU->commandPool;
            commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            commandBufferAllocateInfo.commandBufferCount = 1;
            VkCommandBuffer commandBuffer = { 0 };
            res = vkAllocateCommandBuffers(vkGPU->device, &commandBufferAllocateInfo, &commandBuffer);
            handle_vk_err(res);
            VkCommandBufferBeginInfo commandBufferBeginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
            commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            res = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
            handle_vk_err(res);
            VkBufferCopy copyRegion = { 0 };
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;
            copyRegion.size = stagingBufferSize;
            vkCmdCopyBuffer(commandBuffer, stagingBuffer, buffer[0], 1, &copyRegion);
            res = vkEndCommandBuffer(commandBuffer);
            handle_vk_err(res);
            VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;
            res = vkQueueSubmit(vkGPU->queue, 1, &submitInfo, vkGPU->fence);
            handle_vk_err(res);
            res = vkWaitForFences(vkGPU->device, 1, &vkGPU->fence, VK_TRUE, 100000000000);
            handle_vk_err(res);
            res = vkResetFences(vkGPU->device, 1, &vkGPU->fence);
            handle_vk_err(res);
            vkFreeCommandBuffers(vkGPU->device, vkGPU->commandPool, 1, &commandBuffer);
            vkDestroyBuffer(vkGPU->device, stagingBuffer, NULL);
            vkFreeMemory(vkGPU->device, stagingBufferMemory, NULL);

        }
        static VkFFTResult transferDataToCPU(easyfft::Device* vkGPU, void* arr, VkBuffer* buffer, uint64_t bufferSize) {
            //a function that transfers data from the GPU to the CPU using staging buffer, because the GPU memory is not host-coherent
            VkFFTResult resFFT = VKFFT_SUCCESS;
            VkResult res = VK_SUCCESS;
            uint64_t stagingBufferSize = bufferSize;
            VkBuffer stagingBuffer = { nullptr };
            VkDeviceMemory stagingBufferMemory = { nullptr };
            allocateBuffer(
                    vkGPU,
                    &stagingBuffer,
                    &stagingBufferMemory,
                    VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                    stagingBufferSize);

            if (resFFT != VKFFT_SUCCESS) return resFFT;
            VkCommandBufferAllocateInfo commandBufferAllocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
            commandBufferAllocateInfo.commandPool = vkGPU->commandPool;
            commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            commandBufferAllocateInfo.commandBufferCount = 1;
            VkCommandBuffer commandBuffer = { 0 };
            res = vkAllocateCommandBuffers(vkGPU->device, &commandBufferAllocateInfo, &commandBuffer);
            if (res != VK_SUCCESS) return VKFFT_ERROR_FAILED_TO_ALLOCATE_COMMAND_BUFFERS;
            VkCommandBufferBeginInfo commandBufferBeginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
            commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            res = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
            if (res != VK_SUCCESS) return VKFFT_ERROR_FAILED_TO_BEGIN_COMMAND_BUFFER;
            VkBufferCopy copyRegion = { 0 };
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;
            copyRegion.size = stagingBufferSize;
            vkCmdCopyBuffer(commandBuffer, buffer[0], stagingBuffer, 1, &copyRegion);
            res = vkEndCommandBuffer(commandBuffer);
            if (res != VK_SUCCESS) return VKFFT_ERROR_FAILED_TO_END_COMMAND_BUFFER;
            VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;
            res = vkQueueSubmit(vkGPU->queue, 1, &submitInfo, vkGPU->fence);
            if (res != VK_SUCCESS) return VKFFT_ERROR_FAILED_TO_SUBMIT_QUEUE;
            res = vkWaitForFences(vkGPU->device, 1, &vkGPU->fence, VK_TRUE, 100000000000);
            if (res != VK_SUCCESS) return VKFFT_ERROR_FAILED_TO_WAIT_FOR_FENCES;
            res = vkResetFences(vkGPU->device, 1, &vkGPU->fence);
            if (res != VK_SUCCESS) return VKFFT_ERROR_FAILED_TO_RESET_FENCES;
            vkFreeCommandBuffers(vkGPU->device, vkGPU->commandPool, 1, &commandBuffer);
            void* data;
            res = vkMapMemory(vkGPU->device, stagingBufferMemory, 0, stagingBufferSize, 0, &data);
            if (resFFT != VKFFT_SUCCESS) return resFFT;
            memcpy(arr, data, stagingBufferSize);
            vkUnmapMemory(vkGPU->device, stagingBufferMemory);
            vkDestroyBuffer(vkGPU->device, stagingBuffer, NULL);
            vkFreeMemory(vkGPU->device, stagingBufferMemory, NULL);
            return resFFT;
        }
        template<typename T>
        static void execute(easyfft::Device *driver_device, VkFFTApplication *vk_app, T *data_in, T *data_out) {
            transferDataFromCPU(driver_device, data_in, &driver_device->buffer, driver_device->buffer_size);


            VkCommandBufferAllocateInfo commandBufferAllocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
            commandBufferAllocateInfo.commandPool = driver_device->commandPool;
            commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            commandBufferAllocateInfo.commandBufferCount = 1;
            VkCommandBuffer commandBuffer = {};
            auto res = vkAllocateCommandBuffers(driver_device->device, &commandBufferAllocateInfo, &commandBuffer);
            handle_vk_err(res);
            VkCommandBufferBeginInfo commandBufferBeginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
            commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            res = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
            handle_vk_err(res);

            VkFFTLaunchParams launchParams = {};
            launchParams.buffer = &driver_device->buffer;

            launchParams.commandBuffer = &commandBuffer;
            auto err = VkFFTAppend(vk_app, -1, &launchParams);
            handle_vk_err(err);

            res = vkEndCommandBuffer(commandBuffer);
            handle_vk_err(res);
            VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;
            res = vkQueueSubmit(driver_device->queue, 1, &submitInfo, driver_device->fence);
            handle_vk_err(res);
            res = vkWaitForFences(driver_device->device, 1, &driver_device->fence, VK_TRUE, 100000000000);
            handle_vk_err(res);

            res = vkResetFences(driver_device->device, 1, &driver_device->fence);
            handle_vk_err(res);
            vkFreeCommandBuffers(driver_device->device, driver_device->commandPool, 1, &commandBuffer);

            err = transferDataToCPU(driver_device, data_out, &driver_device->buffer, driver_device->buffer_size);
            handle_vk_err(err);

        }

        static void closeDevice(easyfft::Device *driver_device) {
            vkDestroyBuffer(driver_device->device, driver_device->buffer, nullptr);
            vkFreeMemory(driver_device->device, driver_device->bufferDeviceMemory, nullptr);
        }


    }
}
#endif //EASYFFT_VULKAN_TOOLS_HPP
