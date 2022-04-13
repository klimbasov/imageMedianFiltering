#pragma once
#include <entity/image.h>
class Padder
{
public:
	static void addPadding(image& dst, const image& src, size_t padding_size);
};

