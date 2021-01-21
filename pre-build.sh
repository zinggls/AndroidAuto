#!/bin/bash

rm -rf *.o  *.d *.elf *.img
cd ../../
git log --pretty=format:"#define GIT_INFO_PRESENT %nstatic const char* GIT_INFO = \"Version Information=%H\"; " -n 1 > gitcommit.h
