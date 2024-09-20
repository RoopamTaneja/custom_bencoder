# Custom Bencoder

A small, single-header C++ serialization library for [bencoded][wikipedia] data.

The library supports serializing and deserializing bencoded data with a simple API. It utilizes smart pointers, STL containers, templates, and concepts for efficient memory management and type safety.

This was written to support [Project Name](repo_link) done as a group project for Computer Networks course.

It follows the specifications in the [standard](http://bittorrent.org/beps/bep_0003.html).

## Requirements

This library has no external dependencies and only requires a C++20 compiler.

It's been tested on [GCC][gcc] 10+ and [Clang][clang] 10+. (Remember to compile with -std=c++20 flag for g++ and -std=c++20 and -libc++ flags for clang as and when required depending on the compiler version.)

## Installation

Since custom_bencoder is a single-file, header-only library, you can just
copy `include/custom_bencoder.h` to your destination of choice.

## Usage

### Data types