/**
 * This file is part of QFM.
 *
 * Copyright (C) 2011-2012 Dmitriy Vilkov, <dav.daemon@gmail.com>
 *
 * QFM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QFM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with QFM. If not, see <http://www.gnu.org/licenses/>.
 */
#include "ldb_DataSet.h"
#include "ldb_conversions_p.h"

#include <cstddef>


namespace LiquidDb {

DataSet::Columns::const_iterator::const_iterator() :
    m_value({ 0, NULL, Query::Fields::const_iterator() })
{}

DataSet::Columns::const_iterator::const_iterator(const DataSet &dataSet, const Query::Fields::const_iterator &iterator) :
    m_value({ 0, &dataSet, iterator })
{}

DataSet::DataSet() :
	m_error(0),
	m_statement(NULL)
{}

#if PLATFORM_COMPILER_SUPPORTS(MOVE_SEMANTIC)
DataSet::DataSet(DataSet &&other) :
    m_error(other.m_error),
    m_statement(std::move(other.m_statement)),
    m_columns(*this, std::move(other.m_columns))
{
    other.m_statement = NULL;
}
#endif

DataSet::~DataSet()
{
    sqlite3_finalize(m_statement);
}

bool DataSet::isValid() const
{
	return m_statement != NULL;
}

bool DataSet::next()
{
	return (m_error = sqlite3_step(m_statement)) == SQLITE_ROW;
}

void DataSet::reset()
{
    m_error = sqlite3_reset(m_statement);
}

bool DataSet::initialize(sqlite3_stmt *statement, const Query::Fields &fields)
{
	ASSERT(statement != NULL);

	m_error = SQLITE_OK;
	sqlite3_finalize(m_statement);
	m_statement = statement;

	m_columns = std::move(Columns(*this, fields));

	return true;
}

bool DataSet::columnIsNull(Table::Column::Id column) const
{
    return sqlite3_column_type(m_statement, column) == SQLITE_NULL;
}

void DataSet::columnValue(Table::Column::Id column, Table::Column::Type type, void *value) const
{
    size_t size;
    columnValueInternal(column, type, &value, size);
}

void DataSet::columnValue(Table::Column::Id column, Table::Column::Type type, const void **value, size_t &size) const
{
    columnValueInternal(column, type, const_cast<void **>(value), size);
}

void DataSet::columnValueInternal(Table::Column::Id column, Table::Column::Type type, void **value, size_t &size) const
{
	ASSERT(value != NULL);
	size = 0;

	switch (type)
	{
		case Table::Column::Int:
		case Table::Column::MediumInt:
			*static_cast<uint32_t *>(*value) = sqlite3_column_int(m_statement, column);
			break;

		case Table::Column::TinyInt:
			*static_cast<uint8_t *>(*value) = sqlite3_column_int(m_statement, column);
			break;

		case Table::Column::SmallInt:
			*static_cast<uint16_t *>(*value) = sqlite3_column_int(m_statement, column);
			break;

		case Table::Column::BigInt:
			*static_cast<uint64_t *>(*value) = sqlite3_column_int64(m_statement, column);
			break;

		case Table::Column::Text:
			*reinterpret_cast<const unsigned char **>(const_cast<const void **>(value)) = sqlite3_column_text(m_statement, column);
			size = sqlite3_column_bytes(m_statement, column);
			break;

		case Table::Column::Real:
		case Table::Column::Double:
			*static_cast<double *>(*value) = sqlite3_column_double(m_statement, column);
			break;

		case Table::Column::Float:
			*static_cast<float *>(*value) = sqlite3_column_double(m_statement, column);
			break;

		case Table::Column::Date:
		case Table::Column::Time:
		case Table::Column::DateTime:
			*static_cast<uint64_t *>(*value) = sqlite3_column_int64(m_statement, column);
			break;

		case Table::Column::Boolean:
			*static_cast<bool *>(*value) = sqlite3_column_int(m_statement, column);
			break;

		case Table::Column::Blob:
			*const_cast<const void **>(value) = sqlite3_column_blob(m_statement, column);
			size = sqlite3_column_bytes(m_statement, column);
			break;

		default:
			break;
	}
}

}
