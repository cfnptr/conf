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
 * Conf writer structure.
 */
typedef struct ConfWriter_T ConfWriter_T;
/*
 * Conf writer instance.
 */
typedef ConfWriter_T* ConfWriter;

/*
 * Create a new conf file writer instance.
 * Returns operation Conf result.
 *
 * filePath - file path string.
 * confWriter - pointer to the conf writer instance.
 */
ConfResult createConfFileWriter(
	const char* filePath, ConfWriter* confWriter);

/*
 * Destroy conf writer instance.
 * confWriter - conf writer instance or NULL.
 */
void destroyConfWriter(ConfWriter confWriter);

/*
 * Write specified comment string to the file.
 * Returns true on success.
 *
 * confWriter - conf writer instance.
 * comment - comment value string.
 */
bool writeConfComment(ConfWriter confWriter, const char* comment);

/*
 * Write new line to the file.
 * Returns true on success.
 *
 * confWriter - conf writer instance.
 */
bool writeConfNewLine(ConfWriter confWriter);

/*
 * Write specified integer item to the file.
 * Returns true on success.
 *
 * confWriter - conf writer instance.
 * key - item key string.
 * value - integer item value.
 */
bool writeConfInt(ConfWriter confWriter,
	const char* key, int64_t value);

/*
 * Write specified floating item to the file.
 * Returns true on success.
 *
 * confWriter - conf writer instance.
 * key - item key string.
 * value - floating item value.
 * precision - floating value precision. (0 = detect)
 */
bool writeConfFloat(ConfWriter confWriter,
	const char* key, double value, uint8_t precision);

/*
 * Write specified boolean item to the file.
 * Returns true on success.
 *
 * confWriter - conf writer instance.
 * key - item key string.
 * value - boolean item value.
 */
bool writeConfBool(ConfWriter confWriter,
	const char* key, bool value);

/*
 * Write specified string item to the file.
 * Returns true on success.
 *
 * confWriter - conf writer instance.
 * key - item key string.
 * value - string item value.
 * length - string item length. (0 = detect)
 */
bool writeConfString(ConfWriter confWriter,
	const char* key, const char* value, size_t length);