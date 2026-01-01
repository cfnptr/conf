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

#include "conf/reader.h"
#include "mpio/file.h"

#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define TEST_FILE_NAME "testing-conf.txt"

inline static bool createTestFile(const char* content)
{
	assert(content);

	FILE* file = openFile(TEST_FILE_NAME, "w");
	if (!file)
	{
		printf("Failed to open test file.\n");
		return false;
	}

	size_t contentLength = strlen(content);
	size_t writeResult = fwrite(content,
		sizeof(char), contentLength, file);

	if (writeResult != contentLength)
	{
		printf("Failed to write test file.\n");
		closeFile(file);
		return false;
	}

	closeFile(file);
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

//**********************************************************************************************************************
inline static bool testFailedToOpenFile()
{
	ConfReader confReader;
	size_t errorLine;

	ConfResult confResult = createFileConfReader(
		"not_existing_file.txt", &confReader, &errorLine);

	if (confResult != FAILED_TO_OPEN_FILE_CONF_RESULT)
	{
		printf("testFailedToOpenFile: "
			"incorrect result. (%d)\n", confResult);
		return false;
	}

	return true;
}
inline static bool testGoodComment()
{
	if (!createTestFile("# Some conf comment x: 1 "))
		return false;

	ConfReader confReader;
	size_t errorLine;

	ConfResult confResult = createFileConfReader(
		TEST_FILE_NAME, &confReader, &errorLine);

	if (confResult != SUCCESS_CONF_RESULT)
	{
		printf("testGoodComment: incorrect result. (%s)\n",
			confResultToString(confResult));
		return false;
	}

	destroyConfReader(confReader);
	return removeTestFile();
}
inline static bool testBadComment()
{
	if (!createTestFile(" # Some bad conf comment"))
		return false;

	ConfReader confReader;
	size_t errorLine;

	ConfResult confResult = createFileConfReader(
		TEST_FILE_NAME, &confReader, &errorLine);

	if (confResult != BAD_ITEM_CONF_RESULT)
	{
		printf("testBadComment: incorrect result. (%s)\n",
			confResultToString(confResult));
		return false;
	}

	return removeTestFile();
}

//**********************************************************************************************************************
inline static bool testBadKey()
{
	if (!createTestFile(": 123"))
		return false;

	ConfReader confReader;
	size_t errorLine;

	ConfResult confResult = createFileConfReader(
		TEST_FILE_NAME, &confReader, &errorLine);

	if (confResult != BAD_KEY_CONF_RESULT)
	{
		printf("testBadKey: incorrect result. (%s)\n",
			confResultToString(confResult));
		return false;
	}

	return removeTestFile();
}
inline static bool testBadValue()
{
	if (!createTestFile("someKey: "))
		return false;

	ConfReader confReader;
	size_t errorLine;

	ConfResult confResult = createFileConfReader(
		TEST_FILE_NAME, &confReader, &errorLine);

	if (confResult != BAD_VALUE_CONF_RESULT)
	{
		printf("testBadValue: incorrect result. (%s)\n",
			confResultToString(confResult));
		return false;
	}

	return removeTestFile();
}
inline static bool testBadValueSpacing()
{
	if (!createTestFile("someKey:123"))
		return false;

	ConfReader confReader;
	size_t errorLine;

	ConfResult confResult = createFileConfReader(
		TEST_FILE_NAME, &confReader, &errorLine);

	if (confResult != BAD_VALUE_CONF_RESULT)
	{
		printf("testBadValueSpacing: incorrect result. (%s)\n",
			confResultToString(confResult));
		return false;
	}

	return removeTestFile();
}

//**********************************************************************************************************************
inline static bool testInteger(int64_t value, const char* stringValue)
{
	assert(stringValue);

	const char* keyName = "someInteger";
	char content[256];

	sprintf(content, "%s: %s", keyName, stringValue);
	if (!createTestFile(content)) return false;

	ConfReader confReader;
	size_t errorLine;

	ConfResult confResult = createFileConfReader(
		TEST_FILE_NAME, &confReader, &errorLine);

	if (confResult != SUCCESS_CONF_RESULT)
	{
		printf("testInteger: incorrect result. "
			"(value: %s, result: %s)\n",
			stringValue, confResultToString(confResult));
		return false;
	}

	int64_t integer;
	bool result = getConfReaderInt(confReader, keyName, &integer);

	if (!result)
	{
		printf("testInteger: failed to get value. "
			"(value: %s)\n", stringValue);
		destroyConfReader(confReader);
		return false;
	}

	if (value != integer)
	{
		printf("testInteger: incorrect value. "
			"(reference: %s, result: %lld)\n",
			stringValue, (long long int)integer);
		destroyConfReader(confReader);
		return false;
	}

	destroyConfReader(confReader);
	return removeTestFile();
}
inline static bool testFloating(double value, const char* stringValue)
{
	assert(stringValue);

	const char* keyName = "someFloating";
	char content[256];

	sprintf(content, "%s: %s", keyName, stringValue);

	if (!createTestFile(content))
		return false;

	ConfReader confReader;
	size_t errorLine;

	ConfResult confResult = createFileConfReader(
		TEST_FILE_NAME, &confReader, &errorLine);

	if (confResult != SUCCESS_CONF_RESULT)
	{
		printf("testFloating: incorrect result. "
			"(value: %s, result: %s)\n",
			stringValue, confResultToString(confResult));
		return false;
	}

	double floating;
	bool result = getConfReaderFloat(confReader, keyName, &floating);

	if (!result)
	{
		printf("testFloating: failed to get value. "
			"(value: %s)\n", stringValue);
		destroyConfReader(confReader);
		return false;
	}

	if (isnan(value) && isnan(floating))
	{
		destroyConfReader(confReader);
		return removeTestFile();
	}

	if (value != floating)
	{
		printf("testFloating: incorrect value. "
			"(reference: %s, result: %f)\n",
			stringValue, floating);
		destroyConfReader(confReader);
		return false;
	}

	destroyConfReader(confReader);
	return removeTestFile();
}

//**********************************************************************************************************************
inline static bool testBoolean(double value, const char* stringValue)
{
	assert(stringValue);

	const char* keyName = "someBoolean";
	char content[256];

	sprintf(content, "%s: %s", keyName, stringValue);
	if (!createTestFile(content)) return false;

	ConfReader confReader;
	size_t errorLine;

	ConfResult confResult = createFileConfReader(
		TEST_FILE_NAME, &confReader, &errorLine);

	if (confResult != SUCCESS_CONF_RESULT)
	{
		printf("testBoolean: incorrect result. "
			"(value: %s, result: %s)\n",
			stringValue, confResultToString(confResult));
		return false;
	}

	bool boolean;
	bool result = getConfReaderBool(confReader, keyName, &boolean);

	if (!result)
	{
		printf("testBoolean: failed to get value. "
			"(value: %s)\n", stringValue);
		destroyConfReader(confReader);
		return false;
	}

	if (value != boolean)
	{
		printf("testBoolean: incorrect value. "
			"(reference: %s, result: %d)\n",
			stringValue, boolean);
		destroyConfReader(confReader);
		return false;
	}

	destroyConfReader(confReader);
	return removeTestFile();
}
inline static bool testKey(const char* key)
{
	assert(key);

	char content[256];
	sprintf(content, "%s: 123", key);
	if (!createTestFile(content)) return false;

	ConfReader confReader;
	size_t errorLine;

	ConfResult confResult = createFileConfReader(
		TEST_FILE_NAME, &confReader, &errorLine);

	if (confResult != SUCCESS_CONF_RESULT)
	{
		printf("testKey: incorrect result. "
			"(key: %s, result: %s)\n",
			key, confResultToString(confResult));
		return false;
	}

	int64_t integer;

	bool result = getConfReaderInt(
		confReader, key, &integer);

	if (!result)
	{
		printf("testKey: failed to get value. "
			"(key: %s)\n", key);
		destroyConfReader(confReader);
		return false;
	}

	if (integer != 123)
	{
		printf("testFloating: incorrect value. "
			"(reference: 123, result: %lld)\n",
			(long long int)integer);
		destroyConfReader(confReader);
		return false;
	}

	destroyConfReader(confReader);
	return removeTestFile();
}

//**********************************************************************************************************************
inline static bool testString(const char* value)
{
	assert(value);

	const char* keyName = "someString";
	char content[256];

	sprintf(content, "%s: %s", keyName, value);
	if (!createTestFile(content)) return false;

	ConfReader confReader;
	size_t errorLine;

	ConfResult confResult = createFileConfReader(
		TEST_FILE_NAME, &confReader, &errorLine);

	if (confResult != SUCCESS_CONF_RESULT)
	{
		printf("testString: incorrect result. "
			"(value: %s, result: %s)\n",
			value, confResultToString(confResult));
		return false;
	}

	const char* string;
	bool result = getConfReaderString(confReader, keyName, &string, NULL);

	if (!result)
	{
		printf("testString: failed to get value. "
			"(value: %s)\n", value);
		destroyConfReader(confReader);
		return false;
	}

	if (strcmp(value, string) != 0)
	{
		printf("testString: incorrect value. "
			"(reference: %s, result: %s)\n",
			value, string);
		destroyConfReader(confReader);
		return false;
	}

	destroyConfReader(confReader);
	return removeTestFile();
}

static const char* const testConfigString =
	"# This is test config\n"
	"#similar to real \n"
	"\n"
	"# Some integer value\n"
	"integer: 123456789\n"
	"\n"
	"\n"
	"# SOME_DOUBLE_VALUE?\n"
	"\n"
	"DOUBLE: 0.123\n"
	"Boolean: True\n"
	"\n"
	"string : Hello world!\n"
	"#comment\n";

//**********************************************************************************************************************
inline static bool testConfig(ConfReader confReader)
{
	assert(confReader);

	int64_t integer;
	bool result = getConfReaderInt(confReader, "integer", &integer);

	if (!result)
	{
		printf("testConfig: failed to get value. "
			"(key: integer)\n");
		return false;
	}

	if (integer != 123456789)
	{
		printf("testConfig: incorrect value. "
			"(reference: 123456789, result: %lld)\n",
			(long long int)integer);
		return false;
	}

	double floating;
	result = getConfReaderFloat(confReader, "DOUBLE", &floating);

	if (!result)
	{
		printf("testConfig: failed to get value. "
			"(key: DOUBLE)\n");
		return false;
	}

	if (floating != 0.123)
	{
		printf("testConfig: incorrect value. "
			"(reference: 0.123, result: %f)\n", floating);
		return false;
	}

	bool boolean;
	result = getConfReaderBool(confReader, "Boolean", &boolean);

	if (!result)
	{
		printf("testConfig: failed to get value. "
			"(key: Boolean)\n");
		return false;
	}

	if (!boolean)
	{
		printf("testConfig: incorrect value. "
			"(reference: True, result: %d)\n",
			boolean);
		return false;
	}

	const char* string;
	uint64_t length;
	result = getConfReaderString(confReader, "string ", &string, &length);

	if (!result)
	{
		printf("testConfig: failed to get value. "
			"(key: string )\n");
		return false;
	}

	if (strcmp(string, "Hello world!") != 0)
	{
		printf("testConfig: incorrect value. "
			"(reference: Hello world!, result: %s)\n",
			string);
		return false;
	}
	if (length != strlen("Hello world!"))
	{
		printf("testConfig: incorrect value length. "
			"(reference: %llu, result: %llu)\n",
			(long long unsigned int)strlen("Hello world!"),
			(long long unsigned int)length);
		return false;
	}

	return true;
}
inline static bool testFileConfig()
{
	if (!createTestFile(testConfigString))
		return false;

	ConfReader confReader;
	size_t errorLine;

	ConfResult confResult = createFileConfReader(
		TEST_FILE_NAME, &confReader, &errorLine);

	if (confResult != SUCCESS_CONF_RESULT)
	{
		printf("testFileConfig: incorrect result. "
			"(result: %s)\n", confResultToString(confResult));
		return false;
	}

	bool result = testConfig(confReader);
	destroyConfReader(confReader);
	return removeTestFile() && result;
}
inline static bool testDataConfig()
{
	ConfReader confReader;
	size_t errorLine;

	ConfResult confResult = createDataConfReader(
		testConfigString, &confReader, &errorLine);

	if (confResult != SUCCESS_CONF_RESULT)
	{
		printf("testDataConfig: incorrect result. "
			"(result: %s)\n", confResultToString(confResult));
		return false;
	}

	bool result = testConfig(confReader);
	destroyConfReader(confReader);
	return result;
}

//**********************************************************************************************************************
int main()
{
	bool result = testFailedToOpenFile();
	result &= testGoodComment();
	result &= testBadComment();
	result &= testBadKey();
	result &= testBadValue();
	result &= testBadValueSpacing();
	result &= testKey("someKey");
	result &= testKey("StartFromBig");
	result &= testKey("and.some.dots");
	result &= testKey("WHAT_ABOUT_MACROS");
	result &= testKey("Bad key usage");
	result &= testKey(" Hello World! ");
	result &= testKey("123456789");
	result &= testKey("Numbeeers 2048 ");
	result &= testKey(" < thisIsSPACE");
	result &= testKey("!@#$%%^&*()_+-={}[]|\";'\\<>?,./");
	result &= testKey("\t");
	result &= testInteger(0, "0");
	result &= testInteger(1, "1");
	result &= testInteger(-123, "-123");
	result &= testInteger(321, "321");
	result &= testInteger(1000, "1000");
	result &= testInteger(-1000, "-1000");
	result &= testInteger(123456789, "123456789");
	result &= testInteger(-123456789, "-123456789");
	result &= testFloating(0.0, "0.0");
	result &= testFloating(1.0, "1.0");
	result &= testFloating(-0.001, "-0.001");
	result &= testFloating(-123.0, "-123.0");
	result &= testFloating(321.0, "321.0");
	result &= testFloating(10.1, "10.1");
	result &= testFloating(1.001, "1.001");
	result &= testFloating(123.456, "123.456");
	result &= testFloating(-123.456, "-123.456");
	result &= testFloating(0.0102, "0.0102");
	result &= testFloating(-0.0102, "-0.0102");
	result &= testFloating(123456789.0, "123456789.0");
	result &= testFloating(-123456789.0, "-123456789.0");
	result &= testFloating(INFINITY, "INF");
	result &= testFloating(-INFINITY, "-inf");
	result &= testFloating(NAN, "NaN");
	result &= testBoolean(true, "true");
	result &= testBoolean(false, "false");
	result &= testBoolean(true, "True");
	result &= testBoolean(false, "FALSE");
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
	result &= testString("!@#$%%^&*()_+-{}[]:|\";'\\<>?,./");
	result &= testFileConfig();
	result &= testDataConfig();
	return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
