#pragma once
#include<string>

struct image {
    std::string magic;
    int width;
    int height;
    int depth;
    char* data;
};