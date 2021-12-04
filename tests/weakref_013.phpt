--TEST--
Weakref: usage #69131: During shutdown, acquired refs can be dtored before the wref
--FILE--
<?php
$wr = [];
for($i = 0; $i < 2; $i++) {
    $c = new stdClass();
    $wr[$i] = new Weakref($c);
    $wr[$i]->acquire();
}
?>
==END==
--EXPECTF--
==END==

