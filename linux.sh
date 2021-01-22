#!/bin/bash

echo "FX3 SDK for linux setup..."

cp -f Device-DEV-PERSISTENT_USB/.cproject_linux Device-DEV-PERSISTENT_USB/.cproject
cp -f Device-DEV/.cproject_linux Device-DEV/.cproject
cp -f Device-PPC-PERSISTENT_USB/.cproject_linux Device-PPC-PERSISTENT_USB/.cproject
cp -f Device-PPC/.cproject_linux Device-PPC/.cproject
cp -f Host-DEV/.cproject_linux Host-DEV/.cproject
cp -f Host-PPC/.cproject_linux Host-PPC/.cproject

echo "FX3 SDK for linux done"
