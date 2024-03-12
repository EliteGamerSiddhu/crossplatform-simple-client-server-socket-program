# Simple Crossplatfrom Server-Client program
This repository contains a simple socket program which is made from scratch using libraries provided at the OS level.
--------------------------------------------------------------------------------------------------------------------

It uses windows socket 2 technology for the Windows OS and UNIX based libraries for UNIX-like OS like Linux and Mac. This repo aims to use the base libraries provided at the OS level and no use of external libraries.

## How to build

If compiled in Windows, LINKER NEEDS TO INCLUDE ```winsock2.h``` and ```ws2tcpip.h```. These execution statements have been added to .vscode/task.json . Hence building the file through Visual Studio Code will work. For Visual Studio, pragma statements exists that also include the required files. If compilation is done through g++ compiler, then whole path of these 2 libraries needs to be provided.

If compiled in Linux, due to already existing standard C libraries for Unix-like OS, it will build.

## How to run

To run, first build both files, then run the built server.cpp file and then the built client.cpp file, together through different terminals.
