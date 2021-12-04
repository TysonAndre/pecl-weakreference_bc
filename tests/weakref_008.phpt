--TEST--
Weakref: zval splitting causes crash
--FILE--
<?php
$obj = new StdClass();
$ref = WeakReference::create($obj);
echo "get...\n";
$obj = $ref->get();
echo "unset...\n";
unset($ref);
echo "done...\n";
?>
--EXPECTF--
get...
unset...
done...
