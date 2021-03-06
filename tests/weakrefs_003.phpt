--TEST--
WeakReference object handlers
--FILE--
<?php
$wr = WeakReference::create(new stdClass);

try {
    var_dump($wr->disallow);
} catch (Error $ex) {
    var_dump($ex->getMessage());
}
try {
    var_dump(isset($wr->disallow));
} catch (Error $ex) {
    var_dump($ex->getMessage());
}
try {
    unset($wr->disallow);
} catch (Error $ex) {
    var_dump($ex->getMessage());
}

try {
    $wr->disallow = "writes";
} catch (Error $ex) {
    var_dump($ex->getMessage());
}

try {
    $disallow = &$wr->disallowed;
} catch (Error $ex) {
    var_dump($ex->getMessage());
}
?>
--EXPECTF--
string(47) "WeakReference objects do not support properties"
string(47) "WeakReference objects do not support properties"
string(47) "WeakReference objects do not support properties"
string(47) "WeakReference objects do not support properties"
string(56) "WeakReference objects do not support property references"