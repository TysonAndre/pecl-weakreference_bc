/*
   +----------------------------------------------------------------------+
   | Weakref                                                        |
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
   | Authors: Etienne Kneuss <colder@php.net>                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "Zend/zend_interfaces.h"
#include "ext/standard/info.h"
#include "wr_weakmap.h"
#include "wr_store.h"
#include "php_weakref.h"

zend_object_handlers wr_handler_WeakMap;
WEAKREF_API zend_class_entry  *wr_ce_WeakMap;

static inline wr_weakmap_object* wr_weakmap_fetch(zend_object *obj) {
	return (wr_weakmap_object *)((char *)obj - XtOffsetOf(wr_weakmap_object, std));
}

#define Z_WEAKMAP_OBJ_P(zv) wr_weakmap_fetch(Z_OBJ_P(zv));

static void wr_weakmap_ref_dtor(zend_object *wmap_obj, zend_object *ref_obj, uint32_t handle) { /* {{{ */
	wr_weakmap_object *wmap = wr_weakmap_fetch(wmap_obj);

	zend_hash_index_del(&wmap->map, handle);
}
/* }}} */

static void wr_weakmap_object_free_storage(zend_object *wmap_obj) /* {{{ */
{
	wr_weakmap_object *wmap = wr_weakmap_fetch(wmap_obj);

	wr_weakmap_refval *refval;
    zend_long handle;

	ZEND_HASH_FOREACH_NUM_KEY_PTR(&wmap->map, handle, refval) {
		wr_store_untrack(wmap_obj, refval->ref, handle);
	} ZEND_HASH_FOREACH_END();

	zend_hash_destroy(&wmap->map);

	zend_object_std_dtor(&wmap->std);
} /* }}} */

static void wr_weakmap_refval_dtor(zval *data) /* {{{ */
{
	ZEND_ASSERT(Z_TYPE_P(data) == IS_PTR);
	wr_weakmap_refval *refval = Z_PTR_P(data);
	zval_ptr_dtor(&refval->val);

	efree(refval);
} /* }}} */

static zend_object* wr_weakmap_object_new_ex(zend_class_entry *ce, zend_object *cloneOf) /* {{{ */
{
	wr_weakmap_object *wmap = ecalloc(1, sizeof(wr_weakmap_object) + zend_object_properties_size(ce));

	zend_object_std_init(&wmap->std, ce);

	zend_hash_init(&wmap->map, 0, NULL, wr_weakmap_refval_dtor, 0);

	if (cloneOf) {
		wr_weakmap_refval *refval;
		wr_weakmap_object *other = wr_weakmap_fetch(cloneOf);

		ZEND_HASH_FOREACH_PTR(&other->map, refval) {
			wr_store_track(&wmap->std, wr_weakmap_ref_dtor, refval->ref);

			Z_TRY_ADDREF_P(&refval->val);

			wr_weakmap_refval *refval_copy = emalloc(sizeof(wr_weakmap_refval));
			refval_copy->ref = refval->ref;
			ZVAL_COPY_VALUE(&refval_copy->val, &refval->val);

			zend_hash_index_update_ptr(&wmap->map, refval->ref->handle, refval_copy);
		} ZEND_HASH_FOREACH_END();
	}

	wmap->std.handlers = &wr_handler_WeakMap;

	return &wmap->std;
} /* }}} */

static zend_object* wr_weakmap_object_clone(zval *cloneOf_zv) /* {{{ */
{
	zend_object *cloneOf = Z_OBJ_P(cloneOf_zv);
	zend_object *clone = wr_weakmap_object_new_ex(cloneOf->ce, cloneOf);

	zend_objects_clone_members(clone, Z_OBJ_P(cloneOf_zv));

	return clone;
}
/* }}} */

static zend_object* wr_weakmap_object_new(zend_class_entry *ce) /* {{{ */
{
	return wr_weakmap_object_new_ex(ce, NULL);
}
/* }}} */

/* {{{ proto void WeakMap::__construct()
*/
PHP_METHOD(WeakMap, __construct)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
} /* }}} */

/* {{{ proto int WeakMap::count(void)
*/
PHP_METHOD(WeakMap, count)
{
	wr_weakmap_object *wmap = Z_WEAKMAP_OBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(zend_hash_num_elements(&wmap->map));
} /* }}} */

/* {{{ proto bool WeakMap::offsetExists(object $index) U
 Returns whether the requested $index exists. */
PHP_METHOD(WeakMap, offsetExists)
{
	wr_weakmap_object *wmap = Z_WEAKMAP_OBJ_P(getThis());
	zval              *ref_zv;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &ref_zv) == FAILURE) {
		return;
	}

	RETURN_BOOL(zend_hash_index_exists(&wmap->map, Z_OBJ_HANDLE_P(ref_zv)));
} /* }}} */

