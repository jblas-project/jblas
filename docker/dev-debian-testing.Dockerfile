# Docker build environment for Ubuntu 20.04

FROM debian:testing

RUN apt-get update
RUN apt-get -y install less htop build-essential vim ruby
RUN apt-get -y install gfortran libopenblas-serial-dev
RUN apt-get -y install openjdk-11-jdk-headless maven
#RUN update-alternatives --set java /usr/lib/jvm/java-11-openjdk-amd64/jre/bin/java

CMD mkdir /home/dev
WORKDIR /home/dev