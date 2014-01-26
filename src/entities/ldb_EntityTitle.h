#ifndef LDB_ENTITYTITLE_H_
#define LDB_ENTITYTITLE_H_

#include <efc/List>
#include <efc/String>
#include <platform/platform.h>


namespace LiquidDb {

class EntityTitle
{
public:
    class Token
    {
    public:
        enum Type
        {
            Text,
            Property
        };

    public:
#if PLATFORM_COMPILER_SUPPORTS(MOVE_SEMANTIC)
        Token(Type type, ::EFC::String &&string) :
            m_type(type),
            m_string(std::move(string))
        {}
#else
        Token(Type type, const ::EFC::String &string) :
            m_type(type),
            m_string(string)
        {}
#endif

        Type type() const { return m_type; }
        const ::EFC::String &string() const { return m_string; }

    private:
        Type m_type;
        ::EFC::String m_string;
    };

    typedef ::EFC::List<Token>        Container;
    typedef Container::const_iterator const_iterator;

public:
    EntityTitle(const ::EFC::String &format);

    bool isValid() const { return !m_items.empty(); }

    const_iterator begin() const { return m_items.begin(); }
    const_iterator end() const { return m_items.end(); }

private:
    Container m_items;
};

}

#endif /* LDB_ENTITYTITLE_H_ */
