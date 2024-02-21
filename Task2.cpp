#define CL_USE_DEPRECATED_OPENCL_2_0_APIS	// using OpenCL 1.2, some functions deprecated in OpenCL 2.0
#define __CL_ENABLE_EXCEPTIONS				// enable OpenCL exemptions

// C++ standard library and STL headers
#include <iostream>
#include <vector>
#include <fstream>
#include <numeric>

// OpenCL header, depending on OS
#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/cl.hpp>
#endif
#include "common.h"
#define CHARLENGTH 52
#define INTLENGTH 512

template <typename T>
std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b){
	std::vector<T> ab;
	ab.reserve(a.size() + b.size());                
	ab.insert(ab.end(), a.begin(), a.end());        
	ab.insert(ab.end(), b.begin(), b.end());        
	return ab;
}

int main(void) {
	cl::Platform platform;			
	cl::Device device;				
	cl::Context context;			
	cl::Program program;			
	cl::Kernel kernel;				
	cl::CommandQueue queue;			
	unsigned int i, j;
	unsigned int fillInt = 1; 

	std::vector<cl_uchar> alphabets;
	std::vector<cl_uchar> alphabetsSmall (CHARLENGTH /2);
	std::vector<cl_uchar> alphabetsCap (CHARLENGTH /2);
	std::vector<cl_uint> intVector;
	std::vector<cl_uchar> charOutput(CHARLENGTH);	
	std::vector<cl_uint> intOutput(INTLENGTH);   

	std::iota(alphabetsSmall.begin(), alphabetsSmall.end(), 'a');
	std::iota(alphabetsCap.begin(), alphabetsCap.end(), 'A');
	alphabets = alphabetsSmall + alphabetsCap;
	for (i = 0; i < INTLENGTH; i++) {
		intVector.push_back(fillInt);
		fillInt++;
	}

	cl::Buffer firstBuffer;		
	cl::Buffer secondBuffer;	
	cl::Buffer thirdBuffer;		

	try {
		if (!select_one_device(&platform, &device))
		{
			quit_program("Device not selected.");
		}
		context = cl::Context(device);
		firstBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_uchar) * alphabets.size(), &alphabets[0]);
		secondBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_uchar) * CHARLENGTH);
		thirdBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(cl_uint) * INTLENGTH);
		queue = cl::CommandQueue(context, device);
		queue.enqueueCopyBuffer(firstBuffer, secondBuffer, 0, 0, sizeof(cl_uchar) * CHARLENGTH);
		queue.enqueueWriteBuffer(thirdBuffer, CL_TRUE, 0, sizeof(cl_uint) * intVector.size(), &intVector[0]);
		if (!build_program(&program, &context, "task2.cl"))
		{
			quit_program("OpenCL program build error.");
		}
		kernel = cl::Kernel(program, "task2");

		float a = 21.43;
		kernel.setArg(0, a); 
		kernel.setArg(1, secondBuffer);  
		kernel.setArg(2, thirdBuffer); 
		queue.enqueueTask(kernel);
		std::cout << "Kernel enqueued." << std::endl;
		std::cout << "--------------------" << std::endl;

		queue.enqueueReadBuffer(secondBuffer, CL_TRUE, 0, sizeof(cl_uchar) * CHARLENGTH, &charOutput[0]);
		std::cout << "\nContents of second buffer after kernel execution: " << std::endl;
		for (int i = 0; i < CHARLENGTH; i++){
			std::cout << charOutput[i] << " ";
		}
		queue.enqueueReadBuffer(thirdBuffer, CL_TRUE, 0, sizeof(cl_uint) * INTLENGTH, &intOutput[0]);
		std::cout << "\n\nContents of third buffer after kernel execution: ";
		for (int i = 0; i < INTLENGTH; i++){
			if ((i % 25) == 0)
			{
				std::cout << "\n";
				std::cout << intOutput[i] << " ";
			}
			else
			{
				std::cout << intOutput[i] << " ";
			}
				
		}
	}
	catch (cl::Error e) {
		handle_error(e);
	}
	return 0;
}