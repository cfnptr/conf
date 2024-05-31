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
 * @brief Conf file writer.
 * 
 * @details
 * Used to write Conf files. It opens a new file stream and writes the key / value pairs to it. 
 * After destroying the Conf writer instance, it closes the file stream.
 **********************************************************************************************************************/

#pragma once
#include "conf/common.h"

#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Conf writer structure.
 */
typedef struct ConfWriter_T ConfWriter_T;
/**
 * @brief Conf writer instance.
 */
typedef ConfWriter_T* ConfWriter;

/**
 * @brief Creates a new Conf file writer instance.
 * @details The main function for writing Conf files.
 * @note You should destroy created Conf instance manually.
 *
 * @param[in] filePath target file path string
 * @param[out] confWriter pointer to the Conf writer instance
 * 
 * @return The @ref ConfResult code and writes reader instance on success.
 * 
 * @retval SUCCESS_CONF_RESULT on success
 * @retval FAILED_TO_ALLOCATE_CONF_RESULT if out of memory
 * @retval FAILED_TO_OPEN_FILE_CONF_RESULT if failed to create a new file
 */
ConfResult createFileConfWriter(const char* filePath, ConfWriter* confWriter);

/**
 * @brief Destroys Conf writer instance.
 * @param confWriter conf writer instance or NULL
 */
void destroyConfWriter(ConfWriter confWriter);

/***********************************************************************************************************************
 * @brief Writes a comment to the config.
 *
 * @param confWriter conf writer instance
 * @param[in] comment target comment string
 */
bool writeConfComment(ConfWriter confWriter, const char* comment);

/**
 * @brief Writes a new line to the config. ('\\n')
 * @param confWriter conf writer instance
 * @return True on success, otherwise false.
 */
bool writeConfNewLine(ConfWriter confWriter);

/**
 * @brief Writes an integer value to the config.
 *
 * @param confWriter conf writer instance
 * @param[in] key target item key string
 * @param value integer item value
 * 
 * @return True on success, otherwise false.
 */
bool writeConfInt(ConfWriter confWriter, const char* key, int64_t value);

/**
 * @brief Writes a floating value to the config.
 *
 * @param confWriter conf writer instance
 * @param[in] key target item key string
 * @param value floating item value
 * @param precision number of digits after the decimal point, or 0 (auto detect)
 * 
 * @return True on success, otherwise false.
 */
bool writeConfFloat(ConfWriter confWriter, const char* key, double value, uint8_t precision);

/**
 * @brief Writes a boolean value to the config.
 *
 * @param confWriter conf writer instance
 * @param[in] key target item key string
 * @param value boolean item value
 * 
 * @return True on success, otherwise false.
 */
bool writeConfBool(ConfWriter confWriter, const char* key, bool value);

/**
 * @brief Writes a string value to the config.
 *
 * @param confWriter conf writer instance
 * @param[in] key target item key string
 * @param[in] value string item value
 * @param length string value length, or 0 (auto detect)
 * 
 * @return True on success, otherwise false.
 */
bool writeConfString(ConfWriter confWriter, const char* key, const char* value, size_t length);