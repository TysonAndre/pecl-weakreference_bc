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

#ifndef WR_WEAKREF_H
#define WR_WEAKREF_H

#include "php_version.h"
/* PHP 7.4 has its own WeakReference class */
#if PHP_VERSION_ID < 70400
#include "php_weakref.h"
#include "wr_store.h"

typedef struct _wr_weakref_object {
	wr_store              *store;
	zend_object           *ref_obj;
	zend_bool              valid;
	zend_object            std;
} wr_weakref_object;

extern WEAKREF_API zend_class_entry *wr_ce_WeakRef;

PHP_MINIT_FUNCTION(wr_weakref);
#endif

#endif /* WR_WEAKREF_H */

/*
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
