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

#include "conf/writer.h"
#include "mpio/file.h"

#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define TEST_FILE_NAME "testing-conf.txt"

inline static bool compareTestFile(const char* confData)
{
	assert(confData);

	FILE* file = openFile(TEST_FILE_NAME, "r");
	if (!file)
	{
		printf("Failed to open test file.\n");
		return false;
	}

	int result = seekFile(file, 0, SEEK_END);
	if (result != 0)
	{
		printf("Failed to seek test file end.\n");
		closeFile(file);
		return false;
	}

	size_t fileSize = tellFile(file);
	if (fileSize == 0)
	{
		printf("Zero size test file.\n");
		closeFile(file);
		return false;
	}

	result = seekFile(file, 0, SEEK_SET);
	if (result != 0)
	{
		printf("Failed to seek test file start.\n");
		closeFile(file);
		return false;
	}

	char* data = malloc(sizeof(char) * (fileSize + 1));
	if (!data)
	{
		printf("Failed to allocate read buffer.\n");
		closeFile(file);
		return false;
	}

	size_t readResult = fread(data, sizeof(char), fileSize, file);
	closeFile(file);

	if (readResult == 0)
	{
		printf("Failed to read test file.\n");
		free(data);
		return false;
	}

	data[readResult] = '\0';

	if (strcmp(confData, data) != 0)
	{
		printf("Incorrect value: %s.\n", data);
		free(data);
		return false;
	}

	free(data);
	return true;
}
inline static bool removeTestFile()
{
	int result = remove(TEST_FILE_NAME);

	if (result != 0)
	{
		printf("Failed to remove test file.\n");
		return false;
	}

	return true;
}

inline static bool testComment()
{
	ConfWriter confWriter;
	ConfResult confResult = createFileConfWriter(TEST_FILE_NAME, &confWriter);
	if (confResult != SUCCESS_CONF_RESULT)
	{
		printf("testComment: "
			"incorrect result. (%s)\n",
			confResultToString(confResult));
		return false;
	}

	const char* comment = "Some conf comment";
	bool result = writeConfComment(confWriter, comment);
	if (!result)
	{
		printf("testComment: failed to write value. "
			"(comment: %s)\n", comment);
		destroyConfWriter(confWriter);
		return false;
	}

	destroyConfWriter(confWriter);

	char buffer[256];
	sprintf(buffer, "# %s\n", comment);

	if (!compareTestFile("# Some conf comment\n"))
	{
		printf("testComment: incorrect value. "
			"(comment: %s)\n", comment);
		return false;
	}

	return removeTestFile();
}
inline static bool testInteger(int64_t value, const char* stringValue)
{
	assert(stringValue);

	ConfWriter confWriter;
	ConfResult confResult = createFileConfWriter(TEST_FILE_NAME, &confWriter);
	if (confResult != SUCCESS_CONF_RESULT)
	{
		printf("testInteger: incorrect result. "
			"(value: %s, result: %s)\n",
			stringValue, confResultToString(confResult));
		return false;
	}

	const char* keyName = "someInteger";
	bool result = writeConfInt(confWriter, keyName, value);
	if (!result)
	{
		printf("testInteger: failed to write value. "
			"(value: %s)\n", stringValue);
		destroyConfWriter(confWriter);
		return false;
	}

	destroyConfWriter(confWriter);

	char buffer[256];
	sprintf(buffer, "%s: %s\n", keyName, stringValue);

	if (!compareTestFile(buffer))
	{
		printf("testInteger: incorrect value. "
			"(value: %s)\n", stringValue);
		return false;
	}

	return removeTestFile();
}
inline static bool testFloating(double value, const char* stringValue)
{
	assert(stringValue);

	ConfWriter confWriter;
	ConfResult confResult = createFileConfWriter(TEST_FILE_NAME, &confWriter);
	if (confResult != SUCCESS_CONF_RESULT)
	{
		printf("testFloating: incorrect result. "
			"(value: %s, result: %s)\n",
			stringValue, confResultToString(confResult));
		return false;
	}

	const char* keyName = "someFloating";
	bool result = writeConfFloat(confWriter, keyName, value, 0);
	if (!result)
	{
		printf("testFloating: failed to write value. "
			"(value: %s)\n", stringValue);
		destroyConfWriter(confWriter);
		return false;
	}

	destroyConfWriter(confWriter);

	char buffer[256];
	sprintf(buffer, "%s: %s\n", keyName, stringValue);

	if (!compareTestFile(buffer))
	{
		printf("testFloating: incorrect value. "
			"(value: %s)\n", stringValue);
		return false;
	}

	return removeTestFile();
}
inline static bool testBoolean(bool value, const char* stringValue)
{
	assert(stringValue);

	ConfWriter confWriter;
	ConfResult confResult = createFileConfWriter(TEST_FILE_NAME, &confWriter);
	if (confResult != SUCCESS_CONF_RESULT)
	{
		printf("testBoolean: incorrect result. "
			"(value: %s, result: %s)\n",
			stringValue, confResultToString(confResult));
		return false;
	}

	const char* keyName = "someBoolean";
	bool result = writeConfBool(confWriter, keyName, value);
	if (!result)
	{
		printf("testBoolean: failed to write value. "
			"(value: %s)\n", stringValue);
		destroyConfWriter(confWriter);
		return false;
	}

	destroyConfWriter(confWriter);

	char buffer[256];
	sprintf(buffer, "%s: %s\n", keyName, stringValue);

	if (!compareTestFile(buffer))
	{
		printf("testBoolean: incorrect value. "
			"(value: %s)\n", stringValue);
		return false;
	}

	return removeTestFile();
}
inline static bool testString(const char* value)
{
	assert(value);

	ConfWriter confWriter;
	ConfResult confResult = createFileConfWriter(TEST_FILE_NAME, &confWriter);

	if (confResult != SUCCESS_CONF_RESULT)
	{
		printf("testString: incorrect result. "
			"(value: %s, result: %s)\n",
			value, confResultToString(confResult));
		return false;
	}

	const char* keyName = "someString";

	bool result = writeConfString(
		confWriter, keyName, value, strlen(value));

	if (!result)
	{
		printf("testString: failed to write value. "
			"(value: %s)\n", value);
		destroyConfWriter(confWriter);
		return false;
	}

	destroyConfWriter(confWriter);

	char buffer[256];
	sprintf(buffer, "%s: %s\n", keyName, value);

	if (!compareTestFile(buffer))
	{
		printf("testString: incorrect value. "
			"(value: %s)\n", value);
		return false;
	}

	return removeTestFile();
}
inline static bool testConfig()
{
	ConfWriter confWriter;
	ConfResult confResult = createFileConfWriter(TEST_FILE_NAME, &confWriter);

	if (confResult != SUCCESS_CONF_RESULT)
	{
		printf("testConfig: incorrect result. "
			"(result: %s)\n",
			confResultToString(confResult));
		return false;
	}

	bool result = writeConfComment(confWriter, "Conf file test");
	if (!result)
	{
		printf("testConfig: failed to write comment. "
			"(comment: Conf file test)\n");
		destroyConfWriter(confWriter);
		return false;
	}

	result = writeConfInt(confWriter, "someInteger", 123);
	if (!result)
	{
		printf("testConfig: failed to write value. "
			"(value: 123)\n");
		destroyConfWriter(confWriter);
		return false;
	}

	result = writeConfNewLine(confWriter);
	if (!result)
	{
		printf("testConfig: failed to write new line.\n");
		destroyConfWriter(confWriter);
		return false;
	}

	result = writeConfFloat(confWriter, "Floating", 1.0, 0);
	if (!result)
	{
		printf("testConfig: failed to write value. "
			"(value: 1.0)\n");
		destroyConfWriter(confWriter);
		return false;
	}

	result = writeConfBool(confWriter, "BOOLEAN", true);
	if (!result)
	{
		printf("testConfig: failed to write value. "
			"(value: true)\n");
		destroyConfWriter(confWriter);
		return false;
	}

	result = writeConfString(confWriter, "string ", "Hello world!", 0);
	if (!result)
	{
		printf("testConfig: failed to write value. "
			"(value: Hello world!)\n");
		destroyConfWriter(confWriter);
		return false;
	}

	destroyConfWriter(confWriter);

	result = compareTestFile(
		"# Conf file test\n"
		"someInteger: 123\n"
		"\n"
		"Floating: 1.0\n"
		"BOOLEAN: true\n"
		"string : Hello world!\n");

	if (!result)
	{
		printf("testConfig: incorrect value.\n");
		return false;
	}

	return removeTestFile();
}

