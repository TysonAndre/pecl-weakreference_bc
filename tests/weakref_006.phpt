--TEST--
Weakref: Destroying the weakref within the std dtor of the object
--FILE--
<?php

class A {
    private $wr = null;
    public function __construct() {
        $this->wr = WeakReference::create($this);
    }
    public function __destruct() {
        unset($this->wr);
    }
}

$a = new A;
unset($a);
?>
==END==
--EXPECTF--
==END==
