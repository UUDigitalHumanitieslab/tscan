FROM proycon/lamachine@sha256:e2c8530455187acdcc6bb45f1f4a84bd40a5c92237c7ebb7683969fb838f27c4

COPY docker/ /deployment/
WORKDIR /deployment
RUN bash config.sh

# TODO: from source
WORKDIR /usr/local/src
RUN git clone https://github.com/UUDigitalHumanitieslab/tscan.git
COPY data/ /usr/local/src/tscan/data

WORKDIR /deployment
RUN bash add-alpino.sh
RUN bash build.sh
RUN bash apply-config.sh

# these static files take up most of the space (1.6 GB)
# linked through a volume bind instead
WORKDIR /usr/local/src/tscan
RUN rm -rf data
