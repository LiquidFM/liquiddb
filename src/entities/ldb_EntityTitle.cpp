#include "ldb_EntityTitle.h"


namespace LiquidDb {

EntityTitle::EntityTitle(const char *format)
{
    if (format)
    {
        const char *start = format;
        Token::Type type = Token::Text;

        while (*format)
            if (*format == '$' && format[1] == '{')
            {
                if (type == Token::Property)
                {
                    m_items.clear();
                    return;
                }

                if (format - start > 0)
                    if (UNLIKELY(m_items.push_back({ type, ::EFC::String(start, format - start) }) == false))
                    {
                        m_items.clear();
                        return;
                    }

                type = Token::Property;
                start = (format += 2);
            }
            else
                if (*format == '}' && type == Token::Property)
                {
                    if (format - start > 0)
                        if (UNLIKELY(m_items.push_back({ type, ::EFC::String(start, format - start) }) == false))
                        {
                            m_items.clear();
                            return;
                        }

                    type = Token::Text;
                    start = (++format);
                }
                else
                    ++format;

        if (format - start > 0 && type == Token::Text)
            if (UNLIKELY(m_items.push_back({ Token::Text, ::EFC::String(start, format - start) }) == false))
            {
                m_items.clear();
                return;
            }
    }
}

}
