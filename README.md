# oastat_static_web_generator

A program to generate static HTML files like the ones from pyqscore but from oastat data.
OaStat: https://github.com/sago007/oastat

An example can be seen on: https://oastat.poulsander.com/oastat_static_web/

## Cloning and building
This repository has a sub modules with levelshots and modelshots. Therefore it must be cloned like:
```
git clone --recurse-submodules https://github.com/sago007/oastat_static_web_generator.git
```
Building:
```
cmake .
make
```

## Usage

The program expects the environment variable SAGO_CONNECTION_STRING to be set and pointing to a database populated by OaStat.
Only mysql has been tested.
```
SAGO_CONNECTION_STRING="mysql:database=oastat;user=openarena;pass=Password1234"  ./oastat_static_web_generator --output-dir ./output
```

## LICENSE
The code is GPLv2 or later.

Contains data from Openarena.

Templates and css are under the MIT license.
