/*
   +----------------------------------------------------------------------+
   | weakreference_bc                                                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2011 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Etienne Kneuss <colder@php.net> (original weakref PECL)     |
   | Authors: Tyson Andre <tandre@php.net> (weakreference_bc)             |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php_version.h"
#if PHP_VERSION_ID < 70400

#include "php.h"
#include "zend_exceptions.h"
#include "Zend/zend_interfaces.h"
#include "ext/standard/info.h"
#include "ext/spl/spl_exceptions.h"
#include "wr_weakref.h"
#include "php_weakref.h"
zend_object_handlers wr_handlerWeakReference;
WEAKREF_API zend_class_entry  *wr_ceWeakReference;

static inline wr_weakref_object* wr_weakref_fetch(zend_object *obj) {
	return (wr_weakref_object *)((char *)obj - XtOffsetOf(wr_weakref_object, std));
}

#define Z_WEAKREF_OBJ_P(zv) wr_weakref_fetch(Z_OBJ_P(zv));

#ifndef GC_ADDREF
#define GC_ADDREF(v) (GC_REFCOUNT((v))++)
#endif

static void wr_weakref_ref_dtor(zend_object *wref_obj, zend_object *ref_obj, uint32_t handle) { /* {{{ */
	wr_weakref_object *wref = wr_weakref_fetch(wref_obj);

	wref->valid = 0;
}
/* }}} */

static void wr_weakref_object_free_storage(zend_object *wref_obj) /* {{{ */
{
	wr_weakref_object *wref     = wr_weakref_fetch(wref_obj);

	if (wref->valid) {
		wr_store_untrack(wref_obj, wref->ref_obj, wref->ref_obj->handle);
	}

	zend_object_std_dtor(&wref->std);
}
/* }}} */

static zend_object* wr_weakref_object_new_ex(zend_class_entry *ce, zend_object *cloneOf) /* {{{ */
{
	wr_weakref_object *wref = ecalloc(1, sizeof(wr_weakref_object) + zend_object_properties_size(ce));

	zend_object_std_init(&wref->std, ce);
	object_properties_init(&wref->std, ce);

	if (cloneOf) {
		wr_weakref_object *other = wr_weakref_fetch(cloneOf);
		wref->valid   = other->valid;
		wref->ref_obj = other->ref_obj;

		wr_store_track(&wref->std, wr_weakref_ref_dtor, other->ref_obj);
	}

	wref->std.handlers = &wr_handlerWeakReference;

	return &wref->std;
}
/* }}} */

static zend_object* wr_weakref_object_new(zend_class_entry *ce) /* {{{ */
{
	return wr_weakref_object_new_ex(ce, NULL);
}
/* }}} */

/* {{{ proto object WeakReference::get()
 Return the reference, or null. */
PHP_METHOD(WeakReference, get)
{
	wr_weakref_object *wref = Z_WEAKREF_OBJ_P(getThis());
	zval ret;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (wref->valid) {
		ZVAL_OBJ(&ret, wref->ref_obj);
		RETURN_ZVAL(&ret, 1, 0);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto noreturn WeakReference::__construct()
*/
ZEND_COLD PHP_METHOD(WeakReference, __construct)
{
	zend_throw_error(NULL,
	    "Direct instantiation of WeakReference is not allowed, "
	    "use WeakReference::create instead");
}
/* }}} */

/* {{{ proto noreturn WeakReference::__wakeup()
*/
ZEND_COLD PHP_METHOD(WeakReference, __wakeup)
{
	zend_throw_exception(NULL, "Unserialization of 'WeakReference' is not allowed", 0);
}
/* }}} */

/* {{{ proto noreturn WeakReference::__sleep()
*/
ZEND_COLD PHP_METHOD(WeakReference, __sleep)
{
	zend_throw_exception(NULL, "Serialization of 'WeakReference' is not allowed", 0);
}
/* }}} */

/* {{{ proto void WeakReference::create(object ref)
 */
PHP_METHOD(WeakReference, create)
{
	zval *ref;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "o", &ref)) {
		return;
	}

	zend_object *ref_obj = Z_OBJ_P(ref);
	wr_store *store = WR_G(store);
	wr_ref_list *cur = zend_hash_index_find_ptr(&store->objs, ref_obj->handle);
	while (cur) {
		if (cur->wref_obj->ce == wr_ceWeakReference) {
			GC_ADDREF(cur->wref_obj);
			RETURN_OBJ(cur->wref_obj);
		}
		cur = cur->next;
	}
    object_init_ex(return_value, wr_ceWeakReference);
	zend_object *wr_obj = Z_OBJ_P(return_value);
	wr_weakref_object *wref = wr_weakref_fetch(wr_obj);

	wref->ref_obj = ref_obj;

	wr_store_track(wr_obj, wr_weakref_ref_dtor, ref_obj);

	wref->valid = 1;
}
/* }}} */

