ARG PHP_IMAGE
ARG PHP_VERSION
FROM $PHP_IMAGE:$PHP_VERSION
WORKDIR /code
# Used for running tests in Docker
# RUN apt-get update && apt-get install -y valgrind && apt-get clean
# NOTE: In order to avoid valgrind false positives, this would need to compile php from source and configure php --with-valgrind (php-src's zend_string_equals uses inline assembly that causes false positives)
# - ci/install_php_custom.sh and ci/generate_php_install_dir.sh may be a useful reference for that.
ADD *.sh *.c *.h *.php *.md config.m4 config.w32 package.xml LICENSE CREDITS ./

# Assume compilation will be the time consuming step.
# Add tests after compiling so that it's faster to update tests and re-run them locally.
#
# Use extremely strict CFLAGS for checking correctness of code before merging it.
# docker hub 7.0 image is too old for -Wformat-truncation  -Wimplicit-fallthrough=1
RUN export CFLAGS='-fno-common -Wlogical-op -Wduplicated-cond -Wall -Wextra -Wno-unused-parameter -O2 -g'; phpize && ./configure && make -j$(nproc) && make install
RUN docker-php-ext-enable weakreference_bc
ADD tests ./tests
ADD ci ./ci
