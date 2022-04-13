#pragma once
#include<sstream>
#include<fstream>
class Skepper
{
public:
    static void skipComment(std::stringstream&);
    static void skipComment(std::ifstream&);
    static void skipEOL(std::stringstream&);
    static void skipEOL(std::ifstream&);
};

