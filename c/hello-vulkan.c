/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include <assert.h>
#include <dlfcn.h>		/* dlopen, Link with -ldl */
#include <inttypes.h>		/* PRIu32 */
#include <stdio.h>		/* FILE, fprintf */
#include <stdlib.h>		/* exit(EXIT_FAILURE) */
#include <unistd.h>
#include <vulkan/vulkan.h>	/* Link with -lvulkan */

/*
extern FILE *_global_err_stream;
*/
FILE *_global_err_stream;

#ifndef _err_stream
#define _err_stream (_global_err_stream ? _global_err_stream : stderr)
#endif

#ifndef logerror
#define logerror(format, ...) \
	do { \
		fflush(stdout); /* avoid POSIX undefined stdout+stderr */ \
		/* https://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html#tag_15_05_01 */ \
		fprintf(_err_stream, "%s:%d: ", __FILE__, __LINE__); \
		fprintf(_err_stream, format __VA_OPT__(,) __VA_ARGS__); \
		fprintf(_err_stream, "\n"); \
	} while (0)
#endif

#ifndef die
#define die(format, ...) \
	do { \
		logerror(format __VA_OPT__(,) __VA_ARGS__); \
		exit(EXIT_FAILURE); \
	} while (0)
#endif

typedef struct vulkan_buffer {
	VkBuffer buffer;
	VkDeviceMemory buf_mem;
} vulkan_buffer_s;

void *load_vulcan_or_die(void)
{
	void *vulkanlib = dlopen("libvulkan.so", RTLD_NOW);
	if (vulkanlib == NULL) {
		die("Could not load Vulkan library!");
	}
	return vulkanlib;
}

const char *vulkan_return_code_to_string(VkResult vk_res)
{
	switch (vk_res) {
	case VK_SUCCESS:
		return "VK_SUCCESS";
	case VK_NOT_READY:
		return "VK_NOT_READY";
	case VK_TIMEOUT:
		return "VK_TIMEOUT";
	case VK_EVENT_SET:
		return "VK_EVENT_SET";
	case VK_EVENT_RESET:
		return "VK_EVENT_RESET";
	case VK_INCOMPLETE:
		return "VK_INCOMPLETE";
	case VK_ERROR_OUT_OF_HOST_MEMORY:
		return "VK_ERROR_OUT_OF_HOST_MEMORY";
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
	case VK_ERROR_INITIALIZATION_FAILED:
		return "VK_ERROR_INITIALIZATION_FAILED";
	case VK_ERROR_DEVICE_LOST:
		return "VK_ERROR_DEVICE_LOST";
	case VK_ERROR_MEMORY_MAP_FAILED:
		return "VK_ERROR_MEMORY_MAP_FAILED";
	case VK_ERROR_LAYER_NOT_PRESENT:
		return "VK_ERROR_LAYER_NOT_PRESENT";
	case VK_ERROR_EXTENSION_NOT_PRESENT:
		return "VK_ERROR_EXTENSION_NOT_PRESENT";
	case VK_ERROR_FEATURE_NOT_PRESENT:
		return "VK_ERROR_FEATURE_NOT_PRESENT";
	case VK_ERROR_INCOMPATIBLE_DRIVER:
		return "VK_ERROR_INCOMPATIBLE_DRIVER";
	case VK_ERROR_TOO_MANY_OBJECTS:
		return "VK_ERROR_TOO_MANY_OBJECTS";
	case VK_ERROR_FORMAT_NOT_SUPPORTED:
		return "VK_ERROR_FORMAT_NOT_SUPPORTED";
	case VK_ERROR_FRAGMENTED_POOL:
		return "VK_ERROR_FRAGMENTED_POOL";
	case VK_ERROR_OUT_OF_POOL_MEMORY:
		return "VK_ERROR_OUT_OF_POOL_MEMORY";
	case VK_ERROR_INVALID_EXTERNAL_HANDLE:
		return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
	case VK_ERROR_SURFACE_LOST_KHR:
		return "VK_ERROR_SURFACE_LOST_KHR";
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
		return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
	case VK_SUBOPTIMAL_KHR:
		return "VK_SUBOPTIMAL_KHR";
	case VK_ERROR_OUT_OF_DATE_KHR:
		return "VK_ERROR_OUT_OF_DATE_KHR";
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
		return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
	case VK_ERROR_VALIDATION_FAILED_EXT:
		return "VK_ERROR_VALIDATION_FAILED_EXT";
	case VK_ERROR_INVALID_SHADER_NV:
		return "VK_ERROR_INVALID_SHADER_NV";
	case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
		return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
	case VK_ERROR_FRAGMENTATION_EXT:
		return "VK_ERROR_FRAGMENTATION_EXT";
	case VK_ERROR_NOT_PERMITTED_EXT:
		return "VK_ERROR_NOT_PERMITTED_EXT";
	case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:
		return "VK_ERROR_INVALID_DEVICE_ADDRESS_EXT";
	case VK_RESULT_MAX_ENUM:
		return "VK_RESULT_MAX_ENUM";
	default:
		return "(unlisted result?!)";
	}
}

