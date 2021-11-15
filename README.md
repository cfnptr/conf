## Features
* Configuration file reading

## Supported operating systems
* Ubuntu
* MacOS
* Windows

## Build requirements
* C99 compiler
* [CMake 3.10+](https://cmake.org/)

## Cloning
```
git clone https://github.com/cfnptr/conf
cd conf
git submodule update --init --recursive
```

## Building
```
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build/
cmake --build build/
```

## Example conf file
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
