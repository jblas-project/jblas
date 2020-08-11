# Docker build environment for Ubuntu 20.04

# run as docker build -f test-ubuntu20.04.Dockerfile .

FROM ubuntu:20.04

ENV JBLAS_VERSION=1.2.4-SNAPSHOT

RUN apt-get update
RUN apt-get -y install libgfortran5
RUN apt-get -y install openjdk-8-jdk-headless
RUN update-alternatives --set java /usr/lib/jvm/java-8-openjdk-amd64/jre/bin/java

CMD mkdir /home/dev
WORKDIR /home/dev

COPY target/jblas-${JBLAS_VERSION}.jar .

CMD java -jar jblas-${JBLAS_VERSION}.jar