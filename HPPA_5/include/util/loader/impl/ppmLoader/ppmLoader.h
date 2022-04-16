#pragma once
#pragma once
#include <util/loader/Loader.h>
#include <fstream>

class ppmLoader : public Loader
{
public:
	ppmLoader() : Loader() {}
	// Inherited via Loader
	virtual void upload(image&, const char*) override;
	virtual void download(image&, const char*) override;
};

