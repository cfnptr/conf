# CONF ![CI](https://github.com/cfnptr/conf/actions/workflows/cmake.yml/badge.svg)

A library providing **configuration** API for file and data reading / writing.

## Features

* Simple configuration syntax (similar to .ini)
* Automatic variable parsing (int, float, bool)
* Built-in configuration syntax validation

## Supported operating systems

* Ubuntu
* MacOS
* Windows

## Build requirements

* C99 compiler
* [Git 2.30+](https://git-scm.com/)
* [CMake 3.16+](https://cmake.org/)

### CMake options

| Name                | Description                | Default value |
|---------------------|----------------------------|---------------|
| CONF_BUILD_SHARED   | Build CONF shared library  | `ON`          |
| CONF_BUILD_TESTS    | Build CONF library tests   | `ON`          |

## Cloning

```
git clone --recursive https://github.com/cfnptr/conf
```

## Configuration example

```
# Conf comment syntax example
integerValue=123
NegativeOne=-1

# Supported floating values
FloatingPI=3.141
minus Infinity=-INF
Oh nooo=NaN

# Also some booleans
is.planet.round=true
caseSensitive=False

# And everything else is strings
hackingExploit=Hello world!
Not recommended key example?= Yes :)
```

## Third-party

* [mpio](https://github.com/cfnptr/mpio/) (Apache-2.0 License)
