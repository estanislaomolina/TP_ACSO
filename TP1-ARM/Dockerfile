FROM ubuntu:20.04
RUN apt update -y && apt upgrade -y
RUN apt install gcc valgrind make -y
COPY . /tp
WORKDIR /tp
# CMD make valgrind