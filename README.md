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
  * [Dependencies](#dependencies)
  * [Installation](#installation)
  * [First Steps](#first-steps)

## Getting Started

### Dependencies

The following dependencies are needed to build pidget:
- automake
- autoconf
- libev
- libxcb
- libxcb-util
- libxcb-icccm
- libxcb-image
- libpng
- libcyaml
- libyaml
- libpng
- libxcb-errors

To install dependencies on Ubuntu, run:

```
sudo apt install automake autoconf libxcb-errors-dev libxcb-icccm4-dev libxcb-image0-dev \
     libxcb-util-dev libpng-dev libcyaml-dev libev-dev libyaml-dev
```

A compositor is also required to render in transparent backgrounds of pet images. Pidget 
has been tested with picom (`sudo apt install picom`).

### Installation

To build pidget run:
```
aclocal
autoreconf -i
./configure
make
sudo make install
```

### First Steps

To launch Pidget, simply run:
```
pidget
```

The default config file is provided in the root of this repository `config.yml`.
This file can be used as a template, and passed into pidget using the -c flag, for
example:

```
pidget -c ~/custom/path/my-config-file.yml
```
