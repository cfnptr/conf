# Conf

A [library](https://github.com/cfnptr/conf) providing API for **configuration** file reading and writing.

See the [documentation](https://cfnptr.github.io/conf).

## Features

* Simple configuration syntax (similar to .ini)
* Automatic variable parsing (int, float, bool, string)
* Built-in configuration syntax validation
* C and C++ implementations

## Usage example

```cpp
void confReaderExampleCPP()
{
    conf::Reader confReader("settings.txt");

    int64_t someValue = 0;
    if (confReader.get("someValue", someValue))
    {
       // use value...
    }
}

void confWriterExampleCPP()
{
    conf::Writer confWriter("settings.txt");
    confWriter.writeComment("Settings file (v1.0.0)");
    confWriter.writeNewLine();
    confWriter.write("someValue", 1337);
}
```

```c
void confReaderExampleC()
{
    ConfReader confReader = NULL;
    ConfResult confResult = createFileConfReader("settings.txt", &confReader, NULL);
    if (confResult != SUCCESS_CONF_RESULT) abort();

    int64_t someValue = 0;
    if (getConfReaderInt(confReader, "someValue", &someValue))
    {
        // use value...
    }

    destroyConfReader(confReader);
}

void confWriterExampleC()
{
    ConfWriter confWriter = NULL;
    ConfResult confResult = createFileConfWriter("settings.txt", &confWriter);
    if (confResult != SUCCESS_CONF_RESULT) abort();

    bool writeResult = writeConfComment(confWriter, "Settings file (v1.0.0)");
    writeResult |= writeConfNewLine(confWriter);
    writeResult |= writeConfInt(confWriter, "someValue", 1337);
    if (!writeResult) abort();

    destroyConfWriter(confReader);
}
```

## Configuration file example

```
# Conf comment syntax example
integerValue = 123
NegativeOne = -1

# Supported floating values
FloatingPI = 3.141
minus Infinity = -INF
Oh nooo = NaN

# Also some booleans
is.planet.round = true
caseSensitive = False

# And everything else is strings
hackingExploit = Hello world!
Not recommended key example?  = Yes :)
```

## Supported operating systems

* Windows
* macOS
* Ubuntu (Linux)

## Build requirements

* C99 compiler
* C++17 compiler (optional)
* [Git 2.30+](https://git-scm.com/)
* [CMake 3.16+](https://cmake.org/)

Use building [instructions](BUILDING.md) to install all required tools and libraries.

### CMake options

| Name              | Description               | Default value |
|-------------------|---------------------------|---------------|
| CONF_BUILD_SHARED | Build Conf shared library | `ON`          |
| CONF_BUILD_TESTS  | Build Conf library tests  | `ON`          |

### CMake targets

| Name        | Description          | Windows | macOS    | Linux |
|-------------|----------------------|---------|----------|-------|
| conf-static | Static Conf library  | `.lib`  | `.a`     | `.a`  |
| conf-shared | Dynamic Conf library | `.dll`  | `.dylib` | `.so` |

## Cloning

```
git clone --recursive https://github.com/cfnptr/conf
```

## Building ![CI](https://github.com/cfnptr/pack/actions/workflows/cmake.yml/badge.svg)

* Windows: ```./scripts/build-release.bat```
* macOS / Ubuntu: ```./scripts/build-release.sh```

## Third-party

* [mpio](https://github.com/cfnptr/mpio/) (Apache-2.0 License)