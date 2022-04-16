#pragma once
#include <entity/image.h>
#include <util/loader/Loader.h>
#include <util/loader/impl/pgmLoader/pgmLoader.h>
#include <util/loader/impl/ppmLoader/ppmLoader.h>
#include <iostream>
#include <handler/cpu/CpuHandler.h>
#include <handler/cpu/rgb/CpuRGBHandler.h>
#include <util/comparator/Comparator.h>
#include <kernel/gpu/gpu.h>
#include <string>

const std::string path_grey_load = "D:/Projects/VSProjects/HPPA_5/x64/Release/grey/";
const std::string path_grey_store = "D:/Projects/VSProjects/HPPA_5/x64/Release/out/grey/";
const std::string path_color_load = "D:/Projects/VSProjects/HPPA_5/x64/Release/color/";
const std::string path_color_store = "D:/Projects/VSProjects/HPPA_5/x64/Release/out/color/";
const std::string name_grey = "moon";
const std::string name_color = "big";
const std::string name_noise_color = "noise";
const std::string name_noise_grey = "noise";
const std::string cpu_prefix = "_cpu";
const std::string color_prefix = "_color";
const std::string gray_prefix = "_grey";
const std::string compare_prefix = "_compare";
const std::string gpu_prefix = "_gpu";
const std::string format_grey = ".pgm";
const std::string format_color = ".ppm";

image cpu();
image c_cpu();
image c_gpu();
image gpu();
image cpu_noise_test(image&);
image gpu_noise_test(image&);
image c_cpu_noise_test(image&);
image c_gpu_noise_test(image&);
int compare(image img_1, image img_2, const std::string path, const std::string name, int mult);
int compare_r(image img_1, image img_2, const std::string path, const std::string name);
image c_gen_noise(int width, int height);
image gen_noise(int width, int height);
void c_test();
void test();
void c_prod();
void prod();

int main(void) {
	c_prod();
	return 0;
}

image cpu() {
	Loader* loader = new pgmLoader();
	image img;
	loader->download(img, (path_grey_load + name_grey + format_grey).c_str());
	std::cout << "start cpu proccessing" << std::endl;
	image cpu_image;
	double time;
	CpuHandler::handle(img, cpu_image, time);
	std::cout << "cpu end in " << time << std::endl;
	loader->upload(cpu_image, (path_grey_store + name_grey + cpu_prefix + format_grey).c_str());
	delete[] img.data;
	delete loader;
	return cpu_image;
}
image gpu() {
	Loader* loader = new pgmLoader();
	image img;
	loader->download(img, (path_grey_load + name_grey + format_grey).c_str());
	std::cout << "start gpu proccessing" << std::endl;
	image proccessed_image;
	double time = 0;
	cudaHandler_opt(img, proccessed_image, time);
	std::cout << std::endl <<"gpu end in " << time << std::endl;
	loader->upload(proccessed_image, (path_grey_store + name_grey + gpu_prefix + format_grey).c_str());
	delete[] img.data;
	delete loader;
	return proccessed_image;
}

int compare(image img_1, image img_2, const std::string path, const std::string name, int mult = 1)
{
	Loader* loader = new pgmLoader();
	image img;
	int result = 0;
	if (img_1.height != img_2.height && img_1.width != img_2.width) {
		result = -1;
	}
	if (result == 0) {
 		img.magic = "P5";
		img.depth = img_1.depth;
		img.height = img_1.height;
		img.width = img_1.width* mult;
		img.data =(char*) new unsigned char[img.height * img.width];
		memset(img.data, 0, img.height * img.width);
		for (int counter_1 = 0; counter_1 < img_1.height; counter_1++) {
			for (int counter = 0; counter < img_2.width * mult; counter++) {
				if (img_1.data[counter_1 * img_2.width* mult + counter] != img_2.data[counter_1 * img_2.width * mult + counter]) {
					img.data[counter_1 * img.width + counter] = 254;
					++result;
				}
			}
		}
		loader->upload(img, (path + name + compare_prefix + format_grey).c_str());
		delete[] img.data;
		delete loader;
	}
	return result;
}

int compare_r(image img_1, image img_2, const std::string path, const std::string name)
{
	Loader* loader = new pgmLoader();
	image img;
	int result = 0;
	if (img_1.height != img_2.height && img_1.width != img_2.width) {
		result = -1;
	}
	if (result == 0) {
		img.magic = "P5";
		img.depth = img_1.depth;
		img.height = img_1.height;
		img.width = img_1.width;
		img.data = (char*) new unsigned char[img.height * img.width];
		memset(img.data, 0, img.height * img.width);
		for (int counter_1 = 0; counter_1 < img_1.height; counter_1++) {
			for (int counter = 0; counter < img_2.width * 3; counter+=3) {
				if (img_1.data[counter_1 * img_2.width * 3 + counter] != img_2.data[counter_1 * img_2.width * 3 + counter]) {
					img.data[counter_1 * img.width + counter/3] = 254;
					++result;
				}
			}
		}
		loader->upload(img, (path + name + compare_prefix + format_grey).c_str());
		delete[] img.data;
		delete loader;
	}
	return result;
}

