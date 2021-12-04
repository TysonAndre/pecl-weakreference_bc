--TEST--
Basic WeakMap behavior (as a map)
--FILE--
<?php

$map = new WeakMap;
var_dump(count($map));

$obj = new stdClass;
$obj->value = 1;
$obj2 = new stdClass;
$obj2->value = 2;

$map[$obj] = $obj2;
var_dump(count($map));
var_dump($map);
var_dump(isset($map[$obj]));
var_dump(!empty($map[$obj]));
var_dump($map[$obj]);

echo "Overriden\n";
$map[$obj] = 42;
var_dump($map);
var_dump(isset($map[$obj]));
var_dump(!empty($map[$obj]));
var_dump($map[$obj]);

$map[$obj] = false;
var_dump($map);
var_dump(isset($map[$obj]));
var_dump(!empty($map[$obj]));
var_dump($map[$obj]);

$map[$obj] = null;
var_dump($map);
var_dump(isset($map[$obj]));
var_dump(!empty($map[$obj]));
var_dump($map[$obj]);

unset($map[$obj]);
var_dump($map);
var_dump(isset($map[$obj]));
var_dump(!empty($map[$obj]));
try {
    var_dump($map[$obj]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

// It's okay to unset an object that's not in the map.
unset($map[new stdClass]);

echo "\nIndirect modification:\n"; // TODO support indirect modification
$map[$obj] = [];
$map[$obj][] = 42;
$map[$obj2] = 41;
$map[$obj2]++;
var_dump($map);

echo "\nMethods:\n";
var_dump($map->offsetSet($obj2, 43));
var_dump($map->offsetGet($obj2));
var_dump($map->offsetExists($obj2));
var_dump($map->count());
var_dump($map->offsetUnset($obj2));
var_dump($map->count());

?>
--EXPECTF--
int(0)
int(1)
object(WeakMap)#1 (0) {
}
bool(true)
bool(true)
object(stdClass)#3 (1) {
  ["value"]=>
  int(2)
}
Overriden
object(WeakMap)#1 (0) {
}
bool(true)
bool(true)
int(42)
object(WeakMap)#1 (0) {
}
bool(true)
bool(false)
bool(false)
object(WeakMap)#1 (0) {
}
bool(true)
bool(false)
NULL
object(WeakMap)#1 (0) {
}
bool(false)
bool(false)
NULL

Indirect modification:

Notice: Indirect modification of overloaded element of WeakMap has no effect in %s on line 52

Notice: Indirect modification of overloaded element of WeakMap has no effect in %s on line 54
object(WeakMap)#1 (0) {
}

Methods:
NULL
int(43)
bool(true)
int(2)
NULL
int(1)
