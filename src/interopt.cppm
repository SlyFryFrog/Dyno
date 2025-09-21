module;
#include <vector>
export module dyno:interopt;

export namespace Dyno
{
	template <typename... Args>
	std::vector<uint8_t> pack_args(const Args&... args)
	{
		std::vector<uint8_t> buffer((sizeof(args) + ...));
		size_t offset = 0; // Used to calculate offset of each arg
		auto pack_one = [&](auto&& val)
		{
			std::memcpy(buffer.data() + offset, &val, sizeof(val));
			offset += sizeof(val);
		};
		(pack_one(args), ...);
		return buffer;
	}
} // namespace Dyno
