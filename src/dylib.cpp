module;
#include <dlfcn.h>
#include <string>
module dyno;

namespace Dyno
{
	Dylib::Dylib(const std::string& path)
	{
		m_handle = dlopen(path.c_str(), RTLD_LAZY);
		if (!m_handle)
		{
			throw std::runtime_error(dlerror());
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
