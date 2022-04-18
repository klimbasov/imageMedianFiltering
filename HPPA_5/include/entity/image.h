#pragma once
#include<string>

struct image {
    std::string magic ="";
    int width = 0;
    int height = 0;
    int depth = 0;
    char* data = nullptr;
};