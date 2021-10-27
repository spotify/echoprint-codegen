#!/usr/bin/env bash
# 
# Simple recipe for building echoprint-codegen on Debian Buster. Called
# from our Cloudbuild config.
set -e

apt update
apt install -y build-essential ffmpeg libboost1.67-dev libtag1-dev zlib1g-dev 

# This thing needs JNI headers to be built, even if you're building only the CLI. 
# Fortunately for us it compiles with the version in Buster.
apt install -y openjdk-11-jdk
export JAVA_HOME="/usr/lib/jvm/java-11-openjdk-amd64/"

cd src
make