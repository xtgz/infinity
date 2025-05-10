FROM nvidia/cuda:11.8.0-base-ubuntu22.04
ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /app

# Install dependencies and Python 3.10 (default on ubuntu 22.04)
RUN apt-get update && apt-get install -y \
    g++ \
    make \
    git \
    ocl-icd-opencl-dev \
    libopencl-clang-dev \
    curl \
    python3 \
    python3-pip \
    clinfo \
    nano

# Install Python packages for Python 3.10
RUN pip3 install \
    pybind11 \
    safe-pysha3 \
    ecdsa \
    web3 \
    coincurve \
    websocket-client \
    websockets \
    dotenv 

    # Configure OpenCL ICD for NVIDIA
RUN mkdir -p /etc/OpenCL/vendors && \
    echo "libnvidia-opencl.so.1" > /etc/OpenCL/vendors/nvidia.icd

COPY . ./

RUN make clean && make 

CMD ["/bin/bash"]