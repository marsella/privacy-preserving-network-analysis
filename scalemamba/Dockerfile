FROM ubuntu:16.04
WORKDIR /root
RUN apt-get update && apt-get install -y \
  bzip2 \
  doxygen \
  g++ \
  gcc \
  git \
  libcrypto++-dev \
  libgmp3-dev \
  m4 \
  make \
  patch \
  python-pip \
  python \
  python3 \
  vim \
  wget \
  yasm \
  zip

ADD install_dependencies.sh /root/
RUN ["bash", "install_dependencies.sh"]

ADD source/ /root/source
ADD install.sh .
RUN ["bash", "install.sh"]

#ADD benchmarks/benchmark.sh /root/
#RUN ["chmod", "u+x", "/root/benchmark.sh"]
#RUN ["./benchmark.sh", "0", "0"]
#RUN ["./benchmark.sh", "0", "1"]
#RUN ["./benchmark.sh", "1", "1"]
