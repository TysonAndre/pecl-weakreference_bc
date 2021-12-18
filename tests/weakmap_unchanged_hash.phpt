--TEST--
WeakMap: spl_object_hash does not change after using as hash key
--FILE--
<?php
$o = new StdClass;
$original_hash = spl_object_hash($o);

$wr = new WeakMap();
$wr[$o] = 123;
$new_hash = spl_object_hash($o);
var_dump($original_hash === $original_hash);
var_dump($o);
var_dump($wr[$o]);
?>
--EXPECTF--
bool(true)
object(stdClass)#1 (0) {
}
int(123)
