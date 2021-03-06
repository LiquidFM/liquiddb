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
#ifndef LDB_CONVERSIONS_P_H_
#define LDB_CONVERSIONS_P_H_

#include "ldb_Database.h"
#include "ldb_Query.h"
#include "ldb_Table.h"
#include "ldb_Field.h"
#include "ldb_DataSet.h"
#include "ldb_Value.h"

#include <brolly/assert.h>
#include <efc/ScopedPointer>
#include <sqlite3.h>

#include <cstdlib>
#include <cstdio>
#include <cstdint>

#if PLATFORM_COMPILER(MSVC)
// MSVC doesn't support C99: snprintf is a part of C99
#define snprintf _snprintf
#endif


enum { BufferSize = 65536 };


namespace {
	using namespace ::LiquidDb;

	const char *typeToString(Table::Column::Type type)
	{
		switch (type)
		{
			case Table::Column::Int:
			case Table::Column::TinyInt:
			case Table::Column::SmallInt:
			case Table::Column::MediumInt:
			case Table::Column::BigInt:
				return "INTEGER";

			case Table::Column::Text:
				return "TEXT";

			case Table::Column::Real:
			case Table::Column::Double:
			case Table::Column::Float:
				return "REAL";

			case Table::Column::Date:
			case Table::Column::Time:
			case Table::Column::DateTime:
			case Table::Column::Boolean:
				return "NUMERIC";

			case Table::Column::Blob:
				return "BLOB";

			default:
				return "INT";
		}
	}

	int printValue(char *buffer, size_t size, const Table::Column *column, const Value *value)
	{
	    if (value == NULL)
            return snprintf(buffer, size, "NULL");

		switch (column->type)
		{
			case Table::Column::Int:
			case Table::Column::MediumInt:
			case Table::Column::TinyInt:
            case Table::Column::Boolean:
				return snprintf(buffer, size, "%d", value->i32());

			case Table::Column::SmallInt:
				return snprintf(buffer, size, "%hd", value->i16());

			case Table::Column::BigInt:
				return snprintf(buffer, size, "%" PRId64, value->i64());

			case Table::Column::Text:
				return snprintf(buffer, size, "'%s'", value->str());

			case Table::Column::Real:
			case Table::Column::Double:
				return snprintf(buffer, size, "%f", value->dbl());

			case Table::Column::Float:
				return snprintf(buffer, size, "%f", value->flt());

			case Table::Column::Date:
			case Table::Column::Time:
			case Table::Column::DateTime:
				return snprintf(buffer, size, "%" PRIu64, value->u64());

			case Table::Column::Blob:
			{
			    ::EFC::ScopedPointer<char> string(new (std::nothrow) char [value->size() * 2 + 1]);

				if (LIKELY(string != NULL))
				{
					size_t i;
					size_t j;
					const char *val = value->str();

					for (i = 0, j = 0; i < value->size(); ++i, j += 2)
					{
						unsigned char token = (val[i] >> (CHAR_BIT / 2)) & 0x0F;

						if (token <= 9)
							string.get()[j] = '0' + token;
						else
							string.get()[j] = token + 'A' - 10;

						token = val[i] & 0x0F;

						if (token <= 9)
							string.get()[j + 1] = '0' + token;
						else
							string.get()[j + 1] = token + 'A' - 10;

						string.get()[j + 2] = 0;
					}

					return snprintf(buffer, size, "X'%s'", string.get());
				}

				return snprintf(buffer, size, "NULL");
			}

			default:
				return snprintf(buffer, size, "NULL");
		}
	}

	class TransactionScope
	{
	public:
		TransactionScope(Database &db, sqlite3 *rawDb) :
			m_db(db),
			m_rawDb(rawDb),
			m_commited(false),
			m_transaction(m_db.transaction())
		{
			ASSERT(m_rawDb != NULL);
		}

		~TransactionScope()
		{
			if (!m_commited && m_transaction)
			{
				sqlite3_stmt *statement;

				if (sqlite3_prepare_v2(m_rawDb, "ROLLBACK", sizeof("ROLLBACK") - 1, &statement, NULL) == SQLITE_OK)
				{
					sqlite3_step(statement);
					sqlite3_finalize(statement);
				}
			}
		}

		bool isValid() const { return m_transaction; }

		bool commit()
		{
			return m_commited = m_db.commit();
		}

	private:
		Database &m_db;
		sqlite3 *m_rawDb;
		bool m_commited;
		bool m_transaction;
	};
}

#endif /* LDB_CONVERSIONS_P_H_ */
