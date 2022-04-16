#pragma once
#ifndef __CUDACC__ 
#define __CUDACC__
#endif
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#include <entity/image.h>
#include "gpu.h"

#define WARP_SIZE 32
#define DEVICE_ID 0

__constant__ unsigned __int32 LAST_BLOCK_HEIGHT;
__constant__ unsigned __int32 FULL_BLOCK_HEIGHT;
__constant__ unsigned __int32 BLOCK_LENGHT_IN_INT;
__constant__ unsigned __int32 BLOCK_LENGHT;
__constant__ unsigned __int32 ITERATIONS_PER_THREAD;
__constant__ unsigned __int32 SRC_PITCH_IN_INT;
__constant__ unsigned __int32 SRC_HEIGHT;
__constant__ unsigned __int32 SRC_WIDTH;
__constant__ unsigned __int32 LAST_PIXEL_INDEX;
__constant__ unsigned __int32 C_PIVOT_ITERATIONS_PER_THREAD;


inline __global__ void _debugKernel(const unsigned __int32* src, unsigned __int32* dst);
inline __global__ void c_debugKernel(const unsigned __int32* src, unsigned __int32* dst);

inline __device__ void sort(const unsigned int** ls_buffer_ptr, unsigned int** ls_sorted_buffer_ptr);
inline __device__ void init_lower_pivot(unsigned char* line, unsigned  char  pivot_window[4]);
inline __device__ void init_median_pivot(unsigned char* line, unsigned  char  pivot_window[4]);
inline __device__ void init_high_pivot(unsigned char* line, unsigned  char  pivot_window[4]);
inline __device__ void c_init_lower_pivot(unsigned char* line, unsigned  char  pivot_window[4]);
inline __device__ void c_init_median_pivot(unsigned char* line, unsigned  char  pivot_window[4]);
inline __device__ void c_init_high_pivot(unsigned char* line, unsigned  char  pivot_window[4]);
inline __device__ void c_pivot_median(unsigned __int32* ls_result_buffer, unsigned __int32* ls_sorted_buffer_ptr[3]);
inline __device__ void pivot_median(unsigned __int32* ls_result_buffer, unsigned __int32* ls_sorted_buffer_ptr[3]);
inline __device__ void store(const unsigned __int32 counter, unsigned int* dst, unsigned int* ls_result_buffer, const unsigned __int32, const unsigned __int32);
inline __device__ void load_line(unsigned int* ls_buffer_ptr, const unsigned int* src, const signed int iter, const unsigned __int32, const unsigned __int32);
inline __device__ void c_load_line(unsigned int* ls_buffer_ptr, const unsigned int* src, const signed int iter, const unsigned __int32, const unsigned __int32);

