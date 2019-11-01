#!/usr/bin/env bash

echo "Creating figures"
jupyter nbconvert  --execute CreateFigures.ipynb
