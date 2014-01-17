/** @file ldb_Storage.h
 *  @brief TODO: Put description here.
 *
 * TODO: Put description here.
 *
 * @copyright
 * This file is part of liquiddb.
 * @n@n
 * Copyright (C) 2011-2013 Dmitriy Vilkov, <dav.daemon@gmail.com>
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
#ifndef LDB_STORAGE_H_
#define LDB_STORAGE_H_

#include <efc/Map>
#include <liquiddb/Database>
#include <liquiddb/EntityValue>


namespace LiquidDb {

class SelectEntity;
class EntityValueReader;


class Storage
{
public:
    Storage(const char *fileName, bool create);

    bool isValid() const { return m_database.lastError() == 0; }

    EntityValueReader perform(const SelectEntity &query);

    Entity createEntity(Entity::Type type, const char *name, const char *title);
    bool removeEntity(const Entity &entity);

    bool addProperty(const Entity &entity, const Entity &property, const char *name);
    bool renameProperty(const Entity &entity, const Entity &property, const char *name);
    bool removeProperty(const Entity &entity, const Entity &property);

    EntityValue addValue(const Entity &entity);
    bool addValue(const EntityValue &entityValue, const EntityValue &propertyValue);
    bool addValue(const EntityValue &entityValue, const EntityValue::List &propertyValues);
    EntityValue addValue(const Entity &entity, const ::EFC::Variant &value);
    bool updateValue(const EntityValue &value, const ::EFC::Variant &newValue);
    bool removeValue(const Entity &entity, const Entity::IdsList &ids);
    bool removeValue(const EntityValue &entityValue, const EntityValue &propertyValue);

private:
    bool loadEntities();
    bool loadProperties();
    bool loadMetaProperties();

private:
    bool isThereCycles(const Entity &entity, const Entity &property) const;
    bool removeEntityValue(const Entity &entity, Entity::Id id);
    bool removeEntityValues(const Entity &entity, const Entity::IdsList &ids);
    bool removeOverlappingIds(const Entity &entity, const Entity &property, Entity::IdsSet &ids);
    bool removeSelfOverlappingIds(const Entity &entity, const Entity::IdsList &entityIds, const Entity &property, Entity::IdsSet &propertyIds);
    bool cleanupParentsValues(const Entity &entity);
    bool cleanupParentsValues(const Entity &entity, const Entity::IdsList &ids);
    bool cleanupPropertyValues(const Entity &entity);
    bool cleanupPropertyValues(const Entity &entity, const Entity::IdsList &ids);
    bool cleanupPropertyValues(const Entity &entity, const Entity &property);

private:
    typedef ::EFC::Map<Entity::Id, Entity> Entities;

private:
    Entities m_entities;
    Database m_database;
};

}

#endif /* LDB_STORAGE_H_ */
