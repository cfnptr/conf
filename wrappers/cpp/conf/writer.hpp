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
 * @brief Pack file writer.
 * @details See the @ref writer.h
 **********************************************************************************************************************/

#pragma once
#include <string>
#include <exception>
#include <filesystem>
#include <string_view>

extern "C"
{
#include "conf/writer.h"
}

namespace conf
{

using namespace std;

/***********************************************************************************************************************
 * @brief Conf writer instance handle.
 * @details See the @ref writer.h
 */
class Writer final
{
private:
	ConfWriter instance = nullptr;
public:
	/**
	 * @brief Creates a new Conf file writer instance.
	 * @details See the @ref createFileConfWriter().
	 * @param[in] filePath target Conf file path string
	 * @throw runtime_error with a @ref ConfResult string on failure.
	 */
	Writer(const filesystem::path& filePath)
	{
		auto pathString = filePath.generic_string();
		auto result = createFileConfWriter(pathString.c_str(), &instance);
		if (result != SUCCESS_CONF_RESULT)
		{
			throw runtime_error(confResultToString(result) +
				(", path:" + filePath.generic_string()));
		}
	}

	/**
	 * @brief Destroys Conf writer instance.
	 * @details See the @ref destroyConfReader().
	 */
	~Writer() { destroyConfWriter(instance); }

	/**
	 * @brief Writes a comment to the config.
	 * @details See the @ref writeConfComment().
	 * @param[in] comment target comment string
	 * @throw runtime_error on a failure.
	 */
	void writeComment(const string& comment)
	{
		if (!writeConfComment(instance, comment.c_str()))
			throw runtime_error("Failed to write a comment");
	}

	/**
	 * @brief Writes a new line to the config. ('\\n')
	 * @details See the @ref writeConfNewLine().
	 * @throw runtime_error on a failure.
	 */
	void writeNewLine()
	{
		if (!writeConfNewLine(instance))
			throw runtime_error("Failed to write a new line");
	}

	/**
	 * @brief Writes an integer value to the config. (int64)
	 * @details See the @ref writeConfInt().
	 *
	 * @param[in] key target item key string
	 * @param value integer item value
	 * 
	 * @throw runtime_error on a failure.
	 */
	void write(const string& key, int64_t value)
	{
		if (!writeConfInt(instance, key.c_str(), value))
			throw runtime_error("Failed to write a integer item");
	}

	/**
	 * @brief Writes an integer value to the config. (int32)
	 * @details See the @ref writeConfInt().
	 *
	 * @param[in] key target item key string
	 * @param value integer item value
	 * 
	 * @throw runtime_error on a failure.
	 */
	void write(const string& key, int32_t value) { write(key, (int64_t)value); }

	/**
	 * @brief Writes an integer value to the config. (uint32)
	 * @details See the @ref writeConfInt().
	 *
	 * @param[in] key target item key string
	 * @param value integer item value
	 * 
	 * @throw runtime_error on a failure.
	 */
	void write(const string& key, uint32_t value) { write(key, (int64_t)value); }

	/**
	 * @brief Writes an integer value to the config. (int16)
	 * @details See the @ref writeConfInt().
	 *
	 * @param[in] key target item key string
	 * @param value integer item value
	 * 
	 * @throw runtime_error on a failure.
	 */
	void write(const string& key, int16_t value) { write(key, (int64_t)value); }

	/**
	 * @brief Writes an integer value to the config. (uint16)
	 * @details See the @ref writeConfInt().
	 *
	 * @param[in] key target item key string
	 * @param value integer item value
	 * 
	 * @throw runtime_error on a failure.
	 */
	void write(const string& key, uint16_t value) { write(key, (int64_t)value); }

	/**
	 * @brief Writes an integer value to the config. (int8)
	 * @details See the @ref writeConfInt().
	 *
	 * @param[in] key target item key string
	 * @param value integer item value
	 * 
	 * @throw runtime_error on a failure.
	 */
	void write(const string& key, int8_t value) { write(key, (int64_t)value); }

	/**
	 * @brief Writes an integer value to the config. (uint8)
	 * @details See the @ref writeConfInt().
	 *
	 * @param[in] key target item key string
	 * @param value integer item value
	 * 
	 * @throw runtime_error on a failure.
	 */
	void write(const string& key, uint8_t value) { write(key, (int64_t)value); }

	/**
	 * @brief Writes a floating value to the config. (double)
	 * @details See the @ref writeConfFloat().
	 *
	 * @param[in] key target item key string
	 * @param value floating item value
	 * @param precision number of digits after the decimal point, or 0 (auto detect).
	 * 
	 * @throw runtime_error on a failure.
	 */
	void write(const string& key, double value, uint8_t precision = 0)
	{
		if (!writeConfFloat(instance, key.data(), value, precision))
			throw runtime_error("Failed to write a floating item");
	}

	/**
	 * @brief Writes a floating value to the config. (float)
	 * @details See the @ref writeConfFloat().
	 *
	 * @param[in] key target item key string
	 * @param value floating item value
	 * @param precision number of digits after the decimal point, or 0 (auto detect).
	 * 
	 * @throw runtime_error on a failure.
	 */
	void write(const string& key, float value, uint8_t precision = 0)
	{
		write(key, (double)value, precision);
	}

	/**
	 * @brief Writes a boolean value to the config.
	 * @details See the @ref writeConfBool().
	 *
	 * @param[in] key target item key string
	 * @param value boolean item value
	 * 
	 * @throw runtime_error on a failure.
	 */
	void write(const string& key, bool value)
	{
		if (!writeConfBool(instance, key.c_str(), value))
			throw runtime_error("Failed to write a boolean item");
	}

	/**
	 * @brief Writes a string value to the config.
	 * @details See the @ref writeConfString().
	 *
	 * @param[in] key target item key string
	 * @param[in] value string item value
	 * 
	 * @throw runtime_error on a failure.
	 */
	void write(const string& key, string_view value)
	{
		if (!writeConfString(instance, key.c_str(), value.data(), value.size()))
			throw runtime_error("Failed to write a string item");
	}
};

} // conf