/** @file ldb_UndoStack.cpp
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

#include "ldb_UndoStack.h"


namespace LiquidDb {

class UndoStack::Command
{
public:
    Command()
    {}
};


UndoStack::UndoStack()
{}

UndoStack::~UndoStack()
{}

bool UndoStack::transaction()
{
    return false;
}

bool UndoStack::commit()
{
    return false;
}

void UndoStack::rollback()
{

}

void UndoStack::undoAddEntity(const Entity &entity)
{

}

void UndoStack::undoRemoveEntity(const Entity &entity)
{

}

void UndoStack::undoAddProperty(const Entity &entity, const Entity &property)
{

}

void UndoStack::undoRenameProperty(const Entity &entity, const Entity &property, ::EFC::String &name)
{

}

void UndoStack::undoRemoveProperty(const Entity &entity, const Entity &property, ::EFC::String &name)
{

}

void UndoStack::undoAddValue(const EntityValue &entityValue, const EntityValue &propertyValue)
{

}

void UndoStack::undoAddValue(const EntityValue &entityValue, const EntityValue::List &propertyValues)
{

}

void UndoStack::undoUpdateValue(const EntityValue &entityValue, ::EFC::Variant &value)
{

}

void UndoStack::undoRemoveValue(const EntityValue &entityValue, const EntityValue &propertyValue)
{

}

}
