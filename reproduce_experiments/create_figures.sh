#!/usr/bin/env bash

echo $'\n\n########################  Creating figures  ########################'
jupyter nbconvert  --execute CreateFigures.ipynb