/* {{{ proto mixed WeakMap::offsetGet(object $index) U
 Returns the value at the specified $index. */
PHP_METHOD(WeakMap, offsetGet)
{
	zval               *ref_zv;
	wr_weakmap_object  *wmap = Z_WEAKMAP_OBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &ref_zv) == FAILURE) {
		return;
	}

	wr_weakmap_refval *refval = zend_hash_index_find_ptr(&wmap->map, Z_OBJ_HANDLE_P(ref_zv));
	if (refval) {
		RETURN_ZVAL(&refval->val, 1, 0);
	} else {
		RETURN_NULL();
	}
} /* }}} */

/* {{{ proto void WeakMap::offsetSet(object $index, mixed $newval) U
 Sets the value at the specified $index to $newval. */
PHP_METHOD(WeakMap, offsetSet)
{
	zval               *ref_zv, *val_zv;
	wr_weakmap_object  *wmap = Z_WEAKMAP_OBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "oz", &ref_zv, &val_zv) == FAILURE) {
		return;
	}
	zend_object *obj_key = Z_OBJ_P(ref_zv);
	uint32_t obj_handle = obj_key->handle;

	if (!zend_hash_index_exists(&wmap->map, obj_handle)) {
		// Object not already in the weakmap, we attach it
		wr_store_track(&wmap->std, wr_weakmap_ref_dtor, obj_key);
	}

	Z_TRY_ADDREF_P(val_zv);

	zval *zv = zend_hash_index_find(&wmap->map, obj_handle);
	if (zv) {
		ZEND_ASSERT(Z_TYPE_P(zv) == IS_PTR);
		wr_weakmap_refval *refval = Z_PTR_P(zv);
		//ZEND_ASSERT(refval->ref == obj_key);
		refval->ref = obj_key;
		/* Because the destructors can have side effects such as resizing or rehashing the WeakMap storage,
		 * free the zval only after overwriting the original value. */
		zval zv_orig;
		ZVAL_COPY_VALUE(&zv_orig, &refval->val);
		ZVAL_COPY_VALUE(&refval->val, val_zv);
		zval_ptr_dtor(&zv_orig);
		return;
	}

	wr_weakmap_refval *refval = emalloc(sizeof(wr_weakmap_refval));

	refval->ref = obj_key;
	ZVAL_COPY_VALUE(&refval->val, val_zv);
	zend_hash_index_update_ptr(&wmap->map, obj_handle, refval);
} /* }}} */

/* {{{ proto void WeakMap::offsetUnset(mixed $index) U
 Unsets the value at the specified $index. */
PHP_METHOD(WeakMap, offsetUnset)
{
	zval               *ref_zv;
	wr_weakmap_object  *wmap = Z_WEAKMAP_OBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &ref_zv) == FAILURE) {
		return;
	}

	zend_object *obj = Z_OBJ_P(ref_zv);
	uint32_t handle = obj->handle;
	/* XXX this may have side effects that modify the map again? */
	if (zend_hash_index_del(&wmap->map, handle) == SUCCESS) {
		wr_store_untrack(&wmap->std, Z_OBJ_P(ref_zv), handle);
	}

} /* }}} */

/* {{{ proto void WeakMap::rewind()
 Rewind to first position */
PHP_METHOD(WeakMap, rewind)
{
	wr_weakmap_object *wmap = Z_WEAKMAP_OBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zend_hash_internal_pointer_reset_ex(&wmap->map, &wmap->pos);
} /* }}} */

/* {{{ proto bool WeakMap::valid()
 Returns whether current position is valid */
PHP_METHOD(WeakMap, valid)
{
	wr_weakmap_object *wmap = Z_WEAKMAP_OBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(zend_hash_has_more_elements_ex(&wmap->map, &wmap->pos) == SUCCESS);
} /* }}} */

/* {{{ proto mixed WeakMap::key()
 Returns current key */
PHP_METHOD(WeakMap, key)
{
	wr_weakmap_object *wmap = Z_WEAKMAP_OBJ_P(getThis());

	zval *refval_zv = zend_hash_get_current_data_ex(&wmap->map, &wmap->pos);

	if (refval_zv) {
		wr_weakmap_refval *refval = Z_PTR_P(refval_zv);
		zval ret;
		ZVAL_OBJ(&ret, refval->ref);

		RETURN_ZVAL(&ret, 1, 0);
	} else {
		RETURN_NULL();
	}
} /* }}} */

