FROM gcc

ENV FPDIR /FIX-protocol

WORKDIR /FIX-protocol/

RUN  apt-get update; apt-get install -y cmake;

ADD CMakeLists.txt CMakeLists.txt

ADD common/ common/

ADD server/ server/

RUN cmake -DServerBuild=TRUE CMakeLists.txt
RUN make

ENTRYPOINT ["/MarketServer"]