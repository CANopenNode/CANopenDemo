#!/bin/bash

echo "Cleaning existing html documentation..."
rm -r CANopenNode.github.io/*
rm -r CANopenLinux/html
rm -r CANopenPIC/html
rm -r CANopenADI/html
rm -r CANopenLinux/CANopenNode/doc/html
rm index.html

echo "Generating documentation for CANopenNode..."
cd CANopenLinux/CANopenNode
doxygen > /dev/null

echo "Generating documentation for CANopenLinux..."
cd ..
doxygen > /dev/null

echo "Generating documentation for CANopenPIC..."
cd ../CANopenPIC/CANopenNode
doxygen > /dev/null
cd ..
doxygen > /dev/null

echo "Generating documentation for CANopenADI..."
cd ../CANopenADI/CANopenNode
doxygen > /dev/null
cd ..
doxygen > /dev/null

echo "Generating documentation for CANopenDemo..."
cd ..
doxygen > /dev/null

echo "Copying files and add links to menudata..."
mkdir CANopenNode.github.io/CANopenNode
cp -r CANopenLinux/CANopenNode/doc/html/* CANopenNode.github.io/CANopenNode
awk '/var menudata=/ { print; print "{text:\"CANopenDemo\",url:\"../index.html\"},"; next }1' CANopenNode.github.io/CANopenNode/menudata.js > menudata.js.tmp
mv menudata.js.tmp CANopenNode.github.io/CANopenNode/menudata.js

mkdir CANopenNode.github.io/CANopenLinux
cp -r CANopenLinux/html/* CANopenNode.github.io/CANopenLinux
awk '/var menudata=/ { print; print "{text:\"CANopenDemo\",url:\"../index.html\"},"; next }1' CANopenNode.github.io/CANopenLinux/menudata.js > menudata.js.tmp
mv menudata.js.tmp CANopenNode.github.io/CANopenLinux/menudata.js

mkdir CANopenNode.github.io/CANopenPIC
cp -r CANopenPIC/html/* CANopenNode.github.io/CANopenPIC
awk '/var menudata=/ { print; print "{text:\"CANopenDemo\",url:\"../index.html\"},"; next }1' CANopenNode.github.io/CANopenPIC/menudata.js > menudata.js.tmp
mv menudata.js.tmp CANopenNode.github.io/CANopenPIC/menudata.js

mkdir CANopenNode.github.io/CANopenADI
cp -r CANopenADI/html/* CANopenNode.github.io/CANopenADI
awk '/var menudata=/ { print; print "{text:\"CANopenDemo\",url:\"../index.html\"},"; next }1' CANopenNode.github.io/CANopenADI/menudata.js > menudata.js.tmp
mv menudata.js.tmp CANopenNode.github.io/CANopenADI/menudata.js

echo "Create softlink index.html..."
ln -s CANopenNode.github.io/index.html

echo "Press enter to continue..."
read
