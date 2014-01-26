#include "ldb_EntityTitle.h"


namespace LiquidDb {

EntityTitle::EntityTitle(const ::EFC::String &string)
{
    if (!string.empty())
    {
        const char *str = string.c_str();
        const char *start = string.c_str();
        Token::Type type = Token::Text;

        while (*str)
            if (*str == '$' && str[1] == '{')
            {
                if (type == Token::Property)
                {
                    m_items.clear();
                    return;
                }

                if (str - start > 0)
                    if (UNLIKELY(m_items.push_back({ type, ::EFC::String(start, str - start) }) == false))
                    {
                        m_items.clear();
                        return;
                    }

                type = Token::Property;
                start = (str += 2);
            }
            else
                if (*str == '}' && type == Token::Property)
                {
                    if (str - start > 0)
                        if (UNLIKELY(m_items.push_back({ type, ::EFC::String(start, str - start) }) == false))
                        {
                            m_items.clear();
                            return;
                        }

                    type = Token::Text;
                    start = (++str);
                }
                else
                    ++str;

        if (str - start > 0 && type == Token::Text)
            if (UNLIKELY(m_items.push_back({ Token::Text, ::EFC::String(start, str - start) }) == false))
            {
                m_items.clear();
                return;
            }
    }
}

}
