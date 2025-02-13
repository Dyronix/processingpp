#include "string/string_entry.h"

namespace ppp
{
    namespace string
    {
        //-------------------------------------------------------------------------
        string_entry::string_entry(std::string_view characters) noexcept
            : m_characters()
            , m_size(characters.size() + 1)
        {
            m_characters = std::make_unique<char[]>(m_size); // NOLINT(modernize-avoid-c-arrays)

            std::memcpy(m_characters.get(), characters.data(), characters.size());
            m_characters[m_size] = 0;
        }
        //-------------------------------------------------------------------------
        string_entry::string_entry(string_entry&& other) noexcept
            : m_characters(std::exchange(other.m_characters, {}))
            , m_size(std::exchange(other.m_size, {}))
        {
        }

        //-------------------------------------------------------------------------
        string_entry::~string_entry() = default;

        //-------------------------------------------------------------------------
        string_entry& string_entry::operator=(string_entry&& other) noexcept
        {
            m_characters = std::exchange(other.m_characters, {});

            return *this;
        }

        //-------------------------------------------------------------------------
        bool string_entry::is_valid() const
        {
            return m_characters && m_size != 0;
        }

        //-------------------------------------------------------------------------
        std::string_view string_entry::characters() const
        {
            return std::string_view(m_characters.get(), m_size);
        }

        //-------------------------------------------------------------------------
        s32 string_entry::size() const
        {
            return m_size;
        }
    }
}