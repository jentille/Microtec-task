# Use an official lightweight Ubuntu image
FROM ubuntu:22.04

# Install dependencies
RUN apt update && apt install -y \
    g++ \
    valgrind \
    clang \
    cmake \
    make \
    git \
    build-essential \
    nano \
    vim \
    && rm -rf /var/lib/apt/lists/*

# Set work directory inside the container
WORKDIR /app

# Copy everything into the container's /app directory
COPY . .

# Default build command (optional)
CMD ["bash"]
