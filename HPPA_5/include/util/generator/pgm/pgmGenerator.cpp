#include "pgmGenerator.h"
#include <ctime>

image pgmGenerator::genarate(const int width, const int height)
{
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
