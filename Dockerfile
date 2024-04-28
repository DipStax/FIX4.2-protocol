FROM gcc

ENV FPDIR /FIX-protocol

WORKDIR /FIX-protocol/

RUN  apt-get update; apt-get install -y cmake;

ADD CMakeLists.txt CMakeLists.txt

ADD common/ common/

ADD server/ server/

ADD script/ script/

RUN python3 script/build.py

ENTRYPOINT ["./build/Server"]