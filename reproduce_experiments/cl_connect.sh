#!/usr/bin/env bash

ssh -N -L localhost:8833:localhost:8833 cl
#jupyter notebook --port=8833 --no-browser