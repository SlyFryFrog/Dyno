module;
#include <dlfcn.h>
#include <stdexcept>
#include <string>
module dyno;

namespace Dyno
{
	Dylib::Dylib(const std::string& path)
	{
		std::string platform_path = path;

		// If no absolut path or verbose relative path is given, prepend './'
		if (!platform_path.starts_with("/") && !platform_path.starts_with("./"))
		{
			platform_path = "./" + platform_path;
		}

		// Append the appropriate lib format
#ifdef WIN32
		platform_path += ".dll";
#elif __APPLE__
		platform_path += ".dylib";
#else
	platform_path += ".so";
#endif

		m_handle = dlopen(platform_path.c_str(), RTLD_LAZY);
		if (!m_handle)
		{
			throw std::runtime_error(std::string("Failed to load library '") + platform_path + "': " + dlerror());
		}
	}

	Dylib::~Dylib()
	{
		if (m_handle)
		{
			dlclose(m_handle);
		}
	}

	void Dylib::unload() const
	{
		if (m_handle)
		{
			dlclose(m_handle);
		}
	}
} // namespace Dyno
