#!/usr/bin/env bash
# 
# Simple recipe for building echoprint-codegen on a Debian system. Called
# from our Cloudbuild config.
set -e

apt update
apt install -y build-essential ffmpeg libboost1.42-dev libtag1-dev zlib1g-dev
cd src
make