void cudaHandler_opt(
		const image& src,
		image& dst,
		double& time
		)
{

	const unsigned __int32 full_block_height = 33;	//low dependense to excec time (less then ~6 cause unhandled misses)
	const unsigned __int32 block_length = 2048;
	const unsigned __int32 block_length_in_int = block_length/4;
	dst.magic = src.magic;
	dst.depth = src.depth;
	dst.height = src.height;
	dst.width = src.width;
	dst.data = new char[dst.height * dst.width];
	const unsigned __int32  warps_per_block = 8;
	const unsigned __int32  threads_per_block = WARP_SIZE * warps_per_block;
	const unsigned __int32 blocks_per_raw = (src.width + block_length - 1) / block_length;
	const unsigned __int32 blocks_per_column = (src.height + full_block_height - 1) / full_block_height;
	const unsigned __int32 last_block_height = src.height%full_block_height == 0 ? full_block_height : src.height % full_block_height;
	const unsigned __int32 iterations_per_thread = block_length / 4 / threads_per_block;
	const unsigned __int32 shared_alloc_size = 6 * (block_length+128);
	const unsigned int aligne_length = blocks_per_raw * block_length;
	const unsigned __int32 aligne_length_in_int = aligne_length / 4;
	const unsigned __int32 last_pixel_index = src.width%block_length == 0 ? block_length : src.width % block_length;
	unsigned __int8* cuda_src, *cuda_dst;
	cudaError_t cudaStatus;
	
	cudaEvent_t start, stop;
	float elapsedTime = 0;

	cudaStatus = cudaSetDevice(DEVICE_ID);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
		goto Error;
	}

	cudaStatus = cudaEventCreate(&start);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
		goto Error;
	}

	cudaStatus = cudaEventCreate(&stop);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
		goto Error;
	}
	
	cudaStatus = cudaMalloc((void**)&cuda_dst, aligne_length * dst.height);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}
	
	cudaStatus = cudaMalloc((void**)&cuda_src, aligne_length * src.height);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}
	
	cudaStatus = cudaMemcpy2D((void*)cuda_src, aligne_length, (const void*)src.data, src.width, src.width, src.height, cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	

	cudaStatus | cudaMemcpyToSymbol(FULL_BLOCK_HEIGHT, &full_block_height, sizeof(unsigned __int32), 0, cudaMemcpyHostToDevice);		//year, that's ridiculous! but i'm done...
	cudaStatus | cudaMemcpyToSymbol(BLOCK_LENGHT_IN_INT, &block_length_in_int, sizeof(unsigned __int32), 0, cudaMemcpyHostToDevice);
	cudaStatus | cudaMemcpyToSymbol(BLOCK_LENGHT, &block_length, sizeof(unsigned __int32), 0, cudaMemcpyHostToDevice);
	cudaStatus | cudaMemcpyToSymbol(SRC_PITCH_IN_INT, &aligne_length_in_int, sizeof(unsigned __int32), 0, cudaMemcpyHostToDevice);
	cudaStatus | cudaMemcpyToSymbol(ITERATIONS_PER_THREAD, &iterations_per_thread, sizeof(unsigned __int32), 0, cudaMemcpyHostToDevice);
	cudaStatus | cudaMemcpyToSymbol(LAST_BLOCK_HEIGHT, &last_block_height, sizeof(unsigned __int32), 0, cudaMemcpyHostToDevice);
	cudaStatus | cudaMemcpyToSymbol(SRC_HEIGHT, &src.height, sizeof(unsigned __int32), 0, cudaMemcpyHostToDevice);
	cudaStatus | cudaMemcpyToSymbol(SRC_WIDTH, &src.width, sizeof(unsigned __int32), 0, cudaMemcpyHostToDevice);
	cudaStatus | cudaMemcpyToSymbol(LAST_PIXEL_INDEX, &last_pixel_index, sizeof(unsigned __int32), 0, cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpyToSymbol failed!");
		goto Error;
	}

	dim3 blockDim(threads_per_block);
	dim3 gridDim(blocks_per_raw, blocks_per_column);

	cudaStatus = cudaEventRecord(start);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}
	_debugKernel << <gridDim, blockDim, shared_alloc_size >> > ((const unsigned int*)cuda_src, (unsigned int*)cuda_dst);

	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching kernel!\n", cudaStatus);
		goto Error;
	}
	cudaStatus = cudaEventRecord(stop);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	cudaStatus = cudaEventSynchronize(stop);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}
	cudaStatus = cudaEventElapsedTime(&elapsedTime, start, stop);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}
	
	cudaStatus = cudaMemcpy2D((void*)dst.data, dst.width, (const void*)cuda_dst, aligne_length,  dst.width, dst.height, cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

Error:
	fprintf(stderr, cudaGetErrorString(cudaStatus));
	cudaFree(cuda_dst);
	cudaFree(cuda_src);

	time = elapsedTime / 1000;
}

