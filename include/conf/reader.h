// Copyright 2021-2025 Nikita Fediuchin. All rights reserved.
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
 * 
 * @details
 * Used to read Conf files. Reads all data from a file and 
 * organizes it into a list optimized for fast retrieval of values by key.
 **********************************************************************************************************************/

#pragma once
#include "conf/common.h"

#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Conf reader structure.
 */
typedef struct ConfReader_T ConfReader_T;
/**
 * @brief Conf reader instance.
 */
typedef ConfReader_T* ConfReader;

/**
 * @brief Creates a new Conf file reader instance.
 * @details The main function for reading Conf files.
 * @note You should destroy created Conf instance manually.
 *
 * @param[in] filePath target Conf file path string
 * @param[out] confReader pointer to the Conf reader instance
 * @param[out] errorLine pointer to the error line or NULL
 * 
 * @return The @ref ConfResult code and writes reader instance on success.
 * 
 * @retval SUCCESS_CONF_RESULT on success
 * @retval FAILED_TO_ALLOCATE_CONF_RESULT if out of memory
 * @retval FAILED_TO_OPEN_FILE_CONF_RESULT if file doesn't exist
 * @retval BAD_KEY_CONF_RESULT if config has invalid key
 * @retval BAD_VALUE_CONF_RESULT if config has invalid value
 * @retval BAD_ITEM_CONF_RESULT if config has invalid key / value pair
 * @retval REPEATING_KEYS_CONF_RESULT if config has duplicate keys
 */
ConfResult createFileConfReader(const char* filePath, ConfReader* confReader, size_t* errorLine);

/**
 * @brief Create a new Conf data reader instance.
 * @details The main function for reading Conf data from strings.
 * @note You should destroy created Conf instance manually.
 *
 * @param[in] data target Conf data string
 * @param[out] confReader pointer to the Conf reader instance
 * @param[out] errorLine pointer to the error line or NULL
 * 
 * @return The @ref ConfResult code and writes reader instance on success.
 * 
 * @retval SUCCESS_CONF_RESULT on success
 * @retval FAILED_TO_ALLOCATE_CONF_RESULT if out of memory
 * @retval FAILED_TO_OPEN_FILE_CONF_RESULT if file doesn't exist
 * @retval BAD_KEY_CONF_RESULT if config has invalid key
 * @retval BAD_VALUE_CONF_RESULT if config has invalid value
 * @retval BAD_ITEM_CONF_RESULT if config has invalid key / value pair
 * @retval REPEATING_KEYS_CONF_RESULT if config has duplicate keys
 */
ConfResult createDataConfReader(const char* data, ConfReader* confReader, size_t* errorLine);

/**
 * @brief Destroys Conf reader instance.
 * @param confReader conf reader instance or NULL
 */
void destroyConfReader(ConfReader confReader);

/***********************************************************************************************************************
 * @brief Returns the type of value by key.
 * @details Useful if we are reading a config we don't know anything about.
 * 
 * @param confReader conf reader instance
 * @param[in] key target item key string
 * @param[out] type pointer to the value type
 * 
 * @return True on success, false if item is not found.
 */
bool getConfReaderType(ConfReader confReader, const char* key, ConfDataType* type);

/**
 * @brief Returns the integer value by key.
 *
 * @param confReader conf reader instance
 * @param[in] key target item key string
 * @param[out] value pointer to the integer value
 * 
 * @return True on success, false if item is not found or has a different type.
 */
bool getConfReaderInt(ConfReader confReader, const char* key, int64_t* value);

/**
 * @brief Returns the floating value by key.
 *
 * @param confReader conf reader instance
 * @param[in] key target item key string
 * @param[out] value pointer to the floating value
 * 
 * @return True on success, false if item is not found or has a different type.
 */
bool getConfReaderFloat(ConfReader confReader, const char* key, double* value);

/**
 * @brief Returns the boolean value by key.
 *
 * @param confReader conf reader instance
 * @param[in] key target item key string
 * @param[out] value pointer to the boolean value
 * 
 * @return True on success, false if item is not found or has a different type.
 */
bool getConfReaderBool(ConfReader confReader, const char* key, bool* value);

/**
 * @brief Returns the string value by key.
 * @warning Yous should not free the returned string.
 *
 * @param confReader conf reader instance
 * @param[in] key target item key string
 * @param[out] value pointer to the string value
 * @param[out] length pointer to the string length or NULL
 * 
 * @return True on success, false if item is not found or has a different type.
 */
bool getConfReaderString(ConfReader confReader, const char* key, const char** value, uint64_t* length);