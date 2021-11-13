// Copyright 2021 Nikita Fediuchin. All rights reserved.
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

// Conf reader instance handle.
typedef struct ConfReader* ConfReader;

/*
 * Create a new conf file reader instance.
 * Returns operation conf result.
 *
 * filePath - conf file path string.
 * confReader - conf reader instance.
 * errorLine - error result line index.
 */
ConfResult createConfFileReader(
	const char* filePath,
	ConfReader* confReader,
	size_t* errorLine);

/*
 * Destroy conf reader instance.
 * confReader - conf reader instance or NULL.
 */
void destroyConfReader(
	ConfReader confReader);

/*
 * Get specified conf item type.
 * Returns true if item is found.
 *
 * confReader - conf reader instance.
 * key - item search key.
 * type - pointer to the type value.
 */
bool getConfReaderType(
	ConfReader confReader,
	const char* key,
	ConfDataType* type);

/*
 * Get specified conf item integer value by key.
 * Returns true if item is found and correct.
 *
 * confReader - conf reader instance.
 * key - item search key.
 * value - pointer to the value.
 */
bool getConfReaderInteger(
	ConfReader confReader,
	const char* key,
	int64_t* value);

/*
 * Get specified conf item floating value by key.
 * Returns true if item is found and correct.
 *
 * confReader - conf reader instance.
 * key - item search key.
 * value - pointer to the value.
 */
bool getConfReaderFloating(
	ConfReader confReader,
	const char* key,
	double* value);

/*
 * Get specified conf item boolean value by key.
 * Returns true if item is found and correct.
 *
 * confReader - conf reader instance.
 * key - item search key.
 * value - pointer to the value.
 */
bool getConfReaderBoolean(
	ConfReader confReader,
	const char* key,
	bool* value);

/*
 * Get specified conf item string value by key.
 * Returns true if item is found and correct.
 *
 * confReader - conf reader instance.
 * key - item search key.
 * value - pointer to the value.
 */
bool getConfReaderString(
	ConfReader confReader,
	const char* key,
	const char** value);
