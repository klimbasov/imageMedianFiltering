#pragma once
#include <lab/5/Lab5.h>
#include <lab/6/Lab6.h>
#include <lab/7/Lab7.h>

const std::string path_grey_load = "D:/Projects/VSProjects/HPPA_5/x64/Release/grey/";
const std::string path_grey_store = "D:/Projects/VSProjects/HPPA_5/x64/Release/out/grey/";
const std::string path_color_load = "D:/Projects/VSProjects/HPPA_5/x64/Release/color/";
const std::string path_color_store = "D:/Projects/VSProjects/HPPA_5/x64/Release/out/color/";
const std::string path_array_store = "D:/Projects/VSProjects/HPPA_5/x64/Release/out/array/";
const std::string name_grey = "moon";
const std::string name_color = "big";
const std::string name_noise_color = "noise";
const std::string name_noise_grey = "noise";


//int compare(image img_1, image img_2, const std::string path, const std::string name, int mult);
//int compare_r(image img_1, image img_2, const std::string path, const std::string name);

int main(void) {
	/*Lab5::test(3001, 4001, path_grey_store, name_noise_grey);
	Lab5::prod(path_grey_load, path_grey_store, name_grey);*/
	//Lab6::test(3001, 4001, path_color_store, name_noise_color);
	//Lab6::prod(path_color_load, path_color_store, name_color);
	Lab7::gpu(path_array_store);
	Lab7::cpu(path_array_store);
	return 0;
}
//
//int compare(image img_1, image img_2, const std::string path, const std::string name, int mult = 1)
//{
//	Loader* loader = new pgmLoader();
//	image img;
//	int result = 0;
//	if (img_1.height != img_2.height && img_1.width != img_2.width) {
//		result = -1;
//	}
//	if (result == 0) {
// 		img.magic = "P5";
//		img.depth = img_1.depth;
//		img.height = img_1.height;
//		img.width = img_1.width* mult;
//		img.data =(char*) new unsigned char[img.height * img.width];
//		memset(img.data, 0, img.height * img.width);
//		for (int counter_1 = 0; counter_1 < img_1.height; counter_1++) {
//			for (int counter = 0; counter < img_2.width * mult; counter++) {
//				if (img_1.data[counter_1 * img_2.width* mult + counter] != img_2.data[counter_1 * img_2.width * mult + counter]) {
//					img.data[counter_1 * img.width + counter] = 254;
//					++result;
//				}
//			}
//		}
//		loader->upload(img, (path + name + compare_prefix + format_grey).c_str());
//		delete[] img.data;
//		delete loader;
//	}
//	return result;
//}
//
//int compare_r(image img_1, image img_2, const std::string path, const std::string name)
//{
//	Loader* loader = new pgmLoader();
//	image img;
//	int result = 0;
//	if (img_1.height != img_2.height && img_1.width != img_2.width) {
//		result = -1;
//	}
//	if (result == 0) {
//		img.magic = "P5";
//		img.depth = img_1.depth;
//		img.height = img_1.height;
//		img.width = img_1.width;
//		img.data = (char*) new unsigned char[img.height * img.width];
//		memset(img.data, 0, img.height * img.width);
//		for (int counter_1 = 0; counter_1 < img_1.height; counter_1++) {
//			for (int counter = 0; counter < img_2.width * 3; counter+=3) {
//				if (img_1.data[counter_1 * img_2.width * 3 + counter] != img_2.data[counter_1 * img_2.width * 3 + counter]) {
//					img.data[counter_1 * img.width + counter/3] = 254;
//					++result;
//				}
//			}
//		}
//		loader->upload(img, (path + name + compare_prefix + format_grey).c_str());
//		delete[] img.data;
//		delete loader;
//	}
//	return result;
//}