int main()
{
	bool result = testComment();
	result &= testInteger(0, "0");
	result &= testInteger(1, "1");
	result &= testInteger(-1, "-1");
	result &= testInteger(123, "123");
	result &= testInteger(1000, "1000");
	result &= testInteger(-1000, "-1000");
	result &= testInteger(123456789, "123456789");
	result &= testInteger(-123456789, "-123456789");
	result &= testFloating(0.0, "0.0");
	result &= testFloating(1.0, "1.0");
	result &= testFloating(-0.002, "-0.002");
	result &= testFloating(-123.0, "-123.0");
	result &= testFloating(321.0, "321.0");
	result &= testFloating(10.1, "10.1");
	result &= testFloating(123.4567, "123.4567");
	result &= testFloating(-123.4567, "-123.4567");
	result &= testFloating(0.0102, "0.0102");
	result &= testFloating(-0.0102, "-0.0102");
	result &= testFloating(123456789.0, "123456789.0");
	result &= testFloating(-123456789.0, "-123456789.0");
	result &= testFloating(INFINITY, "inf");
	result &= testFloating(-INFINITY, "-inf");
	result &= testFloating(NAN, "nan");
	result &= testBoolean(true, "true");
	result &= testBoolean(false, "false");
	result &= testString(" 123");
	result &= testString(" -123");
	result &= testString("123456789 ");
	result &= testString("1. 01");
	result &= testString("1.0 ");
	result &= testString("-1.0f");
	result &= testString("123x");
	result &= testString("0x123");
	result &= testString("abcdef");
	result &= testString("Hello!");
	result &= testString("Some text test");
	result &= testString("\tTab symbol test");
	result &= testString("looksLikeKey");
	result &= testString("PLEASE DON'T SCREAM");
	result &= testString("!@#$%%^&*()_+-={}[]:|\";'\\<>?,./");
	result &= testConfig();
	return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
