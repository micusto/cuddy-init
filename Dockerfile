FROM debian:jessie

RUN apt-get update
RUN apt-get install -y gcc

RUN mkdir -p /work
