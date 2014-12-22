#!/bin/bash
mkdir libcanopy-0.9
cp -r include libcanopy-0.9
cp -r src libcanopy-0.9
cp *.txt makefile libcanopy-0.9
tar -cvzf libcanopy_0.9.tar.gz libcanopy-0.9
