module;
#include <julia.h>
#include <string>
#include <vector>
export module dyno:julia_engine;

import :dylib;
import :julia_types;

export namespace Dyno::Julia
{
	void init();

	void shutdown();

	void load_script(const std::string& path);

	jl_value_t* call_function(const std::string& mod, const std::string& func,
									 std::vector<jl_value_t*>* args);

	template <typename... Args>
	jl_value_t* call_function(Dylib& lib, const std::string& name, Args... args)
	{
		using generic_call_t = jl_value_t* (*)(const char*, jl_value_t**, int);
		const auto generic_call = lib.load<generic_call_t>("generic_call");

		std::vector<jl_value_t*> julia_args = {box_arg(args)...};  // empty if no args

		return generic_call(name.c_str(), julia_args.data(), static_cast<int>(julia_args.size()));
	}


	jl_value_t* call_function(Dylib& lib, const std::string& name, std::vector<jl_value_t*>& args);


	std::string get_concrete_type_name(jl_value_t* variable);

	std::string get_immediate_supertype_name(jl_value_t* variable);
} // namespace Julia
