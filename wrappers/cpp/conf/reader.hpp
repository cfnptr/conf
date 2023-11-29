// Copyright 2021-2023 Nikita Fediuchin. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#include <string>
#include <exception>
#include <filesystem>
#include <string_view>

extern "C"
{
#include "conf/reader.h"
}

namespace conf
{

using namespace std;

/*
 * Conf reader instance handle.
 */
class Reader final
{
private:
	ConfReader instance = nullptr;
public:
	/*
	 * Create a new conf file reader instance.
	 * Throws runtime exception on failure.
	 *
	 * filePath - conf file path string.
	 */
	Reader(const filesystem::path& filePath)
	{
		size_t errorLine = 0;
		auto string = filePath.generic_string();
		auto result = createFileConfReader(
			string.c_str(), &instance, &errorLine);
		if (result != SUCCESS_CONF_RESULT)
		{
			throw runtime_error(confResultToString(result) + (" at line " + 
				to_string(errorLine) + ", path: " + filePath.generic_string()));
		}
	}
	/*
	 * Create a new conf data reader instance.
	 * Throws runtime exception on failure.
	 *
	 * data - conf data string.
	 */
	Reader(const char* data)
	{
		size_t errorLine = 0;
		auto result = createDataConfReader(
			data, &instance, &errorLine);
		if (result != SUCCESS_CONF_RESULT)
		{
			throw runtime_error(confResultToString(result) +
				(" at line " + to_string(errorLine)));
		}
	}
	/*
	 * Destroy conf reader instance.
	 */
	~Reader() { destroyConfReader(instance); }

	/*
	 * Get specified item type.
	 * Returns true if item is found.
	 *
	 * key - string item search key.
	 * type - reference to the value type.
	 */
	bool getType(const string& key, ConfDataType& type) const noexcept
	{
		return getConfReaderType(instance, key.c_str(), &type);
	}

	/*
	 * Get specified item integer value by key.
	 * Returns true if item is found and correct.
	 *
	 * key - item search key string.
	 * value - reference to the integer value.
	 */
	bool get(const string& key, int64_t& value) const noexcept
	{
		return getConfReaderInt(instance, key.c_str(), &value);
	}
	/*
	 * Get specified item integer value by key.
	 * Returns true if item is found and correct.
	 *
	 * key - item search key string.
	 * value - reference to the integer value.
	 */
	bool get(const string& key, int32_t& value) const noexcept
	{
		int64_t v; auto result = get(key, v);
		if (result)
		{
			value = (int32_t)v;
			return true;
		}
		return false;
	}
	/*
	 * Get specified item integer value by key.
	 * Returns true if item is found and correct.
	 *
	 * key - item search key string.
	 * value - reference to the integer value.
	 */
	bool get(const string& key, uint32_t& value) const noexcept
	{
		int64_t v; auto result = get(key, v);
		if (result)
		{
			value = (uint32_t)v;
			return true;
		}
		return false;
	}
	/*
	 * Get specified item integer value by key.
	 * Returns true if item is found and correct.
	 *
	 * key - item search key string.
	 * value - reference to the integer value.
	 */
	bool get(const string& key, int16_t& value) const noexcept
	{
		int64_t v; auto result = get(key, v);
		if (result)
		{
			value = (int16_t)v;
			return true;
		}
		return false;
	}
	/*
	 * Get specified item integer value by key.
	 * Returns true if item is found and correct.
	 *
	 * key - item search key string.
	 * value - reference to the integer value.
	 */
	bool get(const string& key, uint16_t& value) const noexcept
	{
		int64_t v; auto result = get(key, v);
		if (result)
		{
			value = (uint16_t)v;
			return true;
		}
		return false;
	}
	/*
	 * Get specified item integer value by key.
	 * Returns true if item is found and correct.
	 *
	 * key - item search key string.
	 * value - reference to the integer value.
	 */
	bool get(const string& key, int8_t& value) const noexcept
	{
		int64_t v; auto result = get(key, v);
		if (result)
		{
			value = (int8_t)v;
			return true;
		}
		return false;
	}
	/*
	 * Get specified item integer value by key.
	 * Returns true if item is found and correct.
	 *
	 * key - item search key string.
	 * value - reference to the integer value.
	 */
	bool get(const string& key, uint8_t& value) const noexcept
	{
		int64_t v; auto result = get(key, v);
		if (result)
		{
			value = (uint8_t)v;
			return true;
		}
		return false;
	}

	/*
	 * Get specified item floating value by key.
	 * Returns true if item is found and correct.
	 *
	 * key - item search key string.
	 * value - reference to the floating value.
	 */
	bool get(const string& key, double& value) const noexcept
	{
		return getConfReaderFloat(instance, key.c_str(), &value);
	}
	/*
	 * Get specified item floating value by key.
	 * Returns true if item is found and correct.
	 *
	 * key - item search key string.
	 * value - reference to the floating value.
	 */
	bool get(const string& key, float& value) const noexcept
	{
		double v; auto result = get(key, v);
		if (result)
		{
			value = (float)v;
			return true;
		}
		return false;
	}

	/*
	 * Get specified item boolean value by key.
	 * Returns true if item is found and correct.
	 *
	 * key - item search key string.
	 * value - reference to the boolean value.
	 */
	bool get(const string& key, bool& value) const noexcept
	{
		return getConfReaderBool(instance, key.c_str(), &value);
	}

	/*
	 * Get specified item string value by key.
	 * Returns true if item is found and correct.
	 *
	 * key - item search key string.
	 * value - reference to the string value.
	 */
	bool get(const string& key, string_view& value) const noexcept
	{
		uint64_t length; const char* _string;
		auto result = getConfReaderString(instance,
			key.data(), &_string, &length);
		if (result)
		{
			value = string_view(_string, (size_t)length);
			return true;
		}
		return false;
	}
};

} // conf