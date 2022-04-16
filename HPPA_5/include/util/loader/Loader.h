#pragma once
#include <string>
#include <entity/image.h>

class Loader {
public:
    virtual void upload(image&, const char*) = 0;
    virtual void download(image&, const char*) = 0;
    ~Loader() = default;
protected:
};
