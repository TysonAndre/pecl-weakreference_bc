--TEST--
WeakMap iteration
--FILE--
<?php

$map = new WeakMap;

$obj0 = new stdClass;
$obj1 = new stdClass;
$map[$obj0] = 0;
$map[$obj1] = 1;

echo "\nObject removed during loop:\n";
foreach ($map as $key => $value) {
    if (isset($obj1) && $key === $obj1) unset($obj1);
    var_dump($key, $value);
}
?>
--EXPECT--
Object removed during loop:
object(stdClass)#2 (0) {
}
int(0)
object(stdClass)#3 (0) {
}
int(1)
