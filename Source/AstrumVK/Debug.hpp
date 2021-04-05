#ifndef __Vulkan_Debug_hpp__
#define __Vulkan_Debug_hpp__

#pragma once

#include "Pch.hpp"

#include "Instance/Instance.hpp"

class Debug
{
public:
#ifdef DEBUG_CONFIG
	inline static constexpr bool validationLayersEnabled{ true };
#else
	inline static constexpr bool validationLayersEnabled{ false };
#endif

	inline static VkDebugUtilsMessengerEXT messenger;
	inline static const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

	static void createMessenger();
	static void destroyMessenger();

	static void populateCreateInfoStruct(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	inline static VkResult createDebugUtilsMessengerEXT(
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger
	)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance::getInstance(), "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(Instance::getInstance(), pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}
	
	inline static void destroyDebugUtilsMessengerEXT(
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator
	)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance::getInstance(), "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(Instance::getInstance(), debugMessenger, pAllocator);
		}
	}

	inline static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	)
	{
		std::cerr << "Validation layer: " << pCallbackData->pMessage << '\n';

		return VK_FALSE;
	}

    static std::vector<const char*> getRequiredExtensions();

	static bool checkValidationLayerSupport();

};

#endif