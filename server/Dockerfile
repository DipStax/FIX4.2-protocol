FROM gcc:13.4.0 AS build

WORKDIR /fix42-build

RUN apt-get update && apt-get install -y cmake make && rm -rf /var/lib/apt/lists/*

COPY CMakeLists.txt .
COPY server/ server/
COPY shared/ shared/

WORKDIR /fix42-build/build

RUN cmake -DServerBuild=TRUE ..
RUN make -j

FROM debian:stable-slim

WORKDIR /app

COPY --from=build /fix42-build/build/server/Fix_Server .
COPY --from=build /fix42-build/build/server/config.json .

CMD ["./Fix_Server", "config.json"]
