/** @file ldb_UndoStack.h
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

#ifndef LDB_UNDOSTACK_H_
#define LDB_UNDOSTACK_H_

#include <efc/Map>
#include <efc/List>
#include <efc/String>
#include <efc/Variant>
#include <efc/SharedPointer>
#include <liquiddb/Entity>
#include <liquiddb/EntityValue>


namespace LiquidDb {

class UndoStack
{
public:
    class Command;

    typedef ::EFC::SharedPointer<Command>         Holder;
    typedef ::EFC::List<Holder>                   List;
    typedef ::EFC::List<List>                     Stack;
    typedef ::EFC::Map<Entity::Id, ::EFC::String> Names;

public:
    UndoStack();
    ~UndoStack();

    bool transaction();
    bool commit();
    void rollback();

    void undoAddEntity(const Entity &entity);
    void undoRemoveEntity(const Entity &entity, Names &names);
    void undoAddProperty(const Entity &entity, const Entity &property);
    void undoRenameProperty(const Entity &entity, const Entity &property, ::EFC::String &name);
    void undoRemoveProperty(const Entity &entity, const Entity &property, ::EFC::String &name);
    void undoAddValue(const EntityValue &entityValue, const EntityValue &propertyValue);
    void undoAddValue(const EntityValue &entityValue, const EntityValue::List &propertyValues);
    void undoUpdateValue(const EntityValue &entityValue, ::EFC::Variant &value);
    void undoRemoveValue(const EntityValue &entityValue, const EntityValue &propertyValue);

private:
    Stack m_stack;
};

}

#endif /* LDB_UNDOSTACK_H_ */
