#include "util/string_ops.h"

namespace ppp
{
    namespace string
    {
		std::string string_replace(const std::string& subject, const std::string& search, const std::string& replace)
		{
			std::string result(subject);

			size_t pos = 0;

			while ((pos = subject.find(search, pos)) != std::string::npos)
			{
				result.replace(pos, search.length(), replace);
				pos += search.length();
			}

			return result;
		}
    }
}