void c_cudaHandler_opt(const image& src, image& dst, double& time)
{
	const unsigned __int32 full_block_height = 33;	//low dependense to excec time (less then ~6 cause unhandled misses)
	const unsigned __int32 block_length = 2048;
	const unsigned __int32 block_length_in_int = block_length / 4;
	dst.magic = src.magic;
	dst.depth = src.depth;
	dst.height = src.height;
	dst.width = src.width;
	dst.data = new char[dst.height * dst.width * 3];
	const unsigned __int32 length_in_byte = src.width * 3;
	const unsigned __int32 warps_per_block = 8;
	const unsigned __int32 threads_per_block = WARP_SIZE * warps_per_block;
	const unsigned __int32 blocks_per_raw = (length_in_byte + block_length - 1) / block_length;
	const unsigned __int32 blocks_per_column = (src.height + full_block_height - 1) / full_block_height;
	const unsigned __int32 last_block_height = src.height % full_block_height == 0 ? full_block_height : src.height % full_block_height;
	const unsigned __int32 iterations_per_thread = block_length / 4 / threads_per_block;
	const unsigned __int32 shared_alloc_size = 6 * (block_length + 128);
	const unsigned int aligne_length = blocks_per_raw * block_length;
	const unsigned __int32 aligne_length_in_int = aligne_length / 4;
	const unsigned __int32 last_pixel_index = length_in_byte % block_length == 0 ? block_length : length_in_byte % block_length;
	const unsigned __int32 c_pivot_iterations_per_thread = block_length_in_int / 3 / threads_per_block;
	unsigned __int8* cuda_src, * cuda_dst;
	cudaError_t cudaStatus;

	cudaEvent_t start, stop;
	float elapsedTime = 0;

	cudaStatus = cudaSetDevice(DEVICE_ID);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
		goto Error;
	}

	cudaStatus = cudaEventCreate(&start);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
		goto Error;
	}

	cudaStatus = cudaEventCreate(&stop);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&cuda_dst, aligne_length * dst.height);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&cuda_src, aligne_length * src.height);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMemcpy2D((void*)cuda_src, aligne_length, (const void*)src.data, length_in_byte, length_in_byte, src.height, cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}



	cudaStatus | cudaMemcpyToSymbol(FULL_BLOCK_HEIGHT, &full_block_height, sizeof(unsigned __int32), 0, cudaMemcpyHostToDevice);		//year, that's ridiculous! but i'm done...
	cudaStatus | cudaMemcpyToSymbol(BLOCK_LENGHT_IN_INT, &block_length_in_int, sizeof(unsigned __int32), 0, cudaMemcpyHostToDevice);
	cudaStatus | cudaMemcpyToSymbol(BLOCK_LENGHT, &block_length, sizeof(unsigned __int32), 0, cudaMemcpyHostToDevice);
	cudaStatus | cudaMemcpyToSymbol(SRC_PITCH_IN_INT, &aligne_length_in_int, sizeof(unsigned __int32), 0, cudaMemcpyHostToDevice);
	cudaStatus | cudaMemcpyToSymbol(ITERATIONS_PER_THREAD, &iterations_per_thread, sizeof(unsigned __int32), 0, cudaMemcpyHostToDevice);
	cudaStatus | cudaMemcpyToSymbol(LAST_BLOCK_HEIGHT, &last_block_height, sizeof(unsigned __int32), 0, cudaMemcpyHostToDevice);
	cudaStatus | cudaMemcpyToSymbol(SRC_HEIGHT, &src.height, sizeof(unsigned __int32), 0, cudaMemcpyHostToDevice);
	cudaStatus | cudaMemcpyToSymbol(SRC_WIDTH, &length_in_byte, sizeof(unsigned __int32), 0, cudaMemcpyHostToDevice);
	cudaStatus | cudaMemcpyToSymbol(LAST_PIXEL_INDEX, &last_pixel_index, sizeof(unsigned __int32), 0, cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpyToSymbol failed!");
		goto Error;
	}

	dim3 blockDim(threads_per_block);
	dim3 gridDim(blocks_per_raw, blocks_per_column);

	cudaStatus = cudaEventRecord(start);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}
	c_debugKernel << <gridDim, blockDim, shared_alloc_size >> > ((const unsigned int*)cuda_src, (unsigned int*)cuda_dst);

	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching kernel!\n", cudaStatus);
		goto Error;
	}
	cudaStatus = cudaEventRecord(stop);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	cudaStatus = cudaEventSynchronize(stop);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}
	cudaStatus = cudaEventElapsedTime(&elapsedTime, start, stop);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	cudaStatus = cudaMemcpy2D((void*)dst.data, length_in_byte, (const void*)cuda_dst, aligne_length, length_in_byte, dst.height, cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

Error:
	fprintf(stderr, cudaGetErrorString(cudaStatus));
	cudaFree(cuda_dst);
	cudaFree(cuda_src);

	time = elapsedTime / 1000;
}



