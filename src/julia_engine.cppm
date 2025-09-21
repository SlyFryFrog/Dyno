module;
#include <julia.h>
#include <ostream>
#include <regex>
#include <string>
#include <vector>
export module dyno:julia_engine;

import :julia_types;

export namespace Dyno::Julia
{
	void init();

	void shutdown();

	void load_script(const std::string& path);

	jl_value_t* call_function(const std::string& mod, const std::string& func,
							  std::vector<jl_value_t*>* args);
} // namespace Dyno::Julia
