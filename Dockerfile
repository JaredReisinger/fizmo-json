FROM alpine:latest

# We can use ENV or ARG to define version variables, but the best-practices
# doc suggests using ENV for this purpose.
# https://docs.docker.com/engine/userguide/eng-image/dockerfile_best-practices/#env

ENV LIBFIZMO_TAG="libfizmo_0-7-15" \
    JANSSON_TAG="v2.10" \
    FIZMO_JSON_TAG="v0.0.1"

LABEL maintainer="jaredreisinger@hotmail.com" \
    libfizmo.tag="${LIBFIZMO_TAG}" \
    jansson.tag="${JANSSON_TAG}" \
    fizmo-json.tag="${FIZMO_JSON_TAG}"

# no patches at the moment!
# COPY *.patch /tmp/patches/

# until the repo is up... or never?
COPY . /tmp/fizmo-json/

RUN apk add --no-cache ca-certificates

RUN set -eux; \
    echo "============================================================"; \
    echo "acquire tools..."; \
    echo "============================================================"; \
	apk add --no-cache --virtual .build-deps \
        autoconf \
        automake \
        g++ \
        gcc \
        git \
        libtool \
        make \
        musl-dev \
	; \
    echo "============================================================"; \
    echo "acquire sources..."; \
    echo "============================================================"; \
    mkdir -p /tmp; \
    cd /tmp; \
    git clone --depth 1 --branch "${LIBFIZMO_TAG}" https://github.com/chrender/libfizmo.git; \
    git clone --depth 1 --branch "${JANSSON_TAG}" https://github.com/akheron/jansson.git; \
    # git clone --depth 1 --branch "${FIZMO_JSON_TAG}" https://github.com/JaredReisinger/fizmo-json.git; \
    echo "apply patches (if any)..."; \
    for p in /tmp/patches/*.patch; do \
        if [ -f "$p" ]; then \
            base=$(basename "$p" .patch); \
            dir=$(echo "$base" | sed -e 's/^\(.*\)\.[0-9]*$/\1/'); \
            cd "$dir"; \
            patch -p 1 -i "$p"; \
            cd ..; \
        fi; \
    done; \
    echo "============================================================"; \
    echo "build..."; \
    echo "============================================================"; \
    echo "------------------------------------------------------------"; \
    echo "libfizmo"; \
    echo "------------------------------------------------------------"; \
    cd /tmp/libfizmo; \
    autoreconf --force --install; \
    ./configure \
        --disable-babel \
        --disable-command-history \
        --disable-filelist \
        --disable-output-history \
        --disable-config-files \
        --disable-prefix-commands \
        ; \
    make install-dev; \
    echo "------------------------------------------------------------"; \
    echo "jansson"; \
    echo "------------------------------------------------------------"; \
    cd /tmp/jansson; \
    autoreconf --force --install; \
    ./configure; \
    make install; \
    echo "------------------------------------------------------------"; \
    echo "fizmo-json"; \
    echo "------------------------------------------------------------"; \
    cd /tmp/fizmo-json; \
    autoreconf --force --install; \
    ./configure LDFLAGS=-static; \
    make install; \
    echo "============================================================"; \
    echo "cleanup"; \
    echo "============================================================"; \
    cd /tmp; \
    echo "remove patches/build files..."; \
    apk del .build-deps; \
    rm -rf /tmp/*; \
    echo "============================================================"; \
    echo "set up games directory"; \
    mkdir -p /usr/local/games; \
    echo "DONE"

# COPY play /usr/local/bin/.

VOLUME /usr/local/games
WORKDIR /usr/local/games
