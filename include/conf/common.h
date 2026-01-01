// Copyright 2021-2026 Nikita Fediuchin. All rights reserved.
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
 * @brief Common Conf library functions.
 */

#pragma once
#include <stdint.h>

/**
 * @brief Conf result codes.
 */
typedef enum ConfResult_T
{
	SUCCESS_CONF_RESULT = 0,
	FAILED_TO_ALLOCATE_CONF_RESULT = 1,
	FAILED_TO_OPEN_FILE_CONF_RESULT = 2,
	BAD_KEY_CONF_RESULT = 3,
	BAD_VALUE_CONF_RESULT = 4,
	BAD_ITEM_CONF_RESULT = 5,
	REPEATING_KEYS_CONF_RESULT = 6,
	CONF_RESULT_COUNT = 7,
} ConfResult_T;
/**
 * @brief Conf result code type.
 */
typedef uint8_t ConfResult;

/**
 * @brief Conf data types.
 */
typedef enum ConfDataType_T
{
	INTEGER_CONF_DATA_TYPE = 0,
	FLOATING_CONF_DATA_TYPE = 1,
	BOOLEAN_CONF_DATA_TYPE = 2,
	STRING_CONF_DATA_TYPE = 3,
	CONF_DATA_TYPE_COUNT = 4,
} ConfDataType_T;
/**
 * @brief Conf data type.
 */
typedef uint8_t ConfDataType;

/**
 * @brief Conf result code string array.
 */
static const char* const confResultStrings[CONF_RESULT_COUNT] = {
	"Success",
	"Failed to allocate",
	"Failed to open file",
	"Bad key",
	"Bad value",
	"Bad item",
	"Repeating keys",
};

/**
 * @brief Returns Conf result code as a string.
 * @param result conf result code
 * @result Conf result code string. Or "Unknown CONF result" if out of range.
 */
inline static const char* confResultToString(ConfResult result)
{
	if (result >= CONF_RESULT_COUNT)
		return "Unknown CONF result";
	return confResultStrings[result];
}