/* {{{ proto mixed WeakMap::current()
 Returns current element */
PHP_METHOD(WeakMap, current)
{
	wr_weakmap_object *wmap = Z_WEAKMAP_OBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zval *refval_zv = zend_hash_get_current_data_ex(&wmap->map, &wmap->pos);

	if (refval_zv) {
		wr_weakmap_refval *refval = Z_PTR_P(refval_zv);
		RETURN_ZVAL(&refval->val, 1, 0);
	} else {
		RETURN_NULL();
	}
} /* }}} */

/* {{{ proto void SplObjectStorage::next()
 Moves position forward */
PHP_METHOD(WeakMap, next)
{
	wr_weakmap_object *wmap = Z_WEAKMAP_OBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zend_hash_move_forward_ex(&wmap->map, &wmap->pos);
} /* }}} */

/* {{{ proto noreturn WeakMap::__wakeup()
*/
ZEND_COLD PHP_METHOD(WeakMap, __wakeup)
{
	zend_throw_exception(NULL, "Unserialization of 'WeakMap' is not allowed", 0);
}
/* }}} */

/* {{{ proto noreturn WeakMap::__sleep()
*/
ZEND_COLD PHP_METHOD(WeakMap, __sleep)
{
	zend_throw_exception(NULL, "Serialization of 'WeakMap' is not allowed", 0);
}
/* }}} */

/*  Function/Class/Method definitions */
ZEND_BEGIN_ARG_INFO(arginfo_wr_weakmap_void, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_wr_weakmap_obj, 0)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_wr_weakmap_obj_val, 0)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

static const zend_function_entry wr_funcs_WeakMap[] = {
	PHP_ME(WeakMap, __construct,     arginfo_wr_weakmap_void,      ZEND_ACC_PUBLIC)
	PHP_ME(WeakMap, count,           arginfo_wr_weakmap_void,      ZEND_ACC_PUBLIC)
	/* ArrayAccess */
	PHP_ME(WeakMap, offsetExists,    arginfo_wr_weakmap_obj,       ZEND_ACC_PUBLIC)
	PHP_ME(WeakMap, offsetGet,       arginfo_wr_weakmap_obj,       ZEND_ACC_PUBLIC)
	PHP_ME(WeakMap, offsetSet,       arginfo_wr_weakmap_obj_val,   ZEND_ACC_PUBLIC)
	PHP_ME(WeakMap, offsetUnset,     arginfo_wr_weakmap_obj,       ZEND_ACC_PUBLIC)
	/* Iterator */
	PHP_ME(WeakMap,  rewind,         arginfo_wr_weakmap_void,      ZEND_ACC_PUBLIC)
	PHP_ME(WeakMap,  valid,          arginfo_wr_weakmap_void,      ZEND_ACC_PUBLIC)
	PHP_ME(WeakMap,  key,            arginfo_wr_weakmap_void,      ZEND_ACC_PUBLIC)
	PHP_ME(WeakMap,  current,        arginfo_wr_weakmap_void,      ZEND_ACC_PUBLIC)
	PHP_ME(WeakMap,  next,           arginfo_wr_weakmap_void,      ZEND_ACC_PUBLIC)
	PHP_ME(WeakMap, __wakeup,        arginfo_wr_weakmap_void,      ZEND_ACC_PUBLIC)
	PHP_ME(WeakMap, __sleep,         arginfo_wr_weakmap_void,      ZEND_ACC_PUBLIC)
	PHP_FE_END
};
/* }}} */

PHP_MINIT_FUNCTION(wr_weakmap) /* {{{ */
{
	zend_class_entry weakmap_ce;

	INIT_CLASS_ENTRY(weakmap_ce, "WeakMap", wr_funcs_WeakMap);

	wr_ce_WeakMap = zend_register_internal_class(&weakmap_ce);

	wr_ce_WeakMap->ce_flags        |= ZEND_ACC_FINAL;
	wr_ce_WeakMap->create_object    = wr_weakmap_object_new;
	wr_ce_WeakMap->serialize        = zend_class_serialize_deny;
	wr_ce_WeakMap->unserialize      = zend_class_unserialize_deny;

	memcpy(&wr_handler_WeakMap, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	wr_handler_WeakMap.clone_obj = wr_weakmap_object_clone;
	wr_handler_WeakMap.free_obj  = wr_weakmap_object_free_storage;
	wr_handler_WeakMap.offset    = XtOffsetOf(wr_weakmap_object, std);

	zend_class_implements(wr_ce_WeakMap, 3, spl_ce_Countable, spl_ce_ArrayAccess, spl_ce_Iterator);

	return SUCCESS;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
