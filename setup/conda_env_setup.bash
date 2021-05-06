#!/usr/bin/env bash

function install_common_packages() {
    conda install -y pkg-config cmake cfitsio cppzmq eigen boost mkl mkl-include gsl fftw
    conda env config vars set MKLROOT="$CONDA_PREFIX"
    conda env config vars set PKG_CONFIG_PATH="$CONDA_PREFIX/share/pkgconfig:$CONDA_PREFIX/lib/pkgconfig:$PKG_CONFIG_PATH"
}
