// Copyright 2021-2022 Nikita Fediuchin. All rights reserved.
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

struct ConfWriter_T
{
	FILE* file;
};

ConfResult createConfFileWriter(
	const char* filePath,
	ConfWriter* confWriter)
{
	assert(filePath);
	assert(confWriter);

	ConfWriter confWriterInstance = malloc(
		sizeof(ConfWriter_T));

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

void destroyConfWriter(
	ConfWriter confWriter)
{
	if (!confWriter)
		return;

	closeFile(confWriter->file);
	free(confWriter);
}

bool writeConfComment(
	ConfWriter confWriter,
	const char* comment)
{
	assert(confWriter);
	assert(comment);

	return fprintf(
		confWriter->file,
		"# %s\n",
		comment) > 0;
}

bool writeConfNewLine(
	ConfWriter confWriter)
{
	assert(confWriter);
	return fputc('\n', confWriter->file) == '\n';
}

bool writeConfInteger(
	ConfWriter confWriter,
	const char* key,
	int64_t value)
{
	assert(confWriter);
	assert(key);

	return fprintf(
		confWriter->file,
		"%s=%lld\n",
		key,
		value) > 0;
}

inline static uint8_t getDoubleDigitCount(double value)
{
	uint8_t count = 0;

	while ((double)((int64_t)value) != value)
	{
		value = value * 10.0;
		count++;

		if (count == UINT8_MAX)
			abort();
	}

	return count != 0 ? count : 1;
}
bool writeConfFloating(
	ConfWriter confWriter,
	const char* key,
	double value)
{
	assert(confWriter);
	assert(key);

	if (value == INFINITY)
	{
		return fprintf(
			confWriter->file,
			"%s=inf\n",
			key) > 0;
	}
	else if (value == -INFINITY)
	{
		return fprintf(
			confWriter->file,
			"%s=-inf\n",
			key) > 0;
	}
	else if (isnan(value))
	{
		return fprintf(
			confWriter->file,
			"%s=nan\n",
			key) > 0;
	}
	else
	{
		return fprintf(
			confWriter->file,
			"%s=%.*f\n",
			key,
			getDoubleDigitCount(value),
			value) > 0;
	}
}

bool writeConfBoolean(
	ConfWriter confWriter,
	const char* key,
	bool value)
{
	assert(confWriter);
	assert(key);

	if (value)
	{
		return fprintf(
			confWriter->file,
			"%s=true\n",
			key) > 0;
	}
	else
	{
		return fprintf(
			confWriter->file,
			"%s=false\n",
			key) > 0;
	}
}

bool writeConfString(
	ConfWriter confWriter,
	const char* key,
	const char* value)
{
	assert(confWriter);
	assert(key);
	assert(value);

	return fprintf(
		confWriter->file,
		"%s=%s\n",
		key,
		value) > 0;
}
