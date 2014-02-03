/** @file ldb_EntityValueReader.h
 *  @brief TODO: Put description here.
 *
 * TODO: Put description here.
 *
 * @copyright
 * This file is part of liquiddb.
 * @n@n
 * Copyright (C) 2011-2014 Dmitriy Vilkov, <dav.daemon@gmail.com>
 * @n@n
 * liquiddb is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * @n@n
 * liquiddb is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * @n@n
 * You should have received a copy of the GNU General Public License
 * along with liquiddb. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LDB_ENTITYVALUEREADER_H_
#define LDB_ENTITYVALUEREADER_H_

#include <efc/SharedPointer>
#include <liquiddb/Query>
#include <liquiddb/DataSet>
#include <liquiddb/EntityValue>


namespace LiquidDb {

class EntityValueReader
{
public:
    EntityValueReader();
    ~EntityValueReader();

    bool isValid() const;

    const Entity &entity() const;
    EntityValue next();
    bool eof() const;
    bool bof() const;
    void refresh();
    void close();

private:
    class Implementation;
    class CompositeProperty;
    class SimpleProperty;
    class Property;
    class Context;

    typedef ::EFC::SharedPointer<Implementation> Holder;
    typedef ::EFC::ScopedPointer<Context> ContextHolder;
    typedef ::EFC::ScopedPointer<Property> PropertyHolder;
    typedef ::EFC::List<PropertyHolder> PropertiesList;

private:
    friend class Storage;
    EntityValueReader(Holder &holder);
    EntityValueReader(Implementation *implementation);

private:
    Holder m_implementation;
};

}

#endif /* LDB_ENTITYVALUEREADER_H_ */
