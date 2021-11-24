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

#include "conf/reader.h"
#include "mpio/file.h"

#include <math.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

#if __linux__ || __APPLE__
#define compareNoCase(a, b, l) strncasecmp(a, b, l)
#elif _WIN32
#define compareNoCase(a, b, l) _strnicmp(a, b, l)
#else
#error Unknown operating system
#endif

typedef union ConfValue
{
	int64_t integer;
	double floating;
	bool boolean;
	const char* string;
} ConfValue;

typedef struct ConfItem
{
	const char* key;
	size_t keySize;
	ConfValue value;
	ConfDataType type;
} ConfItem;

struct ConfReader
{
	ConfItem* items;
	size_t itemCount;
};

static int compareConfItems(
	const void* a,
	const void* b)
{
	const ConfItem* itemA = a;
	const ConfItem* itemB = b;

	if (itemA->keySize != itemB->keySize)
		return itemA->keySize < itemB->keySize ? -1 : 1;

	return memcmp(
		itemA->key,
		itemB->key,
		itemA->keySize);
}

inline static void destroyConfItems(
	ConfItem* items,
	size_t itemCount)
{
	for (size_t i = 0; i < itemCount; i++)
	{
		ConfItem item = items[i];

		if (item.type == STRING_CONF_DATA_TYPE)
			free((char*)item.value.string);

		free((char*)item.key);
	}

	free(items);
}
inline static ConfResult createConfItems(
	int(*getNextChar)(void*),
	void* handle,
	ConfItem** _items,
	size_t* _itemCount,
	size_t* errorLine)
{
	ConfItem* items = malloc(
		sizeof(struct ConfItem));

	if (items == NULL)
	{
		*errorLine = 0;
		return FAILED_TO_ALLOCATE_CONF_RESULT;
	}

	size_t itemCount = 0;
	size_t itemCapacity = 1;

	char* buffer = malloc(sizeof(char));

	if (buffer == NULL)
	{
		free(items);
		*errorLine = 0;
		return FAILED_TO_ALLOCATE_CONF_RESULT;
	}

	size_t bufferSize = 0;
	size_t bufferCapacity = 1;
	size_t lineIndex = 0;

	ConfItem item;

	memset(
		&item,
		0,
		sizeof(struct ConfItem));

	while (true)
	{
		int currentChar = getNextChar(handle);

		if (currentChar == '=' && item.keySize == 0)
		{
			if (bufferSize == 0)
			{
				free(buffer);
				destroyConfItems(
					items,
					itemCount);
				*errorLine = lineIndex + 1;
				return BAD_KEY_CONF_RESULT;
			}

			char* key = malloc(
				(bufferSize + 1) * sizeof(char));

			if (key == NULL)
			{
				free(buffer);
				destroyConfItems(
					items,
					itemCount);
				*errorLine = lineIndex + 1;
				return FAILED_TO_ALLOCATE_CONF_RESULT;
			}

			memcpy(
				key,
				buffer,
				bufferSize);
			key[bufferSize] = '\0';

			item.key = key;
			item.keySize = bufferSize;
			bufferSize = 0;
			continue;
		}
		else if (currentChar == '\n' || currentChar == EOF)
		{
			if (item.keySize == 0)
			{
				if (bufferSize != 0)
				{
					free(buffer);
					destroyConfItems(
						items,
						itemCount);
					*errorLine = lineIndex + 1;
					return BAD_ITEM_CONF_RESULT;
				}

				if (currentChar == EOF)
					break;

				bufferSize = 0;
				lineIndex++;
				continue;
			}

			if (bufferSize == 0)
			{
				free((char*)item.key);
				free(buffer);
				destroyConfItems(
					items,
					itemCount);
				*errorLine = lineIndex + 1;
				return BAD_VALUE_CONF_RESULT;
			}

			if (bufferSize == bufferCapacity)
			{
				bufferCapacity *= 2;

				char* newBuffer = realloc(
					buffer,
					bufferCapacity * sizeof(char));

				if (newBuffer == NULL)
				{
					free((char*)item.key);
					free(buffer);
					destroyConfItems(
						items,
						itemCount);
					*errorLine = lineIndex + 1;
					return FAILED_TO_REALLOCATE_CONF_RESULT;
				}

				buffer = newBuffer;
			}

			buffer[bufferSize] = '\n';
			char firstChar = buffer[0];

			bool converted = false;
			char* endChar = NULL;

			if (isdigit(firstChar) != 0 || firstChar == '-')
			{
				int64_t integer = strtoll(
					buffer,
					&endChar,
					10);

				if (buffer != endChar)
				{
					char end = *endChar;

					if (end == '\n')
					{
						item.value.integer = integer;
						item.type = INTEGER_CONF_DATA_TYPE;
						converted = true;
					}
					else if (end == '.' && isdigit(*(endChar + 1)) != 0)
					{
						endChar++;

						char* fractionEndChar;

						int64_t fraction = strtoll(
							endChar,
							&fractionEndChar,
							10);

						if (endChar != fractionEndChar &&
							*fractionEndChar == '\n')
						{
							double half = (double)fraction /
								pow(10, (double)(fractionEndChar - endChar));
							item.value.floating = firstChar == '-' ?
								(double)integer - half : (double)integer + half;
							item.type = FLOATING_CONF_DATA_TYPE;
							converted = true;
						}
					}
				}
			}

			if (converted == false)
			{
				if (compareNoCase(buffer, "true\n", 5) == 0)
				{
					item.value.boolean = true;
					item.type = BOOLEAN_CONF_DATA_TYPE;
					converted = true;
				}
				else if (compareNoCase(buffer, "false\n", 6) == 0)
				{
					item.value.boolean = false;
					item.type = BOOLEAN_CONF_DATA_TYPE;
					converted = true;
				}
				else if (compareNoCase(buffer, "inf\n", 4) == 0)
				{
					item.value.floating = INFINITY;
					item.type = FLOATING_CONF_DATA_TYPE;
					converted = true;
				}
				else if (compareNoCase(buffer, "-inf\n", 5) == 0)
				{
					item.value.floating = -INFINITY;
					item.type = FLOATING_CONF_DATA_TYPE;
					converted = true;
				}
				else if (compareNoCase(buffer, "nan\n", 4) == 0)
				{
					item.value.floating = NAN;
					item.type = FLOATING_CONF_DATA_TYPE;
					converted = true;
				}
			}

			if (converted == false)
			{
				char* string = malloc(
					(bufferSize + 1) * sizeof(char));

				if (string == NULL)
				{
					free((char*)item.key);
					free(buffer);
					destroyConfItems(
						items,
						itemCount);
					*errorLine = lineIndex + 1;
					return FAILED_TO_ALLOCATE_CONF_RESULT;
				}

				memcpy(
					string,
					buffer,
					bufferSize);
				string[bufferSize] = '\0';

				item.value.string = string;
				item.type = STRING_CONF_DATA_TYPE;
			}

			if (itemCount == itemCapacity)
			{
				itemCapacity *= 2;

				ConfItem* newItems = realloc(
					items,
					itemCapacity * sizeof(struct ConfItem));

				if (newItems == NULL)
				{
					if (item.type == STRING_CONF_DATA_TYPE)
						free((char*)item.value.string);
					free((char*)item.key);
					free(buffer);
					destroyConfItems(
						items,
						itemCount);
					*errorLine = lineIndex + 1;
					return FAILED_TO_REALLOCATE_CONF_RESULT;
				}

				items = newItems;
			}

			items[itemCount++] = item;

			if (currentChar == EOF)
				break;

			item.keySize = 0;
			bufferSize = 0;
			lineIndex++;
			continue;
		}
		else if (currentChar == '#' && bufferSize == 0)
		{
			while(true)
			{
				currentChar = getNextChar(handle);

				if (currentChar == '\n' || currentChar == EOF)
					break;
			}

			bufferSize = 0;
			lineIndex++;
			continue;
		}

		if (bufferSize == bufferCapacity)
		{
			bufferCapacity *= 2;

			char* newBuffer = realloc(
				buffer,
				bufferCapacity * sizeof(char));

			if (newBuffer == NULL)
			{
				if (item.keySize != 0)
					free((char*)item.key);
				free(buffer);
				destroyConfItems(
					items,
					itemCount);
				*errorLine = lineIndex + 1;
				return FAILED_TO_REALLOCATE_CONF_RESULT;
			}

			buffer = newBuffer;
		}

		buffer[bufferSize++] = (char)currentChar;
	}

	free(buffer);

	qsort(
		items,
		itemCount,
		sizeof(struct ConfItem),
		compareConfItems);

	*_items = items;
	*_itemCount = itemCount;
	return SUCCESS_CONF_RESULT;
}

