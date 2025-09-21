module;
#include <dlfcn.h>
#include <iostream>
#include <stdexcept>
#include <string>
export module dyno:dylib;

namespace Dyno
{
	export class Dylib
	{
		void* m_handle;

	public:
		explicit Dylib(const std::string& path);

		~Dylib();

		template <typename T>
		T load(const std::string& name)
		{
			dlerror(); // clear previous errors
			void* sym = dlsym(m_handle, name.c_str());
			if (const char* err = dlerror())
			{
				throw std::runtime_error("Failed to load symbol '" + name +
										 "': " + err);
			}
			if (!sym)
			{
				throw std::runtime_error("Symbol '" + name + "' is null");
			}

			return reinterpret_cast<T>(sym);
		}

		void unload() const;
	};
} // namespace Dyno
