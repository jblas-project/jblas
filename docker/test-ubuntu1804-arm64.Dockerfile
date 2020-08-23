# Docker build environment for Ubuntu 20.04

# run as docker build -f test-ubuntu2004.Dockerfile .

FROM ubuntu:18.04

ENV JBLAS_VERSION=1.2.5-SNAPSHOT

RUN apt-get update
RUN apt-get -y install openjdk-8-jdk-headless
RUN update-alternatives --set java /usr/lib/jvm/java-8-openjdk-arm64/jre/bin/java

RUN mkdir /home/dev
WORKDIR /home/dev

COPY target/jblas-${JBLAS_VERSION}.jar .

RUN java -jar jblas-${JBLAS_VERSION}.jar