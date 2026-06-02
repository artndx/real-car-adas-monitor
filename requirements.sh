#!/usr/bin/env bash

set -e

echo "=== Updating system ==="
sudo apt-get update

echo "=== Installing base dev tools ==="
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    gdb \
    wget \
    curl \
    unzip \
    python3 \
    python3-pip \
    v4l-utils

echo "=== Installing GCC 13 ==="

# На Ubuntu 22.04+ может потребоваться toolchain PPA
sudo apt-get install -y software-properties-common

sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y
sudo apt-get update

sudo apt-get install -y \
    gcc-13 \
    g++-13

# Doxygen для документации
sudo apt install doxygen

# Делаем GCC 13 дефолтным (опционально)
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 130
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 130

echo "GCC version:"
gcc --version

echo "=== Installing OpenCV and GTK ==="
sudo apt-get install -y \
    libopencv-dev \
    libgtk-3-0

echo "=== Installing Qt/XCB dependencies ==="
sudo apt-get install -y \
    libx11-xcb1 \
    libxkbcommon-x11-0 \
    libxcb-icccm4 \
    libxcb-image0 \
    libxcb-keysyms1 \
    libxcb-randr0 \
    libxcb-render-util0 \
    libxcb-shape0 \
    libxcb-xfixes0 \
    libxcb-xinerama0 \
    libxcb-xkb1 \
    libxkbcommon0 \
    libfontconfig1 \
    libfreetype6 \
    libdbus-1-3

echo "=== Installing ONNX Runtime ==="

ONNX_VERSION="1.24.3"
TMP_DIR="/tmp/onnxruntime"

mkdir -p "$TMP_DIR"
cd "$TMP_DIR"

wget -q "https://github.com/microsoft/onnxruntime/releases/download/v${ONNX_VERSION}/onnxruntime-linux-x64-${ONNX_VERSION}.tgz"

tar -xzf "onnxruntime-linux-x64-${ONNX_VERSION}.tgz"

sudo mv "onnxruntime-linux-x64-${ONNX_VERSION}" /opt/onnxruntime

echo "Cleaning up..."
rm -rf "$TMP_DIR"

echo "=== Done ==="
echo "ONNX Runtime installed at /opt/onnxruntime"