static int onNextFileChar(void* handle)
{
	return getc(handle);
}
ConfResult createConfFileReader(
	const char* filePath,
	ConfReader* _confReader,
	size_t* errorLine)
{
	assert(filePath != NULL);
	assert(_confReader != NULL);
	assert(errorLine != NULL);

	ConfReader confReader = malloc(
		sizeof(struct ConfReader));

	if (confReader == NULL)
	{
		*errorLine = 0;
		return FAILED_TO_ALLOCATE_CONF_RESULT;
	}

	FILE* file = openFile(
		filePath,
		"r");

	if (file == NULL)
	{
		free(confReader);
		*errorLine = 0;
		return FAILED_TO_OPEN_FILE_CONF_RESULT;
	}

	ConfItem* items;
	size_t itemCount;

	ConfResult result = createConfItems(
		onNextFileChar,
		file,
		&items,
		&itemCount,
		errorLine);

	closeFile(file);

	if (result != SUCCESS_CONF_RESULT)
	{
		free(confReader);
		return result;
	}

	confReader->items = items;
	confReader->itemCount = itemCount;

	*_confReader = confReader;
	return SUCCESS_CONF_RESULT;
}

void destroyConfReader(
	ConfReader confReader)
{
	if (confReader == NULL)
		return;

	destroyConfItems(
		confReader->items,
		confReader->itemCount);
	free(confReader);
}

