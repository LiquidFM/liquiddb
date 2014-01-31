/** @file ldb_Value.h
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

#ifndef LDB_VALUE_H_
#define LDB_VALUE_H_

#include <cstdint>
#include <cstring>
#include <platform/platform.h>


namespace LiquidDb {

struct Value
{
    inline Value() : m_data() {}
    inline Value(uint64_t value) : m_data() { m_data.uint_value.uint64_value = value; }
    inline Value(uint32_t value) : m_data() { m_data.uint_value.uint32_value[LoDWord] = value; }
    inline Value(uint16_t value) : m_data() { m_data.uint_value.uint16_value[LoWord] = value; }
    inline Value(uint8_t value) : m_data() { m_data.uint_value.uint8_value[LoByte] = value; }
    inline Value(int64_t value) : m_data() { m_data.int_value.int64_value = value; }
    inline Value(int32_t value) : m_data() { m_data.int_value.int32_value[LoDWord] = value; }
    inline Value(int16_t value) : m_data() { m_data.int_value.int16_value[LoWord] = value; }
    inline Value(int8_t value) : m_data() { m_data.int_value.int8_value[LoByte] = value; }
    inline Value(const char * value) : m_data() { m_data.bin_value.data = value; }
    inline Value(float value) : m_data() { m_data.float_value = value; }
    inline Value(double value) : m_data() { m_data.double_value = value; }

    inline operator uint64_t () const { return m_data.uint_value.uint64_value; }
    inline operator uint32_t () const { return m_data.uint_value.uint32_value[LoDWord]; }
    inline operator uint16_t () const { return m_data.uint_value.uint16_value[LoWord]; }
    inline operator uint8_t () const { return m_data.uint_value.uint8_value[LoByte]; }

    inline operator int64_t () const { return m_data.int_value.int64_value; }
    inline operator int32_t () const { return m_data.int_value.int32_value[LoDWord]; }
    inline operator int16_t () const { return m_data.int_value.int16_value[LoWord]; }
    inline operator int8_t () const { return m_data.int_value.int8_value[LoByte]; }

    inline operator const char * () const { return m_data.bin_value.data; }
    inline operator float () const { return m_data.float_value; }
    inline operator double () const { return m_data.double_value; }

    inline uint64_t u64() const { return m_data.uint_value.uint64_value; }
    inline uint32_t u32() const { return m_data.uint_value.uint32_value[LoDWord]; }
    inline uint16_t u16() const { return m_data.uint_value.uint16_value[LoWord]; }
    inline uint8_t u8() const { return m_data.uint_value.uint8_value[LoByte]; }

    inline int64_t i64() const { return m_data.int_value.int64_value; }
    inline int32_t i32() const { return m_data.int_value.int32_value[LoDWord]; }
    inline int16_t i16() const { return m_data.int_value.int16_value[LoWord]; }
    inline int8_t i8() const { return m_data.int_value.int8_value[LoByte]; }

    inline const char *str() const { return m_data.bin_value.data; }
    inline void setStr(const char *value, size_t size) { m_data.bin_value.data = value; m_data.bin_value.size = size; }
    inline size_t size() const { return m_data.bin_value.size; }
    inline float flt() const { return m_data.float_value; }
    inline double dbl() const { return m_data.double_value; }

    inline uint64_t &u64() { return m_data.uint_value.uint64_value; }
    inline uint32_t &u32() { return m_data.uint_value.uint32_value[LoDWord]; }
    inline uint16_t &u16() { return m_data.uint_value.uint16_value[LoWord]; }
    inline uint8_t &u8() { return m_data.uint_value.uint8_value[LoByte]; }

    inline int64_t &i64() { return m_data.int_value.int64_value; }
    inline int32_t &i32() { return m_data.int_value.int32_value[LoDWord]; }
    inline int16_t &i16() { return m_data.int_value.int16_value[LoWord]; }
    inline int8_t &i8() { return m_data.int_value.int8_value[LoByte]; }

    inline const char *&str() { return m_data.bin_value.data; }
    inline size_t &size() { return m_data.bin_value.size; }
    inline float &flt() { return m_data.float_value; }
    inline double &dbl() { return m_data.double_value; }

    inline Value &operator=(uint64_t value) { m_data.uint_value.uint64_value = value; return *this; }
    inline Value &operator=(uint32_t value) { m_data.uint_value.uint32_value[LoDWord] = value; return *this; }
    inline Value &operator=(uint16_t value) { m_data.uint_value.uint16_value[LoWord] = value; return *this; }
    inline Value &operator=(uint8_t value) { m_data.uint_value.uint8_value[LoByte] = value; return *this; }

    inline Value &operator=(int64_t value) { m_data.int_value.int64_value = value; return *this; }
    inline Value &operator=(int32_t value) { m_data.int_value.int32_value[LoDWord] = value; return *this; }
    inline Value &operator=(int16_t value) { m_data.int_value.int16_value[LoWord] = value; return *this; }
    inline Value &operator=(int8_t value) { m_data.int_value.int8_value[LoByte] = value; return *this; }

    inline Value &operator=(const char *value) { m_data.bin_value.data = value; return *this; }
    inline Value &operator=(float value) { m_data.float_value = value; return *this; }
    inline Value &operator=(double value) { m_data.double_value = value; return *this; }

private:
#if PLATFORM_CPU(LITTLE_ENDIAN)
enum
{
    LoByte = 0,
    LoWord = 0,
    LoDWord = 0,
    HiByte = sizeof(uint64_t) - 1,
    HiWord = sizeof(uint64_t) / sizeof(uint16_t) - 1,
    HiDWord = sizeof(uint64_t) / sizeof(uint32_t) - 1
};
#else
#error This platform is not supported
#endif

    union
    {
        union
        {
            uint64_t uint64_value;
            uint32_t uint32_value[HiDWord + 1];
            uint16_t uint16_value[HiWord + 1];
            uint8_t uint8_value[HiByte + 1];
        } uint_value;
        union
        {
            int64_t int64_value;
            int32_t int32_value[HiDWord + 1];
            int16_t int16_value[HiWord + 1];
            int8_t int8_value[HiByte + 1];
        } int_value;
        struct
        {
            size_t size;
            const char *data;
        } bin_value;
        float float_value;
        double double_value;
    } m_data;
};

}

#endif /* LDB_VALUE_H_ */
