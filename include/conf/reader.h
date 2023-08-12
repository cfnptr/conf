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
#include "conf/defines.h"

#include <stddef.h>
#include <stdbool.h>

/*
 * Conf reader structure.
 */
typedef struct ConfReader_T ConfReader_T;
/*
 * Conf reader instance.
 */
typedef ConfReader_T* ConfReader;

/*
 * Create a new conf file reader instance.
 * Returns operation Conf result.
 *
 * filePath - conf file path string.
 * confReader - pointer to the conf reader instance.
 * errorLine - pointer to the error line or NULL.
 */
ConfResult createConfFileReader(const char* filePath,
	ConfReader* confReader, size_t* errorLine);
/*
 * Create a new conf data reader instance.
 * Returns operation Conf result.
 *
 * data - conf data string.
 * confReader - pointer to the conf reader instance.
 * errorLine - pointer to the error line or NULL.
 */
ConfResult createConfDataReader(const char* data,
	ConfReader* confReader, size_t* errorLine);

/*
 * Destroy conf reader instance.
 * confReader - conf reader instance or NULL.
 */
void destroyConfReader(ConfReader confReader);

/*
 * Get specified item type.
 * Returns true if item is found.
 *
 * confReader - conf reader instance.
 * key - item search key string.
 * type - pointer to the value type.
 */
bool getConfReaderType(ConfReader confReader,
	const char* key, ConfDataType* type);

/*
 * Get specified item integer value by key.
 * Returns true if item is found and correct.
 *
 * confReader - conf reader instance.
 * key - item search key string.
 * value - pointer to the integer value.
 */
bool getConfReaderInt(ConfReader confReader,
	const char* key, int64_t* value);

/*
 * Get specified item floating value by key.
 * Returns true if item is found and correct.
 *
 * confReader - conf reader instance.
 * key - item search key string.
 * value - pointer to the floating value.
 */
bool getConfReaderFloat(ConfReader confReader,
	const char* key, double* value);

/*
 * Get specified item boolean value by key.
 * Returns true if item is found and correct.
 *
 * confReader - conf reader instance.
 * key - item search key string.
 * value - pointer to the boolean value.
 */
bool getConfReaderBool(ConfReader confReader,
	const char* key, bool* value);

/*
 * Get specified item string value by key.
 * Returns true if item is found and correct.
 *
 * confReader - conf reader instance.
 * key - item search key string.
 * value - pointer to the string value.
 * length - pointer to the value length or NULL.
 */
bool getConfReaderString(ConfReader confReader,
	const char* key, const char** value, uint64_t* length);