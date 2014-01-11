#include "ldb_EntityTitle.h"


namespace LiquidDb {

EntityTitle::EntityTitle(const char *format)
{
//    ::EFC::String token;
//    token.reserve(m_format.size());
//
//    for (::EFC::String::size_type pos = 0; pos < m_format.size(); ++pos)
//        if (m_format.at(pos) == '$')
//            dollarToken(pos, token, m_format);
//        else
//            token.append(m_format.at(pos));
//
//    if (!token.isEmpty())
//    {
//        token.truncate(token.size());
//        m_items.push_back(Token(Token::Text, token));
//    }
}

bool EntityTitle::isValid() const
{
    return !m_items.empty();
}

void EntityTitle::dollarToken(::EFC::String::size_type &pos, ::EFC::String &token, const ::EFC::String &source)
{
//    if (pos + 1 < source.size())
//        if (source.at(pos + 1) == '{')
//            nameToken(++pos, token, source);
//        else
//            token.append(source.at(pos));
//    else
//        token.append(source.at(pos));
}

void EntityTitle::nameToken(::EFC::String::size_type &pos, ::EFC::String &token, const ::EFC::String &source)
{
//    if (pos + 2 < source.size())
//    {
//        ::EFC::String name;
//
//        for (++pos; pos < source.size(); ++pos)
//            if (source[pos] == '}')
//            {
//                if (!name.isEmpty())
//                {
//                    token.truncate(token.size());
//                    m_items.push_back(Token(Token::Text, token));
//                    m_items.push_back(Token(Token::Property, name));
//
//                    token.clear();
//                    token.reserve(source.size());
//                }
//
//                break;
//            }
//            else
//                name.append(source.at(pos));
//    }
//    else
//        token.append(source.at(pos));
}

}
