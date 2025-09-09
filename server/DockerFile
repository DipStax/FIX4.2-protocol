FROM gcc:13.4.0 AS build

WORKDIR /fix42-build

# Install build tools
RUN apt-get update && apt-get install -y cmake make && rm -rf /var/lib/apt/lists/*

# Copy sources
COPY CMakeLists.txt .
COPY server/ server/
COPY shared/ shared/

# Build
WORKDIR /fix42-build/build

RUN cmake -DServerBuild=TRUE ..
RUN make -j

FROM debian:stable-slim

WORKDIR /app

COPY --from=build /fix42-build/build/server/Fix_Server .

CMD ["./Fix_Server"]
