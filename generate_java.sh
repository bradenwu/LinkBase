#!/bin/bash

SRC_DIR=./
DST_DIR=java
protoc -I=$SRC_DIR --java_out=$DST_DIR ./linkbase.proto
