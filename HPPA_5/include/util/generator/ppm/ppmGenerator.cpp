#include "ppmGenerator.h"
#include <ctime>

image ppmGenerator::genarate(const int width, const int height)
{
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
