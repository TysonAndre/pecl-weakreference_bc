--TEST--
Weakref: cloning
--FILE--
<?php
$o = new StdClass;

$wr1 = WeakReference::create($o);
try {
    $wr2 = clone $wr1;
} catch (Error $e) {
    echo "Caught: {$e->getMessage()}\n";
}
$wr3 = WeakReference::create($o);

unset($o);

var_dump($wr1->valid());
var_dump($wr1 === $wr3);
?>
==END==
--EXPECTF--
Caught: Trying to clone an uncloneable object of class WeakReference
bool(false)
bool(true)
==END==
