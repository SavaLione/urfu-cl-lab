```cpp
#define __CL_ENABLE_EXCEPTIONS

#if defined(__APPLE__) || defined(__MACOSX)
	#include <OpenCL/cl.hpp>
#else
	#include <CL/cl.hpp>
#endif
#include <cstdlib>
#include <exception>
#include <iostream>
#include <vector>

static cl_int AllocateAndGetPlatformInfoString(cl_platform_id platformId, cl_platform_info param_name, char *&param_value)
{
	cl_int errorCode = CL_SUCCESS;
	size_t size		 = 0;

	if(errorCode == CL_SUCCESS)
	{
		if(param_value != NULL)
		{
			delete[] param_value;
			param_value = NULL;
		}
	}

	if(errorCode == CL_SUCCESS)
	{
		errorCode = clGetPlatformInfo(platformId, param_name, 0, NULL, &size);
	}

	if(errorCode == CL_SUCCESS)
	{
		if(size != 0)
		{
			param_value = new char[size];
			if(param_value == NULL)
			{
				errorCode = CL_OUT_OF_HOST_MEMORY;
			}
		}
	}

	if(errorCode == CL_SUCCESS)
	{
		errorCode = clGetPlatformInfo(platformId, param_name, size, param_value, NULL);
	}

	if(errorCode != CL_SUCCESS)
	{
		delete[] param_value;
		param_value = NULL;
	}

	return errorCode;
}

static cl_int PrintPlatformInfoSummary(cl_platform_id platformId)
{
	cl_int errorCode = CL_SUCCESS;

	char *platformName	  = NULL;
	char *platformVendor  = NULL;
	char *platformVersion = NULL;

	errorCode |= AllocateAndGetPlatformInfoString(platformId, CL_PLATFORM_NAME, platformName);
	errorCode |= AllocateAndGetPlatformInfoString(platformId, CL_PLATFORM_VENDOR, platformVendor);
	errorCode |= AllocateAndGetPlatformInfoString(platformId, CL_PLATFORM_VERSION, platformVersion);

	printf("\tName:           %s\n", platformName);
	printf("\tVendor:         %s\n", platformVendor);
	printf("\tDriver Version: %s\n", platformVersion);

	delete[] platformName;
	delete[] platformVendor;
	delete[] platformVersion;

	platformName	= NULL;
	platformVendor	= NULL;
	platformVersion = NULL;

	return errorCode;
}

static cl_int AllocateAndGetDeviceInfoString(cl_device_id device, cl_device_info param_name, char *&param_value)
{
	cl_int errorCode = CL_SUCCESS;
	size_t size		 = 0;

	if(errorCode == CL_SUCCESS)
	{
		if(param_value != NULL)
		{
			delete[] param_value;
			param_value = NULL;
		}
	}

	if(errorCode == CL_SUCCESS)
	{
		errorCode = clGetDeviceInfo(device, param_name, 0, NULL, &size);
	}

	if(errorCode == CL_SUCCESS)
	{
		if(size != 0)
		{
			param_value = new char[size];
			if(param_value == NULL)
			{
				errorCode = CL_OUT_OF_HOST_MEMORY;
			}
		}
	}

	if(errorCode == CL_SUCCESS)
	{
		errorCode = clGetDeviceInfo(device, param_name, size, param_value, NULL);
	}

	if(errorCode != CL_SUCCESS)
	{
		delete[] param_value;
		param_value = NULL;
	}

	return errorCode;
}

static void PrintDeviceType(const char *label, cl_device_type type)
{
	printf(
		"%s%s%s%s%s%s\n",
		label,
		(type & CL_DEVICE_TYPE_DEFAULT) ? "DEFAULT " : "",
		(type & CL_DEVICE_TYPE_CPU) ? "CPU " : "",
		(type & CL_DEVICE_TYPE_GPU) ? "GPU " : "",
		(type & CL_DEVICE_TYPE_ACCELERATOR) ? "ACCELERATOR " : "",
		(type & CL_DEVICE_TYPE_CUSTOM) ? "CUSTOM " : "");
}

static cl_int PrintDeviceInfoSummary(cl_device_id *devices, size_t numDevices)
{
	cl_int errorCode = CL_SUCCESS;

	cl_device_type deviceType;
	char *deviceName	= NULL;
	char *deviceVendor	= NULL;
	char *deviceVersion = NULL;
	char *deviceProfile = NULL;
	char *driverVersion = NULL;

	size_t i = 0;
	for(i = 0; i < numDevices; i++)
	{
		errorCode |= clGetDeviceInfo(devices[i], CL_DEVICE_TYPE, sizeof(deviceType), &deviceType, NULL);
		errorCode |= AllocateAndGetDeviceInfoString(devices[i], CL_DEVICE_NAME, deviceName);
		errorCode |= AllocateAndGetDeviceInfoString(devices[i], CL_DEVICE_VENDOR, deviceVendor);
		errorCode |= AllocateAndGetDeviceInfoString(devices[i], CL_DEVICE_VERSION, deviceVersion);
		errorCode |= AllocateAndGetDeviceInfoString(devices[i], CL_DEVICE_PROFILE, deviceProfile);
		errorCode |= AllocateAndGetDeviceInfoString(devices[i], CL_DRIVER_VERSION, driverVersion);

		if(errorCode == CL_SUCCESS)
		{
			printf("Device[%d]:\n", (int)i);

			PrintDeviceType("\tType:           ", deviceType);

			printf("\tName:           %s\n", deviceName);
			printf("\tVendor:         %s\n", deviceVendor);
			printf("\tDevice Version: %s\n", deviceVersion);
			printf("\tDevice Profile: %s\n", deviceProfile);
			printf("\tDriver Version: %s\n", driverVersion);
		}
		else
		{
			fprintf(stderr, "Error getting device info for device %d.\n", (int)i);
		}

		delete[] deviceName;
		delete[] deviceVendor;
		delete[] deviceVersion;
		delete[] deviceProfile;
		delete[] driverVersion;

		deviceName	  = NULL;
		deviceVendor  = NULL;
		deviceVersion = NULL;
		deviceProfile = NULL;
		driverVersion = NULL;
	}

	return errorCode;
}

int main(int argc, char **argv)
{
	bool printUsage = false;

	int i = 0;

	if(argc < 1)
	{
		printUsage = true;
	}
	else
	{
		for(i = 1; i < argc; i++)
		{
			{
				printUsage = true;
			}
		}
	}
	if(printUsage)
	{
		fprintf(
			stderr,
			"Usage: enumopencl      [options]\n"
			"Options:\n");

		return -1;
	}

	cl_uint numPlatforms = 0;
	clGetPlatformIDs(0, NULL, &numPlatforms);
	printf("Enumerated %u platforms.\n\n", numPlatforms);

	std::vector<cl_platform_id> platforms;
	platforms.resize(numPlatforms);
	clGetPlatformIDs(numPlatforms, platforms.data(), NULL);

	for(auto &platform : platforms)
	{
		printf("Platform:\n");
		PrintPlatformInfoSummary(platform);

		cl_uint numDevices = 0;
		clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);

		std::vector<cl_device_id> devices;
		devices.resize(numDevices);
		clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, numDevices, devices.data(), NULL);

		PrintDeviceInfoSummary(devices.data(), numDevices);
		printf("\n");
	}

	printf("Done.\n");

	return 0;
}

// int main()
// {
// 	try
// 	{
// 		std::vector<cl::Platform> platforms;
// 		cl::Platform::get(&platforms);

// 		std::cout << "Found " << platforms.size() << " platform" << (platforms.size() > 1 ? "s.\n" : ".\n") << std::endl;

// 		for(const auto &platform : platforms)
// 		{
// 			std::cout << platform.getInfo<CL_PLATFORM_VENDOR>() << std::endl;

// 			std::vector<cl::Device> devices;
// 			platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

// 			for(const auto &device : devices)
// 				std::cout << "\t" << device.getInfo<CL_DEVICE_NAME>() << std::endl;
// 		}
// 	}
// 	catch(cl::Error &error) // If any OpenCL error occurs
// 	{
// 		if(error.err() == CL_PLATFORM_NOT_FOUND_KHR)
// 		{
// 			std::cout << "No OpenCL platform found." << std::endl;
// 			std::exit(EXIT_SUCCESS);
// 		}
// 		else
// 		{
// 			std::cerr << error.what() << "(" << error.err() << ")" << std::endl;
// 			std::exit(error.err());
// 		}
// 	}
// 	catch(std::exception &error) // If STL/CRT error occurs
// 	{
// 		std::cerr << error.what() << std::endl;
// 		std::exit(EXIT_FAILURE);
// 	}
// }

// /*
//  * Copyright (c) 2022, Savely Pototsky (SavaLione)
//  * All rights reserved.
//  */
// #define __CL_ENABLE_EXCEPTIONS

// #if defined(__APPLE__) || defined(__MACOSX)
// 	#include <OpenCL/cl.hpp>
// #else
// 	#include <CL/cl.hpp>
// #endif
// #include <cstdio>
// #include <cstdlib>
// #include <iostream>
// #include <vector>

// const char *helloStr = "__kernel void "
// 					   "hello(void) "
// 					   "{ "
// 					   "  "
// 					   "} ";

// int main(void)
// {
// 	cl_int err = CL_SUCCESS;
// 	try
// 	{
// 		std::vector<cl::Platform> platforms;
// 		cl::Platform::get(&platforms);
// 		if(platforms.size() == 0)
// 		{
// 			std::cout << "Platform size 0\n";
// 			return -1;
// 		}

// 		// Print number of platforms and list of platforms
// 		std::cout << "Platform number is: " << platforms.size() << std::endl;
// 		std::string platformVendor;
// 		for(unsigned int i = 0; i < platforms.size(); ++i)
// 		{
// 			platforms[i].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
// 			std::cout << "Platform is by: " << platformVendor << std::endl;
// 		}

// 		cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0};
// 		cl::Context context(CL_DEVICE_TYPE_ALL, properties);

// 		std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

// 		// Print number of devices and list of devices
// 		std::cout << "Device number is: " << devices.size() << std::endl;
// 		for(unsigned int i = 0; i < devices.size(); ++i)
// 		{
// 			std::cout << "Device #" << i << ": " << devices[i].getInfo<CL_DEVICE_NAME>() << std::endl;
// 		}

// 		cl::Program::Sources source(1, std::make_pair(helloStr, strlen(helloStr)));
// 		cl::Program program_ = cl::Program(context, source);
// 		program_.build(devices);

// 		cl::Kernel kernel(program_, "hello", &err);

// 		cl::Event event;
// 		cl::CommandQueue queue(context, devices[0], 0, &err);
// 		queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(4, 4), cl::NullRange, NULL, &event);

// 		event.wait();
// 	}
// 	catch(cl::Error err)
// 	{
// 		std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
// 	}

// 	return EXIT_SUCCESS;
// }
```