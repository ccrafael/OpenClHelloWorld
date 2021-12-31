#include "OpenClHelloWorld.h"


void show_cl_info(cl_platform_id platform, cl_device_id* devices, int num_devices) {
	size_t value_size = 0;
	char device_string[1024];

	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, num_devices, devices, NULL);
	for (int i = 0; i < num_devices; ++i) {
		clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(device_string), &device_string, NULL);
		cout << "  CL_DEVICE_NAME: \t\t\t" <<  device_string << endl;

		clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR, sizeof(device_string), &device_string, NULL);
		cout << "  CL_DEVICE_VENDOR: \t\t\t" << device_string << endl;

		clGetDeviceInfo(devices[i], CL_DRIVER_VERSION, sizeof(device_string), &device_string, NULL);
		cout << "  CL_DRIVER_VERSION: \t\t\t" << device_string << endl;

		cl_uint compute_units;
		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(compute_units), &compute_units, NULL);
		cout << "  CL_DEVICE_MAX_COMPUTE_UNITS:\t\t" << compute_units << endl;

		size_t workitem_dims;
		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(workitem_dims), &workitem_dims, NULL);
		cout << "  CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS:\t" << workitem_dims << endl;

		size_t workitem_size[3];
		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(workitem_size), &workitem_size, NULL);
		cout << "  CL_DEVICE_MAX_WORK_ITEM_SIZES:\t("<<workitem_size[0] <<","<< workitem_size[1] << ","<< workitem_size[2] <<")" << endl;

		size_t workgroup_size;
		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(workgroup_size), &workgroup_size, NULL);
		cout << "  CL_DEVICE_MAX_WORK_GROUP_SIZE:\t" << workgroup_size << endl;

		cl_uint clock_frequency;
		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(clock_frequency), &clock_frequency, NULL);
		cout << "  CL_DEVICE_MAX_CLOCK_FREQUENCY:\t" << clock_frequency  << "MHz" << endl;

		cl_uint addr_bits;
		clGetDeviceInfo(devices[i], CL_DEVICE_ADDRESS_BITS, sizeof(addr_bits), &addr_bits, NULL);
		cout << "  CL_DEVICE_ADDRESS_BITS:\t\t" << addr_bits << endl;

		cl_ulong max_mem_alloc_size;
		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(max_mem_alloc_size), &max_mem_alloc_size, NULL);
		cout << "  CL_DEVICE_MAX_MEM_ALLOC_SIZE:\t\t" << (unsigned int)(max_mem_alloc_size / (1024 * 1024))  << " MByte" << endl;

		cl_ulong mem_size;
		clGetDeviceInfo(devices[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(mem_size), &mem_size, NULL);
		cout << "  CL_DEVICE_GLOBAL_MEM_SIZE:\t\t" << (unsigned int)(mem_size / (1024 * 1024)) << " MByte" << endl;

		bool bPassed = true;

		char cBuffer[1024];
		cl_uint num_platforms;
		cl_platform_id clSelectedPlatformID = NULL;
		cl_platform_id* clPlatformIDs;

		cl_int ciErrNum = clGetPlatformIDs(0, NULL, &num_platforms);
		std::string sProfileString = "clDeviceQuery, Platform Name = ";
		printf("%d OpenCL Platforms found\n\n", num_platforms);

		if ((clPlatformIDs = (cl_platform_id*)malloc(num_platforms * sizeof(cl_platform_id))) == NULL) {
			printf("Failed to allocate memory for cl_platform ID's!\n\n");
			bPassed = false;
		}

		ciErrNum = clGetPlatformIDs(num_platforms, clPlatformIDs, NULL);
		for (cl_uint i = 0; i < num_platforms; ++i) {
			ciErrNum = clGetPlatformInfo(clPlatformIDs[i], CL_PLATFORM_NAME, 1024, &cBuffer, NULL);
			if (ciErrNum == CL_SUCCESS) {
				clSelectedPlatformID = clPlatformIDs[i];
				
				ciErrNum = clGetPlatformInfo(clSelectedPlatformID, CL_PLATFORM_NAME, sizeof(cBuffer), cBuffer, NULL);
				if (ciErrNum == CL_SUCCESS) {
					printf(" CL_PLATFORM_NAME: \t%s\n", cBuffer);
					sProfileString += cBuffer;
				} else {
					printf(" Error %i in clGetPlatformInfo Call !!!\n\n", ciErrNum);
					bPassed = false;
				}

				sProfileString += ", Platform Version = ";
				ciErrNum = clGetPlatformInfo(clSelectedPlatformID, CL_PLATFORM_VERSION, sizeof(cBuffer), cBuffer, NULL);
				if (ciErrNum == CL_SUCCESS) {
					printf(" CL_PLATFORM_VERSION: \t%s\n", cBuffer);
					sProfileString += cBuffer;
				} else {
					printf(" Error %i in clGetPlatformInfo Call !!!\n\n", ciErrNum);
					bPassed = false;
				}
			}
		}
	}
}

