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
#include "conf/writer.h"
}

namespace conf
{

using namespace std;

/*
 * Conf writer instance handle.
 */
class Writer final
{
private:
	ConfWriter instance = nullptr;
public:
	/*
	 * Create a new conf file writer instance.
	 * Throws runtime exception on failure.
	 *
	 * filePath - file path string.
	 */
	Writer(const filesystem::path& filePath)
	{
		auto pathString = filePath.generic_string();
		auto result = createConfFileWriter(pathString.c_str(), &instance);
		if (result != SUCCESS_CONF_RESULT)
		{
			throw runtime_error(confResultToString(result) +
				(", path:" + filePath.generic_string()));
		}
	}
	/*
	 * Destroy conf writer instance.
	 */
	~Writer() { destroyConfWriter(instance); }

	/*
	 * Write specified comment string to the file.
	 * Throws runtime exception on failure.
	 *
	 * comment - comment value string.
	 */
	void writeComment(const string& comment)
	{
		auto result = writeConfComment(instance, comment.c_str());
		if (!result) throw runtime_error("Failed to write a comment");
	}

	/*
	 * Write new line to the file.
	 * Throws runtime exception on failure.
	 */
	void writeNewLine()
	{
		auto result = writeConfNewLine(instance);
		if (!result) throw runtime_error("Failed to write a new line");
	}

	/*
	 * Write specified integer item to the file.
	 * Throws runtime exception on failure.
	 *
	 * key - item key string.
	 * value - integer item value.
	 */
	void write(const string& key, int64_t value)
	{
		auto result = writeConfInt(instance, key.c_str(), value);
		if (!result) throw runtime_error("Failed to write an item");
	}
	/*
	 * Write specified integer item to the file.
	 * Throws runtime exception on failure.
	 *
	 * key - item key string.
	 * value - integer item value.
	 */
	void write(const string& key, int32_t value) { write(key, (int64_t)value); }
	/*
	 * Write specified integer item to the file.
	 * Throws runtime exception on failure.
	 *
	 * key - item key string.
	 * value - integer item value.
	 */
	void write(const string& key, uint32_t value) { write(key, (int64_t)value); }
	/*
	 * Write specified integer item to the file.
	 * Throws runtime exception on failure.
	 *
	 * key - item key string.
	 * value - integer item value.
	 */
	void write(const string& key, int16_t value) { write(key, (int64_t)value); }
	/*
	 * Write specified integer item to the file.
	 * Throws runtime exception on failure.
	 *
	 * key - item key string.
	 * value - integer item value.
	 */
	void write(const string& key, uint16_t value) { write(key, (int64_t)value); }
	/*
	 * Write specified integer item to the file.
	 * Throws runtime exception on failure.
	 *
	 * key - item key string.
	 * value - integer item value.
	 */
	void write(const string& key, int8_t value) { write(key, (int64_t)value); }
	/*
	 * Write specified integer item to the file.
	 * Throws runtime exception on failure.
	 *
	 * key - item key string.
	 * value - integer item value.
	 */
	void write(const string& key, uint8_t value) { write(key, (int64_t)value); }

	/*
	 * Write specified floating item to the file.
	 * Throws runtime exception on failure.
	 *
	 * key - item key string.
	 * value - floating item value.
	 * precision - floating value precision. (0 = detect)
	 */
	void write(const string& key, double value, uint8_t precision = 0)
	{
		auto result = writeConfFloat(instance, key.data(), value, precision);
		if (!result) throw runtime_error("Failed to write an item");
	}
	/*
	 * Write specified floating item to the file.
	 * Throws runtime exception on failure.
	 *
	 * key - item key string.
	 * value - integer item value.
	 * precision - floating value precision. (0 = detect)
	 */
	void write(const string& key, float value, uint8_t precision = 0)
	{
		write(key, (double)value, precision);
	}

	/*
	 * Write specified boolean item to the file.
	 * Throws runtime exception on failure.
	 *
	 * key - item key string.
	 * value - boolean item value.
	 */
	void write(const string& key, bool value)
	{
		auto result = writeConfBool(instance, key.c_str(), value);
		if (!result) throw runtime_error("Failed to write an item");
	}

	/*
	 * Write specified string item to the file.
	 * Throws runtime exception on failure.
	 *
	 * key - item key string.
	 * value - string item value.
	 */
	void write(const string& key, string_view value)
	{
		auto result = writeConfString(instance, key.c_str(),
			value.data(), value.size());
		if (!result) throw runtime_error("Failed to write an item");
	}
};

} // conf