
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include <iostream>

#define VK_CHECK(result)                                     \
	if (result != VK_SUCCESS)                                \
	{                                                        \
		std::cout << "Vulkan Error:" << result << std::endl; \
		__debugbreak();                                      \
		return false;                                        \
	}

struct VkContext
{
	VkInstance instance;
};

bool vk_init(VkContext *vkcontext, void *window)
{
    VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	//TODO Cambiar a un json o alguna especie de datafile para tenmer un acceso rapido y hacer cambios
	appInfo.pApplicationName = "MyCraft";
	appInfo.pEngineName = "SKOVoxels";

	VkInstanceCreateInfo instanceInfo = {};
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pApplicationInfo = &appInfo;

	VkInstance instance;

	VK_CHECK(vkCreateInstance(&instanceInfo, 0, &vkcontext->instance));

	

	return true;
}
