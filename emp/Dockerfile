FROM ubuntu:16.04
WORKDIR /root
RUN apt-get update && apt-get install -y \
  build-essential \
  cmake \
  git \
  libgmp-dev \
  libssl-dev \
  psmisc \
  python-pip \
  sudo \
  software-properties-common \
  wget \
  vim
ADD source/ /root/source
ADD README.md .

ADD install_dependencies.sh .
RUN ["bash", "install_dependencies.sh"]

ADD install.sh .
RUN ["bash", "install.sh"]

ADD benchmarks/benchmark.sh .
RUN ["chmod", "u+x","benchmark.sh"]
#RUN ["bash", "benchmark.sh"]

CMD ["/bin/bash"]