image gen_noise(int width, int height) {
	image img;
	img.magic = "P5";
	img.depth = 255;
	img.height = height;
	img.width = width;
	img.data = new char[img.height * img.width];
	srand(time(0));
	for (int counter = 0; counter < img.height * img.width; counter++) {
		img.data[counter] = rand() & 255;
	}
	return img;
}

image c_gen_noise(int width, int height) {
	image img;
	img.magic = "P6";
	img.depth = 255;
	img.height = height;
	img.width = width;
	img.data = new char[img.height * img.width * 3];
	srand(time(0));
	for (int counter = 0; counter < img.height * img.width * 3; counter++) {
		img.data[counter] = rand() & 255;
	}
	return img;
}

image cpu_noise_test(image& img) {
	Loader* loader = new pgmLoader();
	std::cout << "start cpu proccessing" << std::endl;
	image cpu_image;
	double time;
	CpuHandler::handle(img, cpu_image, time);
	std::cout << "cpu end in " << time << std::endl;
	loader->upload(cpu_image, (path_grey_store + name_noise_grey + cpu_prefix + format_grey).c_str());
	delete loader;
	return cpu_image;
}
image gpu_noise_test(image& img) {
	Loader* loader = new pgmLoader();
	std::cout << "start gpu proccessing" << std::endl;
	image proccessed_image;
	double time = 0;
	cudaHandler_opt(img, proccessed_image, time);
	std::cout << std::endl << "gpu end in " << time << std::endl;
	loader->upload(proccessed_image, (path_grey_store + name_grey + gpu_prefix + format_grey).c_str());
	delete loader;
	return proccessed_image;
}

image c_cpu_noise_test(image& img) {
	Loader* loader = new ppmLoader();
	std::cout << "start cpu proccessing" << std::endl;
	image cpu_image;
	double time;
	CpuRGBHandler::handle(img, cpu_image, time);
	std::cout << "cpu end in " << time << std::endl;
	loader->upload(cpu_image, (path_color_store + name_noise_color + cpu_prefix + format_color).c_str());
	delete loader;
	return cpu_image;
}
image c_gpu_noise_test(image& img) {
	Loader* loader = new ppmLoader();
	std::cout << "start gpu proccessing" << std::endl;
	image proccessed_image;
	double time = 0;
	c_cudaHandler_opt(img, proccessed_image, time);
	std::cout << std::endl << "gpu end in " << time << std::endl;
	loader->upload(proccessed_image, (path_color_store + name_noise_color + gpu_prefix + format_color).c_str());
	delete loader;
	return proccessed_image;
}

void test() {
	image img = gen_noise(2047, 4000);
	image img_gpu = gpu_noise_test(img);
	image img_cpu = cpu_noise_test(img);
	int result = compare(img_cpu, img_gpu, path_grey_store, "noise");
	std::cout << "missmatch: " << result << std::endl;
	delete[] img_cpu.data;
	delete[] img_gpu.data;
}

void c_test() {
	image img = c_gen_noise(3001, 4000);
	image img_gpu = c_gpu_noise_test(img);
	image img_cpu = c_cpu_noise_test(img);
	int result = compare(img_cpu, img_gpu, path_color_store, name_noise_color, 3);
	std::cout << "missmatch: " << result << std::endl;
	delete[] img_cpu.data;
	delete[] img_gpu.data;
}

void c_prod() {
	image img_gpu = c_gpu();
	image img_cpu = c_cpu();
	//std::cout << "missmatch: " << Comparator::compare(img_cpu.data, img_gpu.data, img_cpu.height * img_cpu.width * 3) << std::endl;
	std::cout << "missmatch: " << compare_r(img_cpu, img_gpu, path_color_store, name_color) << std::endl;
	delete[] img_cpu.data;
	delete[] img_gpu.data;
}

void prod() {
	image img_gpu = gpu();
	image img_cpu = cpu();
	int result = compare(img_cpu, img_gpu, path_grey_store, name_grey);
	std::cout << "missmatch: " << result << std::endl;
}

image c_cpu() {
	Loader* loader = new ppmLoader();
	image img;
	loader->download(img, (path_color_load + name_color + format_color).c_str());
	std::cout << "start cpu proccessing" << std::endl;
	image cpu_image;
	double time;
	CpuRGBHandler::handle(img, cpu_image, time);
	std::cout << "cpu end in " << time << std::endl;
	loader->upload(cpu_image, (path_color_store + name_color + cpu_prefix + color_prefix + format_color).c_str());
	delete[] img.data;
	delete loader;
	return cpu_image;
}

image c_gpu() {
	Loader* loader = new ppmLoader();
	image img;
	loader->download(img, (path_color_load + name_color + format_color).c_str());
	std::cout << "start gpu proccessing" << std::endl;
	image proccessed_image;
	double time = 0;
	c_cudaHandler_opt(img, proccessed_image, time);
	std::cout << std::endl << "gpu end in " << time << std::endl;
	loader->upload(proccessed_image, (path_color_store + name_color + gpu_prefix + color_prefix + format_color).c_str());
	delete[] img.data;
	delete loader;
	return proccessed_image;
}