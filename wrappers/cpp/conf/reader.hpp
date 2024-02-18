// Copyright 2021-2024 Nikita Fediuchin. All rights reserved.
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

/***********************************************************************************************************************
 * @file
 * @brief Conf file reader.
 * @details See the @ref reader.h
 **********************************************************************************************************************/

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

/***********************************************************************************************************************
 * @brief Conf reader instance handle.
 * @details See the @ref reader.h
 */
class Reader final
{
private:
	ConfReader instance = nullptr;
public:
	/**
	 * @brief Creates a new Conf file reader instance.
	 * @details See the @ref createFileConfReader().
	 * @param[in] filePath target Conf file path string
	 * @throw runtime_error with a @ref ConfResult string on failure.
	 */
	Reader(const filesystem::path& filePath)
	{
		size_t errorLine = 0;
		auto string = filePath.generic_string();
		auto result = createFileConfReader(string.c_str(), &instance, &errorLine);
		if (result != SUCCESS_CONF_RESULT)
		{
			throw runtime_error(confResultToString(result) + (" at line " + 
				to_string(errorLine) + ", path: " + filePath.generic_string()));
		}
	}

	/**
	 * @brief Creates a new Conf data reader instance.
	 * @details See the @ref createDataConfReader().
	 * @param[in] data target Conf data string
	 * @throw runtime_error with a @ref ConfResult string on failure.
	 */
	Reader(const char* data)
	{
		size_t errorLine = 0;
		auto result = createDataConfReader(data, &instance, &errorLine);
		if (result != SUCCESS_CONF_RESULT)
		{
			throw runtime_error(confResultToString(result) +
				(" at line " + to_string(errorLine)));
		}
	}

	/**
	 * @brief Destroys Conf reader instance.
	 * @details See the @ref destroyConfReader().
	 */
	~Reader() { destroyConfReader(instance); }

	/***********************************************************************************************************************
	 * @brief Returns the type of value by key.
	 * @details See the @ref getConfReaderType().
	 * 
	 * @param[in] key target item key string
	 * @param[out] type reference to the value type
	 * 
	 * @return True on succes, false if item is not found.
	 */
	bool getType(const string& key, ConfDataType& type) const noexcept
	{
		return getConfReaderType(instance, key.c_str(), &type);
	}

	/**
	 * @brief Returns the integer value by key. (int64)
	 * @details See the @ref getConfReaderInt().
	 *
	 * @param[in] key target item key string
	 * @param[out] value reference to the integer value
	 * 
	 * @return True on succes, false if item is not found or has a different type.
	 */
	bool get(const string& key, int64_t& value) const noexcept
	{
		return getConfReaderInt(instance, key.c_str(), &value);
	}

	/**
	 * @brief Returns the integer value by key. (int32)
	 * @details See the @ref getConfReaderInt().
	 *
	 * @param[in] key target item key string
	 * @param[out] value reference to the integer value
	 * 
	 * @return True on succes, false if item is not found or has a different type.
	 */
	bool get(const string& key, int32_t& value) const noexcept
	{
		int64_t v;
		if (get(key, v))
		{
			value = (int32_t)v;
			return true;
		}
		return false;
	}

	/**
	 * @brief Returns the integer value by key. (uint32)
	 * @details See the @ref getConfReaderInt().
	 *
	 * @param[in] key target item key string
	 * @param[out] value reference to the integer value
	 * 
	 * @return True on succes, false if item is not found or has a different type.
	 */
	bool get(const string& key, uint32_t& value) const noexcept
	{
		int64_t v;
		if (get(key, v))
		{
			value = (uint32_t)v;
			return true;
		}
		return false;
	}

	/**
	 * @brief Returns the integer value by key. (int16)
	 * @details See the @ref getConfReaderInt().
	 *
	 * @param[in] key target item key string
	 * @param[out] value reference to the integer value
	 * 
	 * @return True on succes, false if item is not found or has a different type.
	 */
	bool get(const string& key, int16_t& value) const noexcept
	{
		int64_t v;
		if (get(key, v))
		{
			value = (int16_t)v;
			return true;
		}
		return false;
	}

	/**
	 * @brief Returns the integer value by key. (uint16)
	 * @details See the @ref getConfReaderInt().
	 *
	 * @param[in] key target item key string
	 * @param[out] value reference to the integer value
	 * 
	 * @return True on succes, false if item is not found or has a different type.
	 */
	bool get(const string& key, uint16_t& value) const noexcept
	{
		int64_t v;
		if (get(key, v))
		{
			value = (uint16_t)v;
			return true;
		}
		return false;
	}
	
	/**
	 * @brief Returns the integer value by key. (int8)
	 * @details See the @ref getConfReaderInt().
	 *
	 * @param[in] key target item key string
	 * @param[out] value reference to the integer value
	 * 
	 * @return True on succes, false if item is not found or has a different type.
	 */
	bool get(const string& key, int8_t& value) const noexcept
	{
		int64_t v;
		if (get(key, v))
		{
			value = (int8_t)v;
			return true;
		}
		return false;
	}

	/**
	 * @brief Returns the integer value by key. (uint8)
	 * @details See the @ref getConfReaderInt().
	 *
	 * @param[in] key target item key string
	 * @param[out] value reference to the integer value
	 * 
	 * @return True on succes, false if item is not found or has a different type.
	 */
	bool get(const string& key, uint8_t& value) const noexcept
	{
		int64_t v;
		if (get(key, v))
		{
			value = (uint8_t)v;
			return true;
		}
		return false;
	}

	/**
	 * @brief Returns the floating value by key. (double)
	 * @details See the @ref getConfReaderFloat().
	 *
	 * @param[in] key target item key string
	 * @param[out] value reference to the floating value
	 * 
	 * @return True on succes, false if item is not found or has a different type.
	 */
	bool get(const string& key, double& value) const noexcept
	{
		return getConfReaderFloat(instance, key.c_str(), &value);
	}

	/**
	 * @brief Returns the floating value by key. (float)
	 * @details See the @ref getConfReaderFloat().
	 *
	 * @param[in] key target item key string
	 * @param[out] value reference to the floating value
	 * 
	 * @return True on succes, false if item is not found or has a different type.
	 */
	bool get(const string& key, float& value) const noexcept
	{
		double v;
		if (get(key, v))
		{
			value = (float)v;
			return true;
		}
		return false;
	}

	/**
	 * @brief Returns the boolean value by key.
	 * @details See the @ref getConfReaderBool().
	 *
	 * @param[in] key target item key string
	 * @param[out] value reference to the boolean value
	 * 
	 * @return True on succes, false if item is not found or has a different type.
	 */
	bool get(const string& key, bool& value) const noexcept
	{
		return getConfReaderBool(instance, key.c_str(), &value);
	}

	/**
	 * @brief Returns the string value by key.
	 * @details See the @ref getConfReaderString().
	 *
	 * @param[in] key target item key string
	 * @param[out] value reference to the string value
	 * 
	 * @return True on succes, false if item is not found or has a different type.
	 */
	bool get(const string& key, string_view& value) const noexcept
	{
		uint64_t length; const char* _string;
		if (getConfReaderString(instance, key.data(), &_string, &length))
		{
			value = string_view(_string, (size_t)length);
			return true;
		}
		return false;
	}
};

} // conf