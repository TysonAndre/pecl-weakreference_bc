--TEST--
Weakref: stable spl_object_hash
--FILE--
<?php
$o = new StdClass();
$oldHash = spl_object_hash($o);
$wr = WeakReference::create($o);
$newHash = spl_object_hash($o);

var_dump($oldHash == $newHash);
?>
==END==
--EXPECT--
bool(true)
==END==
