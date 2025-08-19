# Stage 1: Build the application
FROM gcc:latest AS builder

# Install dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    git \
    libssl-dev \
    meson \
    libunistring-dev

# Clone and build cpr
RUN git clone https://github.com/libcpr/cpr.git /usr/src/cpr && \
    cd /usr/src/cpr && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make && \
    make install

# Clone and build nlohmann/json
RUN git clone https://github.com/nlohmann/json.git /usr/src/json && \
    cd /usr/src/json && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make && \
    make install

# Download httplib.h
RUN git clone https://github.com/yhirose/cpp-httplib.git /usr/src/cpp-httplib && \
    cp /usr/src/cpp-httplib/httplib.h /usr/local/include/

# Copy source code
WORKDIR /app
COPY src/ ./src
COPY include/ ./include
COPY main.cpp .

# Compile the application
RUN g++ -std=c++17 -I/app/include src/*.cpp main.cpp -o keepalive -I/usr/local/include -L/usr/local/lib -lcpr -lcurl -lpthread

# Stage 2: Create the final image
FROM debian:trixie-slim
WORKDIR /app
COPY --from=builder /app/keepalive .

# Install runtime dependencies
RUN apt-get update && apt-get install -y libcurl4 libssl3 libstdc++6 && rm -rf /var/lib/apt/lists/*

# Copy CPR library from builder
COPY --from=builder /usr/local/lib/libcpr.so* /usr/local/lib/
COPY --from=builder /usr/local/lib/libcpr.so.1 /usr/local/lib/

# Update library cache
RUN ldconfig

EXPOSE 8080

CMD ["./keepalive"]