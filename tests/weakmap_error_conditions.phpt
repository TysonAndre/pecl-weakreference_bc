--TEST--
WeakMap error conditions
--FILE--
<?php
// TODO: Update std handlers and make these throw
$map = new WeakMap;
try {
    $map[1] = 2;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($map[1]);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    isset($map[1]);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    unset($map[1]);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $map[] = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $map[][1] = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($map[new stdClass]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

var_dump($map->prop);
var_dump(isset($map->prop));
unset($map->prop);

try {
    $map->prop = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $map->prop[] = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $r =& $map->prop;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    serialize($map);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
try {
    unserialize('C:7:"WeakMap":0:{}');
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Warning: WeakMap::offsetSet() expects parameter 1 to be object, int%S given in %s on line 5

Warning: WeakMap::offsetGet() expects parameter 1 to be object, int%S given in %s on line 10
NULL

Warning: WeakMap::offsetExists() expects parameter 1 to be object, int%S given in %s on line 15

Warning: WeakMap::offsetUnset() expects parameter 1 to be object, int%S given in %s on line 20

Warning: WeakMap::offsetSet() expects parameter 1 to be object, null given in %s on line 26

Warning: WeakMap::offsetGet() expects parameter 1 to be object, null given in %s on line 31

Notice: Indirect modification of overloaded element of WeakMap has no effect in %s on line 31
NULL

Notice: Undefined property: WeakMap::$prop in %s on line 41
NULL
bool(false)

Warning: Cannot use a scalar value as an array in %s on line 51

Warning: Class WeakMap has no unserializer in %s on line 67
