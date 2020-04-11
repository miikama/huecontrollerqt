#!/bin/bash

#   Downloads and builds 5.12 QT libraries.
#
#   Compiles and packages our application for use.

VERSION="0.1"
APP_NAME="HueControllerQT"

# stop at first fail
set -e

# this script sould be run in the project root directory
test -f main.cpp || { echo "No main.cpp found. Run in the HueControllerQT project root directory."; exit 1; }

ROOT_DIR=$(pwd)
BUILD_DIR=$ROOT_DIR/build

# create a build directory if not existing and go there
test -d build || mkdir build
cd build



################################################################
#                    build static QT
################################################################

initRepo() {

    git clone https://code.qt.io/qt/qt5.git
    cd qt5
    git checkout 5.12

    # get used submodules
    perl init-repository --module-subset=default
    #, -qtwebengine

    # get back
    cd $BUILD_DIR
}


# clone repo if not yet existing
test -d qt5 || initRepo

# compile static QT in a parallel directory
test -d static_build && rm -r static_build
mkdir static_build

test -d install && rm -r install
mkdir install

cd static_build

../qt5/configure -y -static \
                -prefix "$BUILD_DIR/install" \
                -release -ltcg -optimize-size -no-pch \
                -opensource \
                -nomake tools \
                -nomake examples \
                -nomake tests \
                -skip qtwebengine \
                -no-feature-geoservices_esri \
                -no-feature-geoservices_here \
                -no-feature-geoservices_itemsoverlay \
                -no-feature-geoservices_mapbox \
                -no-feature-geoservices_mapboxgl \
                -no-feature-geoservices_osm

# compile and install static libraries
CORE_COUNT=$(nproc)
make -j $CORE_COUNT && make install && echo "\n\nQT built successfully!\n\n" && sleep 0.5


################################################################
#                    Actually compile app
################################################################

cd $BUILD_DIR
$BUILD_DIR/install/bin/qmake -config release .. && make -j $CORE_COUNT

# copy app into install directory
cp $APP_NAME $ROOT_DIR/install

# and package the compiled app to original directory
tar -cf HueControllerQT_$VERSION.tar $APP_NAME
mv HueControllerQT_$VERSION.tar $ROOT_DIR





