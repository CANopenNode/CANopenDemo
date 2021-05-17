#!/bin/bash

git_version=$(git describe --tags --long --dirty)
sed -i "/CO_SW_VERSION/ s/\".*\"/\""$git_version"\"/" CO_ident_defs.h