#define weakref_unsupported(thing) \
	zend_throw_error(NULL, "WeakReference objects do not support " thing);

#if PHP_VERSION_ID >= 70400
static ZEND_COLD zval* wr_weakref_no_write(zval *object, zval *member, zval *value, void **rtc)
#else
static ZEND_COLD void wr_weakref_no_write(zval *object, zval *member, zval *value, void **rtc)
#endif
{
	weakref_unsupported("properties");

#if PHP_VERSION_ID >= 70400
	return &EG(uninitialized_zval);
#endif
}

static ZEND_COLD zval* wr_weakref_no_read(zval *object, zval *member, int type, void **rtc, zval *rv) {
	if (!EG(exception)) {
		weakref_unsupported("properties");
	}

	return &EG(uninitialized_zval);
}

static ZEND_COLD zval *wr_weakref_no_read_ptr(zval *object, zval *member, int type, void **rtc) {
	weakref_unsupported("property references");
	return NULL;
}

static ZEND_COLD int wr_weakref_no_isset(zval *object, zval *member, int hse, void **rtc) {
	if (hse != 2) {
		weakref_unsupported("properties");
	}
	return 0;
}

static ZEND_COLD void wr_weakref_no_unset(zval *object, zval *member, void **rtc) {
	weakref_unsupported("properties");
}

/*  Function/Class/Method definitions */
ZEND_BEGIN_ARG_INFO(arginfo_wr_weakref_void, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_wr_weakref_obj, 0)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

static const zend_function_entry wr_funcsWeakReference[] = {
	PHP_ME(WeakReference, __construct,     arginfo_wr_weakref_void,            ZEND_ACC_PUBLIC)
	PHP_ME(WeakReference, create,          arginfo_wr_weakref_obj,             ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(WeakReference, get,             arginfo_wr_weakref_void,            ZEND_ACC_PUBLIC)
	PHP_ME(WeakReference, __sleep,         arginfo_wr_weakref_void,            ZEND_ACC_PUBLIC)
	PHP_ME(WeakReference, __wakeup,        arginfo_wr_weakref_void,            ZEND_ACC_PUBLIC)
	PHP_FE_END
};
/* }}} */

PHP_MINIT_FUNCTION(wr_weakref) /* {{{ */
{
	zend_class_entry weakref_ce;

	INIT_CLASS_ENTRY(weakref_ce, "WeakReference", wr_funcsWeakReference);

	wr_ceWeakReference = zend_register_internal_class(&weakref_ce);

	wr_ceWeakReference->ce_flags      |= ZEND_ACC_FINAL;
	wr_ceWeakReference->create_object  = wr_weakref_object_new;
	wr_ceWeakReference->serialize      = zend_class_serialize_deny;
	wr_ceWeakReference->unserialize    = zend_class_unserialize_deny;

	memcpy(&wr_handlerWeakReference, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	wr_handlerWeakReference.read_property = wr_weakref_no_read;
	wr_handlerWeakReference.write_property = wr_weakref_no_write;
	wr_handlerWeakReference.has_property = wr_weakref_no_isset;
	wr_handlerWeakReference.unset_property = wr_weakref_no_unset;
	wr_handlerWeakReference.get_property_ptr_ptr = wr_weakref_no_read_ptr;
	wr_handlerWeakReference.clone_obj = NULL;
	wr_handlerWeakReference.free_obj  = wr_weakref_object_free_storage;
	wr_handlerWeakReference.offset    = XtOffsetOf(wr_weakref_object, std);

	return SUCCESS;
}
/* }}} */

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
