FROM ubuntu:20.04

# Set non-interactive mode for apt
ENV DEBIAN_FRONTEND=noninteractive

# Update and install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    ffmpeg \
    cmake \
    libopencv-dev \
    libsndfile1-dev \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy the entire project to the container
COPY . /app

RUN mkdir -p bin

# Build the C++ code
WORKDIR /app/bin
RUN cmake ../src && make

# Remove the src directory
RUN rm -rf /app/src

# Install Python requirements
WORKDIR /app/server
RUN pip3 install --no-cache-dir -r requirements.txt

# Expose port 5000 for the server
EXPOSE 5000

# Command to run the server
CMD ["python3", "server.py"]
