--TEST--
WeakReference no inheritance
--FILE--
<?php
class Test extends WeakReference {}
?>
--EXPECTF--
Fatal error: Class Test %s final class %SWeakReference%S in %s on line %d