inline __global__ void _debugKernel(const unsigned __int32* src, unsigned __int32* dst) {
	extern __shared__ unsigned __int32 allocated_space[];
	
	unsigned __int32* ls_buffer_ptr[3];
	ls_buffer_ptr[0] = (unsigned __int32*) allocated_space;
	ls_buffer_ptr[1] = (unsigned __int32*) (allocated_space + BLOCK_LENGHT_IN_INT + 32);
	ls_buffer_ptr[2] = (unsigned __int32*) (allocated_space + (BLOCK_LENGHT_IN_INT + 32) * 2);
	unsigned __int32* ls_sorted_buffer_ptr[3];
	ls_sorted_buffer_ptr[0] = (unsigned __int32*) (allocated_space + (BLOCK_LENGHT_IN_INT + 32) * 3);
	ls_sorted_buffer_ptr[1] = (unsigned __int32*) (allocated_space + (BLOCK_LENGHT_IN_INT + 32) * 4);
	ls_sorted_buffer_ptr[2] = (unsigned __int32*) (allocated_space + (BLOCK_LENGHT_IN_INT + 32) * 5);

	const unsigned __int32 BLOCK_HEIGHT = blockIdx.y == gridDim.y - 1 ?  LAST_BLOCK_HEIGHT : FULL_BLOCK_HEIGHT;
	const unsigned __int32 BLOCK_OFFSET_X = blockIdx.x * BLOCK_LENGHT_IN_INT;
	const unsigned __int32 BLOCK_OFFSET_Y = blockIdx.y * FULL_BLOCK_HEIGHT;

	load_line(ls_buffer_ptr[1], src, -1, BLOCK_OFFSET_X, BLOCK_OFFSET_Y);
	load_line(ls_buffer_ptr[2], src, 0, BLOCK_OFFSET_X, BLOCK_OFFSET_Y);

	for (int counter = 0; counter < BLOCK_HEIGHT; counter++) {
		unsigned __int32* ls_result_buffer = ls_buffer_ptr[(counter+1) % 3];
		
		load_line(ls_buffer_ptr[ counter % 3 ], src, counter + 1, BLOCK_OFFSET_X, BLOCK_OFFSET_Y);
		
		sort(ls_buffer_ptr, ls_sorted_buffer_ptr);

		pivot_median(ls_result_buffer, ls_sorted_buffer_ptr); 
	
		store(counter, dst, ls_result_buffer, BLOCK_OFFSET_X, BLOCK_OFFSET_Y);
	}
}


