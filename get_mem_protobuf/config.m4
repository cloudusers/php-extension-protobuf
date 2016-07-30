dnl $Id$
dnl config.m4 for extension get_mem_protobuf

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(get_mem_protobuf, for get_mem_protobuf support,
Make sure that the comment is aligned:
[  --with-get_mem_protobuf             Include get_mem_protobuf support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(get_mem_protobuf, whether to enable get_mem_protobuf support,
dnl Make sure that the comment is aligned:
dnl [  --enable-get_mem_protobuf           Enable get_mem_protobuf support])

if test "$PHP_GET_MEM_PROTOBUF" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-get_mem_protobuf -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/get_mem_protobuf.h"  # you most likely want to change this
  dnl if test -r $PHP_GET_MEM_PROTOBUF/$SEARCH_FOR; then # path given as parameter
  dnl   GET_MEM_PROTOBUF_DIR=$PHP_GET_MEM_PROTOBUF
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for get_mem_protobuf files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       GET_MEM_PROTOBUF_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$GET_MEM_PROTOBUF_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the get_mem_protobuf distribution])
  dnl fi

  dnl # --with-get_mem_protobuf -> add include path
  PHP_REQUIRE_CXX()
  SRC_PATH=../url_attribute_table
  LIB_PATH=../url_attribute_table
  PHP_ADD_INCLUDE($SRC_PATH/src)
  LIBNAME=url_attribute_table
  dnl LIBSYMBOL=get_mem_protobuf # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  PHP_ADD_LIBRARY(stdc++, 1, GET_MEM_PROTOBUF_SHARED_LIBADD)
  PHP_ADD_LIBRARY(protobuf, 1, GET_MEM_PROTOBUF_SHARED_LIBADD)
  PHP_ADD_LIBRARY(snappy, 1, GET_MEM_PROTOBUF_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $LIB_PATH/lib, GET_MEM_PROTOBUF_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_GET_MEM_PROTOBUFLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong get_mem_protobuf lib version or lib not found])
  dnl ],[
  dnl   -L$GET_MEM_PROTOBUF_DIR/lib -lm
  dnl ])
  dnl
  PHP_SUBST(GET_MEM_PROTOBUF_SHARED_LIBADD)

  PHP_NEW_EXTENSION(get_mem_protobuf, get_mem_protobuf.cc, $ext_shared)
fi
