#ifndef LDB_ENTITYTITLE_H_
#define LDB_ENTITYTITLE_H_

#include <efc/List>
#include <efc/String>


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
        Token(Type type, const ::EFC::String &string) :
            m_type(type),
            m_string(string)
        {}

        Type type() const { return m_type; }
        const ::EFC::String &string() const { return m_string; }

    private:
        Type m_type;
        ::EFC::String m_string;
    };

    typedef ::EFC::List<Token>        Container;
    typedef Container::const_iterator const_iterator;

public:
    EntityTitle(const char *format);

    bool isValid() const;

    const_iterator begin() const { return m_items.begin(); }
    const_iterator end() const { return m_items.end(); }

private:
    void dollarToken(::EFC::String::size_type &pos, ::EFC::String &token, const ::EFC::String &source);
    void nameToken(::EFC::String::size_type &pos, ::EFC::String &token, const ::EFC::String &source);

private:
    Container m_items;
};

}

#endif /* LDB_ENTITYTITLE_H_ */