inline __global__ 
void c_debugKernel(const unsigned __int32* src, unsigned __int32* dst) {
	extern __shared__ unsigned __int32 allocated_space[];

	unsigned __int32* ls_buffer_ptr[3];
	ls_buffer_ptr[0] = (unsigned __int32*)allocated_space;
	ls_buffer_ptr[1] = (unsigned __int32*)(allocated_space + BLOCK_LENGHT_IN_INT + 32);
	ls_buffer_ptr[2] = (unsigned __int32*)(allocated_space + (BLOCK_LENGHT_IN_INT + 32) * 2);
	unsigned __int32* ls_sorted_buffer_ptr[3];
	ls_sorted_buffer_ptr[0] = (unsigned __int32*)(allocated_space + (BLOCK_LENGHT_IN_INT + 32) * 3);
	ls_sorted_buffer_ptr[1] = (unsigned __int32*)(allocated_space + (BLOCK_LENGHT_IN_INT + 32) * 4);
	ls_sorted_buffer_ptr[2] = (unsigned __int32*)(allocated_space + (BLOCK_LENGHT_IN_INT + 32) * 5);

	const unsigned __int32 BLOCK_HEIGHT = blockIdx.y == gridDim.y - 1 ? LAST_BLOCK_HEIGHT : FULL_BLOCK_HEIGHT;
	const unsigned __int32 BLOCK_OFFSET_X = blockIdx.x * BLOCK_LENGHT_IN_INT;
	const unsigned __int32 BLOCK_OFFSET_Y = blockIdx.y * FULL_BLOCK_HEIGHT;

	c_load_line(ls_buffer_ptr[1], src, -1, BLOCK_OFFSET_X, BLOCK_OFFSET_Y);
	c_load_line(ls_buffer_ptr[2], src, 0, BLOCK_OFFSET_X, BLOCK_OFFSET_Y);

	for (int counter = 0; counter < BLOCK_HEIGHT; counter++) {
		unsigned __int32* ls_result_buffer = ls_buffer_ptr[(counter + 1) % 3];

		c_load_line(ls_buffer_ptr[counter % 3], src, counter + 1, BLOCK_OFFSET_X, BLOCK_OFFSET_Y);

		sort(ls_buffer_ptr, ls_sorted_buffer_ptr);

		c_pivot_median(ls_result_buffer, ls_sorted_buffer_ptr);

		store(counter, dst, ls_result_buffer, BLOCK_OFFSET_X, BLOCK_OFFSET_Y);
	}
}

inline __device__ 
void pivot_median(unsigned __int32 * ls_result_buffer, unsigned __int32* ls_sorted_buffer_ptr[3]) {
	for (int counter_1 = 0; counter_1 < ITERATIONS_PER_THREAD; counter_1++) {
		const int OFFSET = blockDim.x * counter_1 + threadIdx.x;
		int ls_line[3];
		int result;
		unsigned char pivot_window[3][4];
		unsigned char* line = (unsigned char*)ls_line;
		
		ls_line[0] = ls_sorted_buffer_ptr[2][OFFSET];
		ls_line[1] = ls_sorted_buffer_ptr[2][OFFSET + 1];
		ls_line[2] = ls_sorted_buffer_ptr[2][OFFSET + 2];
		
		init_lower_pivot(line, pivot_window[0]);

		ls_line[0] = ls_sorted_buffer_ptr[1][OFFSET];
		ls_line[1] = ls_sorted_buffer_ptr[1][OFFSET + 1];
		ls_line[2] = ls_sorted_buffer_ptr[1][OFFSET + 2];
		
		init_median_pivot(line, pivot_window[1]);
		
		ls_line[0] = ls_sorted_buffer_ptr[0][OFFSET];
		ls_line[1] = ls_sorted_buffer_ptr[0][OFFSET + 1];
		ls_line[2] = ls_sorted_buffer_ptr[0][OFFSET + 2];
		
		init_high_pivot(line, pivot_window[2]);

		unsigned int sum = __vadd4(*((int*)(pivot_window[0])), __vadd4(*((int*)(pivot_window[1])), *((int*)(pivot_window[2]))));
		unsigned int min = __vminu4(*((int*)(pivot_window[0])), __vminu4(*((int*)(pivot_window[1])), *((int*)(pivot_window[2]))));
		unsigned int max = __vmaxu4(*((int*)(pivot_window[0])), __vmaxu4(*((int*)(pivot_window[1])), *((int*)(pivot_window[2]))));
		result = __vsub4(sum, __vadd4(min, max));
		
		ls_result_buffer[OFFSET] = result;
	}
}

