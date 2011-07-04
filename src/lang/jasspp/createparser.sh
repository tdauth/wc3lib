#!/bin/bash
flex -v --c++ jasspp.ll
bison -v --language=c++ jasspp.yy