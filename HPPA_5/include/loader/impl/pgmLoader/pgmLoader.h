#pragma once
#include <loader/Loader.h>
#include <fstream>

class pgmLoader : public Loader
{
public:
	pgmLoader() : Loader() {}
	// Inherited via Loader
	virtual void upload(image&, const char*) override;
	virtual void download(image&, const char*) override;
};