inline __device__
void c_pivot_median(unsigned __int32* ls_result_buffer, unsigned __int32* ls_sorted_buffer_ptr[3]) {
	for (int counter_1 = 0; counter_1 < ITERATIONS_PER_THREAD; counter_1++) {
		const int OFFSET = blockDim.x * counter_1 + threadIdx.x;
		int ls_line[3];
		int result;
		unsigned char pivot_window[3][4];
		unsigned char* line = (unsigned char*)ls_line;

		ls_line[0] = ls_sorted_buffer_ptr[2][OFFSET];
		ls_line[1] = ls_sorted_buffer_ptr[2][OFFSET + 1];
		ls_line[2] = ls_sorted_buffer_ptr[2][OFFSET + 2];

		c_init_lower_pivot(line, pivot_window[0]);

		ls_line[0] = ls_sorted_buffer_ptr[1][OFFSET];
		ls_line[1] = ls_sorted_buffer_ptr[1][OFFSET + 1];
		ls_line[2] = ls_sorted_buffer_ptr[1][OFFSET + 2];

		c_init_median_pivot(line, pivot_window[1]);

		ls_line[0] = ls_sorted_buffer_ptr[0][OFFSET];
		ls_line[1] = ls_sorted_buffer_ptr[0][OFFSET + 1];
		ls_line[2] = ls_sorted_buffer_ptr[0][OFFSET + 2];

		c_init_high_pivot(line, pivot_window[2]);

		unsigned int sum = __vadd4(*((int*)(pivot_window[0])), __vadd4(*((int*)(pivot_window[1])), *((int*)(pivot_window[2]))));
		unsigned int min = __vminu4(*((int*)(pivot_window[0])), __vminu4(*((int*)(pivot_window[1])), *((int*)(pivot_window[2]))));
		unsigned int max = __vmaxu4(*((int*)(pivot_window[0])), __vmaxu4(*((int*)(pivot_window[1])), *((int*)(pivot_window[2]))));
		result = __vsub4(sum, __vadd4(min, max));

		ls_result_buffer[OFFSET] = result;
	}
}

inline __device__ 
void init_high_pivot(unsigned char* line,unsigned  char  pivot_window[4])
{
	for (int counter_2 = 3; counter_2 < 7; counter_2++) {
		unsigned int index = counter_2;
		if (line[counter_2] < line[counter_2 + 1]) {
			index = counter_2 + 1;
		}
		if (line[index] < line[counter_2 + 2]) {
			index = counter_2 + 2;
		}
		pivot_window[counter_2-3] = line[index];
	}
}

inline __device__ 
void init_median_pivot(unsigned char* line, unsigned  char pivot_window[4])
{
	for (int counter_2 = 3; counter_2 < 7; counter_2++) {
		unsigned int index_1 = counter_2;
		unsigned int index_2 = counter_2 + 1;
		if (line[counter_2] > line[counter_2 + 1]) {
			index_1 = counter_2 + 1;
			index_2 = counter_2;
		}
		unsigned int index_median = index_2;
		if (line[index_2] > line[counter_2 + 2]) {
			index_median = index_1;
			if (line[index_1] < line[counter_2+2]) {
				index_median = counter_2 + 2;
			}
			
		}
		pivot_window[counter_2-3] = line[index_median];
	}
}

inline __device__ 
void init_lower_pivot(unsigned char* line, unsigned char  pivot_window[4])
{
	for (int counter_2 = 3; counter_2 < 7; counter_2++) {
		unsigned int index = counter_2;
		if (line[counter_2] > line[counter_2 + 1]) {
			index = counter_2 + 1;
		}
		if (line[index] > line[counter_2 + 2]) {
			index = counter_2 + 2;
		}
		pivot_window[counter_2-3] = line[index];
	}
}

inline __device__
void c_init_high_pivot(unsigned char* line, unsigned  char  pivot_window[4])
{
	for (int counter_2 = 1; counter_2 < 5; counter_2++) {
		unsigned int index = counter_2;
		if (line[counter_2] < line[counter_2 + 3]) {
			index = counter_2 + 3;
		}
		if (line[index] < line[counter_2 + 6]) {
			index = counter_2 + 6;
		}
		pivot_window[counter_2 - 1] = line[index];
	}
}

