#define CL_USE_DEPRECATED_OPENCL_2_0_APIS	// using OpenCL 1.2, some functions deprecated in OpenCL 2.0
#define __CL_ENABLE_EXCEPTIONS				// enable OpenCL exemptions

// C++ standard library and STL headers
#include <iostream>
#include <vector>
#include <fstream>

// OpenCL header, depending on OS
#ifdef __APPLE__
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif
#include "common.h"
#define LENGTH 1024

int main(void) 
{
	cl::Platform platform;			
	cl::Device device;				
	cl::Context context;			
	cl::Program program;			
	cl::Kernel kernel;			
	cl::CommandQueue queue;			
	unsigned int i;
	int inputMultiplier; 
	std::vector<cl_uint> intVector(LENGTH); 
	cl::Buffer buffer, resultBuffer;	


	try 
	{
		if (!select_one_device(&platform, &device))
		{
			quit_program("Device not selected.");
		}

		context = cl::Context(device);

		if (!build_program(&program, &context, "task3.cl"))
		{
			quit_program("OpenCL program build error.");
		}

		std::cout << std::endl;
		std::cout << "Please enter a multipler between 1 and 89 (inclusive): ";
		std::cin >> inputMultiplier;

		if (inputMultiplier < 1 || inputMultiplier > 89) 
		{
			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
			quit_program("Please enter a valid integer between 1 and 89. ");
		}
		else 
		{
			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
		}

		kernel = cl::Kernel(program, "fillArray");
		queue = cl::CommandQueue(context, device);
		buffer = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, sizeof(cl_uint) * LENGTH, &intVector[0]);
		kernel.setArg(0, inputMultiplier);
		kernel.setArg(1, buffer);
		queue.enqueueNDRangeKernel(kernel, cl::NDRange(0), cl::NDRange(LENGTH));
		std::cout << "\nKernel enqueued." << std::endl;
		std::cout << "--------------------" << std::endl;

		queue.enqueueReadBuffer(buffer, CL_TRUE, 0, sizeof(cl_uint) * LENGTH, &intVector[0]);
		std::cout << "\nContents of the buffer after kernel execution: " << std::endl;
		for (int i = 0; i < LENGTH; i++) 
		{
			std::cout << intVector[i] << " ";
		}

	}
	catch (cl::Error err) 
	{
		handle_error(err);
	}

}