/* VkInstance is a pointer type */
VkInstance hello_vulkanCreate(VkAllocationCallbacks *vk_allocator)
{
	assert(sizeof(VkInstance) == sizeof(void *));

	VkApplicationInfo vk_app_info;
	vk_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	vk_app_info.pNext = NULL;
	vk_app_info.pApplicationName = "hello";
	vk_app_info.applicationVersion = 1;
	vk_app_info.pEngineName = "hello-engine";
	vk_app_info.engineVersion = 0;
	vk_app_info.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo vk_inst_create_info;
	vk_inst_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	vk_inst_create_info.pNext = NULL;
	vk_inst_create_info.flags = 0;
	vk_inst_create_info.pApplicationInfo = &vk_app_info;
	vk_inst_create_info.enabledLayerCount = 0;
	vk_inst_create_info.ppEnabledLayerNames = NULL;
	vk_inst_create_info.enabledExtensionCount = 0;
	vk_inst_create_info.ppEnabledExtensionNames = NULL;

	VkInstance vk = NULL;
	VkResult vk_res;
	vk_res = vkCreateInstance(&vk_inst_create_info, vk_allocator, &vk);
	if (vk_res != VK_SUCCESS) {
		const char *err_str = vulkan_return_code_to_string(vk_res);
		unsigned long ule = vk_res;
		logerror("vkCreateInstance returned %lu, %s", ule, err_str);
		return NULL;
	}
	return vk;
}

uint32_t queue_family_index(VkPhysicalDevice phys_dev, VkQueueFlags flags)
{
	uint32_t q_fam_cnt = UINT32_MAX;
	vkGetPhysicalDeviceQueueFamilyProperties(phys_dev, &q_fam_cnt, NULL);
	if (q_fam_cnt == UINT32_MAX) {
		logerror("vkGetPhysicalDeviceQueueFamilyProperties");
		return UINT32_MAX;
	}

	VkQueueFamilyProperties q_fam_props[q_fam_cnt];
	vkGetPhysicalDeviceQueueFamilyProperties(phys_dev, &q_fam_cnt,
						 q_fam_props);
	uint32_t q_fam_idx = UINT32_MAX;
	for (size_t i = 0; i < q_fam_cnt && q_fam_idx == UINT32_MAX; ++i) {
		VkQueueFamilyProperties p = q_fam_props[i];
		if ((p.queueCount > 0) && (p.queueFlags & flags)) {
			q_fam_idx = i;
		}
	}
	if (q_fam_idx == UINT32_MAX) {
		logerror("no match in %lu options\n", (unsigned long)q_fam_cnt);
	}
	return q_fam_idx;
}

VkDevice hello_CreateDevice(VkPhysicalDevice phys_dev,
			    uint32_t q_fam_idx,
			    VkAllocationCallbacks *allocator)
{
	assert(sizeof(VkDevice) == sizeof(void *));

	float queue_prios[1] = { 1.0 };
	VkDeviceQueueCreateInfo queue_create_info;
	queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_create_info.pNext = NULL;
	queue_create_info.flags = 0;
	queue_create_info.queueFamilyIndex = q_fam_idx;
	queue_create_info.queueCount = 1;
	queue_create_info.pQueuePriorities = queue_prios;

	VkPhysicalDeviceFeatures enabled_features;
	memset(&enabled_features, 0x00, sizeof(VkPhysicalDeviceFeatures));

	VkDeviceCreateInfo dev_create_info;
	dev_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	dev_create_info.pNext = NULL;
	dev_create_info.flags = 0;
	dev_create_info.queueCreateInfoCount = 1;
	dev_create_info.pQueueCreateInfos = &queue_create_info;
	dev_create_info.enabledLayerCount = 0;
	dev_create_info.ppEnabledLayerNames = NULL;
	dev_create_info.enabledExtensionCount = 0;
	dev_create_info.ppEnabledExtensionNames = NULL;
	dev_create_info.pEnabledFeatures = &enabled_features;

	VkDevice dev = NULL;
	VkResult result;
	result = vkCreateDevice(phys_dev, &dev_create_info, allocator, &dev);
	if (result != VK_SUCCESS) {
		const char *err_str = vulkan_return_code_to_string(result);
		unsigned long ule = (unsigned long)result;
		logerror("vkCreateDevice returned %lu, %s", ule, err_str);
		return NULL;
	}
	return dev;
}

uint32_t memory_type_idx(VkPhysicalDevice phys_dev,
			 VkMemoryRequirements mem_reqs,
			 VkMemoryPropertyFlags mem_prop_flags)
{
	VkPhysicalDeviceMemoryProperties mem_props;
	vkGetPhysicalDeviceMemoryProperties(phys_dev, &mem_props);

	for (size_t i = 0; i < mem_props.memoryTypeCount; ++i) {
		if (mem_reqs.memoryTypeBits & (1ULL << i)) {
			VkMemoryType mtype = mem_props.memoryTypes[i];
			VkMemoryPropertyFlags mpflags = mtype.propertyFlags;
			if ((mpflags & mem_prop_flags) == mem_prop_flags) {
				return i;
			}
		}
	}

	logerror("VkMemoryType with flags 0x%lx not found in %lu types",
		 (unsigned long)mem_prop_flags,
		 (unsigned long)mem_props.memoryTypeCount);
	return UINT32_MAX;
}

