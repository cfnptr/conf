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

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

// Conf writer instance handle.
typedef struct ConfWriter* ConfWriter;

/*
 * Create a new conf file writer instance.
 * Returns operation conf result.
 *
 * filePath - conf file path string.
 * confWriter - pointer to the conf writer instance.
 */
ConfResult createConfFileWriter(
	const char* filePath,
	ConfWriter* confWriter);

/*
 * Destroy conf writer instance.
 * confWriter - conf writer instance or NULL.
 */
void destroyConfWriter(
	ConfWriter confWriter);

/*
 * Write specified comment to the file.
 * Returns true on success.
 *
 * confWriter - conf writer instance.
 * comment - comment string value.
 */
bool writeConfComment(
	ConfWriter confWriter,
	const char* comment);

/*
 * Write new line to the file.
 * Returns true on success.
 *
 * confWriter - conf writer instance.
 */
bool writeConfNewLine(
	ConfWriter confWriter);

/*
 * Write specified item to the file.
 * Returns true on success.
 *
 * confWriter - conf writer instance.
 * key - string item key.
 * value - integer item value.
 */
bool writeConfInteger(
	ConfWriter confWriter,
	const char* key,
	int64_t value);

/*
 * Write specified item to the file.
 * Returns true on success.
 *
 * confWriter - conf writer instance.
 * key - string item key.
 * value - floating item value.
 */
bool writeConfFloating(
	ConfWriter confWriter,
	const char* key,
	double value);

/*
 * Write specified item to the file.
 * Returns true on success.
 *
 * confWriter - conf writer instance.
 * key - string item key.
 * value - boolean item value.
 */
bool writeConfBoolean(
	ConfWriter confWriter,
	const char* key,
	bool value);

/*
 * Write specified item to the file.
 * Returns true on success.
 *
 * confWriter - conf writer instance.
 * key - string item key.
 * value - string item value.
 */
bool writeConfString(
	ConfWriter confWriter,
	const char* key,
	const char* value);
