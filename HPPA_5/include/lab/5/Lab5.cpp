#pragma once
#include "Lab5.h"
#include <util/comparator/Comparator.h>
#include <iostream>
#include <handler/cpu/CpuHandler.h>
#include <util/loader/Loader.h>
#include <util/loader/impl/pgmLoader/pgmLoader.h>
#include <kernel/gpu/gpu.h>
#include <util/generator/pgm/pgmGenerator.h>

const std::string gray_prefix = "_grey";
const std::string cpu_prefix = "_cpu";
const std::string gpu_prefix = "_gpu";
const std::string format_grey = ".pgm";

void Lab5::test(const unsigned int height, const unsigned int width, const std::string dst_path, const std::string name)
{
	image img = pgmGenerator::genarate(width, height);
	image img_gpu = gpu_noise_test(img, dst_path, name);
	image img_cpu = cpu_noise_test(img, dst_path, name);
	int result = Comparator::compare(img_cpu.data, img_gpu.data, img_cpu.height * img_cpu.width);
	std::cout << "missmatch: " << result << std::endl;
	delete[] img_cpu.data;
	delete[] img_gpu.data;
}

void Lab5::prod(const std::string src_path, const std::string dst_path, const std::string name)
{
	image img_gpu = gpu(src_path, dst_path, name);
	image img_cpu = cpu(src_path, dst_path, name);
	int result = Comparator::compare(img_cpu.data, img_gpu.data, img_cpu.height * img_cpu.width);
	std::cout << "missmatch: " << result << std::endl;
	delete[] img_cpu.data, img_gpu.data;
}
image Lab5::cpu_noise_test(image& img, const std::string path, const std::string name) {
	Loader* loader = new pgmLoader();
	std::cout << "start cpu proccessing" << std::endl;
	image cpu_image;
	double time;
	CpuHandler::handle(img, cpu_image, time);
	std::cout << "cpu end in " << time << std::endl;
	loader->upload(cpu_image, (path + name + cpu_prefix + format_grey).c_str());
	delete loader;
	return cpu_image;
}
image Lab5::gpu_noise_test(image& img, const std::string path, const std::string name) {
	Loader* loader = new pgmLoader();
	std::cout << "start gpu proccessing" << std::endl;
	image proccessed_image;
	double time = 0;
	cudaHandler_opt(img, proccessed_image, time);
	std::cout << std::endl << "gpu end in " << time << std::endl;
	loader->upload(proccessed_image, (path + name + gpu_prefix + format_grey).c_str());
	delete loader;
	return proccessed_image;
}

image Lab5::cpu(const std::string src_path, const std::string dst_path, const std::string name) {
	Loader* loader = new pgmLoader();
	image img;
	loader->download(img, (src_path + name + format_grey).c_str());
	std::cout << "start cpu proccessing" << std::endl;
	image cpu_image;
	double time;
	CpuHandler::handle(img, cpu_image, time);
	std::cout << "cpu end in " << time << std::endl;
	loader->upload(cpu_image, (dst_path + name + cpu_prefix + format_grey).c_str());
	delete[] img.data;
	delete loader;
	return cpu_image;
}
image Lab5::gpu(const std::string src_path, const std::string dst_path, const std::string name) {
	Loader* loader = new pgmLoader();
	image img;
	loader->download(img, (src_path + name + format_grey).c_str());
	std::cout << "start gpu proccessing" << std::endl;
	image proccessed_image;
	double time = 0;
	cudaHandler_opt(img, proccessed_image, time);
	std::cout << std::endl << "gpu end in " << time << std::endl;
	loader->upload(proccessed_image, (dst_path + name + gpu_prefix + format_grey).c_str());
	delete[] img.data;
	delete loader;
	return proccessed_image;
}