inline __device__
void c_init_median_pivot(unsigned char* line, unsigned  char pivot_window[4])
{
	for (int counter_2 = 1; counter_2 < 5; counter_2++) {
		unsigned int index_1 = counter_2;
		unsigned int index_2 = counter_2 + 3;
		if (line[counter_2] > line[counter_2 + 3]) {
			index_1 = counter_2 + 3;
			index_2 = counter_2;
		}
		unsigned int index_median = index_2;
		if (line[index_2] > line[counter_2 + 6]) {
			index_median = index_1;
			if (line[index_1] < line[counter_2 + 6]) {
				index_median = counter_2 + 6;
			}

		}
		pivot_window[counter_2 - 1] = line[index_median];
	}
}

inline __device__
void c_init_lower_pivot(unsigned char* line, unsigned char  pivot_window[4])
{
	for (int counter_2 = 1; counter_2 < 5; counter_2++) {
		unsigned int index = counter_2;
		if (line[counter_2] > line[counter_2 + 3]) {
			index = counter_2 + 3;
		}
		if (line[index] > line[counter_2 + 6]) {
			index = counter_2 + 6;
		}
		pivot_window[counter_2 - 1] = line[index];
	}
}

inline __device__
void store(const unsigned __int32 counter, unsigned int* dst, unsigned int* ls_result_buffer, const unsigned __int32 BLOCK_OFFSET_X, const unsigned __int32 BLOCK_OFFSET_Y)
{
	const unsigned int BASE = SRC_PITCH_IN_INT * (BLOCK_OFFSET_Y + counter) + BLOCK_OFFSET_X;
	for (int counter_1 = 0; counter_1 < ITERATIONS_PER_THREAD; counter_1++) {
		const unsigned int OFFSET = counter_1 * blockDim.x + threadIdx.x;
		dst[BASE + OFFSET] = ls_result_buffer[OFFSET];
	}
	__syncthreads();
}

inline __device__ 
void sort(const unsigned int** ls_buffer_ptr, unsigned int** ls_sorted_buffer_ptr)
{
	unsigned int ls_column[3];
	unsigned int ls_sorted_column[3];
	char* column = (char*)ls_column;
	for (int counter = 0; counter < ITERATIONS_PER_THREAD; counter++) {
		const unsigned int INDEX = blockDim.x * counter + threadIdx.x;

		for (int counter_1 = 0; counter_1 < 3; counter_1++) {
			ls_column[counter_1] = ls_buffer_ptr[counter_1][INDEX];
		}

		ls_sorted_column[0] = __vminu4(ls_column[0], __vminu4(ls_column[1], ls_column[2]));
		ls_sorted_column[2] = __vmaxu4(ls_column[0], __vmaxu4(ls_column[1], ls_column[2]));
		unsigned int sum = __vadd4(ls_column[0], __vadd4(ls_column[1], ls_column[2]));
		ls_sorted_column[1] = __vsub4(__vsub4(sum, ls_sorted_column[2]), ls_sorted_column[0]);

		for (int counter_1 = 0; counter_1 < 3; counter_1++) {
			ls_sorted_buffer_ptr[counter_1][INDEX] = ls_sorted_column[counter_1];
		}
	}

	if (threadIdx.x < 2) {

		for (int counter_1 = 0; counter_1 < 3; counter_1++) {
			ls_column[counter_1] = ls_buffer_ptr[counter_1][BLOCK_LENGHT_IN_INT + threadIdx.x];
		}

		ls_sorted_column[0] = __vminu4(ls_column[0], __vminu4(ls_column[1], ls_column[2]));
		ls_sorted_column[2] = __vmaxu4(ls_column[0], __vmaxu4(ls_column[1], ls_column[2]));
		unsigned int sum = __vadd4(ls_column[0], __vadd4(ls_column[1], ls_column[2]));
		ls_sorted_column[1] = __vsub4(sum, __vadd4(ls_sorted_column[0], ls_sorted_column[2]));

		for (int counter_1 = 0; counter_1 < 3; counter_1++) {
			ls_sorted_buffer_ptr[counter_1][BLOCK_LENGHT_IN_INT + threadIdx.x] = ls_sorted_column[counter_1];
		}
	}
	__syncthreads();
}

