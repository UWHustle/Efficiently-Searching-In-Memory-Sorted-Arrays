FROM ubuntu:14.04

# update
RUN sudo apt-get -y update

# ssh
RUN apt-get install -y curl openssh-server
RUN ssh-keygen -q -N "" -t rsa -f /root/.ssh/id_rsa
RUN cp /root/.ssh/id_rsa.pub /root/.ssh/authorized_keys

# java
RUN mkdir -p /usr/java/default && \
    curl -Ls 'http://download.oracle.com/otn-pub/java/jdk/7u51-b13/jdk-7u51-linux-x64.tar.gz' -H 'Cookie: oraclelicense=accept-securebackup-cookie' | \
    tar --strip-components=1 -xz -C /usr/java/default/

ENV JAVA_HOME /usr/java/default/
ENV PATH $PATH:$JAVA_HOME/bin

# python
RUN sudo apt-get -y install python python-apsw

# exareme
RUN apt-get -y install git maven
RUN git clone https://github.com/madgik/exareme.git /root/exareme-src
WORKDIR /root/exareme-src
RUN mvn clean install -DskipTests
RUN mv /root/exareme-src/exareme-distribution/target/exareme* /root/exareme

EXPOSE 9090
WORKDIR /root/exareme

# bootstrap
ADD bootstrap.sh /root/exareme/bootstrap.sh
ENTRYPOINT /bin/bash bootstrap.sh