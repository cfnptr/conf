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
#include <exception>
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
	 * errorLine - pointer to the error line or NULL.
	 */
	Reader(string_view filePath, size_t* errorLine = nullptr)
	{
		auto result = createConfFileReader(
			filePath.data(), &instance, errorLine);
		if (result != SUCCESS_CONF_RESULT)
			throw runtime_error(confResultToString(result));
	}
	/*
	 * Create a new conf data reader instance.
	 * Throws runtime exception on failure.
	 *
	 * data - conf data string.
	 * errorLine - pointer to the error line or NULL.
	 */
	Reader(const char* data, size_t* errorLine = nullptr)
	{
		auto result = createConfDataReader(
			data, &instance, errorLine);
		if (result != SUCCESS_CONF_RESULT)
			throw runtime_error(confResultToString(result));
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
	bool getType(string_view key, ConfDataType& type) const
	{
		return getConfReaderType(instance, key.data(), &type);
	}

	/*
	 * Get specified item integer value by key.
	 * Returns true if item is found and correct.
	 *
	 * key - item search key string.
	 * value - reference to the integer value.
	 */
	bool get(string_view key, int64_t& value)
	{
		return getConfReaderInteger(instance, key.data(), &value);
	}

	/*
	 * Get specified item floating value by key.
	 * Returns true if item is found and correct.
	 *
	 * key - item search key string.
	 * value - reference to the floating value.
	 */
	bool get(string_view key, double& value)
	{
		return getConfReaderFloating(instance, key.data(), &value);
	}

	/*
	 * Get specified item boolean value by key.
	 * Returns true if item is found and correct.
	 *
	 * key - item search key string.
	 * value - reference to the boolean value.
	 */
	bool get(string_view key, bool& value)
	{
		return getConfReaderBoolean(instance, key.data(), &value);
	}

	/*
	 * Get specified item string value by key.
	 * Returns true if item is found and correct.
	 *
	 * key - item search key string.
	 * value - reference to the string value.
	 */
	bool get(string_view key, string_view& value)
	{
		uint64_t length; const char* string;
		auto result = getConfReaderString(instance,
			key.data(), &string, &length);
		if (result)
		{
			value = string_view(string, (size_t)length);
			return true;
		}
		return false;
	}
};

} // conf