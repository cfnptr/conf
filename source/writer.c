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

#include "conf/writer.h"
#include "mpio/file.h"

#include <math.h>
#include <assert.h>
#include <stdlib.h>

struct ConfWriter_T
{
	FILE* file;
};

ConfResult createFileConfWriter(const char* filePath, ConfWriter* confWriter)
{
	assert(filePath);
	assert(confWriter);

	ConfWriter confWriterInstance = malloc(sizeof(ConfWriter_T));
	if (!confWriterInstance)
		return FAILED_TO_ALLOCATE_CONF_RESULT;

	FILE* file = openFile(filePath, "w");
	if (!file)
	{
		free(confWriterInstance);
		return FAILED_TO_OPEN_FILE_CONF_RESULT;
	}

	confWriterInstance->file = file;
	*confWriter = confWriterInstance;
	return SUCCESS_CONF_RESULT;
}

void destroyConfWriter(ConfWriter confWriter)
{
	if (!confWriter)
		return;
	if (confWriter->file)
		closeFile(confWriter->file);
	free(confWriter);
}

//**********************************************************************************************************************
bool writeConfComment(ConfWriter confWriter, const char* comment)
{
	assert(confWriter);
	assert(comment);
	return fprintf(confWriter->file, "# %s\n", comment) > 0;
}

bool writeConfNewLine(ConfWriter confWriter)
{
	assert(confWriter);
	return fputc('\n', confWriter->file) == '\n';
}

bool writeConfInt(ConfWriter confWriter, const char* key, int64_t value)
{
	assert(confWriter);
	assert(key);
	return fprintf(confWriter->file, "%s: %lld\n", key, (long long int)value) > 0;
}

inline static bool getDoubleDigitCount(double value, uint8_t* count)
{
	uint8_t digitCount = 0;

	while ((double)((int64_t)value) != value)
	{
		value = value * 10.0;
		digitCount++;
		if (digitCount == UINT8_MAX)
			return false;
	}

	*count = digitCount > 0 ? digitCount : 1;
	return true;
}
bool writeConfFloat(ConfWriter confWriter, const char* key, double value, uint8_t precision)
{
	assert(confWriter);
	assert(key);

	if (value == INFINITY)
	{
		return fprintf( confWriter->file, "%s: inf\n", key) > 0;
	}
	else if (value == -INFINITY)
	{
		return fprintf(confWriter->file, "%s: -inf\n", key) > 0;
	}
	else if (isnan(value))
	{
		return fprintf(confWriter->file, "%s: nan\n", key) > 0;
	}
	else
	{
		uint8_t digitCount;
		if (!getDoubleDigitCount(value, &digitCount))
			return false;

		if (precision > 0 && precision < digitCount)
			digitCount = precision;

		return fprintf(confWriter->file, "%s: %.*f\n", key, digitCount, value) > 0;
	}
}

bool writeConfBool(ConfWriter confWriter, const char* key, bool value)
{
	assert(confWriter);
	assert(key);
	
	if (value)
		return fprintf(confWriter->file, "%s: true\n", key) > 0;
	else
		return fprintf( confWriter->file, "%s: false\n", key) > 0;
}

bool writeConfString(ConfWriter confWriter, const char* key, const char* value, size_t length)
{
	assert(confWriter);
	assert(key);
	assert(value);

	if (length == 0)
		return fprintf(confWriter->file, "%s: %s\n", key, value) > 0;
	else
		return fprintf(confWriter->file, "%s: %.*s\n", key, (int)length, value) > 0;
}