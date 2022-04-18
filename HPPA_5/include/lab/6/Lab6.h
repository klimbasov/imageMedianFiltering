#pragma once
#include <string>
#include <util/comparator/Comparator.h>
#include <iostream>
#include <handler/cpu/CpuHandler.h>
#include <util/loader/Loader.h>
#include <util/loader/impl/pgmLoader/pgmLoader.h>
#include <kernel/gpu/gpu.h>

class Lab6
{
public:
	static void test(const unsigned int height, const unsigned int width, const std::string dst_path, const std::string name);
	static void prod(const std::string src_path, const std::string dst_path, const std::string name);
private:
	static image cpu(const std::string src_path, const std::string dst_path, const std::string name);
	static image gpu(const std::string src_path, const std::string dst_path, const std::string name);
	static image cpu_noise_test(image&, const std::string path, const std::string name);
	static image gpu_noise_test(image&, const std::string path, const std::string name);
};

