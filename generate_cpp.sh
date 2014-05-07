#!/bin/bash

SRC_DIR=./
DST_DIR=cpp
protoc -I=$SRC_DIR --cpp_out=$DST_DIR ./linkbase.proto
cp cpp/linkbase.pb.* tools/
