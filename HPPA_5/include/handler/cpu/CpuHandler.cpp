#include "CpuHandler.h"
#include <algorithm>
#include <timer/Timer.h>

void CpuHandler::handle(const image& src, image & dst, double& time)
{
	Timer timer;
	dst.magic = src.magic;
	dst.depth = src.depth;
	dst.width = src.width;
	dst.height = src.height;
	dst.data = new char[dst.width * dst.height];
	unsigned char window[9];
	timer.start();
	for (int i = 0; i < src.height; i++) {
		for (int j = 0; j < src.width; j++) {
			for (int a = 0; a < 3; a++) {
				for (int b = 0; b < 3; b++) {
					window[ a * 3 + b] = *((unsigned char*) src.data + std::max(0, std::min(src.height - 1, i - a + 1)) * src.width + std::max(0, std::min(src.width - 1, j - b + 1)));
				}
			}
			std::sort(window, window + sizeof(window));
			dst.data[i * dst.width + j] = window[4];
		}
	}
	time = timer.check();
}