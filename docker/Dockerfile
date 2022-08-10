FROM ubuntu:22.04

RUN useradd -u 1000 -ms /bin/bash must

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Berlin

RUN apt-get update \
    && apt-get -y -qq --no-install-recommends install \
    cmake \
    make \
    mpich \
    libmpich-dev \
    libxml2-dev \
    python3 \
    python3-pip \
    gfortran \
    gcc \
    g++ \
    git \
    graphviz \
    libgtest-dev \
    clang-12 \
    libomp-12-dev \
    llvm-12-tools \
    llvm-12-dev \
    && apt-get -yq clean \
    && rm --recursive --force /var/lib/apt/lists/*

RUN pip3 install --no-input --no-cache-dir --disable-pip-version-check 'lit==14.0.0'

# ensure that LLVM 12 toolset is used
RUN ln -s /usr/bin/FileCheck-12 /usr/bin/FileCheck
RUN ln -s /usr/bin/clang-12 /usr/bin/clang
RUN ln -s /usr/bin/clang++-12 /usr/bin/clang++

USER must