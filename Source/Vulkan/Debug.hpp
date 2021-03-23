#ifndef __Vulkan_Debug_hpp__
#define __Vulkan_Debug_hpp__

#pragma once

#include "Pch.hpp"

class Debug
{
public:
#ifdef NDEBUG
	inline static constexpr bool validationLayersEnabled{ false };
#else
	inline static constexpr bool validationLayersEnabled{ true };
#endif
	inline static VkDebugUtilsMessengerEXT messenger;
	inline static const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

	static void CreateMessenger(const VkInstance& instance);
	static void DestroyMessenger(const VkInstance& instance);

	static void PopulateCreateInfoStruct(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	inline static VkResult CreateDebugUtilsMessengerEXT(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger
	)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}
	
	inline static void DestroyDebugUtilsMessengerEXT(
		VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator
	)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}

	inline static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	)
	{
		std::cerr << "Validation layer: " << pCallbackData->pMessage << '\n';

		return VK_FALSE;
	}

	static bool CheckValidationLayerSupport();

};

#endif