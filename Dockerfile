FROM openjdk:11

ENV JBLAS_VERSION=1.2.4-SNAPSHOT

RUN apt-get update
RUN apt-get -y install libgfortran5

COPY jblas-1.2.4-SNAPSHOT.jar .

CMD java -jar jblas-${JBLAS_VERSION}.jar