__kernel void helloworld(__global int* A, __global int* B, __global int* C) {
	int global_id = get_global_id(0);
	int i = global_id;

	C[i] = A[i] + B[i];
}