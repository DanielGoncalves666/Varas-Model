#!/bin/bash

gcc -o build/varas.exe src/*.c -lm -Wall && ./build/varas.exe "$@"