dnl config.m4 for extension Weakref

PHP_ARG_ENABLE(weakreference_bc, enable Weakref suppport,
[  --enable-weakreference_bc     Enable Weakref])

if test "$PHP_WEAKREFERENCE_BC" != "no"; then
  PHP_NEW_EXTENSION(weakreference_bc, php_weakref.c  wr_store.c wr_weakref.c wr_weakmap.c, $ext_shared)
fi
