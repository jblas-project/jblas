# Docker build environment for Ubuntu 20.04

FROM ubuntu:20.04

RUN apt-get update
RUN apt-get -y install less htop build-essential vim ruby
RUN apt-get -y install openjdk-8-jdk-headless maven ant
RUN apt-get -y install gfortran libopenblas-serial-dev
RUN update-alternatives --set java /usr/lib/jvm/java-8-openjdk-amd64/jre/bin/java

CMD mkdir /home/dev
WORKDIR /home/dev