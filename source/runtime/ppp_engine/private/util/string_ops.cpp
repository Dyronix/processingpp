#include "util/string_ops.h"

#include <unordered_set>

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

        std::vector<std::string> split_string(const std::string& str, const std::string& delimiters) 
        {
            std::vector<std::string> tokens;
            std::string token;

            std::unordered_set<char> delimiter_set(delimiters.begin(), delimiters.end());

            for (char ch : str) 
            {
                if (delimiter_set.count(ch))
                {
                    // If the character is a delimiter, finalize the current token
                    if (!token.empty()) 
                    {
                        tokens.push_back(token);
                        token.clear();
                    }
                }
                else 
                {
                    // If it's not a delimiter, add the character to the current token
                    token += ch;
                }
            }

            // Add the last token if it exists
            if (!token.empty()) 
            {
                tokens.push_back(token);
            }

            return tokens;
        }
    }
}