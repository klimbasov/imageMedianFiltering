#pragma once
#include <entity/image.h>
#include <loader/Loader.h>
#include <loader/impl/pgmLoader/pgmLoader.h>
#include <iostream>
#include <loader/util/padder/Padder.h>
#include <handler/cpu/CpuHandler.h>
#include <kernel/gpu/gpu.h>
#include <string>

const std::string path = "D:\\Projects\\VSProjects\\HPPA_5\\x64\\Debug\\";
const std::string name = "big";
const std::string cpu_prefix = "_cpu";
const std::string compare_prefix = "_compare";
const std::string gpu_prefix = "_gpu";
const std::string format = ".pgm";

image cpu();
image gpu();
image cpu_noise_test(image&);
image gpu_noise_test(image&);
int compare(image img_1, image img_2);
image gen_noise(int width, int height);
void test();
void prod();

int main(void) {
	//test();
	prod();
	return 0;
}

image cpu() {
	Loader* loader = new pgmLoader();
	image img;
	loader->download(img, (path + name + format).c_str());
	std::cout << "start cpu proccessing" << std::endl;
	image cpu_image;
	double time;
	CpuHandler::handle(img, cpu_image, time);
	std::cout << "cpu end in " << time << std::endl;
	loader->upload(cpu_image, (path + name + cpu_prefix + format).c_str());
	delete[] img.data;
	delete loader;
	return cpu_image;
}
image gpu() {
	Loader* loader = new pgmLoader();
	image img;
	loader->download(img, (path + name + format).c_str());
	std::cout << "start gpu proccessing" << std::endl;
	image proccessed_image;
	double time = 0;
	cudaHandler_opt(img, proccessed_image, time);
	std::cout << std::endl <<"gpu end in " << time << std::endl;
	loader->upload(proccessed_image, (path + name + gpu_prefix + format).c_str());
	delete[] img.data;
	delete loader;
	return proccessed_image;
}

int compare(image img_1, image img_2)
{
	Loader* loader = new pgmLoader();
	image img;
	int result = 0;
	if (img_1.height != img_2.height && img_1.width != img_2.width) {
		result = -1;
	}
	if (result == 0) {
		img.magic = img_1.magic;
		img.depth = img_1.depth;
		img.height = img_1.height;
		img.width = img_1.width;
		img.data =(char*) new unsigned char[img.height * img.width];
		memset(img.data, 0, img.height * img.width);
		for (int counter_1 = 0; counter_1 < img_1.height; counter_1++) {
			for (int counter = 0; counter < img_2.width; counter++) {
				if (img_1.data[counter_1 * img_2.width + counter] != img_2.data[counter_1 * img_2.width + counter]) {
					img.data[counter_1 * img.width + counter] = 254;
					++result;
				}
			}
		}
		loader->upload(img, (path + name + compare_prefix + format).c_str());
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

image cpu_noise_test(image& img) {
	Loader* loader = new pgmLoader();
	std::cout << "start cpu proccessing" << std::endl;
	image cpu_image;
	double time;
	CpuHandler::handle(img, cpu_image, time);
	std::cout << "cpu end in " << time << std::endl;
	loader->upload(cpu_image, (path + name + cpu_prefix + format).c_str());
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
	loader->upload(proccessed_image, (path + name + gpu_prefix + format).c_str());
	delete loader;
	return proccessed_image;
}

void test() {
	image img = gen_noise(2047, 4000);
	image img_gpu = gpu_noise_test(img);
	image img_cpu = cpu_noise_test(img);
	int result = compare(img_cpu, img_gpu);
	std::cout << "missmatch: " << result << std::endl;
	delete[] img_cpu.data;
	delete[] img_gpu.data;
}

void prod() {
	image img_gpu = gpu();
	image img_cpu = cpu();
	int result = compare(img_cpu, img_gpu);
	std::cout << "missmatch: " << result << std::endl;
}