VkResult create_exclusive_storage_buffer(vulkan_buffer_s *vulkan_buf,
					 VkPhysicalDevice phys_dev,
					 VkDevice device,
					 VkDeviceSize size,
					 VkAllocationCallbacks *alloctr)
{
	assert(vulkan_buf != NULL);

	vulkan_buf->buffer = NULL;
	vulkan_buf->buf_mem = NULL;

	VkResult result;

	VkBufferCreateInfo buf_create_info;
	buf_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buf_create_info.pNext = NULL;
	buf_create_info.flags = 0;
	buf_create_info.size = size;
	buf_create_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	buf_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	buf_create_info.queueFamilyIndexCount = 0;
	buf_create_info.pQueueFamilyIndices = NULL;

	VkBuffer buffer = NULL;
	result = vkCreateBuffer(device, &buf_create_info, alloctr, &buffer);
	if (result != VK_SUCCESS) {
		const char *func_str = "vkCreateBuffer";
		const char *err_str = vulkan_return_code_to_string(result);
		unsigned long ule = (unsigned long)result;
		logerror("%s returned %lu, %s", func_str, ule, err_str);
		return result;
	}

	VkMemoryRequirements mem_reqs;
	vkGetBufferMemoryRequirements(device, buffer, &mem_reqs);

	VkMemoryPropertyFlags mpflags =
	    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
	    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	uint32_t mem_type_idx = memory_type_idx(phys_dev, mem_reqs, mpflags);
	if (mem_type_idx == UINT32_MAX) {
		vkDestroyBuffer(device, buffer, alloctr);
		return VK_RESULT_MAX_ENUM;
	}

	VkMemoryAllocateInfo alloc_info;
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.pNext = NULL;
	alloc_info.allocationSize = mem_reqs.size;
	alloc_info.memoryTypeIndex = mem_type_idx;

	VkDeviceMemory buf_mem = NULL;
	result = vkAllocateMemory(device, &alloc_info, alloctr, &buf_mem);
	if (result != VK_SUCCESS) {
		const char *func_str = "vkAllocateMemory";
		const char *err_str = vulkan_return_code_to_string(result);
		unsigned long ule = (unsigned long)result;
		logerror("%s returned %lu, %s", func_str, ule, err_str);
		vkDestroyBuffer(device, buffer, alloctr);
		return result;
	}

	VkDeviceSize mem_offset = 0;
	result = vkBindBufferMemory(device, buffer, buf_mem, mem_offset);
	if (result != VK_SUCCESS) {
		const char *func_str = "vkBindBufferMemory";
		const char *err_str = vulkan_return_code_to_string(result);
		unsigned long ule = (unsigned long)result;
		logerror("%s returned %lu, %s", func_str, ule, err_str);
		vkFreeMemory(device, buf_mem, alloctr);
		vkDestroyBuffer(device, buffer, alloctr);
		return result;
	}

	return result;
}

int main(void)
{
	VkAllocationCallbacks *alloctr = NULL;
	VkInstance vk = hello_vulkanCreate(alloctr);
	if (!vk) {
		die("No vk?");
	}

	uint32_t phys_dev_cnt = 0;
	vkEnumeratePhysicalDevices(vk, &phys_dev_cnt, NULL);
	printf("Physical Devices: %" PRIu32 "\n", phys_dev_cnt);
	if (!phys_dev_cnt) {
		die("no Physical Devices?");
	}

	VkPhysicalDevice phys_devs[phys_dev_cnt];
	vkEnumeratePhysicalDevices(vk, &phys_dev_cnt, phys_devs);

	VkQueueFlags q_flags = VK_QUEUE_COMPUTE_BIT;
	uint32_t q_fam_idx = queue_family_index(phys_devs[0], q_flags);
	assert(q_fam_idx < UINT32_MAX);
	VkDevice device = hello_CreateDevice(phys_devs[0], q_fam_idx, alloctr);
	if (!device) {
		die("no Device?");
	}

	VkQueue queue = NULL;
	uint32_t q_idx = 0;
	vkGetDeviceQueue(device, q_fam_idx, q_idx, &queue);

	vulkan_buffer_s vulkan_buf;
	VkDeviceSize size = (800 * 600 * 4);

	VkResult result = create_exclusive_storage_buffer(&vulkan_buf,
				phys_devs[0], device, size, alloctr);

	vkFreeMemory(device, vulkan_buf.buf_mem, alloctr);
	vkDestroyBuffer(device, vulkan_buf.buffer, alloctr);
	vkDestroyDevice(device, alloctr);
	vkDestroyInstance(vk, alloctr);

	return (result == VK_SUCCESS) ? 0 : 1;
}
