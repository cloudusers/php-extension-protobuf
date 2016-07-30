/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2007 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id: header,v 1.16.2.1.2.1 2007/01/01 19:32:09 iliaa Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_get_mem_protobuf.h"

//include some header files which to process compressed data and protobuf format data
#include <snappy.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/io/gzip_stream.h>
#include "url_attribute_table.pb.h"
#include <iostream>
#include <sstream>
//include end

/* If you declare any globals in php_get_mem_protobuf.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(get_mem_protobuf)
*/

/* True global resources - no need for thread safety here */
static int le_get_mem_protobuf;

/* {{{ get_mem_protobuf_functions[]
 *
 * Every user visible function must have an entry in get_mem_protobuf_functions[].
 */
zend_function_entry get_mem_protobuf_functions[] = {
	PHP_FE(proto_decode,   NULL)
	{NULL, NULL, NULL}	/* Must be the last line in get_mem_protobuf_functions[] */
};
/* }}} */

/* {{{ get_mem_protobuf_module_entry
 */
zend_module_entry get_mem_protobuf_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"get_mem_protobuf",
	get_mem_protobuf_functions,
	PHP_MINIT(get_mem_protobuf),
	PHP_MSHUTDOWN(get_mem_protobuf),
	PHP_RINIT(get_mem_protobuf),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(get_mem_protobuf),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(get_mem_protobuf),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_GET_MEM_PROTOBUF
