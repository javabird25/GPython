#include <Python.h>
#include "GarrysMod/Lua/Interface.h"

#include "Console.hpp"
#include "addon_launcher.hpp"
#include "../../python_extensions/gmod/luastack.h"
#include "../../python_extensions/gmod/streams.h"

using namespace GarrysMod::Lua;
using std::to_string;

// Adds "luastack" and "streams" modules to builtins and initializes them.
void addAndInitializeGPythonBuiltins() {
    PyImport_AppendInittab("luastack", PyInit_luastack);
	PyImport_AppendInittab("streams", PyInit_streams);
}

// Sets the "lua" variable in the "luastack" module to the pointer to ILuaBase.
void giveILuaBasePtrToLuastack(ILuaBase* ptr) {
    PyImport_ImportModule("luastack");
    setup(ptr);  // Declaration and definition of this function is in "luastack.pyx"
}

// Redirects the Python stdout and stderr to Garry's Mod console.
void redirectIO_toGmod() {
    PyImport_ImportModule("streams");
    // set_stream() uses "luastack" module and don't need ILuaBase pointer to be passed here.
    // Declaration and defintion of this function is in "streams.pyx".
    set_streams();
}

// Redirects the Python stdout and stderr to "gpy.log" for debugging errors which prevent Garry's Mod IO from working.
void redirectIO_toLogFile() {
    PyRun_SimpleString("import sys; sys.stdout = sys.stderr = open('gpy.log', 'w+')");
}

DLL_EXPORT int gpython_run(lua_State *state, bool client) {
	Console cons(LUA);

	cons.log("Binary module loaded");

    addAndInitializeGPythonBuiltins();

	Py_Initialize();
	cons.log("Python initialized!");

    giveILuaBasePtrToLuastack(LUA);
    
    //redirectIO_toLogFile();
    redirectIO_toGmod();

    if (PyErr_Occurred()) {
        cons.error("Setup failed");
        return -1;
    }

	launchAddons(cons);

    if (PyErr_Occurred()) {
        cons.error("Something went wrong");
        return -2;
    }

	return 0;
}

DLL_EXPORT int gpython_finalize(lua_State *state) {
	Console cons(LUA);

	cons.log("Binary module shutting down.");

	Py_FinalizeEx();
	cons.log("Python finalized!");

	return 0;
}