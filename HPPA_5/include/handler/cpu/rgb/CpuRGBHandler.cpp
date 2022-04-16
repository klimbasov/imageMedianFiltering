#include "CpuRGBHandler.h"

#include <algorithm>
#include <util/timer/Timer.h>

void CpuRGBHandler::handle(const image& src, image& dst, double& time)
{
	Timer timer;
	dst.magic = src.magic;
	dst.depth = src.depth;
	dst.width = src.width;
	dst.height = src.height;
	dst.data = new char[dst.width * dst.height * 3];
	unsigned char window[3][9];
	timer.start();
	for (int i = 0; i < src.height; i++) {
		for (int j = 0; j < src.width * 3; j+=3) {
			for (int a = 0; a < 3; a++) {
				for (int b = 0; b < 3; b++) {
					window[0][a * 3 + b] = *((unsigned char*)src.data + std::max(0, std::min(src.height - 1, i + a - 1)) * src.width * 3 + std::max(0, std::min(src.width*3 - 1 - 2, j + (b - 1) * 3 + 0)));
					window[1][a * 3 + b] = *((unsigned char*)src.data + std::max(0, std::min(src.height - 1, i + a - 1)) * src.width * 3 + std::max(1, std::min(src.width*3 - 1 - 1, j + (b - 1) * 3 + 1)));
					window[2][a * 3 + b] = *((unsigned char*)src.data + std::max(0, std::min(src.height - 1, i + a - 1)) * src.width * 3 + std::max(2, std::min(src.width*3 - 1 - 0, j + (b - 1) * 3 + 2)));
				}
			}
			std::sort(window[0], window[0] + sizeof(window[0]));
			std::sort(window[1], window[1] + sizeof(window[0]));
			std::sort(window[2], window[2] + sizeof(window[0]));
			dst.data[i * dst.width * 3 + j + 0] = window[0][4];
			dst.data[i * dst.width * 3 + j + 1] = window[1][4];
			dst.data[i * dst.width * 3 + j + 2] = window[2][4];
		}
	}
	time = timer.check();
}
