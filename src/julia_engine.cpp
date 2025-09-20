module;
#include <filesystem>
#include <iostream>
#include <jlcxx/jlcxx.hpp>
#include <julia.h>
#include <string>
#include <vector>
module dyno;

namespace Dyno::Julia
{
	void init()
	{
		jl_init();
	}

	void shutdown()
	{
		jl_atexit_hook(0);
	}

	void load_script(const std::string& path)
	{
		jl_eval_string(("Base.include(Main, \"" + path + "\")").c_str());
	}

	jl_value_t* call_function(const std::string& mod, const std::string& func,
							  std::vector<jl_value_t*>* args)
	{
		// Fetch module safely from Main
		jl_value_t* maybe_mod = jl_eval_string(("Main." + mod).c_str());

		if (!maybe_mod || !jl_is_module(maybe_mod))
		{
			jl_printf(jl_stderr_stream(), "Failed to find module: %s\n", mod.c_str());
			return nullptr;
		}

		auto* jl_mod = reinterpret_cast<jl_module_t*>(maybe_mod);
		jl_function_t* jl_func = jl_get_function(jl_mod, func.c_str());

		if (!jl_func)
		{
			jl_printf(jl_stderr_stream(), "Function `%s` not found in module `%s`\n", func.c_str(),
					  mod.c_str());
			return nullptr;
		}

		jl_value_t* result = nullptr;

		JL_TRY
		{
			if (args && !args->empty())
			{
				result = jl_call(jl_func, args->data(), args->size());
			}
			else
			{
				result = jl_call0(jl_func);
			}

			if (jl_exception_occurred())
			{
				jl_printf(jl_stderr_stream(), "Julia exception in `%s.%s`: %s\n", mod.c_str(),
						  func.c_str(), jl_typeof_str(jl_exception_occurred()));
				// Get exception thrown in Julia script
				jl_call2(jl_get_function(jl_base_module, "showerror"), jl_stderr_obj(),
						 jl_exception_occurred());
				jl_printf(jl_stderr_stream(), "\n");
				result = nullptr;
			}
		}
		JL_CATCH
		{
			jl_printf(jl_stderr_stream(), "Caught low-level Julia exception in `%s.%s`\n",
					  mod.c_str(), func.c_str());
			result = nullptr;
		}

		return result;
	}

	jl_value_t* call_function(Dyno::Dylib& lib, const std::string& name,
							  std::vector<jl_value_t*>& args)
	{
		using generic_call_t = jl_value_t* (*)(const char*, jl_value_t**, int);
		const auto generic_call = lib.load<generic_call_t>("generic_call");

		return generic_call(name.c_str(), args.data(), args.size());
	}

	std::string get_concrete_type_name(jl_value_t* variable)
	{
		jl_value_t* type = jl_typeof(variable);
		const auto* datatype = reinterpret_cast<jl_datatype_t*>(type);
		return jl_symbol_name(datatype->name->name);
	}

	std::string get_immediate_supertype_name(jl_value_t* variable)
	{
		jl_value_t* type = jl_typeof(variable);
		jl_function_t* supertype_fn = jl_get_function(jl_base_module, "supertype");
		jl_value_t* result = jl_call1(supertype_fn, type);

		const auto* datatype = reinterpret_cast<jl_datatype_t*>(result);
		return jl_symbol_name(datatype->name->name);
	}
} // namespace Julia
