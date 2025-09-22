module;
#include <array>
#include <deque>
#include <julia.h>
#include <list>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
export module dyno:julia_types;

export namespace Dyno::Julia
{
	jl_value_t* box_arg(const bool arg)
	{
		return jl_box_bool(arg);
	}

	jl_value_t* box_arg(const char arg)
	{
		return jl_box_int8(arg);
	}

	jl_value_t* box_arg(const unsigned char arg)
	{
		return jl_box_uint8(arg);
	}

	jl_value_t* box_arg(const short arg)
	{
		return jl_box_int16(arg);
	}

	jl_value_t* box_arg(const unsigned short arg)
	{
		return jl_box_uint16(arg);
	}

	jl_value_t* box_arg(const int arg)
	{
		return jl_box_int32(arg);
	}

	jl_value_t* box_arg(const unsigned int arg)
	{
		return jl_box_uint32(arg);
	}

	jl_value_t* box_arg(const long arg)
	{
		return jl_box_int64(arg);
	}

	jl_value_t* box_arg(const unsigned long arg)
	{
		return jl_box_uint64(arg);
	}

	jl_value_t* box_arg(const long long arg)
	{
		return jl_box_int64(arg);
	}

	jl_value_t* box_arg(const unsigned long long arg)
	{
		return jl_box_uint64(arg);
	}

	jl_value_t* box_arg(const float arg)
	{
		return jl_box_float32(arg);
	}

	jl_value_t* box_arg(const double arg)
	{
		return jl_box_float64(arg);
	}


	jl_value_t* box_arg(const std::string& arg)
	{
		return jl_cstr_to_string(arg.c_str());
	}

	jl_value_t* box_arg(const char* arg)
	{
		return jl_cstr_to_string(arg);
	}


	template <typename T>
	jl_value_t* box_arg(const std::vector<T>& vec)
	{
		jl_value_t* array_type = jl_apply_array_type((jl_value_t*)jl_typeof(box_arg(T())), 1);
		jl_array_t* julia_array = jl_alloc_array_1d(array_type, vec.size());
		auto* data = (T*)jl_array_data(julia_array, T); // pass T here
		std::copy(vec.begin(), vec.end(), data);
		return (jl_value_t*)julia_array;
	}

	template <typename T>
	jl_value_t* box_arg(const std::list<T>& lst)
	{
		return box_arg(std::vector<T>(lst.begin(), lst.end()));
	}

	template <typename T>
	jl_value_t* box_arg(const std::deque<T>& dq)
	{
		return box_arg(std::vector<T>(dq.begin(), dq.end()));
	}

	template <typename T>
	jl_value_t* box_arg(const std::set<T>& st)
	{
		return box_arg(std::vector<T>(st.begin(), st.end()));
	}

	template <typename K, typename V>
	jl_value_t* box_arg(const std::unordered_map<K, V>& m)
	{
		return box_arg(std::map<K, V>(m.begin(), m.end()));
	}

	template <typename T>
	jl_value_t* box_arg(const T&)
	{
		throw std::runtime_error("Unsupported type for Julia boxing");
	}
} // namespace Dyno
