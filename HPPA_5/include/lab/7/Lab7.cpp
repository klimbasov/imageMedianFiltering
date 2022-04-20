#include "Lab7.h"
#include <entity/image.h>
#include <util/generator/ppm/ppmGenerator.h>
#include <kernel/gpu/gpu.h>
#include <util/loader/impl/ppmLoader/ppmLoader.h>
#include <omp.h>
#include <handler/cpu/rgb/CpuRGBHandler.h>
#include <iostream>
#include <util/timer/Timer.h>
#include <mpi.h>


const unsigned int size = 10;
const unsigned int width = 300;
const unsigned int height = 401;

void Lab7::gpu(const std::string& path)
{
	Timer timer;
	ppmLoader loader;
	timer.start();
	image* images = new image[size];
	image* result_images = new image[size];
	std::cout << "lab7: cuda started." << std::endl <<std::endl;
	for (int counter = 0; counter < size; counter++) {
		loader.download(images[counter], "D:/Projects/VSProjects/HPPA_5/x64/Release/color/big.ppm");
		std::cout << "cuda: " << "\t:" << "image " << counter << " loaded;" << std::endl;
	}

	cudaHanglerMultipleData(images, result_images, size);
	for (int counter = 0; counter < size; counter++) {
		loader.upload(result_images[counter], (path + std::to_string(counter) + std::string(".ppm")).c_str());
		std::cout << "cuda:" << "\t:" << "image " << counter << " saved;" << std::endl;
		delete[] images[counter].data, result_images[counter].data;
	}
	delete[] images, result_images;
	double time = timer.check();
	std::cout << "lab7: cuda fineshed in " << time << " s." << std::endl << std::endl;
}

void Lab7::cpu(const std::string& path)
{
	Timer timer;
	ppmLoader loader;
	image* images = new image[size];
	image* result_images = new image[size];
	timer.start();
	omp_set_num_threads(5);
	std::cout << "lab7: cpu omp started." << std::endl << std::endl;
#pragma omp parallel for
	for (int counter = 0; counter < size; counter++) {
		loader.download(images[counter], "D:/Projects/VSProjects/HPPA_5/x64/Release/color/big.ppm");

		std::cout << "opm thread " << omp_get_thread_num() << "\t:" << "image " << counter << " loaded;" << std::endl;
	}

#pragma omp parallel for
	for (int counter = 0; counter < size; counter++) {
		double time;
		CpuRGBHandler::handle(images[counter], result_images[counter], time);

		std::cout << "opm thread " << omp_get_thread_num() << "\t:" << "image " << counter << " proccessed;" << std::endl;
	}

#pragma omp parallel for
	for (int counter = 0; counter < size; counter++) {
		loader.upload(result_images[counter], (path + std::to_string(counter) + std::string("_cpu") + std::string(".ppm")).c_str());
		std::cout << "opm thread " << omp_get_thread_num() << ":\t" << "image " << counter << " saved;" << std::endl;
		delete[] images[counter].data, result_images[counter].data;
	}
	double time = timer.check();
	delete[] images, result_images;
	std::cout << "lab7: cpu omp fineshed in " << time << " s." << std::endl << std::endl;
}

//void Lab7::mpi_cpu(const std::string& path)
//{
//	Timer timer;
//	ppmLoader loader;
//	image* images = new image[size];
//	image* result_images = new image[size];
//	timer.start();
//	std::cout << "lab7: cpu omp started." << std::endl << std::endl;
//	if (MPI_Init(null, null) !=0) {
//		std::cout << "lab7: MPI initialization fault." << std::endl << std::endl;
//		return;
//	}
//	int MPI_rank, MPI_size;
//	
//	MPI_Comm_rank(MPI_COMM_WORLD, &MPI_rank);
//	MPI_Comm_size(MPI_COMM_WORLD, &MPI_size);
//
//	for (int counter = 0; counter < size; counter++) {
//		loader.download(images[counter], "D:/Projects/VSProjects/HPPA_5/x64/Release/color/big.ppm");
//	}
//	for (int counter = 0; counter < size; counter++) {
//		double time;
//		CpuRGBHandler::handle(images[counter], result_images[counter], time);
//	}
//	for (int counter = 0; counter < size; counter++) {
//		loader.upload(result_images[counter], (path + std::to_string(counter) + std::string("_cpu") + std::string(".ppm")).c_str());
//		delete[] images[counter].data, result_images[counter].data;
//	}
//	double time = timer.check();
//	delete[] images, result_images;
//	std::cout << "lab7: cpu omp fineshed in " << time << " s." << std::endl << std::endl;
//}
