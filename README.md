# The weakreference_bc PECL extension

[![Build Status](https://github.com/TysonAndre/pecl-weakreference_bc/actions/workflows/main.yml/badge.svg?branch=master)](https://github.com/TysonAndre/pecl-weakreference_bc/actions/workflows/main.yml?query=branch%3Amaster)
[![Build Status (Windows)](https://ci.appveyor.com/api/projects/status/5duym2sdcy348e67?svg=true)](https://ci.appveyor.com/project/TysonAndre/pecl-weakreference-bc)


**[weakreference_bc](https://github.com/TysonAndre/pecl-weakreference_bc) is a fork of the unmaintained https://pecl.php.net/weakref PECL. The `weakreference_bc` PECL changes the API to provide polyfills for [WeakReference](https://www.php.net/manual/en/class.weakreference.php)/[WeakMap](https://www.php.net/manual/en/class.weakmap.php)**

A weak reference provides a gateway to an object without preventing that object
from being collected by the garbage collector (GC). It allows to associate
information to volatile object. It is useful to associate metadata or cache
information to objects. Indeed, the cache entry should not be preventing the
garbage collection of the object AND the cached info when the object is no
longer used.

## WeakReference
The WeakReference class is a simple class that allows to access its referenced object
as long as it exists. Unlike other references, having this WeakReference object will
not prevent the object to be collected.

See https://www.php.net/manual/en/class.weakreference.php

```php
<?php
class MyClass {
    public function __destruct() {
        echo "Destroying object!\n";
    }
}

$o1 = new MyClass;

$r1 = WeakReference::create($o1);

if (is_object($r1->get())) { // It does have a reference
    echo "Object still exists!\n";
    var_dump($r1->get());
} else {
    echo "Object is dead!\n";
}

unset($o1);

if (is_object($r1->get())) { // It doesn't have a reference
    echo "Object still exists!\n";
    var_dump($r1->get());
} else {
    echo "Object is dead!\n";
}
?>
```

## WeakMap
The WeakMap class is very similar to WeakReference, only that it also allows to
associate data to each object. When the target object gets destroyed, the
associated data is automatically freed.

This is similar to https://www.php.net/manual/en/class.weakmap.php
(but currently implements Iterator instead of IteratorAggregate)

```php
<?php
$wm = new WeakMap();

$o = new StdClass;

class A {
    public function __destruct() {
        echo "Dead!\n";
    }
}

$wm[$o] = new A;

var_dump(count($wm)); // int(1)
echo "Unsetting..\n";
unset($o); // Will destroy the 'new A' object as well
echo "Done\n";
var_dump(count($wm)); // int(0)
?>
```

Iterating over WeakMap provides access to both the key (the reference)
and the value:

```php
<?php
$wm = new WeakMap();

$wmk = new StdClass;
$wmv = new StdClass;

$wm[$wmk] = $wmv;

foreach($wm as $k => $v) {
    // $k == $wmk
    // $v == $wmv
}
?>
```

## Limitations

Prior to php 7.0.3, using an object with `WeakReference::create` or as a key in an entry of of `WeakMap` would cause this to change the value of `spl_object_hash` and `SplObjectStorage::getHash`.
