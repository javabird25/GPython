#include "init/PyGmod.hpp"

pygmod::init::PyGmod* pygmod_instance;

GMOD_MODULE_OPEN()
{
	pygmod_instance = new pygmod::init::PyGmod(LUA);
	return 0;
}

GMOD_MODULE_CLOSE()
{
	delete pygmod_instance;
	pygmod_instance = nullptr;
	return 0;
}