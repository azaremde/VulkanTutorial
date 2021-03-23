#ifndef __Vulkan_Device_hpp__
#define __Vulkan_Device_hpp__

#pragma once

#include "Pch.hpp"

class Device
{
private:
	Device(const Device&) = delete;
	Device& operator=(const Device&) = delete;

	VkInstance& instance;

public:
	Device(VkInstance& _instance);
	~Device();
};

#endif