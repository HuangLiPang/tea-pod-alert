#!/bin/bash
tockloader erase-apps

echo "install led...\n\n"
pushd led && make && tockloader install && popd

echo "install temperature sensor...\n\n"
pushd temperature && make && tockloader install && popd

echo "install logic...\n\n"
pushd logic && make && tockloader install && popd

echo "install completed!\n"
echo "show application list\n"
tockloader list