inline __device__
void load_line(unsigned int* ls_buffer_ptr, const unsigned int* src, const signed int iter, const unsigned __int32 BLOCK_OFFSET_X, const unsigned __int32 BLOCK_OFFSET_Y)
{
	const unsigned __int32 y_offset = min((signed int)SRC_HEIGHT - 1, max(0, (signed int)(BLOCK_OFFSET_Y + iter)));
	const unsigned __int32 offset = SRC_PITCH_IN_INT * y_offset + BLOCK_OFFSET_X;
	for (int counter = 0; counter < ITERATIONS_PER_THREAD; counter++) {
		const unsigned int x_offset = threadIdx.x + blockDim.x * counter;
		ls_buffer_ptr[x_offset + 1] = src[offset + x_offset];
	}

	if (threadIdx.x == 0) {
		const int step = BLOCK_OFFSET_X == 0 ? 0 : -1;
		const int byte_y_offset = (SRC_PITCH_IN_INT * y_offset) << 2;
		unsigned int load_addr = 4 + BLOCK_LENGHT;
		unsigned int x_offset = (BLOCK_OFFSET_X + BLOCK_LENGHT_IN_INT) << 2;
		if (x_offset >= SRC_WIDTH) {
			load_addr = 4 + LAST_PIXEL_INDEX;
			x_offset = SRC_WIDTH - 1;
			
		}
		((unsigned char*)ls_buffer_ptr)[3] = ((unsigned char*)src)[byte_y_offset + (BLOCK_OFFSET_X << 2) + step];
		((unsigned char*)ls_buffer_ptr)[load_addr] = ((unsigned char*)src)[byte_y_offset + x_offset];
	}
	__syncthreads();
}

inline __device__
void c_load_line(unsigned int* ls_buffer_ptr, const unsigned int* src, const signed int iter, const unsigned __int32 BLOCK_OFFSET_X, const unsigned __int32 BLOCK_OFFSET_Y)
{
	const unsigned __int32 y_offset = min((signed int)SRC_HEIGHT - 1, max(0, (signed int)(BLOCK_OFFSET_Y + iter)));
	const unsigned __int32 offset = SRC_PITCH_IN_INT * y_offset + BLOCK_OFFSET_X;
	for (int counter = 0; counter < ITERATIONS_PER_THREAD; counter++) {
		const unsigned int x_offset = threadIdx.x + blockDim.x * counter;
		ls_buffer_ptr[x_offset + 1] = src[offset + x_offset];
	}

	if (threadIdx.x == 0) {
		const int byte_y_offset = (SRC_PITCH_IN_INT * y_offset) << 2;
		for (int counter = 0; counter < 3; counter++) {
			const int step = BLOCK_OFFSET_X == 0 ? counter : counter - 3;
			unsigned int load_addr = 4 + BLOCK_LENGHT + counter;
			unsigned int x_offset = ((BLOCK_OFFSET_X + BLOCK_LENGHT_IN_INT) << 2) + counter;
			if (x_offset >= SRC_WIDTH) {
				load_addr = 4 + LAST_PIXEL_INDEX + counter;
				x_offset = SRC_WIDTH - 3 + counter;
			}
			((unsigned char*)ls_buffer_ptr)[counter + 1] = ((unsigned char*)src)[byte_y_offset + (BLOCK_OFFSET_X << 2) + step];
			((unsigned char*)ls_buffer_ptr)[load_addr] = ((unsigned char*)src)[byte_y_offset + x_offset];
		}
	}
	__syncthreads();
}