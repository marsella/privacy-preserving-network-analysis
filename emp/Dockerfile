FROM ubuntu:20.04
WORKDIR /root

ENV TZ=America/New_York
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update
RUN apt-get install -y tzdata

RUN apt-get update && apt-get install -y \
  build-essential \
  cmake \
  git \
  libgmp-dev \
  libssl-dev \
  psmisc \
  sudo \
  software-properties-common \
  wget \
  vim \
  python \
  python3-pip
ADD source/ /root/source
ADD README.md .

ADD install_dependencies.sh .
RUN ["bash", "install_dependencies.sh"]

#ADD install.sh .
#RUN ["bash", "install.sh"]

ADD benchmarks/benchmark.sh .
RUN ["chmod", "u+x","benchmark.sh"]
#RUN ["bash", "benchmark.sh"]

CMD ["/bin/bash"]