ZEND_GET_MODULE(get_mem_protobuf)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("get_mem_protobuf.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_get_mem_protobuf_globals, get_mem_protobuf_globals)
    STD_PHP_INI_ENTRY("get_mem_protobuf.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_get_mem_protobuf_globals, get_mem_protobuf_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_get_mem_protobuf_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_get_mem_protobuf_init_globals(zend_get_mem_protobuf_globals *get_mem_protobuf_globals)
{
	get_mem_protobuf_globals->global_value = 0;
	get_mem_protobuf_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(get_mem_protobuf)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(get_mem_protobuf)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(get_mem_protobuf)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(get_mem_protobuf)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(get_mem_protobuf)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "get_mem_protobuf support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_get_mem_protobuf_compiled(string arg)
   Return a string to confirm that the module is compiled in */
/*
PHP_FUNCTION(confirm_get_mem_protobuf_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "get_mem_protobuf", arg);
	RETURN_STRINGL(strg, len, 0);
}
*/
/* }}} */
/* {{{ proto array proto_decode(string data [, bool use_snappy])
    */
PHP_FUNCTION(proto_decode) {
	char *data = NULL;
	int argc = ZEND_NUM_ARGS();
	int data_len;
	zend_bool use_snappy = 1;

	if (zend_parse_parameters(argc TSRMLS_CC, "s|b", &data, &data_len, &use_snappy) == FAILURE) 
		return;

	array_init(return_value);
	snappy::string raw_data;
	if (use_snappy) {
		snappy::Uncompress(data, data_len, &raw_data);
	} else {
		raw_data = snappy::string(data, data_len);
	}

    using namespace cloud_search::messages;
	PageAttributes page_attr_table;
	size_t idx = 0;
	size_t idj = 0;
	if (!raw_data.empty() && page_attr_table.ParseFromString(raw_data)) {
			uint32_t id ;
			int64_t val ;
			double vald;
			std::string vals;
			std::string s_idx;
			std::string s_val;
			char pk[1024];
			char pv[1024];

			//if(page_attr_table.int_val.has_id())
			{//int_val
				zval* intvalbody= NULL;
				MAKE_STD_ZVAL(intvalbody);
				array_init(intvalbody);

				for(idx =0; idx <page_attr_table.int_val_size(); ++idx){
					id = page_attr_table.int_val(idx).id();
					val = page_attr_table.int_val(idx).val();
					sprintf(pk,"%d",id);
					sprintf(pv,"%d",val);
					s_idx = pk;
					s_val = pv;
					add_assoc_string(intvalbody, const_cast<char*>(s_idx.c_str()),const_cast<char*>(s_val.c_str()), 1);
				}
				add_assoc_zval(return_value, "int_val", intvalbody);
			}
			{//int_vec
				zval* intvecbody= NULL;
				MAKE_STD_ZVAL(intvecbody);
				array_init(intvecbody);
				for(idx =0; idx <page_attr_table.int_vec_size(); ++idx){
					id = page_attr_table.int_vec(idx).id();

					zval* intvecbody_sub= NULL;
					MAKE_STD_ZVAL(intvecbody_sub);
					array_init(intvecbody_sub);
					for (idj = 0; idj < page_attr_table.int_vec(idx).vec_size(); ++idj){
						val = page_attr_table.int_vec(idx).vec(idj);
						sprintf(pk,"%d",idj);
						sprintf(pv,"%d",val);
						s_idx = pk;
						s_val = pv;
						add_assoc_string(intvecbody_sub, const_cast<char*>(s_idx.c_str()),const_cast<char*>(s_val.c_str()), 1);
					}

					sprintf(pk,"%d",id);
					s_idx = pk;
					add_assoc_zval(intvecbody, const_cast<char*>(s_idx.c_str()), intvecbody_sub);
				}
				//add into header
				add_assoc_zval(return_value, "int_vec", intvecbody);
			}
			{//dbl_val
				zval* dblvalbody= NULL;
				MAKE_STD_ZVAL(dblvalbody);
				array_init(dblvalbody);

				for(idx =0; idx <page_attr_table.dbl_val_size(); ++idx){
					id = page_attr_table.dbl_val(idx).id();
					vald = page_attr_table.dbl_val(idx).val();
					sprintf(pk,"%d",id);
					sprintf(pv,"%f",vald);
					s_idx = pk;
					s_val = pv;
					add_assoc_string(dblvalbody, const_cast<char*>(s_idx.c_str()),const_cast<char*>(s_val.c_str()), 1);
				}
				add_assoc_zval(return_value, "dbl_val", dblvalbody);
			}
			{//dbl_vec
				zval* dblvecbody= NULL;
				MAKE_STD_ZVAL(dblvecbody);
				array_init(dblvecbody);

				for(idx =0; idx <page_attr_table.dbl_vec_size(); ++idx){
					id = page_attr_table.dbl_vec(idx).id();

					zval* dblvecbody_sub = NULL;
					MAKE_STD_ZVAL(dblvecbody_sub);
					array_init(dblvecbody_sub);
					for (idj = 0; idj < page_attr_table.dbl_vec(idx).vec_size(); ++idj){
						vald = page_attr_table.dbl_vec(idx).vec(idj);
						sprintf(pk,"%d",idj);
						sprintf(pv,"%f",vald);
						s_idx = pk;
						s_val = pv;
						add_assoc_string(dblvecbody_sub, const_cast<char*>(s_idx.c_str()),const_cast<char*>(s_val.c_str()), 1);
					}
					sprintf(pk,"%d",id);
					s_idx = pk;
					add_assoc_zval(dblvecbody, const_cast<char*>(s_idx.c_str()), dblvecbody_sub);
				}
				//add into header
				add_assoc_zval(return_value, "dbl_vec", dblvecbody);
			}
			{//str_val
				zval* strvalbody= NULL;
				MAKE_STD_ZVAL(strvalbody);
				array_init(strvalbody);

				for(idx =0; idx <page_attr_table.str_val_size(); ++idx){
					id = page_attr_table.str_val(idx).id();
					vals = page_attr_table.str_val(idx).val();
					sprintf(pk,"%d",id);
					s_idx = pk;
					s_val = vals;
					add_assoc_string(strvalbody, const_cast<char*>(s_idx.c_str()),const_cast<char*>(s_val.c_str()), 1);
				}
				add_assoc_zval(return_value, "str_val", strvalbody);
			}
			{//str_vec
				zval* strvecbody= NULL;
				MAKE_STD_ZVAL(strvecbody);
				array_init(strvecbody);

				for(idx =0; idx <page_attr_table.str_vec_size(); ++idx){
					id = page_attr_table.str_vec(idx).id();

					zval* strvecbody_sub= NULL;
					MAKE_STD_ZVAL(strvecbody_sub);
					array_init(strvecbody_sub);
					for (idj = 0; idj < page_attr_table.str_vec(idx).vec_size(); ++idj){
						vals = page_attr_table.str_vec(idx).vec(idj);
						sprintf(pk,"%d",idj);
						s_idx = pk;
						s_val = vals;
						add_assoc_string(strvecbody_sub, const_cast<char*>(s_idx.c_str()),const_cast<char*>(s_val.c_str()), 1);
					}
					sprintf(pk,"%d",id);
					s_idx = pk;
					add_assoc_zval(strvecbody, const_cast<char*>(s_idx.c_str()), strvecbody_sub);
				}
				//add into header
				add_assoc_zval(return_value, "str_vec", strvecbody);
			}
	}
}
/* }}} */

/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
