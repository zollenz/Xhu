#!/bin/sh
if [ "${CONFIGURATION}" = "Debug" ]; then
TARGET_BUILD_DIR=$PROJECT_DIR/build/modiqus/Build/Products/Debug
fi

if [ "${CONFIGURATION}" = "Release" ]; then
TARGET_BUILD_DIR=$PROJECT_DIR/build/modiqus/Build/Products/Release
fi

if [ ! -d ${TARGET_BUILD_DIR}/lib ]
then
mkdir ${TARGET_BUILD_DIR}/lib
fi

if [ ! -d ${TARGET_BUILD_DIR}/resources ]
then
mkdir ${TARGET_BUILD_DIR}/resources
fi

if [ "${CONFIGURATION}" = "Debug" ]; then
cp -R $PROJECT_DIR/ext/csound/debug/CsoundLib.framework ${TARGET_BUILD_DIR}/lib
#cp -R $PROJECT_DIR/ext/csound/debug/librtauhal.dylib ${TARGET_BUILD_DIR}/lib
fi

if [ "${CONFIGURATION}" = "Release" ]; then
cp -R $PROJECT_DIR/ext/csound/release/CsoundLib.framework ${TARGET_BUILD_DIR}/lib
#cp -R $PROJECT_DIR/ext/csound/release/librtauhal.dylib ${TARGET_BUILD_DIR}/lib
fi

#cp -R $PROJECT_DIR/ext/libsndfile.1.dylib ${TARGET_BUILD_DIR}/lib
cp -R $PROJECT_DIR/resources/csound ${TARGET_BUILD_DIR}/resources

