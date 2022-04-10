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
  python \
  python3 \
  vim \
  wget \
  yasm \
  zip

ADD install_dependencies.sh /root/
RUN ["bash", "install_dependencies.sh"]

ADD src/ /root/src
ADD install.sh .
RUN ["bash", "install.sh"]

ADD testondata.sh .
RUN ["bash", "testondata.sh"]
