--TEST--
Weakref: usage
--FILE--
<?php
$o = new StdClass;

$wr = WeakReference::create($o);

var_dump($wr->get());
unset($o);
var_dump($wr->get());
?>
==END==
--EXPECTF--
object(stdClass)#1 (0) {
}
NULL
==END==
