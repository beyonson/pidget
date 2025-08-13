<p align="center">
  <img src="images/banner.png" alt="Pidget banner"/>
</p>

<p align="center">
<b>Pidget (pet widget)</b>
</p>

<p align="center">
<a href="https://github.com/beyonson/pidget/graphs/contributors"><img src="https://img.shields.io/github/contributors/beyonson/pidget.svg"></a>
<a href="https://github.com/beyonson/pidget/issues"><img src="https://img.shields.io/github/issues/beyonson/pidget.svg"></a>
<a href="https://github.com/beyonson/pidget/issues?q=is%3Aissue%20state%3Aclosed"><img src="https://img.shields.io/github/issues-closed/beyonson/pidget.svg">
</p>

## Table of Contents

* [Getting Started](#getting-started)
  * [Installation](#installation)
  * [First Steps](#first-steps)

## Getting Started

### Installation

The following dependencies are needed to build pidget:
- libev
- libxcb
- libpng
- libcyaml
- xcb-errors

To build pidget run:
```
autoreconf -i
./configure
make
sudo make install
```

### First Steps

The default config file is provided in the root of this repository `config.yml`.
This file can be used as a template, and passed into pidget using the -c flag, for
example:

```
pidget -c ~/custom/path/my-config-file.yml
```
