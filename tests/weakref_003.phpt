--TEST--
Weakref: usage
--FILE--
<?php

$r = new StdClass;


$wr1 = WeakReference::create($r);
var_dump($wr1->get());
unset($wr1);

$wr2 = WeakReference::create($r);
var_dump($wr2->get());
unset($wr2);
?>
==END==
--EXPECT--
object(stdClass)#1 (0) {
}
object(stdClass)#1 (0) {
}
==END==
