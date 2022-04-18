#include "Lab6.h"
#include <util/comparator/Comparator.h>
#include <iostream>
#include <handler/cpu/rgb/CpuRGBHandler.h>
#include <util/loader/Loader.h>
#include <util/loader/impl/ppmLoader/ppmLoader.h>
#include <kernel/gpu/gpu.h>
#include <util/generator/ppm/ppmGenerator.h>

const std::string color_prefix = "_color";
const std::string cpu_prefix = "_cpu";
const std::string gpu_prefix = "_gpu";
const std::string format_color = ".ppm";

image Lab6::cpu_noise_test(image& img, const std::string path, const std::string name) {
	Loader* loader = new ppmLoader();
	std::cout << "start cpu proccessing" << std::endl;
	image cpu_image;
	double time;
	CpuRGBHandler::handle(img, cpu_image, time);
	std::cout << "cpu end in " << time << std::endl;
	loader->upload(cpu_image, (path + name + cpu_prefix + format_color).c_str());
	delete loader;
	return cpu_image;
}
image Lab6::gpu_noise_test(image& img, const std::string path, const std::string name) {
	Loader* loader = new ppmLoader();
	std::cout << "start gpu proccessing" << std::endl;
	image proccessed_image;
	double time = 0;
	c_cudaHandler_opt(img, proccessed_image, time);
	std::cout << std::endl << "gpu end in " << time << std::endl;
	loader->upload(proccessed_image, (path + name + gpu_prefix + format_color).c_str());
	delete loader;
	return proccessed_image;
}

image Lab6::cpu(const std::string src_path, const std::string dst_path, const std::string name) {
	Loader* loader = new ppmLoader();
	image img;
	loader->download(img, (src_path + name + format_color).c_str());
	std::cout << "start cpu proccessing" << std::endl;
	image cpu_image;
	double time;
	CpuRGBHandler::handle(img, cpu_image, time);
	std::cout << "cpu end in " << time << std::endl;
	loader->upload(cpu_image, (dst_path + name + cpu_prefix + format_color).c_str());
	delete[] img.data;
	delete loader;
	return cpu_image;
}

image Lab6::gpu(const std::string src_path, const std::string dst_path, const std::string name) {
	Loader* loader = new ppmLoader();
	image img;
	loader->download(img, (src_path + name + format_color).c_str());
	std::cout << "start gpu proccessing" << std::endl;
	image proccessed_image;
	double time = 0;
	c_cudaHandler_opt(img, proccessed_image, time);
	std::cout << std::endl << "gpu end in " << time << std::endl;
	loader->upload(proccessed_image, (dst_path + name + gpu_prefix + format_color).c_str());
	delete[] img.data;
	delete loader;
	return proccessed_image;
}

void Lab6::test(const unsigned int height, const unsigned int width, const std::string dst_path, const std::string name)
{
	image img = ppmGenerator::genarate(width, height);
	image img_gpu = gpu_noise_test(img, dst_path, name);
	image img_cpu = cpu_noise_test(img, dst_path, name);
	int result = Comparator::compare(img_cpu.data, img_gpu.data, img_cpu.height * img_cpu.width * 3);
	std::cout << "missmatch: " << result << std::endl;
	delete[] img_cpu.data;
	delete[] img_gpu.data;
}

void Lab6::prod(const std::string src_path, const std::string dst_path, const std::string name)
{
	image img_gpu = gpu(src_path, dst_path, name);
	image img_cpu = cpu(src_path, dst_path, name);
	int result = Comparator::compare(img_cpu.data, img_gpu.data, img_cpu.height * img_cpu.width * 3);
	std::cout << "missmatch: " << result << std::endl;
	delete[] img_cpu.data;
	delete[] img_gpu.data;
}
