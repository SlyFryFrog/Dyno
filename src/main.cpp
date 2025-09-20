#include <filesystem>
#include <iostream>
#include <julia.h>
#include <stdexcept>
#include <string>
#include <print>

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
	Dylib lib("libgame.dylib");

	using get_registry_t = jl_value_t* (*)();

	auto get_registry = lib.load<get_registry_t>("get_registry");

	// Get the registry (Dict)
	jl_value_t* dict = get_registry();

	// Get Base.keys(dict)
	jl_function_t* keys_fn = jl_get_function(jl_base_module, "keys");
	jl_value_t* keyset = jl_call1(keys_fn, dict);

	// Convert KeySet -> Array
	jl_function_t* collect_fn = jl_get_function(jl_base_module, "collect");
	jl_array_t* keys = (jl_array_t*)jl_call1(collect_fn, keyset);

	if (jl_exception_occurred())
	{
		std::println(std::cerr, "Error getting keys: {}", jl_typeof_str(jl_exception_occurred()));
		return;
	}

	// Now it’s safe to iterate
	size_t len = jl_array_len(keys);
	jl_value_t** data = jl_array_data(keys, jl_value_t*);

	for (size_t i = 0; i < len; i++)
	{
		jl_value_t* key = data[i];
		if (jl_is_string(key))
		{
			std::println("{}", jl_string_ptr(key));
		}
	}

	Julia::call_function(lib, "GameA._process", 0.016);
	Julia::call_function(lib, "GameB._process", 0.016);

	lib.unload();
}

void julia_engine_impl()
{
	Julia::init();
	Julia::load_script("scripts/main.jl");

	auto dict = Julia::call_function("Dyno.Dispatcher", "get_registry", nullptr);

	jl_function_t* keys_fn = jl_get_function(jl_base_module, "keys");
	jl_value_t* keyset = jl_call1(keys_fn, dict);

	// Convert KeySet -> Array
	jl_function_t* collect_fn = jl_get_function(jl_base_module, "collect");
	jl_array_t* keys = (jl_array_t*)jl_call1(collect_fn, keyset);

	if (jl_exception_occurred())
	{
		std::println(std::cerr, "Error getting keys: {}", jl_typeof_str(jl_exception_occurred()));
		return;
	}

	// Now it’s safe to iterate
	size_t len = jl_array_len(keys);
	jl_value_t** data = jl_array_data(keys, jl_value_t*);

	for (size_t i = 0; i < len; i++)
	{
		jl_value_t* key = data[i];
		if (jl_is_string(key))
		{
			std::println("{}", jl_string_ptr(key));
		}
	}

	auto args = std::vector{Julia::box_arg(23.0)};
	Julia::call_function("GameA", "_process", &args);

	Julia::shutdown();
}

int main()
{
	std::filesystem::current_path(WorkingDirectory);

	dynamic_library_impl();
	//julia_engine_impl();
}
