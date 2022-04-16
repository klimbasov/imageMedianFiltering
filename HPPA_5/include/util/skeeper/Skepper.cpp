#include "Skepper.h"

void Skepper::skipComment(std::stringstream& stream)
{
    char next = (char)stream.peek();
    while (next == ' ' || next == '\n') {
        stream.ignore(1);
        next = (char)stream.peek();
    }

    while (next == '#')
    {
        stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        next = (char)stream.peek();
        while (next == ' ') {
            next = (char)stream.peek();
            stream.ignore(1);
        }
    }
}

void Skepper::skipComment(std::ifstream& stream)
{
    char next = (char)stream.peek();
    while (next == ' ' || next == '\n') {
        stream.ignore(1);
        next = (char)stream.peek();
    }

    while (next == '#')
    {
        stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        next = (char)stream.peek();
        while (next == ' ') {
            next = (char)stream.peek();
            stream.ignore(1);
        }
    }
}

void Skepper::skipEOL(std::stringstream& stream)
{
    stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void Skepper::skipEOL(std::ifstream& stream)
{
    stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
