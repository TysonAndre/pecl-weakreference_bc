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

#ifndef PHP_WEAKREF_H
#define PHP_WEAKREF_H

#include <php.h>

#define PHP_WEAKREF_VERSION "0.4.0"

#ifdef PHP_WIN32
#define WEAKREF_API __declspec(dllexport)
#else
#define WEAKREF_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#include "wr_weakref.h"
#include "wr_store.h"

extern zend_module_entry weakreference_bc_module_entry;
#define phpext_weakreference_bc_ptr &weakreference_bc_module_entry

PHP_MINFO_FUNCTION(weakreference_bc);

PHP_MINIT_FUNCTION(weakreference_bc);
PHP_RINIT_FUNCTION(weakreference_bc);
PHP_RSHUTDOWN_FUNCTION(weakreference_bc);

ZEND_BEGIN_MODULE_GLOBALS(weakreference_bc)
    wr_store *store;
	HashTable replacement_handlers;
	HashTable old_handlers;
ZEND_END_MODULE_GLOBALS(weakreference_bc)

#ifdef ZTS
#define WR_G(v) TSRMG(weakreference_bc_globals_id, zend_weakreference_bc_globals *, v)
extern int weakreference_bc_globals_id;
#else
#define WR_G(v) (weakreference_bc_globals.v)
extern zend_weakreference_bc_globals weakreference_bc_globals;
#endif

#endif /* PHP_WEAKREF_H */

/*
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
