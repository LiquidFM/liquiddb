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

DataSet::DataSet() :
	m_error(0),
	m_statement(NULL)
{}

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

void DataSet::columnValue(const Table::Column *column, void *value) const
{
	size_t size;
	columnValueInternal(column, &value, size);
}

void DataSet::columnValue(const Table::Column *column, const void **value, size_t &size) const
{
	columnValueInternal(column, const_cast<void **>(value), size);
}

bool DataSet::initialize(sqlite3_stmt *statement, const Query::Fields &fields)
{
	ASSERT(statement != NULL);

	m_error = SQLITE_OK;
	sqlite3_finalize(m_statement);
	m_statement = statement;

	int number = 0;
	for (Query::Fields::const_iterator i = fields.begin(), end = fields.end(); i != end; ++i, ++number)
		m_fields[(*i).column] = number;

	return true;
}

void DataSet::columnValueInternal(const Table::Column *column, void **value, size_t &size) const
{
	ASSERT(value != NULL);
	ASSERT(column != NULL);
	Map::const_iterator number = m_fields.find(column);
	ASSERT(number != m_fields.end());

	size = 0;

	switch (column->type)
	{
		case Table::Column::Int:
		case Table::Column::MediumInt:
			*static_cast<uint32_t *>(*value) = sqlite3_column_int(m_statement, (*number).second);
			break;

		case Table::Column::TinyInt:
			*static_cast<uint8_t *>(*value) = sqlite3_column_int(m_statement, (*number).second);
			break;

		case Table::Column::SmallInt:
			*static_cast<uint16_t *>(*value) = sqlite3_column_int(m_statement, (*number).second);
			break;

		case Table::Column::BigInt:
			*static_cast<uint64_t *>(*value) = sqlite3_column_int64(m_statement, (*number).second);
			break;

		case Table::Column::Text:
			*reinterpret_cast<const unsigned char **>(const_cast<const void **>(value)) = sqlite3_column_text(m_statement, (*number).second);
			size = sqlite3_column_bytes(m_statement, (*number).second);
			break;

		case Table::Column::Real:
		case Table::Column::Double:
			*static_cast<double *>(*value) = sqlite3_column_double(m_statement, (*number).second);
			break;

		case Table::Column::Float:
			*static_cast<float *>(*value) = sqlite3_column_double(m_statement, (*number).second);
			break;

		case Table::Column::Date:
		case Table::Column::Time:
		case Table::Column::DateTime:
			*static_cast<uint64_t *>(*value) = sqlite3_column_int64(m_statement, (*number).second);
			break;

		case Table::Column::Boolean:
			*static_cast<bool *>(*value) = sqlite3_column_int(m_statement, (*number).second);
			break;

		case Table::Column::Blob:
			*const_cast<const void **>(value) = sqlite3_column_blob(m_statement, (*number).second);
			size = sqlite3_column_bytes(m_statement, (*number).second);
			break;

		default:
			break;
	}
}

}
