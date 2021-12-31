#include "OpenClHelloWorld.h"

int main(int argc, char* argv[]) {
	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::duration;
	using std::chrono::milliseconds;

	size_t length = 80000000;

	int* A = new int[length];
	int* B = new int[length];
	int* C = new int[length];

	for (int i = 0; i < length; ++i) {
		A[i] = i;
		B[i] = i;
		C[i] = 0;
	}

	auto t1 = high_resolution_clock::now();
	sum(length, A, B, C);
	auto t2 = high_resolution_clock::now();
	auto ms_int = duration_cast<milliseconds>(t2 - t1);

	std::cout << "Secuential computation " << ms_int.count() << "ms\n";

	for (int i = 0; i < length; ++i) {
		C[i] = 0;
	}

	const char* filename = "kernel.cl";
	string kernel_source_str = load_file_as_string(filename);

	const char* source = kernel_source_str.c_str();

	t1 = high_resolution_clock::now();
	parallel_sum(source, length, A, B, C);
	t2 = high_resolution_clock::now();
	ms_int = duration_cast<milliseconds>(t2 - t1);
	std::cout << "Parallel computation " << ms_int.count() << "ms\n";

	int error = 0;
	for (int i = 0; i < length; ++i) {
		if (A[i] + B[i] != C[i]) {
			error++;
		}
	}

	if (error > 0) {
		cout << "Computation with errors=" << error << endl;
	}
	else {
		cout << "Computation OK" << endl;
	}

	delete A;
	delete B;
	delete C;

	return 0;
}