bool getConfReaderType(
	ConfReader confReader,
	const char* key,
	ConfDataType* type)
{
	assert(confReader != NULL);
	assert(key != NULL);
	assert(type != NULL);

	ConfItem item;
	item.key = key;
	item.keySize = strlen(key);

	ConfItem* foundItem = bsearch(
		&item,
		confReader->items,
		confReader->itemCount,
		sizeof(struct ConfItem),
		compareConfItems);

	if (foundItem == NULL)
		return false;

	*type = foundItem->type;
	return true;
}

bool getConfReaderInteger(
	ConfReader confReader,
	const char* key,
	int64_t* value)
{
	assert(confReader != NULL);
	assert(key != NULL);
	assert(value != NULL);

	ConfItem item;
	item.key = key;
	item.keySize = strlen(key);

	ConfItem* foundItem = bsearch(
		&item,
		confReader->items,
		confReader->itemCount,
		sizeof(struct ConfItem),
		compareConfItems);

	if (foundItem == NULL ||
		foundItem->type != INTEGER_CONF_DATA_TYPE)
	{
		return false;
	}

	*value = foundItem->value.integer;
	return true;
}

bool getConfReaderFloating(
	ConfReader confReader,
	const char* key,
	double* value)
{
	assert(confReader != NULL);
	assert(key != NULL);
	assert(value != NULL);

	ConfItem item;
	item.key = key;
	item.keySize = strlen(key);

	ConfItem* foundItem = bsearch(
		&item,
		confReader->items,
		confReader->itemCount,
		sizeof(struct ConfItem),
		compareConfItems);

	if (foundItem == NULL ||
		foundItem->type != FLOATING_CONF_DATA_TYPE)
	{
		return false;
	}

	*value = foundItem->value.floating;
	return true;
}

bool getConfReaderBoolean(
	ConfReader confReader,
	const char* key,
	bool* value)
{
	assert(confReader != NULL);
	assert(key != NULL);
	assert(value != NULL);

	ConfItem item;
	item.key = key;
	item.keySize = strlen(key);

	ConfItem* foundItem = bsearch(
		&item,
		confReader->items,
		confReader->itemCount,
		sizeof(struct ConfItem),
		compareConfItems);

	if (foundItem == NULL ||
		foundItem->type != BOOLEAN_CONF_DATA_TYPE)
	{
		return false;
	}

	*value = foundItem->value.boolean;
	return true;
}

bool getConfReaderString(
	ConfReader confReader,
	const char* key,
	const char** value)
{
	assert(confReader != NULL);
	assert(key != NULL);
	assert(value != NULL);

	ConfItem item;
	item.key = key;
	item.keySize = strlen(key);

	ConfItem* foundItem = bsearch(
		&item,
		confReader->items,
		confReader->itemCount,
		sizeof(struct ConfItem),
		compareConfItems);

	if (foundItem == NULL ||
		foundItem->type != STRING_CONF_DATA_TYPE)
	{
		return false;
	}

	*value = foundItem->value.string;
	return true;
}
