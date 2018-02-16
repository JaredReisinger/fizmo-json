# This file is part of fizmo-json.  Please see LICENSE.md for the license.

SUBDIRS = src/fizmo-json
man6_MANS = src/man/fizmo-json.6

# Add docker targets...
# TODO: Figure out how to parameterize the docker image name: jaredreisinger/fizmo-json...
image:
	docker build \
		-t jaredreisinger/${PACKAGE_NAME}:${PACKAGE_VERSION} \
		-t jaredreisinger/${PACKAGE_NAME}:latest \
		.
.PHONY: image

shell:
	docker run \
		--rm \
		--interactive \
		--tty \
		jaredreisinger/${PACKAGE_NAME}:latest \
		/bin/sh
.PHONY: shell
