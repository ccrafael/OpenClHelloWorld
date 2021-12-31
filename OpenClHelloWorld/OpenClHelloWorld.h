#pragma once

#include <iostream>
#include <string>
#include <CL/cl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
#include <chrono>

using namespace std;

string load_file_as_string(const char* filename);
void show_cl_info(cl_platform_id platform, cl_device_id* devices, int num_devices);
void sum(int size, int* A, int* B, int* C);
void parallel_sum(const char* source, int size, int* A, int* B, int* C);