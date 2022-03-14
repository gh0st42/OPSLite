#!/bin/sh

../out/clang-release/src/OPSLite -m -u Qtenv -n .:../src:../../inet4.3/src:../../inet4.3/examples:../../inet4.3/tutorials:../../inet4.3/showcases --image-path=../../inet4.3/images -l ../../inet4.3/src/INET omnetpp.ini
