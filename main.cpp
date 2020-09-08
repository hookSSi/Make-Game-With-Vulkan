#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <SDL.h>

/* I use other github project for using cout with color */
/* https://github.com/yurablok/colored-cout */
#include <colored_cout.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <string>
#include <functional>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class HelloTriangleApplication
{
public:
	void run()
	{
		InitWindow();
		InitVulkan();
		MainLoop();
		Cleanup();
	}
private:
	GLFWwindow* window;
	VkInstance vkInstance;

	/// create window using glfw
	void InitWindow()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	}

	/// initialize vulkan objects
	void InitVulkan()
	{
		CreateInstance();
	}

	void CreateInstance()
	{
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		if (glfwExtensions == NULL)
		{
			throw std::runtime_error("Vulkan is not available on this machine");
		}
		else
		{
			CheckRequiredExtensionsValid(glfwExtensionCount, glfwExtensions);
		}

		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;

		createInfo.enabledLayerCount = 0;

		VkResult result = vkCreateInstance(&createInfo, nullptr, &vkInstance);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create instance!");
		}
	}

	void CheckRequiredExtensionsValid(uint32_t requiredExtensionCount, const char** requiredExtensionList)
	{
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		bool* checkList = new bool[requiredExtensionCount]; // extension이 사용되었는지 체크
		for (int i = 0; i < requiredExtensionCount; i++)
		{
			checkList[i] = false;
		}

		std::cout << "available extensions:\n";
		for (const auto& extension : extensions)
		{
			/// 해시 테이블로 좀더 간결하게 표현할 수 있지 않을까? 
			bool isUsed = false;
			for (int i = 0; i < requiredExtensionCount; i++)
			{
				if (strcmp(extension.extensionName, requiredExtensionList[i]) == 0)
				{
					isUsed = true;
					checkList[i] = true;
				}
			}

			const char* result = isUsed ? "(USED)" : "(NOT USED)";
			clr color = isUsed ? clr::green : clr::red;
			std::cout << '\t' << extension.extensionName << " " << color << result << clr::reset << '\n';
		}

		std::cout << "required but not available extensions:\n";
		for (int i = 0; i < requiredExtensionCount; i++)
		{
			if (!checkList[i])
			{
				std::cout << '\t' << requiredExtensionList[i] << '\n';
			}
		}

		delete[] checkList;
		checkList = NULL;
	}

	/// render per frame
	void MainLoop()
	{
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
		}
	}

	/// release memories
	void Cleanup()
	{
		vkDestroyInstance(vkInstance, nullptr);

		glfwDestroyWindow(window);
		
		glfwTerminate();
	}
};

int main(int argc, char* argv[]) 
{
	HelloTriangleApplication app;

	try
	{
		app.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}