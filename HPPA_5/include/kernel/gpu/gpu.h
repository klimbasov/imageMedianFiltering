#pragma once
#include <entity/image.h>


void cudaHandler_opt(
	const image& src,
	image& dst,
	double& time
);

void c_cudaHandler_opt(
	const image& src,
	image& dst,
	double& time
);