string load_file_as_string(const char* filename) {
	cout << "Loading kernel=" << filename << endl;
	std::fstream kernel_file(filename, (std::fstream::in | std::fstream::binary));
	if (kernel_file.is_open()) {
		kernel_file.seekg(0, std::fstream::end);
		size_t file_size = (size_t)kernel_file.tellg();
		kernel_file.seekg(0, std::fstream::beg);
		char * str = new char[file_size + 1];
		kernel_file.read(str, file_size);
		kernel_file.close();
		str[file_size] = '\0';
		
		return string(str);
	}
	throw std::exception("Error opening kernel.");
}

void sum(int size, int* A, int* B, int* C) {
	for (int i = 0; i < size; ++i) {
		C[i] = A[i] + B[i];
	}
}

void parallel_sum(const char* source, int size, int* A, int* B, int* C) {
	cl_uint num_devices = 0;
	cl_device_id* devices;
	cl_uint num_platforms;
	cl_platform_id* platforms = NULL;
	cl_platform_id platform = NULL;

	clGetPlatformIDs(0, NULL, &num_platforms);
	if (num_platforms > 0) {
		cout << "Found " << num_platforms << " platforms." << endl;
		platforms = (cl_platform_id*)malloc(num_platforms * sizeof(cl_platform_id));
		clGetPlatformIDs(num_platforms, platforms, NULL);
		platform = platforms[0];
	} else {
		cout << "Platforms not found. " << endl;
		exit(-1);
	}

	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &num_devices);
	devices = (cl_device_id*)malloc(num_devices * sizeof(cl_device_id));

	show_cl_info(platform, devices, num_devices);

	cl_context context = clCreateContext(NULL, 1, devices, NULL, NULL, NULL);
	cl_command_queue command_queue = clCreateCommandQueue(context, devices[0], 0, NULL);

	size_t sourceSize[] = { strlen(source) };
	cl_program program = clCreateProgramWithSource(context, 1, &source, sourceSize, NULL);

	clBuildProgram(program, 1, devices, NULL, NULL, NULL);

	cl_mem a_input_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, size * sizeof(int), (void*)A, NULL);
	cl_mem b_input_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, size * sizeof(int), (void*)B, NULL);
	cl_mem c_output_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size * sizeof(int), NULL, NULL);

	cl_kernel kernel = clCreateKernel(program, "helloworld", NULL);

	clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&a_input_buffer);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&b_input_buffer);
	clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&c_output_buffer);

	size_t global_work_size[1] = { size };
	size_t local_work_size[1] = { 128 };

	clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global_work_size, local_work_size, 0, NULL, NULL);
	clEnqueueReadBuffer(command_queue, c_output_buffer, CL_TRUE, 0, size * sizeof(int), C, 0, NULL, NULL);

	clReleaseKernel(kernel);
	clReleaseProgram(program);

	clReleaseMemObject(a_input_buffer);
	clReleaseMemObject(b_input_buffer);
	clReleaseMemObject(c_output_buffer);

	clReleaseCommandQueue(command_queue);
	clReleaseContext(context);

	delete platforms;
}
