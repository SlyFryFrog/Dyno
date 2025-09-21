#include <filesystem>
#include <iostream>
#include <julia.h>
#include <print>
#include <stdexcept>
#include <string>

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

import dyno;

using namespace Dyno;

/**
 * @brief Returns the absolute path to the running executable.
 */
std::string get_working_directory()
{
#if defined(__APPLE__)
	char buffer[PATH_MAX];
	uint32_t size = sizeof(buffer);
	if (_NSGetExecutablePath(buffer, &size) == 0)
	{
		return std::filesystem::canonical(buffer).parent_path().string();
	}
#endif
	return std::filesystem::current_path().string();
}

/**
 * @brief Current working directory where the executable is located. Determined at runtime.
 */
const inline std::string WorkingDirectory = get_working_directory();

void dynamic_library_impl()
{
	Dylib lib("libgame");

	auto size = lib.load<size_t (*)()>("get_registry_size")();
	std::println("Number of registered functions: ", size);

	auto keys = lib.load<const char** (*)()>("get_registry_keys")();

	// Iterate over the keys
	std::println("Registered Functions: ");
	for (size_t i = 0; i < size; ++i)
	{
		std::println("\t{}", keys[i]);
	}


	auto ret = lib.load<int (*)()>("jl_GameA__init")();

	lib.load<void (*)(void*, int)>("jl_GameB__process")(pack_args(35.9).data(), 1);

	lib.load<void (*)(void*, int)>("jl_GameB_my_func")(pack_args(3, 4, 6.4).data(), 3);

	double args = 40.6;

	auto process_ret = lib.load<double (*)(void*, int)>("jl_GameA__process")(&args, 1);

	int arg1 = 2;
	std::vector my_func_args = {(void*)&arg1};
	auto returnVal = lib.load<int (*)(void*, int)>("jl_GameA_my_func_int")(my_func_args.data(), 1);


	lib.unload();
}

void julia_engine_impl()
{
	Julia::init();
	Julia::load_script("scripts/precompile.jl");
	Julia::shutdown();
}

int main()
{
	std::filesystem::current_path(WorkingDirectory);

	// Running both JIT and AOT runtimes does not work, you must choose 1
	//julia_engine_impl();
	dynamic_library_impl();
}
