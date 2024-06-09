// Copyright 2021-2024 Nikita Fediuchin. All rights reserved.
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
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#if __linux__ || __APPLE__
#define compareNoCase(a, b, l) strncasecmp(a, b, l)
#elif _WIN32
#define compareNoCase(a, b, l) _strnicmp(a, b, l)
#else
#error Unknown operating system
#endif

typedef struct ConfString
{
	char* value;
	uint64_t length;
} ConfString;
typedef union ConfValue
{
	int64_t integer;
	double floating;
	bool boolean;
	ConfString string;
} ConfValue;

typedef struct ConfItem
{
	char* key;
	size_t keySize;
	ConfValue value;
	ConfDataType type;
} ConfItem;

struct ConfReader_T
{
	ConfItem* items;
	size_t itemCount;
};

inline static int compareConfItems(const void* a, const void* b)
{
	// NOTE: a and b should not be NULL!
	// Skipping here assertions for debug build speed.
	const ConfItem* itemA = a;
	const ConfItem* itemB = b;
	if (itemA->keySize != itemB->keySize)
		return itemA->keySize < itemB->keySize ? -1 : 1;
	return memcmp(itemA->key, itemB->key, itemA->keySize * sizeof(char));
}

inline static void destroyConfItems(ConfItem* items, size_t itemCount)
{
	assert(itemCount == 0 || (items != NULL && itemCount > 0));

	for (size_t i = 0; i < itemCount; i++)
	{
		ConfItem item = items[i];
		if (item.type == STRING_CONF_DATA_TYPE)
			free(item.value.string.value);
		free(item.key);
	}

	free(items);
}
inline static ConfResult createConfItems(int(*getNextChar)(void*), void* handle,
	ConfItem** _items, size_t* _itemCount, size_t* errorLine)
{
	assert(getNextChar != NULL);
	assert(_items != NULL);
	assert(_itemCount != NULL);

	ConfItem* items = malloc(sizeof(struct ConfItem));
	if (!items)
	{
		if (errorLine)
			*errorLine = 0;
		return FAILED_TO_ALLOCATE_CONF_RESULT;
	}

	size_t itemCount = 0;
	size_t itemCapacity = 1;

	char* buffer = malloc(sizeof(char));
	if (!buffer)
	{
		free(items);
		if (errorLine)
			*errorLine = 0;
		return FAILED_TO_ALLOCATE_CONF_RESULT;
	}

	size_t bufferSize = 0, bufferCapacity = 1, lineIndex = 0;
	
	ConfItem item;
	memset(&item, 0, sizeof(struct ConfItem));

	while (true)
	{
		int currentChar = getNextChar(handle);
		if (currentChar == ':' && item.keySize == 0)
		{
			if (bufferSize == 0)
			{
				free(buffer); destroyConfItems(items, itemCount);
				if (errorLine)
					*errorLine = lineIndex + 1;
				return BAD_KEY_CONF_RESULT;
			}

			char* key = malloc((bufferSize + 1) * sizeof(char));
			if (!key)
			{
				free(buffer); destroyConfItems(items, itemCount);
				if (errorLine)
					*errorLine = lineIndex + 1;
				return FAILED_TO_ALLOCATE_CONF_RESULT;
			}

			memcpy(key, buffer, bufferSize * sizeof(char));
			key[bufferSize] = '\0';

			item.key = key;
			item.keySize = bufferSize;
			bufferSize = 0;
			continue;
		}
		else if (currentChar == '\n' || currentChar == EOF || currentChar == '\0')
		{
			if (item.keySize == 0)
			{
				if (bufferSize > 0)
				{
					free(buffer);
					destroyConfItems(items, itemCount);
					if (errorLine)
						*errorLine = lineIndex + 1;
					return BAD_ITEM_CONF_RESULT;
				}

				if (currentChar == EOF || currentChar == '\0') break;

				bufferSize = 0;
				lineIndex++;
				continue;
			}

			if (bufferSize <= 1 || buffer[0] != ' ')
			{
				free(item.key); free(buffer);
				destroyConfItems(items, itemCount);
				if (errorLine)
					*errorLine = lineIndex + 1;
				return BAD_VALUE_CONF_RESULT;
			}

			if (bufferSize == bufferCapacity)
			{
				bufferCapacity *= 2;

				char* newBuffer = realloc(buffer,
					bufferCapacity * sizeof(char));

				if (!newBuffer)
				{
					free(item.key);
					free(buffer);
					destroyConfItems(items, itemCount);
					if (errorLine)
						*errorLine = lineIndex + 1;
					return FAILED_TO_ALLOCATE_CONF_RESULT;
				}

				buffer = newBuffer;
			}

			buffer[bufferSize] = '\n';
			char* bufferValue = buffer + 1;
			size_t bufferValueSize = bufferSize - 1;
			char firstChar = bufferValue[0];

			bool converted = false;
			char* endChar = NULL;

			if (isdigit(firstChar) != 0 || firstChar == '-')
			{
				errno = 0;
				int64_t integer = strtoll(bufferValue, &endChar, 10);

				if (bufferValue != endChar && errno == 0)
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

						errno = 0;
						char* fractionEndChar;
						int64_t fraction = strtoll(endChar, &fractionEndChar, 10);

						if (endChar != fractionEndChar && errno == 0 && 
							*fractionEndChar == '\n' && fraction >= 0)
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

			if (!converted)
			{
				if (bufferValueSize >= 5)
				{
					if (compareNoCase(bufferValue, "false", 5) == 0)
					{
						item.value.boolean = false;
						item.type = BOOLEAN_CONF_DATA_TYPE;
						converted = true;
					}
				}
				else if (bufferValueSize >= 4)
				{
					if (compareNoCase(bufferValue, "true", 4) == 0)
					{
						item.value.boolean = true;
						item.type = BOOLEAN_CONF_DATA_TYPE;
						converted = true;
					}
					else if (compareNoCase(bufferValue, "-inf", 4) == 0)
					{
						item.value.floating = -INFINITY;
						item.type = FLOATING_CONF_DATA_TYPE;
						converted = true;
					}
				}
				else if (bufferValueSize >= 3)
				{
					if (compareNoCase(bufferValue, "inf", 3) == 0)
					{
						item.value.floating = INFINITY;
						item.type = FLOATING_CONF_DATA_TYPE;
						converted = true;
					}
					else if (compareNoCase(bufferValue, "nan", 3) == 0)
					{
						item.value.floating = NAN;
						item.type = FLOATING_CONF_DATA_TYPE;
						converted = true;
					}
				}
			}

			if (!converted)
			{
				char* string = malloc(bufferSize * sizeof(char));
				if (!string)
				{
					free(item.key); free(buffer);
					destroyConfItems(items, itemCount);
					if (errorLine)
						*errorLine = lineIndex + 1;
					return FAILED_TO_ALLOCATE_CONF_RESULT;
				}

				memcpy(string, bufferValue, bufferValueSize * sizeof(char));
				string[bufferValueSize] = '\0';

				item.value.string.value = string;
				item.value.string.length = bufferValueSize;
				item.type = STRING_CONF_DATA_TYPE;
			}

			if (itemCount == itemCapacity)
			{
				itemCapacity *= 2;

				ConfItem* newItems = realloc(items,
					itemCapacity * sizeof(struct ConfItem));

				if (!newItems)
				{
					if (item.type == STRING_CONF_DATA_TYPE)
						free(item.value.string.value);
					free(item.key); free(buffer);
					destroyConfItems(items, itemCount);
					if (errorLine)
						*errorLine = lineIndex + 1;
					return FAILED_TO_ALLOCATE_CONF_RESULT;
				}

				items = newItems;
			}

			items[itemCount++] = item;
			if (currentChar == EOF || currentChar == '\0')
				break;

			item.keySize = 0;
			bufferSize = 0;
			lineIndex++;
			continue;
		}
		else if (currentChar == '#' && bufferSize == 0 && item.keySize == 0)
		{
			while(true)
			{
				currentChar = getNextChar(handle);
				if (currentChar == '\n' || currentChar == EOF || currentChar == '\0')
					break;
			}

			lineIndex++;
			continue;
		}
		else if (currentChar == '\r')
		{
			continue;
		}

		if (bufferSize == bufferCapacity)
		{
			bufferCapacity *= 2;

			char* newBuffer = realloc(buffer,
				bufferCapacity * sizeof(char));

			if (!newBuffer)
			{
				if (item.keySize > 0)
					free(item.key);
				free(buffer); destroyConfItems(items, itemCount);
				if (errorLine)
					*errorLine = lineIndex + 1;
				return FAILED_TO_ALLOCATE_CONF_RESULT;
			}

			buffer = newBuffer;
		}

		buffer[bufferSize++] = (char)currentChar;
	}

	free(buffer);

	for (size_t i = 0; i < itemCount; i++)
	{
		for (size_t j = 0; j < itemCount; j++)
		{
			if (i != j && !compareConfItems(&items[i], &items[j]))
			{
				destroyConfItems(items, itemCount);
				if (errorLine)
					*errorLine = 0;
				return REPEATING_KEYS_CONF_RESULT;
			}
		}
	}

	qsort(items, itemCount, sizeof(struct ConfItem), compareConfItems);

	*_items = items;
	*_itemCount = itemCount;
	return SUCCESS_CONF_RESULT;
}

static int onNextFileChar(void* handle)
{
	// NOTE: handle should not be NULL!
	// Skipping here assertion for debug build speed.
	return getc(handle);
}
ConfResult createFileConfReader(const char* filePath, ConfReader* confReader, size_t* errorLine)
{
	assert(filePath != NULL);
	assert(confReader != NULL);

	ConfReader confReaderInstance = malloc(sizeof(ConfReader_T));
	if (!confReaderInstance)
	{
		if (errorLine)
			*errorLine = 0;
		return FAILED_TO_ALLOCATE_CONF_RESULT;
	}

	FILE* file = openFile(filePath, "r");
	if (!file)
	{
		free(confReaderInstance);
		if (errorLine)
			*errorLine = 0;
		return FAILED_TO_OPEN_FILE_CONF_RESULT;
	}

	ConfItem* items;
	size_t itemCount;

	ConfResult result = createConfItems(onNextFileChar,
		file, &items, &itemCount, errorLine);
	closeFile(file);

	if (result != SUCCESS_CONF_RESULT)
	{
		free(confReaderInstance);
		return result;
	}

	confReaderInstance->items = items;
	confReaderInstance->itemCount = itemCount;
	*confReader = confReaderInstance;
	if (errorLine)
		*errorLine = 0;
	return SUCCESS_CONF_RESULT;
}

typedef struct ConfReaderIterator
{
	const char* data;
	size_t index;
} ConfReaderIterator;
static int onNextDataChar(void* handle)
{
	// NOTE: handle should not be NULL!
	// Skipping here assertion for debug build speed.
	ConfReaderIterator* iterator = handle;
	return iterator->data[iterator->index++];
}
ConfResult createDataConfReader(const char* data, ConfReader* confReader, size_t* errorLine)
{
	assert(data != NULL);
	assert(confReader != NULL);
	assert(errorLine != NULL);

	ConfReader confReaderInstance = malloc(sizeof(ConfReader_T));
	if (!confReaderInstance)
	{
		*errorLine = 0;
		return FAILED_TO_ALLOCATE_CONF_RESULT;
	}

	ConfReaderIterator iterator;
	iterator.data = data;
	iterator.index = 0;

	ConfItem* items;
	size_t itemCount;

	ConfResult result = createConfItems(onNextDataChar,
		&iterator, &items, &itemCount, errorLine);

	if (result != SUCCESS_CONF_RESULT)
	{
		free(confReaderInstance);
		return result;
	}

	confReaderInstance->items = items;
	confReaderInstance->itemCount = itemCount;
	*confReader = confReaderInstance;
	return SUCCESS_CONF_RESULT;
}

void destroyConfReader(ConfReader confReader)
{
	if (!confReader)
		return;
	destroyConfItems(confReader->items, confReader->itemCount);
	free(confReader);
}

bool getConfReaderType(ConfReader confReader, const char* key, ConfDataType* type)
{
	assert(confReader != NULL);
	assert(key != NULL);
	assert(type != NULL);

	ConfItem item;
	item.key = (char*)key;
	item.keySize = strlen(key);

	ConfItem* foundItem = bsearch(&item, confReader->items,
		confReader->itemCount, sizeof(struct ConfItem), compareConfItems);
	if (!foundItem)
		return false;

	*type = foundItem->type;
	return true;
}

bool getConfReaderInt(ConfReader confReader, const char* key, int64_t* value)
{
	assert(confReader != NULL);
	assert(key != NULL);
	assert(value != NULL);

	ConfItem item;
	item.key = (char*)key;
	item.keySize = strlen(key);

	ConfItem* foundItem = bsearch(&item, confReader->items,
		confReader->itemCount, sizeof(struct ConfItem), compareConfItems);

	if (!foundItem || foundItem->type != INTEGER_CONF_DATA_TYPE)
		return false;

	*value = foundItem->value.integer;
	return true;
}

bool getConfReaderFloat(ConfReader confReader, const char* key, double* value)
{
	assert(confReader != NULL);
	assert(key != NULL);
	assert(value != NULL);

	ConfItem item;
	item.key = (char*)key;
	item.keySize = strlen(key);

	ConfItem* foundItem = bsearch(&item, confReader->items,
		confReader->itemCount, sizeof(struct ConfItem), compareConfItems);

	if (!foundItem)
		return false;

	if (foundItem->type == FLOATING_CONF_DATA_TYPE)
	{
		*value = foundItem->value.floating;
		return true;
	}
	else if (foundItem->type == INTEGER_CONF_DATA_TYPE)
	{
		*value = foundItem->value.integer;
		return true;
	}
	return false;
}

bool getConfReaderBool(ConfReader confReader, const char* key, bool* value)
{
	assert(confReader != NULL);
	assert(key != NULL);
	assert(value != NULL);

	ConfItem item;
	item.key = (char*)key;
	item.keySize = strlen(key);

	ConfItem* foundItem = bsearch(&item, confReader->items,
		confReader->itemCount, sizeof(struct ConfItem), compareConfItems);

	if (!foundItem || foundItem->type != BOOLEAN_CONF_DATA_TYPE)
		return false;

	*value = foundItem->value.boolean;
	return true;
}

bool getConfReaderString(ConfReader confReader, const char* key, const char** value, uint64_t* length)
{
	assert(confReader != NULL);
	assert(key != NULL);
	assert(value != NULL);

	ConfItem item;
	item.key = (char*)key;
	item.keySize = strlen(key);

	ConfItem* foundItem = bsearch(&item, confReader->items,
		confReader->itemCount, sizeof(struct ConfItem), compareConfItems);

	if (!foundItem || foundItem->type != STRING_CONF_DATA_TYPE)
		return false;

	*value = foundItem->value.string.value;
	if (length)
		*length = foundItem->value.string.length;
	return true;
}