#include "conf/reader.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define TEST_FILE_NAME "testing-conf.txt"

inline static bool createTestFile(
	const char* content)
{
	FILE* file = fopen(
		TEST_FILE_NAME,
		"w");

	if (file == NULL)
	{
		printf("Failed to open test file.");
		return false;
	}

	size_t contentLength = strlen(content);

	size_t writeResult = fwrite(content,
		sizeof(char),
		contentLength,
		file);

	if (writeResult != contentLength)
	{
		printf("Failed to write test file.");
		fclose(file);
		return false;
	}

	fclose(file);
	return true;
}
inline static bool removeTestFile()
{
	int result = remove(TEST_FILE_NAME);

	if (result != 0)
	{
		printf("Failed to remove test file.");
		return false;
	}

	return true;
}

inline static bool testFailedToOpenFile()
{
	ConfReader confReader;
	size_t errorLine;

	ConfResult confResult = createConfFileReader(
		"not_existing_file.txt",
		&confReader,
		&errorLine);

	if (confResult != FAILED_TO_OPEN_FILE_CONF_RESULT)
	{
		printf("testFailedToOpenFile: "
			"incorrect result. (%d)\n",
			confResult);
		return false;
	}

	return true;
}
inline static bool testGoodComment()
{
	bool result = createTestFile(
		"# Some conf comment x=1 ");

	if (result == false)
		return false;

	ConfReader confReader;
	size_t errorLine;

	ConfResult confResult = createConfFileReader(
		TEST_FILE_NAME,
		&confReader,
		&errorLine);

	if (confResult != SUCCESS_CONF_RESULT)
	{
		printf("testGoodComment: "
			"incorrect result. (%s)\n",
			confResultToString(confResult));
		return false;
	}

	destroyConfReader(confReader);
	return removeTestFile();
}
inline static bool testBadComment()
{
	bool result = createTestFile(
		" # Some bad conf comment");

	if (result == false)
		return false;

	ConfReader confReader;
	size_t errorLine;

	ConfResult confResult = createConfFileReader(
		TEST_FILE_NAME,
		&confReader,
		&errorLine);

	if (confResult != BAD_ITEM_CONF_RESULT)
	{
		printf("testBadComment: "
			"incorrect result. (%s)\n",
			confResultToString(confResult));
		return false;
	}

	return removeTestFile();
}
inline static bool testBadKey()
{
	bool result = createTestFile(
		"=123");

	if (result == false)
		return false;

	ConfReader confReader;
	size_t errorLine;

	ConfResult confResult = createConfFileReader(
		TEST_FILE_NAME,
		&confReader,
		&errorLine);

	if (confResult != BAD_KEY_CONF_RESULT)
	{
		printf("testBadKey: "
			"incorrect result. (%s)\n",
			confResultToString(confResult));
		return false;
	}

	return removeTestFile();
}
inline static bool testBadValue()
{
	bool result = createTestFile(
		"someKey=");

	if (result == false)
		return false;

	ConfReader confReader;
	size_t errorLine;

	ConfResult confResult = createConfFileReader(
		TEST_FILE_NAME,
		&confReader,
		&errorLine);

	if (confResult != BAD_VALUE_CONF_RESULT)
	{
		printf("testBadValue: "
			"incorrect result. (%s)\n",
			confResultToString(confResult));
		return false;
	}

	return removeTestFile();
}
inline static bool testInteger(
	int64_t value,
	const char* stringValue)
{
	const char* keyName = "someInteger";

	char content[256];

	sprintf(
		content,
		"%s=%s",
		keyName,
		stringValue);

	bool result = createTestFile(content);

	if (result == false)
		return false;

	ConfReader confReader;
	size_t errorLine;

	ConfResult confResult = createConfFileReader(
		TEST_FILE_NAME,
		&confReader,
		&errorLine);

	if (confResult != SUCCESS_CONF_RESULT)
	{
		printf("testInteger: incorrect result. "
			"(value: %s, result: %s)\n",
			stringValue,
			confResultToString(confResult));
		return false;
	}

	int64_t integer;

	result = getConfReaderInteger(
		confReader,
		keyName,
		&integer);

	if (result == false)
	{
		printf("testInteger: failed to get value. "
			"(value: %s)\n",
			stringValue);
		destroyConfReader(confReader);
		return false;
	}

	if (value != integer)
	{
		printf("testInteger: incorrect value "
			"(%lld instead of %s).\n",
			integer,
			stringValue);
		destroyConfReader(confReader);
		return false;
	}

	destroyConfReader(confReader);
	return removeTestFile();
}
inline static bool testFloating(
	double value,
	const char* stringValue)
{
	const char* keyName = "someFloating";

	char content[256];

	sprintf(
		content,
		"%s=%s",
		keyName,
		stringValue);

	bool result = createTestFile(content);

	if (result == false)
		return false;

	ConfReader confReader;
	size_t errorLine;

	ConfResult confResult = createConfFileReader(
		TEST_FILE_NAME,
		&confReader,
		&errorLine);

	if (confResult != SUCCESS_CONF_RESULT)
	{
		printf("testFloating: incorrect result. "
			"(value: %s, result: %s)\n",
			stringValue,
			confResultToString(confResult));
		return false;
	}

	double floating;

	result = getConfReaderFloating(
		confReader,
		keyName,
		&floating);

	if (result == false)
	{
		printf("testFloating: failed to get value. "
			"(value: %s)\n",
			stringValue);
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
		printf("testFloating: incorrect value "
			"(%f instead of %s).\n",
			floating,
			stringValue);
		destroyConfReader(confReader);
		return false;
	}

	destroyConfReader(confReader);
	return removeTestFile();
}
inline static bool testBoolean(
	double value,
	const char* stringValue)
{
	const char* keyName = "someBoolean";

	char content[256];

	sprintf(
		content,
		"%s=%s",
		keyName,
		stringValue);

	bool result = createTestFile(content);

	if (result == false)
		return false;

	ConfReader confReader;
	size_t errorLine;

	ConfResult confResult = createConfFileReader(
		TEST_FILE_NAME,
		&confReader,
		&errorLine);

	if (confResult != SUCCESS_CONF_RESULT)
	{
		printf("testBoolean: incorrect result. "
			"(value: %s, result: %s)\n",
			stringValue,
			confResultToString(confResult));
		return false;
	}

	bool boolean;

	result = getConfReaderBoolean(
		confReader,
		keyName,
		&boolean);

	if (result == false)
	{
		printf("testBoolean: failed to get value. "
			"(value: %s)\n",
			stringValue);
		destroyConfReader(confReader);
		return false;
	}

	if (value != boolean)
	{
		printf("testBoolean: incorrect value "
			"(%d instead of %s).\n",
			boolean,
			stringValue);
		destroyConfReader(confReader);
		return false;
	}

	destroyConfReader(confReader);
	return removeTestFile();
}
inline static bool testKey(const char* key)
{
	char content[256];

	sprintf(
		content,
		"%s=123",
		key);

	bool result = createTestFile(content);

	if (result == false)
		return false;

	ConfReader confReader;
	size_t errorLine;

	ConfResult confResult = createConfFileReader(
		TEST_FILE_NAME,
		&confReader,
		&errorLine);

	if (confResult != SUCCESS_CONF_RESULT)
	{
		printf("testKey: incorrect result. "
			"(key: %s, result: %s)\n",
			key,
			confResultToString(confResult));
		return false;
	}

	int64_t integer;

	result = getConfReaderInteger(
		confReader,
		key,
		&integer);

	if (result == false)
	{
		printf("testKey: failed to get value. "
			"(key: %s)\n",
			key);
		destroyConfReader(confReader);
		return false;
	}

	if (integer != 123)
	{
		printf("testFloating: incorrect value "
			"(%lld instead of 123).\n",
			integer);
		destroyConfReader(confReader);
		return false;
	}

	destroyConfReader(confReader);
	return removeTestFile();
}
inline static bool testString(
	const char* value)
{
	const char* keyName = "someString";

	char content[256];

	sprintf(
		content,
		"%s=%s",
		keyName,
		value);

	bool result = createTestFile(content);

	if (result == false)
		return false;

	ConfReader confReader;
	size_t errorLine;

	ConfResult confResult = createConfFileReader(
		TEST_FILE_NAME,
		&confReader,
		&errorLine);

	if (confResult != SUCCESS_CONF_RESULT)
	{
		printf("testString: incorrect result. "
			"(value: %s, result: %s)\n",
			value,
			confResultToString(confResult));
		return false;
	}

	const char* string;

	result = getConfReaderString(
		confReader,
		keyName,
		&string);

	if (result == false)
	{
		printf("testString: failed to get value. "
			"(value: %s)\n",
			value);
		destroyConfReader(confReader);
		return false;
	}

	if (strcmp(value, string) != 0)
	{
		printf("testString: incorrect value "
			"(%s instead of %s).\n",
			string,
			value);
		destroyConfReader(confReader);
		return false;
	}

	destroyConfReader(confReader);
	return removeTestFile();
}
inline static bool testConfig()
{
	bool result = createTestFile(
		"# This is test config\n"
		"#Similar to real \n"
		"\n"
		"# Some integer value\n"
		"integer=123456789\n"
		"\n"
		"\n"
		"# some double value?\n"
		"\n"
		"DOUBLE=0.123\n"
		"Boolean=True\n"
		"\n"
		"string =Hello world!\n"
		"#comment\n");

	if (result == false)
		return false;

	ConfReader confReader;
	size_t errorLine;

	ConfResult confResult = createConfFileReader(
		TEST_FILE_NAME,
		&confReader,
		&errorLine);

	if (confResult != SUCCESS_CONF_RESULT)
	{
		printf("testConfig: incorrect result. "
			"(result: %s)\n",
			confResultToString(confResult));
		return false;
	}

	int64_t integer;

	result = getConfReaderInteger(
		confReader,
		"integer",
		&integer);

	if (result == false)
	{
		printf("testConfig: failed to get value. (integer)\n");
		destroyConfReader(confReader);
		return false;
	}

	if (integer != 123456789)
	{
		printf("testConfig: incorrect value "
			"(%lld instead of 123456789).\n",
			integer);
		destroyConfReader(confReader);
		return false;
	}

	double floating;

	result = getConfReaderFloating(
		confReader,
		"DOUBLE",
		&floating);

	if (result == false)
	{
		printf("testConfig: failed to get value. (DOUBLE)\n");
		destroyConfReader(confReader);
		return false;
	}

	if (floating != 0.123)
	{
		printf("testConfig: incorrect value "
			"(%f instead of 0.123).\n",
			floating);
		destroyConfReader(confReader);
		return false;
	}

	bool boolean;

	result = getConfReaderBoolean(
		confReader,
		"Boolean",
		&boolean);

	if (result == false)
	{
		printf("testConfig: failed to get value. (Boolean)\n");
		destroyConfReader(confReader);
		return false;
	}

	if (boolean != true)
	{
		printf("testConfig: incorrect value "
			"(%d instead of True).\n",
			boolean);
		destroyConfReader(confReader);
		return false;
	}

	const char* string;

	result = getConfReaderString(
		confReader,
		"string ",
		&string);

	if (result == false)
	{
		printf("testConfig: failed to get value. (string )\n");
		destroyConfReader(confReader);
		return false;
	}

	if (strcmp(string, "Hello world!") != 0)
	{
		printf("testConfig: incorrect value "
			"(%s instead of Hello world!).\n",
			string);
		destroyConfReader(confReader);
		return false;
	}

	destroyConfReader(confReader);
	return removeTestFile();
}

int main()
{
	bool result = testFailedToOpenFile();
	result &= testGoodComment();
	result &= testBadComment();
	result &= testBadKey();
	result &= testBadValue();
	result &= testKey("someKey");
	result &= testKey("StartFromBig");
	result &= testKey("and.some.dots");
	result &= testKey("WHAT_ABOUT_MACROS");
	result &= testKey("Bad key usage");
	result &= testKey(" Hello World! ");
	result &= testKey("123456789");
	result &= testKey("Numbeeers 2048 ");
	result &= testKey(" < thisIsSPACE");
	result &= testKey("!@#$%^&*()_+-{}[]:|\";'\\<>?,./");
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
	result &= testKey("!@#$%^&*()_+-{}[]:|\";'\\<>?,./");
	result &= testConfig();
	return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
