//-------json_config.h-------



#define STDC_HEADERS 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_MEMORY_H 1
#define HAVE_STRINGS_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_STDINT_H 1
#define HAVE_UNISTD_H 1
#define HAVE_FCNTL_H 1
#define HAVE_LIMITS_H 1
#define HAVE_STRINGS_H 1
#define HAVE_SYSLOG_H 1
#define HAVE_UNISTD_H 1
#define HAVE_SYS_CDEFS_H 1
#define HAVE_SYS_PARAM_H 1
#define HAVE_STDARG_H 1
#define HAVE_LOCALE_H 1
#define HAVE_ENDIAN_H 1
#define JSON_C_HAVE_INTTYPES_H 1
#define HAVE_VPRINTF 1
#define HAVE_STDLIB_H 1
#define HAVE_MALLOC 1
#define HAVE_STDLIB_H 1
#define HAVE_REALLOC 1
#define HAVE_STRCASECMP 1
#define HAVE_STRDUP 1
#define HAVE_STRERROR 1
#define HAVE_SNPRINTF 1
#define HAVE_VSNPRINTF 1
#define HAVE_VASPRINTF 1
#define HAVE_OPEN 1
#define HAVE_VSYSLOG 1
#define HAVE_STRNCASECMP 1
#define HAVE_SETLOCALE 1
#define HAVE_DECL_INFINITY 1
#define HAVE_DECL_NAN 1
#define HAVE_DECL_ISNAN 1
#define HAVE_DECL_ISINF 1
#define HAVE_DECL__ISNAN 0
#define HAVE_DECL__FINITE 0
//-------math_compat.h-------

#ifndef __math_compat_h
#define __math_compat_h

/* Define isnan and isinf on Windows/MSVC */

#ifndef HAVE_DECL_ISNAN
# ifdef HAVE_DECL__ISNAN
#include <float.h>
#define isnan(x) _isnan(x)
# endif
#endif

#ifndef HAVE_DECL_ISINF
# ifdef HAVE_DECL__FINITE
#include <float.h>
#define isinf(x) (!_finite(x))
# endif
#endif

#ifndef HAVE_DECL_NAN
#error This platform does not have nan()
#endif

#ifndef HAVE_DECL_INFINITY
#error This platform does not have INFINITY
#endif

#endif
//-------arraylist.h-------

/*
 * $Id: arraylist.h,v 1.4 2006/01/26 02:16:28 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

#ifndef _arraylist_h_
#define _arraylist_h_

#ifdef dontdefinethis
extern "C" {
#endif

#define ARRAY_LIST_DEFAULT_SIZE 32

typedef void (array_list_free_fn) (void *data);

struct array_list
{
  void **array;
  int length;
  int size;
  array_list_free_fn *free_fn;
};

extern struct array_list*
array_list_new(array_list_free_fn *free_fn);

extern void
array_list_free(struct array_list *al);

extern void*
array_list_get_idx(struct array_list *al, int i);

extern int
array_list_put_idx(struct array_list *al, int i, void *data);

extern int
array_list_add(struct array_list *al, void *data);

extern int
array_list_length(struct array_list *al);

extern void
array_list_sort(struct array_list *arr, int(*compar)(const void *, const void *));

#ifdef dontdefinethis
}
#endif

#endif
//-------bits.h-------

/**
 * @file
 * @deprecated Use json_util.h instead.
 *
 * $Id: bits.h,v 1.10 2006/01/30 23:07:57 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

#ifndef _bits_h_
#define _bits_h_

/**
 * @deprecated
 */
#define hexdigit(x) (((x) <= '9') ? (x) - '0' : ((x) & 7) + 9)
/**
 * @deprecated
 */
#define error_ptr(error) ((void*)error)
/**
 * @deprecated
 */
#define error_description(error)  (json_tokener_get_error(error))
/**
 * @deprecated
 */
#define is_error(ptr) (ptr == NULL)

#endif
//-------json_c_version.h-------

/*
 * Copyright (c) 2012 Eric Haszlakiewicz
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 */

#ifndef _json_c_version_h_
#define _json_c_version_h_

#define JSON_C_MAJOR_VERSION 0
#define JSON_C_MINOR_VERSION 12
#define JSON_C_MICRO_VERSION 99
#define JSON_C_VERSION_NUM ((JSON_C_MAJOR_VERSION << 16) | \
                            (JSON_C_MINOR_VERSION << 8) | \
                            JSON_C_MICRO_VERSION)
#define JSON_C_VERSION "0.12.99"

const char *json_c_version(void); /* Returns JSON_C_VERSION */
int json_c_version_num(void);     /* Returns JSON_C_VERSION_NUM */

#endif
//-------printbuf.h-------

/*
 * $Id: printbuf.h,v 1.4 2006/01/26 02:16:28 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 *
 * Copyright (c) 2008-2009 Yahoo! Inc.  All rights reserved.
 * The copyrights to the contents of this file are licensed under the MIT License
 * (http://www.opensource.org/licenses/mit-license.php)
 */

#ifndef _printbuf_h_
#define _printbuf_h_

#ifdef dontdefinethis
extern "C" {
#endif

struct printbuf {
  char *buf;
  int bpos;
  int size;
};

extern struct printbuf*
printbuf_new(void);

/* As an optimization, printbuf_memappend_fast is defined as a macro
 * that handles copying data if the buffer is large enough; otherwise
 * it invokes printbuf_memappend_real() which performs the heavy
 * lifting of realloc()ing the buffer and copying data.
 * Your code should not use printbuf_memappend directly--use
 * printbuf_memappend_fast instead.
 */
extern int
printbuf_memappend(struct printbuf *p, const char *buf, int size);

#define printbuf_memappend_fast(p, bufptr, bufsize)          \
do {                                                         \
  if ((p->size - p->bpos) > bufsize) {                       \
    memcpy(p->buf + p->bpos, (bufptr), bufsize);             \
    p->bpos += bufsize;                                      \
    p->buf[p->bpos]= '\0';                                   \
  } else {  printbuf_memappend(p, (bufptr), bufsize); }      \
} while (0)

#define printbuf_length(p) ((p)->bpos)

/**
 * Set len bytes of the buffer to charvalue, starting at offset offset.
 * Similar to calling memset(x, charvalue, len);
 *
 * The memory allocated for the buffer is extended as necessary.
 *
 * If offset is -1, this starts at the end of the current data in the buffer.
 */
extern int
printbuf_memset(struct printbuf *pb, int offset, int charvalue, int len);

extern int
sprintbuf(struct printbuf *p, const char *msg, ...);

extern void
printbuf_reset(struct printbuf *p);

extern void
printbuf_free(struct printbuf *p);

#ifdef dontdefinethis
}
#endif

#endif
//-------random_seed.h-------

/*
 * random_seed.h
 *
 * Copyright (c) 2013 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

#ifndef seed_h
#define seed_h

#ifdef dontdefinethis
extern "C" {
#endif

extern int json_c_get_random_seed();

#ifdef dontdefinethis
}
#endif

#endif
//-------debug.h-------

/*
 * $Id: debug.h,v 1.5 2006/01/30 23:07:57 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 * Copyright (c) 2009 Hewlett-Packard Development Company, L.P.
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdlib.h>

#ifdef dontdefinethis
extern "C" {
#endif

extern void mc_set_debug(int debug);
extern int mc_get_debug(void);

extern void mc_set_syslog(int syslog);

extern void mc_debug(const char *msg, ...);
extern void mc_error(const char *msg, ...);
extern void mc_info(const char *msg, ...);

#ifndef __STRING
#define __STRING(x) #x
#endif

#ifndef PARSER_BROKEN_FIXED

#define JASSERT(cond) do {} while(0)

#else

#define JASSERT(cond) do { \
		if (!(cond)) { \
			mc_error("cjson assert failure %s:%d : cond \"" __STRING(cond) "failed\n", __FILE__, __LINE__); \
			*(int *)0 = 1;\
			abort(); \
		}\
	} while(0)

#endif

#define MC_ERROR(x, ...) mc_error(x, ##__VA_ARGS__)

#ifdef MC_MAINTAINER_MODE
#define MC_SET_DEBUG(x) mc_set_debug(x)
#define MC_GET_DEBUG() mc_get_debug()
#define MC_SET_SYSLOG(x) mc_set_syslog(x)
#define MC_DEBUG(x, ...) mc_debug(x, ##__VA_ARGS__)
#define MC_INFO(x, ...) mc_info(x, ##__VA_ARGS__)
#else
#define MC_SET_DEBUG(x) if (0) mc_set_debug(x)
#define MC_GET_DEBUG() (0)
#define MC_SET_SYSLOG(x) if (0) mc_set_syslog(x)
#define MC_DEBUG(x, ...) if (0) mc_debug(x, ##__VA_ARGS__)
#define MC_INFO(x, ...) if (0) mc_info(x, ##__VA_ARGS__)
#endif

#ifdef dontdefinethis
}
#endif

#endif
//-------json_inttypes.h-------


#ifndef _json_inttypes_h_
#define _json_inttypes_h_



#if defined(_MSC_VER) && _MSC_VER <= 1700

/* Anything less than Visual Studio C++ 10 is missing stdint.h and inttypes.h */
typedef __int32 int32_t;
#define INT32_MIN    ((int32_t)_I32_MIN)
#define INT32_MAX    ((int32_t)_I32_MAX)
typedef __int64 int64_t;
#define INT64_MIN    ((int64_t)_I64_MIN)
#define INT64_MAX    ((int64_t)_I64_MAX)
#define PRId64 "I64d"
#define SCNd64 "I64d"

#else

#ifdef JSON_C_HAVE_INTTYPES_H
#include <inttypes.h>
#endif
/* inttypes.h includes stdint.h */

#endif

#endif
//-------json_object.h-------

/*
 * $Id: json_object.h,v 1.12 2006/01/30 23:07:57 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 * Copyright (c) 2009 Hewlett-Packard Development Company, L.P.
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

#ifndef _json_object_h_
#define _json_object_h_

#ifdef __GNUC__
#define THIS_FUNCTION_IS_DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define THIS_FUNCTION_IS_DEPRECATED(func) __declspec(deprecated) func
#else
#define THIS_FUNCTION_IS_DEPRECATED(func) func
#endif



#ifdef dontdefinethis
extern "C" {
#endif

#define JSON_OBJECT_DEF_HASH_ENTRIES 16

/**
 * A flag for the json_object_to_json_string_ext() and
 * json_object_to_file_ext() functions which causes the output
 * to have no extra whitespace or formatting applied.
 */
#define JSON_C_TO_STRING_PLAIN      0
/**
 * A flag for the json_object_to_json_string_ext() and
 * json_object_to_file_ext() functions which causes the output to have
 * minimal whitespace inserted to make things slightly more readable.
 */
#define JSON_C_TO_STRING_SPACED     (1<<0)
/**
 * A flag for the json_object_to_json_string_ext() and
 * json_object_to_file_ext() functions which causes
 * the output to be formatted.
 *
 * See the "Two Space Tab" option at http://jsonformatter.curiousconcept.com/
 * for an example of the format.
 */
#define JSON_C_TO_STRING_PRETTY     (1<<1)
/**
 * A flag to drop trailing zero for float values
 */
#define JSON_C_TO_STRING_NOZERO     (1<<2)

#undef FALSE
#define FALSE ((json_bool)0)

#undef TRUE
#define TRUE ((json_bool)1)

extern const char *json_number_chars;
extern const char *json_hex_chars;

/* CAW: added for ANSI C iteration correctness */
struct json_object_iter
{
	char *key;
	struct json_object *val;
	struct lh_entry *entry;
};

/* forward structure definitions */

typedef int json_bool;
typedef struct printbuf printbuf;
typedef struct lh_table lh_table;
typedef struct array_list array_list;
typedef struct json_object json_object;
typedef struct json_object_iter json_object_iter;
typedef struct json_tokener json_tokener;

/**
 * Type of custom user delete functions.  See json_object_set_serializer.
 */
typedef void (json_object_delete_fn)(struct json_object *jso, void *userdata);

/**
 * Type of a custom serialization function.  See json_object_set_serializer.
 */
typedef int (json_object_to_json_string_fn)(struct json_object *jso,
						struct printbuf *pb,
						int level,
						int flags);

/* supported object types */

typedef enum json_type {
  /* If you change this, be sure to update json_type_to_name() too */
  json_type_null,
  json_type_boolean,
  json_type_double,
  json_type_int,
  json_type_object,
  json_type_array,
  json_type_string,
} json_type;

/* reference counting functions */

/**
 * Increment the reference count of json_object, thereby grabbing shared 
 * ownership of obj.
 *
 * @param obj the json_object instance
 */
extern struct json_object* json_object_get(struct json_object *obj);

/**
 * Decrement the reference count of json_object and free if it reaches zero.
 * You must have ownership of obj prior to doing this or you will cause an
 * imbalance in the reference count.
 *
 * @param obj the json_object instance
 * @returns 1 if the object was freed.
 */
int json_object_put(struct json_object *obj);

/**
 * Check if the json_object is of a given type
 * @param obj the json_object instance
 * @param type one of:
     json_type_null (i.e. obj == NULL),
     json_type_boolean,
     json_type_double,
     json_type_int,
     json_type_object,
     json_type_array,
     json_type_string,
 */
extern int json_object_is_type(struct json_object *obj, enum json_type type);

/**
 * Get the type of the json_object.  See also json_type_to_name() to turn this
 * into a string suitable, for instance, for logging.
 *
 * @param obj the json_object instance
 * @returns type being one of:
     json_type_null (i.e. obj == NULL),
     json_type_boolean,
     json_type_double,
     json_type_int,
     json_type_object,
     json_type_array,
     json_type_string,
 */
extern enum json_type json_object_get_type(struct json_object *obj);


/** Stringify object to json format.
 * Equivalent to json_object_to_json_string_ext(obj, JSON_C_TO_STRING_SPACED)
 * @param obj the json_object instance
 * @returns a string in JSON format
 */
extern const char* json_object_to_json_string(struct json_object *obj);

/** Stringify object to json format
 * @param obj the json_object instance
 * @param flags formatting options, see JSON_C_TO_STRING_PRETTY and other constants
 * @returns a string in JSON format
 */
extern const char* json_object_to_json_string_ext(struct json_object *obj, int
flags);

/**
 * Set a custom serialization function to be used when this particular object
 * is converted to a string by json_object_to_json_string.
 *
 * If a custom serializer is already set on this object, any existing 
 * user_delete function is called before the new one is set.
 *
 * If to_string_func is NULL, the other parameters are ignored
 * and the default behaviour is reset.
 *
 * The userdata parameter is optional and may be passed as NULL.  If provided,
 * it is passed to to_string_func as-is.  This parameter may be NULL even
 * if user_delete is non-NULL.
 *
 * The user_delete parameter is optional and may be passed as NULL, even if
 * the userdata parameter is non-NULL.  It will be called just before the
 * json_object is deleted, after it's reference count goes to zero
 * (see json_object_put()).
 * If this is not provided, it is up to the caller to free the userdata at
 * an appropriate time. (i.e. after the json_object is deleted)
 *
 * @param jso the object to customize
 * @param to_string_func the custom serialization function
 * @param userdata an optional opaque cookie
 * @param user_delete an optional function from freeing userdata
 */
extern void json_object_set_serializer(json_object *jso,
	json_object_to_json_string_fn to_string_func,
	void *userdata,
	json_object_delete_fn *user_delete);

/**
 * Simply call free on the userdata pointer.
 * Can be used with json_object_set_serializer().
 *
 * @param jso unused
 * @param userdata the pointer that is passed to free().
 */
json_object_delete_fn json_object_free_userdata;

/**
 * Copy the jso->_userdata string over to pb as-is.
 * Can be used with json_object_set_serializer().
 *
 * @param jso The object whose _userdata is used.
 * @param pb The destination buffer.
 * @param level Ignored.
 * @param flags Ignored.
 */
json_object_to_json_string_fn json_object_userdata_to_json_string;


/* object type methods */

/** Create a new empty object with a reference count of 1.  The caller of
 * this object initially has sole ownership.  Remember, when using
 * json_object_object_add or json_object_array_put_idx, ownership will
 * transfer to the object/array.  Call json_object_get if you want to maintain
 * shared ownership or also add this object as a child of multiple objects or
 * arrays.  Any ownerships you acquired but did not transfer must be released
 * through json_object_put.
 *
 * @returns a json_object of type json_type_object
 */
extern struct json_object* json_object_new_object(void);

/** Get the hashtable of a json_object of type json_type_object
 * @param obj the json_object instance
 * @returns a linkhash
 */
extern struct lh_table* json_object_get_object(struct json_object *obj);

/** Get the size of an object in terms of the number of fields it has.
 * @param obj the json_object whose length to return
 */
extern int json_object_object_length(struct json_object* obj);

/** Add an object field to a json_object of type json_type_object
 *
 * The reference count will *not* be incremented. This is to make adding
 * fields to objects in code more compact. If you want to retain a reference
 * to an added object, independent of the lifetime of obj, you must wrap the
 * passed object with json_object_get.
 *
 * Upon calling this, the ownership of val transfers to obj.  Thus you must
 * make sure that you do in fact have ownership over this object.  For instance,
 * json_object_new_object will give you ownership until you transfer it,
 * whereas json_object_object_get does not.
 *
 * @param obj the json_object instance
 * @param key the object field name (a private copy will be duplicated)
 * @param val a json_object or NULL member to associate with the given field
 */
extern void json_object_object_add(struct json_object* obj, const char *key,
				   struct json_object *val);

/** Get the json_object associate with a given object field
 *
 * *No* reference counts will be changed.  There is no need to manually adjust
 * reference counts through the json_object_put/json_object_get methods unless
 * you need to have the child (value) reference maintain a different lifetime
 * than the owning parent (obj). Ownership of the returned value is retained
 * by obj (do not do json_object_put unless you have done a json_object_get).
 * If you delete the value from obj (json_object_object_del) and wish to access
 * the returned reference afterwards, make sure you have first gotten shared
 * ownership through json_object_get (& don't forget to do a json_object_put
 * or transfer ownership to prevent a memory leak).
 *
 * @param obj the json_object instance
 * @param key the object field name
 * @returns the json_object associated with the given field name
 * @deprecated Please use json_object_object_get_ex
 */
THIS_FUNCTION_IS_DEPRECATED(extern struct json_object* json_object_object_get(struct json_object* obj,
						  const char *key));

/** Get the json_object associated with a given object field.  
 *
 * This returns true if the key is found, false in all other cases (including 
 * if obj isn't a json_type_object).
 *
 * *No* reference counts will be changed.  There is no need to manually adjust
 * reference counts through the json_object_put/json_object_get methods unless
 * you need to have the child (value) reference maintain a different lifetime
 * than the owning parent (obj).  Ownership of value is retained by obj.
 *
 * @param obj the json_object instance
 * @param key the object field name
 * @param value a pointer where to store a reference to the json_object 
 *              associated with the given field name.
 *
 *              It is safe to pass a NULL value.
 * @returns whether or not the key exists
 */
extern json_bool json_object_object_get_ex(struct json_object* obj,
						  const char *key,
                                                  struct json_object **value);

/** Delete the given json_object field
 *
 * The reference count will be decremented for the deleted object.  If there
 * are no more owners of the value represented by this key, then the value is
 * freed.  Otherwise, the reference to the value will remain in memory.
 *
 * @param obj the json_object instance
 * @param key the object field name
 */
extern void json_object_object_del(struct json_object* obj, const char *key);

/**
 * Iterate through all keys and values of an object.
 *
 * Adding keys to the object while iterating is NOT allowed.
 *
 * Deleting an existing key, or replacing an existing key with a
 * new value IS allowed.
 *
 * @param obj the json_object instance
 * @param key the local name for the char* key variable defined in the body
 * @param val the local name for the json_object* object variable defined in
 *            the body
 */
#if defined(__GNUC__) && !defined(__STRICT_ANSI__) && __STDC_VERSION__ >= 199901L

# define json_object_object_foreach(obj,key,val) \
	char *key; \
	struct json_object *val __attribute__((__unused__)); \
	for(struct lh_entry *entry ## key = json_object_get_object(obj)->head, *entry_next ## key = NULL; \
		({ if(entry ## key) { \
			key = (char*)entry ## key->k; \
			val = (struct json_object*)entry ## key->v; \
			entry_next ## key = entry ## key->next; \
		} ; entry ## key; }); \
		entry ## key = entry_next ## key )

#else /* ANSI C or MSC */

# define json_object_object_foreach(obj,key,val) \
	char *key;\
	struct json_object *val; \
	struct lh_entry *entry ## key; \
	struct lh_entry *entry_next ## key = NULL; \
	for(entry ## key = json_object_get_object(obj)->head; \
		(entry ## key ? ( \
			key = (char*)entry ## key->k, \
			val = (struct json_object*)entry ## key->v, \
			entry_next ## key = entry ## key->next, \
			entry ## key) : 0); \
		entry ## key = entry_next ## key)

#endif /* defined(__GNUC__) && !defined(__STRICT_ANSI__) && __STDC_VERSION__ >= 199901L */

/** Iterate through all keys and values of an object (ANSI C Safe)
 * @param obj the json_object instance
 * @param iter the object iterator
 */
#define json_object_object_foreachC(obj,iter) \
 for(iter.entry = json_object_get_object(obj)->head; (iter.entry ? (iter.key = (char*)iter.entry->k, iter.val = (struct json_object*)iter.entry->v, iter.entry) : 0); iter.entry = iter.entry->next)

/* Array type methods */

/** Create a new empty json_object of type json_type_array
 * @returns a json_object of type json_type_array
 */
extern struct json_object* json_object_new_array(void);

/** Get the arraylist of a json_object of type json_type_array
 * @param obj the json_object instance
 * @returns an arraylist
 */
extern struct array_list* json_object_get_array(struct json_object *obj);

/** Get the length of a json_object of type json_type_array
 * @param obj the json_object instance
 * @returns an int
 */
extern int json_object_array_length(struct json_object *obj);

/** Sorts the elements of jso of type json_type_array
*
* Pointers to the json_object pointers will be passed as the two arguments
* to @sort_fn
*
* @param obj the json_object instance
* @param sort_fn a sorting function
*/
extern void json_object_array_sort(struct json_object *jso, int(*sort_fn)(const void *, const void *));

/** Add an element to the end of a json_object of type json_type_array
 *
 * The reference count will *not* be incremented. This is to make adding
 * fields to objects in code more compact. If you want to retain a reference
 * to an added object you must wrap the passed object with json_object_get
 *
 * @param obj the json_object instance
 * @param val the json_object to be added
 */
extern int json_object_array_add(struct json_object *obj,
				 struct json_object *val);

/** Insert or replace an element at a specified index in an array (a json_object of type json_type_array)
 *
 * The reference count will *not* be incremented. This is to make adding
 * fields to objects in code more compact. If you want to retain a reference
 * to an added object you must wrap the passed object with json_object_get
 *
 * The reference count of a replaced object will be decremented.
 *
 * The array size will be automatically be expanded to the size of the
 * index if the index is larger than the current size.
 *
 * @param obj the json_object instance
 * @param idx the index to insert the element at
 * @param val the json_object to be added
 */
extern int json_object_array_put_idx(struct json_object *obj, int idx,
				     struct json_object *val);

/** Get the element at specificed index of the array (a json_object of type json_type_array)
 * @param obj the json_object instance
 * @param idx the index to get the element at
 * @returns the json_object at the specified index (or NULL)
 */
extern struct json_object* json_object_array_get_idx(struct json_object *obj,
						     int idx);

/* json_bool type methods */

/** Create a new empty json_object of type json_type_boolean
 * @param b a json_bool TRUE or FALSE (0 or 1)
 * @returns a json_object of type json_type_boolean
 */
extern struct json_object* json_object_new_boolean(json_bool b);

/** Get the json_bool value of a json_object
 *
 * The type is coerced to a json_bool if the passed object is not a json_bool.
 * integer and double objects will return FALSE if there value is zero
 * or TRUE otherwise. If the passed object is a string it will return
 * TRUE if it has a non zero length. If any other object type is passed
 * TRUE will be returned if the object is not NULL.
 *
 * @param obj the json_object instance
 * @returns a json_bool
 */
extern json_bool json_object_get_boolean(struct json_object *obj);


/* int type methods */

/** Create a new empty json_object of type json_type_int
 * Note that values are stored as 64-bit values internally.
 * To ensure the full range is maintained, use json_object_new_int64 instead.
 * @param i the integer
 * @returns a json_object of type json_type_int
 */
extern struct json_object* json_object_new_int(int32_t i);


/** Create a new empty json_object of type json_type_int
 * @param i the integer
 * @returns a json_object of type json_type_int
 */
extern struct json_object* json_object_new_int64(int64_t i);


/** Get the int value of a json_object
 *
 * The type is coerced to a int if the passed object is not a int.
 * double objects will return their integer conversion. Strings will be
 * parsed as an integer. If no conversion exists then 0 is returned
 * and errno is set to EINVAL. null is equivalent to 0 (no error values set)
 *
 * Note that integers are stored internally as 64-bit values.
 * If the value of too big or too small to fit into 32-bit, INT32_MAX or
 * INT32_MIN are returned, respectively.
 *
 * @param obj the json_object instance
 * @returns an int
 */
extern int32_t json_object_get_int(struct json_object *obj);

/** Get the int value of a json_object
 *
 * The type is coerced to a int64 if the passed object is not a int64.
 * double objects will return their int64 conversion. Strings will be
 * parsed as an int64. If no conversion exists then 0 is returned.
 *
 * NOTE: Set errno to 0 directly before a call to this function to determine
 * whether or not conversion was successful (it does not clear the value for
 * you).
 *
 * @param obj the json_object instance
 * @returns an int64
 */
extern int64_t json_object_get_int64(struct json_object *obj);


/* double type methods */

/** Create a new empty json_object of type json_type_double
 * @param d the double
 * @returns a json_object of type json_type_double
 */
extern struct json_object* json_object_new_double(double d);

/**
 * Create a new json_object of type json_type_double, using
 * the exact serialized representation of the value.
 *
 * This allows for numbers that would otherwise get displayed
 * inefficiently (e.g. 12.3 => "12.300000000000001") to be
 * serialized with the more convenient form.
 *
 * Note: this is used by json_tokener_parse_ex() to allow for
 *   an exact re-serialization of a parsed object.
 *
 * An equivalent sequence of calls is:
 * @code
 *   jso = json_object_new_double(d);
 *   json_object_set_serializer(d, json_object_userdata_to_json_string,
 *       strdup(ds), json_object_free_userdata)
 * @endcode
 *
 * @param d the numeric value of the double.
 * @param ds the string representation of the double.  This will be copied.
 */
extern struct json_object* json_object_new_double_s(double d, const char *ds);

/** Get the double floating point value of a json_object
 *
 * The type is coerced to a double if the passed object is not a double.
 * integer objects will return their double conversion. Strings will be
 * parsed as a double. If no conversion exists then 0.0 is returned and
 * errno is set to EINVAL. null is equivalent to 0 (no error values set)
 *
 * If the value is too big to fit in a double, then the value is set to
 * the closest infinity with errno set to ERANGE. If strings cannot be
 * converted to their double value, then EINVAL is set & NaN is returned.
 *
 * Arrays of length 0 are interpreted as 0 (with no error flags set).
 * Arrays of length 1 are effectively cast to the equivalent object and
 * converted using the above rules.  All other arrays set the error to
 * EINVAL & return NaN.
 *
 * NOTE: Set errno to 0 directly before a call to this function to
 * determine whether or not conversion was successful (it does not clear
 * the value for you).
 *
 * @param obj the json_object instance
 * @returns a double floating point number
 */
extern double json_object_get_double(struct json_object *obj);


/* string type methods */

/** Create a new empty json_object of type json_type_string
 *
 * A copy of the string is made and the memory is managed by the json_object
 *
 * @param s the string
 * @returns a json_object of type json_type_string
 */
extern struct json_object* json_object_new_string(const char *s);

extern struct json_object* json_object_new_string_len(const char *s, int len);

/** Get the string value of a json_object
 *
 * If the passed object is not of type json_type_string then the JSON
 * representation of the object is returned.
 *
 * The returned string memory is managed by the json_object and will
 * be freed when the reference count of the json_object drops to zero.
 *
 * @param obj the json_object instance
 * @returns a string
 */
extern const char* json_object_get_string(struct json_object *obj);

/** Get the string length of a json_object
 *
 * If the passed object is not of type json_type_string then zero
 * will be returned.
 *
 * @param obj the json_object instance
 * @returns int
 */
extern int json_object_get_string_len(struct json_object *obj);

#ifdef dontdefinethis
}
#endif

#endif
//-------json_object_private.h-------

/*
 * $Id: json_object_private.h,v 1.4 2006/01/26 02:16:28 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

#ifndef _json_object_private_h_
#define _json_object_private_h_

#ifdef dontdefinethis
extern "C" {
#endif

typedef void (json_object_private_delete_fn)(struct json_object *o);

struct json_object
{
  enum json_type o_type;
  json_object_private_delete_fn *_delete;
  json_object_to_json_string_fn *_to_json_string;
  int _ref_count;
  struct printbuf *_pb;
  union data {
    json_bool c_boolean;
    double c_double;
    int64_t c_int64;
    struct lh_table *c_object;
    struct array_list *c_array;
    struct {
        char *str;
        int len;
    } c_string;
  } o;
  json_object_delete_fn *_user_delete;
  void *_userdata;
};

#ifdef dontdefinethis
}
#endif

#endif
//-------json_object_iterator.h-------

/**
*******************************************************************************
* @file json_object_iterator.h
*
* Copyright (c) 2009-2012 Hewlett-Packard Development Company, L.P.
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the MIT license. See COPYING for details.
*
* @brief  json-c forces clients to use its private data
*         structures for JSON Object iteration.  This API
*         corrects that by abstracting the private json-c
*         details.
*
* API attributes: <br>
*   * Thread-safe: NO<br>
*   * Reentrant: NO
*
*******************************************************************************
*/


#ifndef JSON_OBJECT_ITERATOR_H
#define JSON_OBJECT_ITERATOR_H

#include <stddef.h>

#ifdef dontdefinethis
extern "C" {
#endif

/**
 * Forward declaration for the opaque iterator information.
 */
struct json_object_iter_info_;

/**
 * The opaque iterator that references a name/value pair within
 * a JSON Object instance or the "end" iterator value.
 */
struct json_object_iterator {
    const void* opaque_;
};


/**
 * forward declaration of json-c's JSON value instance structure
 */
struct json_object;


/**
 * Initializes an iterator structure to a "default" value that
 * is convenient for initializing an iterator variable to a
 * default state (e.g., initialization list in a class'
 * constructor).
 *
 * @code
 * struct json_object_iterator iter = json_object_iter_init_default();
 * MyClass() : iter_(json_object_iter_init_default())
 * @endcode
 *
 * @note The initialized value doesn't reference any specific
 *       pair, is considered an invalid iterator, and MUST NOT
 *       be passed to any json-c API that expects a valid
 *       iterator.
 *
 * @note User and internal code MUST NOT make any assumptions
 *       about and dependencies on the value of the "default"
 *       iterator value.
 *
 * @return json_object_iterator
 */
struct json_object_iterator
json_object_iter_init_default(void);

/** Retrieves an iterator to the first pair of the JSON Object.
 *
 * @warning 	Any modification of the underlying pair invalidates all
 * 		iterators to that pair.
 *
 * @param obj	JSON Object instance (MUST be of type json_object)
 *
 * @return json_object_iterator If the JSON Object has at
 *              least one pair, on return, the iterator refers
 *              to the first pair. If the JSON Object doesn't
 *              have any pairs, the returned iterator is
 *              equivalent to the "end" iterator for the same
 *              JSON Object instance.
 *
 * @code
 * struct json_object_iterator it;
 * struct json_object_iterator itEnd;
 * struct json_object* obj;
 *
 * obj = json_tokener_parse("{'first':'george', 'age':100}");
 * it = json_object_iter_begin(obj);
 * itEnd = json_object_iter_end(obj);
 *
 * while (!json_object_iter_equal(&it, &itEnd)) {
 *     printf("%s\n",
 *            json_object_iter_peek_name(&it));
 *     json_object_iter_next(&it);
 * }
 *
 * @endcode
 */
struct json_object_iterator
json_object_iter_begin(struct json_object* obj);

/** Retrieves the iterator that represents the position beyond the
 *  last pair of the given JSON Object instance.
 *
 *  @warning Do NOT write code that assumes that the "end"
 *        iterator value is NULL, even if it is so in a
 *        particular instance of the implementation.
 *
 *  @note The reason we do not (and MUST NOT) provide
 *        "json_object_iter_is_end(json_object_iterator* iter)"
 *        type of API is because it would limit the underlying
 *        representation of name/value containment (or force us
 *        to add additional, otherwise unnecessary, fields to
 *        the iterator structure). The "end" iterator and the
 *        equality test method, on the other hand, permit us to
 *        cleanly abstract pretty much any reasonable underlying
 *        representation without burdening the iterator
 *        structure with unnecessary data.
 *
 *  @note For performance reasons, memorize the "end" iterator prior
 *        to any loop.
 *
 * @param obj JSON Object instance (MUST be of type json_object)
 *
 * @return json_object_iterator On return, the iterator refers
 *              to the "end" of the Object instance's pairs
 *              (i.e., NOT the last pair, but "beyond the last
 *              pair" value)
 */
struct json_object_iterator
json_object_iter_end(const struct json_object* obj);

/** Returns an iterator to the next pair, if any
 *
 * @warning	Any modification of the underlying pair
 *       	invalidates all iterators to that pair.
 *
 * @param iter [IN/OUT] Pointer to iterator that references a
 *         name/value pair; MUST be a valid, non-end iterator.
 *         WARNING: bad things will happen if invalid or "end"
 *         iterator is passed. Upon return will contain the
 *         reference to the next pair if there is one; if there
 *         are no more pairs, will contain the "end" iterator
 *         value, which may be compared against the return value
 *         of json_object_iter_end() for the same JSON Object
 *         instance.
 */
void
json_object_iter_next(struct json_object_iterator* iter);


/** Returns a const pointer to the name of the pair referenced
 *  by the given iterator.
 *
 * @param iter pointer to iterator that references a name/value
 *             pair; MUST be a valid, non-end iterator.
 *
 * @warning	bad things will happen if an invalid or
 *             	"end" iterator is passed.
 *
 * @return const char* Pointer to the name of the referenced
 *         name/value pair.  The name memory belongs to the
 *         name/value pair, will be freed when the pair is
 *         deleted or modified, and MUST NOT be modified or
 *         freed by the user.
 */
const char*
json_object_iter_peek_name(const struct json_object_iterator* iter);


/** Returns a pointer to the json-c instance representing the
 *  value of the referenced name/value pair, without altering
 *  the instance's reference count.
 *
 * @param iter 	pointer to iterator that references a name/value
 *             	pair; MUST be a valid, non-end iterator.
 *
 * @warning	bad things will happen if invalid or
 *             "end" iterator is passed.
 *
 * @return struct json_object* Pointer to the json-c value
 *         instance of the referenced name/value pair;  the
 *         value's reference count is not changed by this
 *         function: if you plan to hold on to this json-c node,
 *         take a look at json_object_get() and
 *         json_object_put(). IMPORTANT: json-c API represents
 *         the JSON Null value as a NULL json_object instance
 *         pointer.
 */
struct json_object*
json_object_iter_peek_value(const struct json_object_iterator* iter);


/** Tests two iterators for equality.  Typically used to test
 *  for end of iteration by comparing an iterator to the
 *  corresponding "end" iterator (that was derived from the same
 *  JSON Object instance).
 *
 *  @note The reason we do not (and MUST NOT) provide
 *        "json_object_iter_is_end(json_object_iterator* iter)"
 *        type of API is because it would limit the underlying
 *        representation of name/value containment (or force us
 *        to add additional, otherwise unnecessary, fields to
 *        the iterator structure). The equality test method, on
 *        the other hand, permits us to cleanly abstract pretty
 *        much any reasonable underlying representation.
 *
 * @param iter1 Pointer to first valid, non-NULL iterator
 * @param iter2 POinter to second valid, non-NULL iterator
 *
 * @warning	if a NULL iterator pointer or an uninitialized
 *       	or invalid iterator, or iterators derived from
 *       	different JSON Object instances are passed, bad things
 *       	will happen!
 *
 * @return json_bool non-zero if iterators are equal (i.e., both
 *         reference the same name/value pair or are both at
 *         "end"); zero if they are not equal.
 */
json_bool
json_object_iter_equal(const struct json_object_iterator* iter1,
                       const struct json_object_iterator* iter2);


#ifdef dontdefinethis
}
#endif


#endif /* JSON_OBJECT_ITERATOR_H */
//-------json_util.h-------

/*
 * $Id: json_util.h,v 1.4 2006/01/30 23:07:57 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

#ifndef _json_util_h_
#define _json_util_h_



#ifndef json_min
#define json_min(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef json_max
#define json_max(a,b) ((a) > (b) ? (a) : (b))
#endif


#ifdef dontdefinethis
extern "C" {
#endif

#define JSON_FILE_BUF_SIZE 4096

/* utility functions */
extern struct json_object* json_object_from_file(const char *filename);
extern int json_object_to_file(const char *filename, struct json_object *obj);
extern int json_object_to_file_ext(const char *filename, struct json_object *obj, int flags);
extern int json_parse_int64(const char *buf, int64_t *retval);
extern int json_parse_double(const char *buf, double *retval);


/**
 * Return a string describing the type of the object.
 * e.g. "int", or "object", etc...
 */
extern const char *json_type_to_name(enum json_type o_type);

#ifdef dontdefinethis
}
#endif

#endif
//-------json_tokener.h-------

/*
 * $Id: json_tokener.h,v 1.10 2006/07/25 03:24:50 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

#ifndef _json_tokener_h_
#define _json_tokener_h_

#include <stddef.h>


#ifdef dontdefinethis
extern "C" {
#endif

enum json_tokener_error {
  json_tokener_success,
  json_tokener_continue,
  json_tokener_error_depth,
  json_tokener_error_parse_eof,
  json_tokener_error_parse_unexpected,
  json_tokener_error_parse_null,
  json_tokener_error_parse_boolean,
  json_tokener_error_parse_number,
  json_tokener_error_parse_array,
  json_tokener_error_parse_object_key_name,
  json_tokener_error_parse_object_key_sep,
  json_tokener_error_parse_object_value_sep,
  json_tokener_error_parse_string,
  json_tokener_error_parse_comment,
  json_tokener_error_size
};

enum json_tokener_state {
  json_tokener_state_eatws,
  json_tokener_state_start,
  json_tokener_state_finish,
  json_tokener_state_null,
  json_tokener_state_comment_start,
  json_tokener_state_comment,
  json_tokener_state_comment_eol,
  json_tokener_state_comment_end,
  json_tokener_state_string,
  json_tokener_state_string_escape,
  json_tokener_state_escape_unicode,
  json_tokener_state_boolean,
  json_tokener_state_number,
  json_tokener_state_array,
  json_tokener_state_array_add,
  json_tokener_state_array_sep,
  json_tokener_state_object_field_start,
  json_tokener_state_object_field,
  json_tokener_state_object_field_end,
  json_tokener_state_object_value,
  json_tokener_state_object_value_add,
  json_tokener_state_object_sep,
  json_tokener_state_array_after_sep,
  json_tokener_state_object_field_start_after_sep,
  json_tokener_state_inf
};

struct json_tokener_srec
{
  enum json_tokener_state state, saved_state;
  struct json_object *obj;
  struct json_object *current;
  char *obj_field_name;
};

#define JSON_TOKENER_DEFAULT_DEPTH 32

struct json_tokener
{
  char *str;
  struct printbuf *pb;
  int max_depth, depth, is_double, st_pos, char_offset;
  enum json_tokener_error err;
  unsigned int ucs_char;
  char quote_char;
  struct json_tokener_srec *stack;
  int flags;
};

/**
 * Be strict when parsing JSON input.  Use caution with
 * this flag as what is considered valid may become more
 * restrictive from one release to the next, causing your
 * code to fail on previously working input.
 *
 * This flag is not set by default.
 *
 * @see json_tokener_set_flags()
 */
#define JSON_TOKENER_STRICT  0x01

/**
 * Given an error previously returned by json_tokener_get_error(),
 * return a human readable description of the error.
 *
 * @return a generic error message is returned if an invalid error value is provided.
 */
const char *json_tokener_error_desc(enum json_tokener_error jerr);

/**
 * Retrieve the error caused by the last call to json_tokener_parse_ex(),
 * or json_tokener_success if there is no error.
 *
 * When parsing a JSON string in pieces, if the tokener is in the middle
 * of parsing this will return json_tokener_continue.
 *
 * See also json_tokener_error_desc().
 */
enum json_tokener_error json_tokener_get_error(struct json_tokener *tok);

extern struct json_tokener* json_tokener_new(void);
extern struct json_tokener* json_tokener_new_ex(int depth);
extern void json_tokener_free(struct json_tokener *tok);
extern void json_tokener_reset(struct json_tokener *tok);
extern struct json_object* json_tokener_parse(const char *str);
extern struct json_object* json_tokener_parse_verbose(const char *str, enum json_tokener_error *error);

/**
 * Set flags that control how parsing will be done.
 */
extern void json_tokener_set_flags(struct json_tokener *tok, int flags);

/** 
 * Parse a string and return a non-NULL json_object if a valid JSON value
 * is found.  The string does not need to be a JSON object or array;
 * it can also be a string, number or boolean value.
 *
 * A partial JSON string can be parsed.  If the parsing is incomplete,
 * NULL will be returned and json_tokener_get_error() will be return 
 * json_tokener_continue.
 * json_tokener_parse_ex() can then be called with additional bytes in str
 * to continue the parsing.  
 *
 * If json_tokener_parse_ex() returns NULL and the error anything other than
 * json_tokener_continue, a fatal error has occurred and parsing must be
 * halted.  Then tok object must not be re-used until json_tokener_reset() is
 * called.
 *
 * When a valid JSON value is parsed, a non-NULL json_object will be
 * returned.  Also, json_tokener_get_error() will return json_tokener_success.
 * Be sure to check the type with json_object_is_type() or
 * json_object_get_type() before using the object.
 *
 * @b XXX this shouldn't use internal fields:
 * Trailing characters after the parsed value do not automatically cause an 
 * error.  It is up to the caller to decide whether to treat this as an
 * error or to handle the additional characters, perhaps by parsing another
 * json value starting from that point.
 *
 * Extra characters can be detected by comparing the tok->char_offset against
 * the length of the last len parameter passed in.
 *
 * The tokener does \b not maintain an internal buffer so the caller is
 * responsible for calling json_tokener_parse_ex with an appropriate str
 * parameter starting with the extra characters.
 *
 * This interface is presently not 64-bit clean due to the int len argument
 * so the function limits the maximum string size to INT32_MAX (2GB).
 * If the function is called with len == -1 then strlen is called to check
 * the string length is less than INT32_MAX (2GB)
 *
 * Example:
 * @code
json_object *jobj = NULL;
const char *mystring = NULL;
int stringlen = 0;
enum json_tokener_error jerr;
do {
	mystring = ...  // get JSON string, e.g. read from file, etc...
	stringlen = strlen(mystring);
	jobj = json_tokener_parse_ex(tok, mystring, stringlen);
} while ((jerr = json_tokener_get_error(tok)) == json_tokener_continue);
if (jerr != json_tokener_success)
{
	fprintf(stderr, "Error: %s\n", json_tokener_error_desc(jerr));
	// Handle errors, as appropriate for your application.
}
if (tok->char_offset < stringlen) // XXX shouldn't access internal fields
{
	// Handle extra characters after parsed object as desired.
	// e.g. issue an error, parse another object from that point, etc...
}
// Success, use jobj here.

@endcode
 *
 * @param tok a json_tokener previously allocated with json_tokener_new()
 * @param str an string with any valid JSON expression, or portion of.  This does not need to be null terminated.
 * @param len the length of str
 */
extern struct json_object* json_tokener_parse_ex(struct json_tokener *tok,
						 const char *str, int len);

#ifdef dontdefinethis
}
#endif

#endif
//-------linkhash.h-------

/*
 * $Id: linkhash.h,v 1.6 2006/01/30 23:07:57 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 * Copyright (c) 2009 Hewlett-Packard Development Company, L.P.
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */
 
#ifndef _linkhash_h_
#define _linkhash_h_



#ifdef dontdefinethis
extern "C" {
#endif

/**
 * golden prime used in hash functions
 */
#define LH_PRIME 0x9e370001UL

/**
 * The fraction of filled hash buckets until an insert will cause the table
 * to be resized.  
 * This can range from just above 0 up to 1.0.
 */
#define LH_LOAD_FACTOR 0.66

/**
 * sentinel pointer value for empty slots
 */
#define LH_EMPTY (void*)-1

/**
 * sentinel pointer value for freed slots
 */
#define LH_FREED (void*)-2

struct lh_entry;

/**
 * callback function prototypes
 */
typedef void (lh_entry_free_fn) (struct lh_entry *e);
/**
 * callback function prototypes
 */
typedef unsigned long (lh_hash_fn) (const void *k);
/**
 * callback function prototypes
 */
typedef int (lh_equal_fn) (const void *k1, const void *k2);

/**
 * An entry in the hash table
 */
struct lh_entry {
	/**
	 * The key.
	 */
	void *k;
	/**
	 * The value.
	 */
	const void *v;
	/**
	 * The next entry
	 */
	struct lh_entry *next;
	/**
	 * The previous entry.
	 */
	struct lh_entry *prev;
};


/**
 * The hash table structure.
 */
struct lh_table {
	/**
	 * Size of our hash.
	 */
	int size;
	/**
	 * Numbers of entries.
	 */
	int count;

	/**
	 * Number of collisions.
	 */
	int collisions;

	/**
	 * Number of resizes.
	 */
	int resizes;

	/**
	 * Number of lookups.
	 */
	int lookups;

	/**
	 * Number of inserts.
	 */
	int inserts;

	/**
	 * Number of deletes.
	 */
	int deletes;

	/**
	 * Name of the hash table.
	 */
	const char *name;

	/**
	 * The first entry.
	 */
	struct lh_entry *head;

	/**
	 * The last entry.
	 */
	struct lh_entry *tail;

	struct lh_entry *table;

	/**
	 * A pointer onto the function responsible for freeing an entry.
	 */
	lh_entry_free_fn *free_fn;
	lh_hash_fn *hash_fn;
	lh_equal_fn *equal_fn;
};


/**
 * Pre-defined hash and equality functions
 */
extern unsigned long lh_ptr_hash(const void *k);
extern int lh_ptr_equal(const void *k1, const void *k2);

extern unsigned long lh_char_hash(const void *k);
extern int lh_char_equal(const void *k1, const void *k2);


/**
 * Convenience list iterator.
 */
#define lh_foreach(table, entry) \
for(entry = table->head; entry; entry = entry->next)

/**
 * lh_foreach_safe allows calling of deletion routine while iterating.
 */
#define lh_foreach_safe(table, entry, tmp) \
for(entry = table->head; entry && ((tmp = entry->next) || 1); entry = tmp)



/**
 * Create a new linkhash table.
 * @param size initial table size. The table is automatically resized
 * although this incurs a performance penalty.
 * @param name the table name.
 * @param free_fn callback function used to free memory for entries
 * when lh_table_free or lh_table_delete is called.
 * If NULL is provided, then memory for keys and values
 * must be freed by the caller.
 * @param hash_fn  function used to hash keys. 2 standard ones are defined:
 * lh_ptr_hash and lh_char_hash for hashing pointer values
 * and C strings respectively.
 * @param equal_fn comparison function to compare keys. 2 standard ones defined:
 * lh_ptr_hash and lh_char_hash for comparing pointer values
 * and C strings respectively.
 * @return a pointer onto the linkhash table.
 */
extern struct lh_table* lh_table_new(int size, const char *name,
				     lh_entry_free_fn *free_fn,
				     lh_hash_fn *hash_fn,
				     lh_equal_fn *equal_fn);

/**
 * Convenience function to create a new linkhash
 * table with char keys.
 * @param size initial table size.
 * @param name table name.
 * @param free_fn callback function used to free memory for entries.
 * @return a pointer onto the linkhash table.
 */
extern struct lh_table* lh_kchar_table_new(int size, const char *name,
					   lh_entry_free_fn *free_fn);


/**
 * Convenience function to create a new linkhash
 * table with ptr keys.
 * @param size initial table size.
 * @param name table name.
 * @param free_fn callback function used to free memory for entries.
 * @return a pointer onto the linkhash table.
 */
extern struct lh_table* lh_kptr_table_new(int size, const char *name,
					  lh_entry_free_fn *free_fn);


/**
 * Free a linkhash table.
 * If a callback free function is provided then it is called for all
 * entries in the table.
 * @param t table to free.
 */
extern void lh_table_free(struct lh_table *t);


/**
 * Insert a record into the table.
 * @param t the table to insert into.
 * @param k a pointer to the key to insert.
 * @param v a pointer to the value to insert.
 */
extern int lh_table_insert(struct lh_table *t, void *k, const void *v);


/**
 * Lookup a record into the table.
 * @param t the table to lookup
 * @param k a pointer to the key to lookup
 * @return a pointer to the record structure of the value or NULL if it does not exist.
 */
extern struct lh_entry* lh_table_lookup_entry(struct lh_table *t, const void *k);

/**
 * Lookup a record into the table
 * @param t the table to lookup
 * @param k a pointer to the key to lookup
 * @return a pointer to the found value or NULL if it does not exist.
 * @deprecated Use lh_table_lookup_ex instead.
 */
THIS_FUNCTION_IS_DEPRECATED(extern const void* lh_table_lookup(struct lh_table *t, const void *k));

/**
 * Lookup a record in the table
 * @param t the table to lookup
 * @param k a pointer to the key to lookup
 * @param v a pointer to a where to store the found value (set to NULL if it doesn't exist).
 * @return whether or not the key was found
 */
extern json_bool lh_table_lookup_ex(struct lh_table *t, const void *k, void **v);

/**
 * Delete a record from the table.
 * If a callback free function is provided then it is called for the
 * for the item being deleted.
 * @param t the table to delete from.
 * @param e a pointer to the entry to delete.
 * @return 0 if the item was deleted.
 * @return -1 if it was not found.
 */
extern int lh_table_delete_entry(struct lh_table *t, struct lh_entry *e);


/**
 * Delete a record from the table.
 * If a callback free function is provided then it is called for the
 * for the item being deleted.
 * @param t the table to delete from.
 * @param k a pointer to the key to delete.
 * @return 0 if the item was deleted.
 * @return -1 if it was not found.
 */
extern int lh_table_delete(struct lh_table *t, const void *k);

extern int lh_table_length(struct lh_table *t);

void lh_abort(const char *msg, ...);
void lh_table_resize(struct lh_table *t, int new_size);

#ifdef dontdefinethis
}
#endif

#endif
//-------json.h-------

/*
 * $Id: json.h,v 1.6 2006/01/26 02:16:28 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 * Copyright (c) 2009 Hewlett-Packard Development Company, L.P.
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

#ifndef _json_h_
#define _json_h_

#ifdef dontdefinethis
extern "C" {
#endif










#ifdef dontdefinethis
}
#endif

#endif
//-------polyfill.c-------

#include <string.h>
void *calloc(size_t size, size_t num) { 
    void *ret = malloc(size * num);

    if (ret)  
        memset(ret, 0, size * num);
    return ret;
}

//char* strdup(const char *);
//-------arraylist.c-------

/*
 * $Id: arraylist.c,v 1.4 2006/01/26 02:16:28 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */



#ifdef STDC_HEADERS
# include <stdlib.h>
# include <string.h>
#endif /* STDC_HEADERS */

#if defined(HAVE_STRINGS_H) && !defined(_STRING_H) && !defined(__USE_BSD)
# include <strings.h>
#endif /* HAVE_STRINGS_H */



struct array_list*
array_list_new(array_list_free_fn *free_fn)
{
  struct array_list *arr;

  arr = (struct array_list*)calloc(1, sizeof(struct array_list));
  if(!arr) return NULL;
  arr->size = ARRAY_LIST_DEFAULT_SIZE;
  arr->length = 0;
  arr->free_fn = free_fn;
  if(!(arr->array = (void**)calloc(sizeof(void*), arr->size))) {
    free(arr);
    return NULL;
  }
  return arr;
}

extern void
array_list_free(struct array_list *arr)
{
  int i;
  for(i = 0; i < arr->length; i++)
    if(arr->array[i]) arr->free_fn(arr->array[i]);
  free(arr->array);
  free(arr);
}

void*
array_list_get_idx(struct array_list *arr, int i)
{
  if(i >= arr->length) return NULL;
  return arr->array[i];
}

static int array_list_expand_internal(struct array_list *arr, int max)
{
  void *t;
  int new_size;

  if(max < arr->size) return 0;
  new_size = arr->size << 1;
  if (new_size < max)
    new_size = max;
  if(!(t = realloc(arr->array, new_size*sizeof(void*)))) return -1;
  arr->array = (void**)t;
  (void)memset(arr->array + arr->size, 0, (new_size-arr->size)*sizeof(void*));
  arr->size = new_size;
  return 0;
}

int
array_list_put_idx(struct array_list *arr, int idx, void *data)
{
  if(array_list_expand_internal(arr, idx+1)) return -1;
  if(arr->array[idx]) arr->free_fn(arr->array[idx]);
  arr->array[idx] = data;
  if(arr->length <= idx) arr->length = idx + 1;
  return 0;
}

int
array_list_add(struct array_list *arr, void *data)
{
  return array_list_put_idx(arr, arr->length, data);
}

void
array_list_sort(struct array_list *arr, int(*sort_fn)(const void *, const void *))
{
  qsort(arr->array, arr->length, sizeof(arr->array[0]),
	(int (*)(const void *, const void *))sort_fn);
}

int
array_list_length(struct array_list *arr)
{
  return arr->length;
}
//-------linkhash.c-------

/*
 * $Id: linkhash.c,v 1.4 2006/01/26 02:16:28 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 * Copyright (c) 2009 Hewlett-Packard Development Company, L.P.
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <limits.h>

#ifdef HAVE_ENDIAN_H
# include <endian.h>    /* attempt to define endianness */
#endif




void lh_abort(const char *msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	vprintf(msg, ap);
	va_end(ap);
	exit(1);
}

unsigned long lh_ptr_hash(const void *k)
{
	/* CAW: refactored to be 64bit nice */
	return (unsigned long)((((ptrdiff_t)k * LH_PRIME) >> 4) & ULONG_MAX);
}

int lh_ptr_equal(const void *k1, const void *k2)
{
	return (k1 == k2);
}

/* 
 * hashlittle from lookup3.c, by Bob Jenkins, May 2006, Public Domain.
 * http://burtleburtle.net/bob/c/lookup3.c
 * minor modifications to make functions static so no symbols are exported
 * minor mofifications to compile with -Werror
 */

/*
-------------------------------------------------------------------------------
lookup3.c, by Bob Jenkins, May 2006, Public Domain.

These are functions for producing 32-bit hashes for hash table lookup.
hashword(), hashlittle(), hashlittle2(), hashbig(), mix(), and final() 
are externally useful functions.  Routines to test the hash are included 
if SELF_TEST is defined.  You can use this free for any purpose.  It's in
the public domain.  It has no warranty.

You probably want to use hashlittle().  hashlittle() and hashbig()
hash byte arrays.  hashlittle() is is faster than hashbig() on
little-endian machines.  Intel and AMD are little-endian machines.
On second thought, you probably want hashlittle2(), which is identical to
hashlittle() except it returns two 32-bit hashes for the price of one.  
You could implement hashbig2() if you wanted but I haven't bothered here.

If you want to find a hash of, say, exactly 7 integers, do
  a = i1;  b = i2;  c = i3;
  mix(a,b,c);
  a += i4; b += i5; c += i6;
  mix(a,b,c);
  a += i7;
  final(a,b,c);
then use c as the hash value.  If you have a variable length array of
4-byte integers to hash, use hashword().  If you have a byte array (like
a character string), use hashlittle().  If you have several byte arrays, or
a mix of things, see the comments above hashlittle().  

Why is this so big?  I read 12 bytes at a time into 3 4-byte integers, 
then mix those integers.  This is fast (you can do a lot more thorough
mixing with 12*3 instructions on 3 integers than you can with 3 instructions
on 1 byte), but shoehorning those bytes into integers efficiently is messy.
-------------------------------------------------------------------------------
*/

/*
 * My best guess at if you are big-endian or little-endian.  This may
 * need adjustment.
 */
#if (defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && \
     __BYTE_ORDER == __LITTLE_ENDIAN) || \
    (defined(i386) || defined(__i386__) || defined(__i486__) || \
     defined(__i586__) || defined(__i686__) || defined(vax) || defined(MIPSEL))
# define HASH_LITTLE_ENDIAN 1
# define HASH_BIG_ENDIAN 0
#elif (defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && \
       __BYTE_ORDER == __BIG_ENDIAN) || \
      (defined(sparc) || defined(POWERPC) || defined(mc68000) || defined(sel))
# define HASH_LITTLE_ENDIAN 0
# define HASH_BIG_ENDIAN 1
#else
# define HASH_LITTLE_ENDIAN 0
# define HASH_BIG_ENDIAN 0
#endif

#define hashsize(n) ((uint32_t)1<<(n))
#define hashmask(n) (hashsize(n)-1)
#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

/*
-------------------------------------------------------------------------------
mix -- mix 3 32-bit values reversibly.

This is reversible, so any information in (a,b,c) before mix() is
still in (a,b,c) after mix().

If four pairs of (a,b,c) inputs are run through mix(), or through
mix() in reverse, there are at least 32 bits of the output that
are sometimes the same for one pair and different for another pair.
This was tested for:
* pairs that differed by one bit, by two bits, in any combination
  of top bits of (a,b,c), or in any combination of bottom bits of
  (a,b,c).
* "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed
  the output delta to a Gray code (a^(a>>1)) so a string of 1's (as
  is commonly produced by subtraction) look like a single 1-bit
  difference.
* the base values were pseudorandom, all zero but one bit set, or 
  all zero plus a counter that starts at zero.

Some k values for my "a-=c; a^=rot(c,k); c+=b;" arrangement that
satisfy this are
    4  6  8 16 19  4
    9 15  3 18 27 15
   14  9  3  7 17  3
Well, "9 15 3 18 27 15" didn't quite get 32 bits diffing
for "differ" defined as + with a one-bit base and a two-bit delta.  I
used http://burtleburtle.net/bob/hash/avalanche.html to choose 
the operations, constants, and arrangements of the variables.

This does not achieve avalanche.  There are input bits of (a,b,c)
that fail to affect some output bits of (a,b,c), especially of a.  The
most thoroughly mixed value is c, but it doesn't really even achieve
avalanche in c.

This allows some parallelism.  Read-after-writes are good at doubling
the number of bits affected, so the goal of mixing pulls in the opposite
direction as the goal of parallelism.  I did what I could.  Rotates
seem to cost as much as shifts on every machine I could lay my hands
on, and rotates are much kinder to the top and bottom bits, so I used
rotates.
-------------------------------------------------------------------------------
*/
#define mix(a,b,c) \
{ \
  a -= c;  a ^= rot(c, 4);  c += b; \
  b -= a;  b ^= rot(a, 6);  a += c; \
  c -= b;  c ^= rot(b, 8);  b += a; \
  a -= c;  a ^= rot(c,16);  c += b; \
  b -= a;  b ^= rot(a,19);  a += c; \
  c -= b;  c ^= rot(b, 4);  b += a; \
}

/*
-------------------------------------------------------------------------------
final -- final mixing of 3 32-bit values (a,b,c) into c

Pairs of (a,b,c) values differing in only a few bits will usually
produce values of c that look totally different.  This was tested for
* pairs that differed by one bit, by two bits, in any combination
  of top bits of (a,b,c), or in any combination of bottom bits of
  (a,b,c).
* "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed
  the output delta to a Gray code (a^(a>>1)) so a string of 1's (as
  is commonly produced by subtraction) look like a single 1-bit
  difference.
* the base values were pseudorandom, all zero but one bit set, or 
  all zero plus a counter that starts at zero.

These constants passed:
 14 11 25 16 4 14 24
 12 14 25 16 4 14 24
and these came close:
  4  8 15 26 3 22 24
 10  8 15 26 3 22 24
 11  8 15 26 3 22 24
-------------------------------------------------------------------------------
*/
#define final(a,b,c) \
{ \
  c ^= b; c -= rot(b,14); \
  a ^= c; a -= rot(c,11); \
  b ^= a; b -= rot(a,25); \
  c ^= b; c -= rot(b,16); \
  a ^= c; a -= rot(c,4);  \
  b ^= a; b -= rot(a,14); \
  c ^= b; c -= rot(b,24); \
}


/*
-------------------------------------------------------------------------------
hashlittle() -- hash a variable-length key into a 32-bit value
  k       : the key (the unaligned variable-length array of bytes)
  length  : the length of the key, counting by bytes
  initval : can be any 4-byte value
Returns a 32-bit value.  Every bit of the key affects every bit of
the return value.  Two keys differing by one or two bits will have
totally different hash values.

The best hash table sizes are powers of 2.  There is no need to do
mod a prime (mod is sooo slow!).  If you need less than 32 bits,
use a bitmask.  For example, if you need only 10 bits, do
  h = (h & hashmask(10));
In which case, the hash table should have hashsize(10) elements.

If you are hashing n strings (uint8_t **)k, do it like this:
  for (i=0, h=0; i<n; ++i) h = hashlittle( k[i], len[i], h);

By Bob Jenkins, 2006.  bob_jenkins@burtleburtle.net.  You may use this
code any way you wish, private, educational, or commercial.  It's free.

Use for hash table lookup, or anything where one collision in 2^^32 is
acceptable.  Do NOT use for cryptographic purposes.
-------------------------------------------------------------------------------
*/

static uint32_t hashlittle( const void *key, size_t length, uint32_t initval)
{
  uint32_t a,b,c;                                          /* internal state */
  union { const void *ptr; size_t i; } u;     /* needed for Mac Powerbook G4 */

  /* Set up the internal state */
  a = b = c = 0xdeadbeef + ((uint32_t)length) + initval;

  u.ptr = key;
  if (HASH_LITTLE_ENDIAN && ((u.i & 0x3) == 0)) {
    const uint32_t *k = (const uint32_t *)key;         /* read 32-bit chunks */

    /*------ all but last block: aligned reads and affect 32 bits of (a,b,c) */
    while (length > 12)
    {
      a += k[0];
      b += k[1];
      c += k[2];
      mix(a,b,c);
      length -= 12;
      k += 3;
    }

    /*----------------------------- handle the last (probably partial) block */
    /* 
     * "k[2]&0xffffff" actually reads beyond the end of the string, but
     * then masks off the part it's not allowed to read.  Because the
     * string is aligned, the masked-off tail is in the same word as the
     * rest of the string.  Every machine with memory protection I've seen
     * does it on word boundaries, so is OK with this.  But VALGRIND will
     * still catch it and complain.  The masking trick does make the hash
     * noticably faster for short strings (like English words).
     */
#ifndef VALGRIND

    switch(length)
    {
    case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
    case 11: c+=k[2]&0xffffff; b+=k[1]; a+=k[0]; break;
    case 10: c+=k[2]&0xffff; b+=k[1]; a+=k[0]; break;
    case 9 : c+=k[2]&0xff; b+=k[1]; a+=k[0]; break;
    case 8 : b+=k[1]; a+=k[0]; break;
    case 7 : b+=k[1]&0xffffff; a+=k[0]; break;
    case 6 : b+=k[1]&0xffff; a+=k[0]; break;
    case 5 : b+=k[1]&0xff; a+=k[0]; break;
    case 4 : a+=k[0]; break;
    case 3 : a+=k[0]&0xffffff; break;
    case 2 : a+=k[0]&0xffff; break;
    case 1 : a+=k[0]&0xff; break;
    case 0 : return c;              /* zero length strings require no mixing */
    }

#else /* make valgrind happy */

    const uint8_t  *k8 = (const uint8_t *)k;
    switch(length)
    {
    case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
    case 11: c+=((uint32_t)k8[10])<<16;  /* fall through */
    case 10: c+=((uint32_t)k8[9])<<8;    /* fall through */
    case 9 : c+=k8[8];                   /* fall through */
    case 8 : b+=k[1]; a+=k[0]; break;
    case 7 : b+=((uint32_t)k8[6])<<16;   /* fall through */
    case 6 : b+=((uint32_t)k8[5])<<8;    /* fall through */
    case 5 : b+=k8[4];                   /* fall through */
    case 4 : a+=k[0]; break;
    case 3 : a+=((uint32_t)k8[2])<<16;   /* fall through */
    case 2 : a+=((uint32_t)k8[1])<<8;    /* fall through */
    case 1 : a+=k8[0]; break;
    case 0 : return c;
    }

#endif /* !valgrind */

  } else if (HASH_LITTLE_ENDIAN && ((u.i & 0x1) == 0)) {
    const uint16_t *k = (const uint16_t *)key;         /* read 16-bit chunks */
    const uint8_t  *k8;

    /*--------------- all but last block: aligned reads and different mixing */
    while (length > 12)
    {
      a += k[0] + (((uint32_t)k[1])<<16);
      b += k[2] + (((uint32_t)k[3])<<16);
      c += k[4] + (((uint32_t)k[5])<<16);
      mix(a,b,c);
      length -= 12;
      k += 6;
    }

    /*----------------------------- handle the last (probably partial) block */
    k8 = (const uint8_t *)k;
    switch(length)
    {
    case 12: c+=k[4]+(((uint32_t)k[5])<<16);
             b+=k[2]+(((uint32_t)k[3])<<16);
             a+=k[0]+(((uint32_t)k[1])<<16);
             break;
    case 11: c+=((uint32_t)k8[10])<<16;     /* fall through */
    case 10: c+=k[4];
             b+=k[2]+(((uint32_t)k[3])<<16);
             a+=k[0]+(((uint32_t)k[1])<<16);
             break;
    case 9 : c+=k8[8];                      /* fall through */
    case 8 : b+=k[2]+(((uint32_t)k[3])<<16);
             a+=k[0]+(((uint32_t)k[1])<<16);
             break;
    case 7 : b+=((uint32_t)k8[6])<<16;      /* fall through */
    case 6 : b+=k[2];
             a+=k[0]+(((uint32_t)k[1])<<16);
             break;
    case 5 : b+=k8[4];                      /* fall through */
    case 4 : a+=k[0]+(((uint32_t)k[1])<<16);
             break;
    case 3 : a+=((uint32_t)k8[2])<<16;      /* fall through */
    case 2 : a+=k[0];
             break;
    case 1 : a+=k8[0];
             break;
    case 0 : return c;                     /* zero length requires no mixing */
    }

  } else {                        /* need to read the key one byte at a time */
    const uint8_t *k = (const uint8_t *)key;

    /*--------------- all but the last block: affect some 32 bits of (a,b,c) */
    while (length > 12)
    {
      a += k[0];
      a += ((uint32_t)k[1])<<8;
      a += ((uint32_t)k[2])<<16;
      a += ((uint32_t)k[3])<<24;
      b += k[4];
      b += ((uint32_t)k[5])<<8;
      b += ((uint32_t)k[6])<<16;
      b += ((uint32_t)k[7])<<24;
      c += k[8];
      c += ((uint32_t)k[9])<<8;
      c += ((uint32_t)k[10])<<16;
      c += ((uint32_t)k[11])<<24;
      mix(a,b,c);
      length -= 12;
      k += 12;
    }

    /*-------------------------------- last block: affect all 32 bits of (c) */
    switch(length)                   /* all the case statements fall through */
    {
    case 12: c+=((uint32_t)k[11])<<24;
    case 11: c+=((uint32_t)k[10])<<16;
    case 10: c+=((uint32_t)k[9])<<8;
    case 9 : c+=k[8];
    case 8 : b+=((uint32_t)k[7])<<24;
    case 7 : b+=((uint32_t)k[6])<<16;
    case 6 : b+=((uint32_t)k[5])<<8;
    case 5 : b+=k[4];
    case 4 : a+=((uint32_t)k[3])<<24;
    case 3 : a+=((uint32_t)k[2])<<16;
    case 2 : a+=((uint32_t)k[1])<<8;
    case 1 : a+=k[0];
             break;
    case 0 : return c;
    }
  }

  final(a,b,c);
  return c;
}

unsigned long lh_char_hash(const void *k)
{
	static volatile int random_seed = -1;

	if (random_seed == -1) {
		int seed;
		/* we can't use -1 as it is the unitialized sentinel */
		while ((seed = json_c_get_random_seed()) == -1);
#if defined __GNUC__
		__sync_val_compare_and_swap(&random_seed, -1, seed);
#elif defined _MSC_VER
		InterlockedCompareExchange(&random_seed, seed, -1);
#else
#warning "racy random seed initializtion if used by multiple threads"
		random_seed = seed; /* potentially racy */
#endif
	}

	return hashlittle((const char*)k, strlen((const char*)k), random_seed); 
}

int lh_char_equal(const void *k1, const void *k2)
{
	return (strcmp((const char*)k1, (const char*)k2) == 0);
}

struct lh_table* lh_table_new(int size, const char *name,
			      lh_entry_free_fn *free_fn,
			      lh_hash_fn *hash_fn,
			      lh_equal_fn *equal_fn)
{
	int i;
	struct lh_table *t;

	t = (struct lh_table*)calloc(1, sizeof(struct lh_table));
	if(!t) lh_abort("lh_table_new: calloc failed\n");
	t->count = 0;
	t->size = size;
	t->name = name;
	t->table = (struct lh_entry*)calloc(size, sizeof(struct lh_entry));
	if(!t->table) lh_abort("lh_table_new: calloc failed\n");
	t->free_fn = free_fn;
	t->hash_fn = hash_fn;
	t->equal_fn = equal_fn;
	for(i = 0; i < size; i++) t->table[i].k = LH_EMPTY;
	return t;
}

struct lh_table* lh_kchar_table_new(int size, const char *name,
				    lh_entry_free_fn *free_fn)
{
	return lh_table_new(size, name, free_fn, lh_char_hash, lh_char_equal);
}

struct lh_table* lh_kptr_table_new(int size, const char *name,
				   lh_entry_free_fn *free_fn)
{
	return lh_table_new(size, name, free_fn, lh_ptr_hash, lh_ptr_equal);
}

void lh_table_resize(struct lh_table *t, int new_size)
{
	struct lh_table *new_t;
	struct lh_entry *ent;

	new_t = lh_table_new(new_size, t->name, NULL, t->hash_fn, t->equal_fn);
	ent = t->head;
	while(ent) {
		lh_table_insert(new_t, ent->k, ent->v);
		ent = ent->next;
	}
	free(t->table);
	t->table = new_t->table;
	t->size = new_size;
	t->head = new_t->head;
	t->tail = new_t->tail;
	t->resizes++;
	free(new_t);
}

void lh_table_free(struct lh_table *t)
{
	struct lh_entry *c;
	for(c = t->head; c != NULL; c = c->next) {
		if(t->free_fn) {
			t->free_fn(c);
		}
	}
	free(t->table);
	free(t);
}


int lh_table_insert(struct lh_table *t, void *k, const void *v)
{
	unsigned long h, n;

	t->inserts++;
	if(t->count >= t->size * LH_LOAD_FACTOR) lh_table_resize(t, t->size * 2);

	h = t->hash_fn(k);
	n = h % t->size;

	while( 1 ) {
		if(t->table[n].k == LH_EMPTY || t->table[n].k == LH_FREED) break;
		t->collisions++;
		if ((int)++n == t->size) n = 0;
	}

	t->table[n].k = k;
	t->table[n].v = v;
	t->count++;

	if(t->head == NULL) {
		t->head = t->tail = &t->table[n];
		t->table[n].next = t->table[n].prev = NULL;
	} else {
		t->tail->next = &t->table[n];
		t->table[n].prev = t->tail;
		t->table[n].next = NULL;
		t->tail = &t->table[n];
	}

	return 0;
}


struct lh_entry* lh_table_lookup_entry(struct lh_table *t, const void *k)
{
	unsigned long h = t->hash_fn(k);
	unsigned long n = h % t->size;
	int count = 0;

	t->lookups++;
	while( count < t->size ) {
		if(t->table[n].k == LH_EMPTY) return NULL;
		if(t->table[n].k != LH_FREED &&
		   t->equal_fn(t->table[n].k, k)) return &t->table[n];
		if ((int)++n == t->size) n = 0;
		count++;
	}
	return NULL;
}


const void* lh_table_lookup(struct lh_table *t, const void *k)
{
	void *result;
	lh_table_lookup_ex(t, k, &result);
	return result;
}

json_bool lh_table_lookup_ex(struct lh_table* t, const void* k, void **v)
{
	struct lh_entry *e = lh_table_lookup_entry(t, k);
	if (e != NULL) {
		if (v != NULL) *v = (void *)e->v;
		return TRUE; /* key found */
	}
	if (v != NULL) *v = NULL;
	return FALSE; /* key not found */
}

int lh_table_delete_entry(struct lh_table *t, struct lh_entry *e)
{
	ptrdiff_t n = (ptrdiff_t)(e - t->table); /* CAW: fixed to be 64bit nice, still need the crazy negative case... */

	/* CAW: this is bad, really bad, maybe stack goes other direction on this machine... */
	if(n < 0) { return -2; }

	if(t->table[n].k == LH_EMPTY || t->table[n].k == LH_FREED) return -1;
	t->count--;
	if(t->free_fn) t->free_fn(e);
	t->table[n].v = NULL;
	t->table[n].k = LH_FREED;
	if(t->tail == &t->table[n] && t->head == &t->table[n]) {
		t->head = t->tail = NULL;
	} else if (t->head == &t->table[n]) {
		t->head->next->prev = NULL;
		t->head = t->head->next;
	} else if (t->tail == &t->table[n]) {
		t->tail->prev->next = NULL;
		t->tail = t->tail->prev;
	} else {
		t->table[n].prev->next = t->table[n].next;
		t->table[n].next->prev = t->table[n].prev;
	}
	t->table[n].next = t->table[n].prev = NULL;
	return 0;
}


int lh_table_delete(struct lh_table *t, const void *k)
{
	struct lh_entry *e = lh_table_lookup_entry(t, k);
	if(!e) return -1;
	return lh_table_delete_entry(t, e);
}

int lh_table_length(struct lh_table *t)
{
	return t->count;
}
//-------printbuf.c-------

/*
 * $Id: printbuf.c,v 1.5 2006/01/26 02:16:28 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 *
 * Copyright (c) 2008-2009 Yahoo! Inc.  All rights reserved.
 * The copyrights to the contents of this file are licensed under the MIT License
 * (http://www.opensource.org/licenses/mit-license.php)
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_STDARG_H
# include <stdarg.h>
#else /* !HAVE_STDARG_H */
# error Not enough var arg support!
#endif /* HAVE_STDARG_H */




static int printbuf_extend(struct printbuf *p, int min_size);

struct printbuf* printbuf_new(void)
{
  struct printbuf *p;

  p = (struct printbuf*)calloc(1, sizeof(struct printbuf));
  if(!p) return NULL;
  p->size = 32;
  p->bpos = 0;
  if(!(p->buf = (char*)malloc(p->size))) {
    free(p);
    return NULL;
  }
  return p;
}


/**
 * Extend the buffer p so it has a size of at least min_size.
 *
 * If the current size is large enough, nothing is changed.
 *
 * Note: this does not check the available space!  The caller
 *  is responsible for performing those calculations.
 */
static int printbuf_extend(struct printbuf *p, int min_size)
{
	char *t;
	int new_size;

	if (p->size >= min_size)
		return 0;

	new_size = p->size * 2;
	if (new_size < min_size + 8)
		new_size =  min_size + 8;
#ifdef PRINTBUF_DEBUG
	MC_DEBUG("printbuf_memappend: realloc "
	  "bpos=%d min_size=%d old_size=%d new_size=%d\n",
	  p->bpos, min_size, p->size, new_size);
#endif /* PRINTBUF_DEBUG */
	if(!(t = (char*)realloc(p->buf, new_size)))
		return -1;
	p->size = new_size;
	p->buf = t;
	return 0;
}

int printbuf_memappend(struct printbuf *p, const char *buf, int size)
{
  if (p->size <= p->bpos + size + 1) {
    if (printbuf_extend(p, p->bpos + size + 1) < 0)
      return -1;
  }
  memcpy(p->buf + p->bpos, buf, size);
  p->bpos += size;
  p->buf[p->bpos]= '\0';
  return size;
}

int printbuf_memset(struct printbuf *pb, int offset, int charvalue, int len)
{
	int size_needed;

	if (offset == -1)
		offset = pb->bpos;
	size_needed = offset + len;
	if (pb->size < size_needed)
	{
		if (printbuf_extend(pb, size_needed) < 0)
			return -1;
	}

	memset(pb->buf + offset, charvalue, len);
	if (pb->bpos < size_needed)
		pb->bpos = size_needed;

	return 0;
}

#if !defined(HAVE_VSNPRINTF) && defined(_MSC_VER)
# define vsnprintf _vsnprintf
#elif !defined(HAVE_VSNPRINTF) /* !HAVE_VSNPRINTF */
# error Need vsnprintf!
#endif /* !HAVE_VSNPRINTF && defined(WIN32) */

#if !defined(HAVE_VASPRINTF)
/* CAW: compliant version of vasprintf */
static int vasprintf(char **buf, const char *fmt, va_list ap)
{
#ifndef WIN32
	static char _T_emptybuffer = '\0';
#endif /* !defined(WIN32) */
	int chars;
	char *b;

	if(!buf) { return -1; }

#ifdef WIN32
	chars = _vscprintf(fmt, ap)+1;
#else /* !defined(WIN32) */
	/* CAW: RAWR! We have to hope to god here that vsnprintf doesn't overwrite
	   our buffer like on some 64bit sun systems.... but hey, its time to move on */
	chars = vsnprintf(&_T_emptybuffer, 0, fmt, ap)+1;
	if(chars < 0) { chars *= -1; } /* CAW: old glibc versions have this problem */
#endif /* defined(WIN32) */

	b = (char*)malloc(sizeof(char)*chars);
	if(!b) { return -1; }

	if((chars = vsprintf(b, fmt, ap)) < 0)
	{
		free(b);
	} else {
		*buf = b;
	}

	return chars;
}
#endif /* !HAVE_VASPRINTF */

int sprintbuf(struct printbuf *p, const char *msg, ...)
{
  va_list ap;
  char *t;
  int size;
  char buf[128];

  /* user stack buffer first */
  va_start(ap, msg);
  size = vsnprintf(buf, 128, msg, ap);
  va_end(ap);
  /* if string is greater than stack buffer, then use dynamic string
     with vasprintf.  Note: some implementation of vsnprintf return -1
     if output is truncated whereas some return the number of bytes that
     would have been written - this code handles both cases. */
  if(size == -1 || size > 127) {
    va_start(ap, msg);
    if((size = vasprintf(&t, msg, ap)) < 0) { va_end(ap); return -1; }
    va_end(ap);
    printbuf_memappend(p, t, size);
    free(t);
    return size;
  } else {
    printbuf_memappend(p, buf, size);
    return size;
  }
}

void printbuf_reset(struct printbuf *p)
{
  p->buf[0] = '\0';
  p->bpos = 0;
}

void printbuf_free(struct printbuf *p)
{
  if(p) {
    free(p->buf);
    free(p);
  }
}
//-------debug.c-------

/*
 * $Id: debug.c,v 1.5 2006/01/26 02:16:28 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#if HAVE_SYSLOG_H
# include <syslog.h>
#endif /* HAVE_SYSLOG_H */

#if HAVE_UNISTD_H
# include <unistd.h>
#endif /* HAVE_UNISTD_H */

#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif /* HAVE_SYS_PARAM_H */



static int _syslog = 0;
static int _debug = 0;

void mc_set_debug(int debug) { _debug = debug; }
int mc_get_debug(void) { return _debug; }

extern void mc_set_syslog(int syslog)
{
  _syslog = syslog;
}

void mc_debug(const char *msg, ...)
{
  va_list ap;
  if(_debug) {
    va_start(ap, msg);
#if HAVE_VSYSLOG
    if(_syslog) {
		vsyslog(LOG_DEBUG, msg, ap);
	} else
#endif
		vprintf(msg, ap);
    va_end(ap);
  }
}

void mc_error(const char *msg, ...)
{
  va_list ap;
  va_start(ap, msg);
#if HAVE_VSYSLOG
    if(_syslog) {
		vsyslog(LOG_ERR, msg, ap);
	} else
#endif
		vfprintf(stderr, msg, ap);
  va_end(ap);
}

void mc_info(const char *msg, ...)
{
  va_list ap;
  va_start(ap, msg);
#if HAVE_VSYSLOG
    if(_syslog) {
		vsyslog(LOG_INFO, msg, ap);
	} else 
#endif
		vfprintf(stderr, msg, ap);
  va_end(ap);
}
//-------random_seed.c-------

/*
 * random_seed.c
 *
 * Copyright (c) 2013 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

#include <stdio.h>


#define DEBUG_SEED(s)


#if defined ENABLE_RDRAND

/* cpuid */

#if defined __GNUC__ && (defined __i386__ || defined __x86_64__)
#define HAS_X86_CPUID 1

static void do_cpuid(int regs[], int h)
{
    __asm__ __volatile__(
#if defined __x86_64__
                         "pushq %%rbx;\n"
#else
                         "pushl %%ebx;\n"
#endif
                         "cpuid;\n"
#if defined __x86_64__
                         "popq %%rbx;\n"
#else
                         "popl %%ebx;\n"
#endif
                         : "=a"(regs[0]), [ebx] "=r"(regs[1]), "=c"(regs[2]), "=d"(regs[3])
                         : "a"(h));
}

#elif defined _MSC_VER

#define HAS_X86_CPUID 1
#define do_cpuid __cpuid

#endif

/* has_rdrand */

#if HAS_X86_CPUID

static int has_rdrand()
{
    // CPUID.01H:ECX.RDRAND[bit 30] == 1
    int regs[4];
    do_cpuid(regs, 1);
    return (regs[2] & (1 << 30)) != 0;
}

#endif

/* get_rdrand_seed - GCC x86 and X64 */

#if defined __GNUC__ && (defined __i386__ || defined __x86_64__)

#define HAVE_RDRAND 1

static int get_rdrand_seed()
{
    DEBUG_SEED("get_rdrand_seed");
    int _eax;
    // rdrand eax
    __asm__ __volatile__("1: .byte 0x0F\n"
                         "   .byte 0xC7\n"
                         "   .byte 0xF0\n"
                         "   jnc 1b;\n"
                         : "=a" (_eax));
    return _eax;
}

#endif

#if defined _MSC_VER

#if _MSC_VER >= 1700
#define HAVE_RDRAND 1

/* get_rdrand_seed - Visual Studio 2012 and above */

static int get_rdrand_seed()
{
    DEBUG_SEED("get_rdrand_seed");
    int r;
    while (_rdrand32_step(&r) == 0);
    return r;
}

#elif defined _M_IX86
#define HAVE_RDRAND 1

/* get_rdrand_seed - Visual Studio 2010 and below - x86 only */

static int get_rdrand_seed()
{
	DEBUG_SEED("get_rdrand_seed");
	int _eax;
retry:
	// rdrand eax
	__asm _emit 0x0F __asm _emit 0xC7 __asm _emit 0xF0
	__asm jnc retry
	__asm mov _eax, eax
	return _eax;
}

#endif
#endif

#endif /* defined ENABLE_RDRAND */


/* has_dev_urandom */

#if defined (__APPLE__) || defined(__unix__) || defined(__linux__)

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>

#define HAVE_DEV_RANDOM 1

static const char *dev_random_file = "/dev/urandom";

static int has_dev_urandom()
{
    struct stat buf;
    if (stat(dev_random_file, &buf)) {
        return 0;
    }
    return ((buf.st_mode & S_IFCHR) != 0);
}


/* get_dev_random_seed */

static int get_dev_random_seed()
{
    DEBUG_SEED("get_dev_random_seed");
    
    int fd = open(dev_random_file, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "error opening %s: %s", dev_random_file, strerror(errno));
        exit(1);
    }
    
    int r;
    ssize_t nread = read(fd, &r, sizeof(r));
    if (nread != sizeof(r)) {
        fprintf(stderr, "error read %s: %s", dev_random_file, strerror(errno));
        exit(1);
    }
    else if (nread != sizeof(r)) {
        fprintf(stderr, "error short read %s", dev_random_file);
        exit(1);
    }
    close(fd);
    return r;
}

#endif


/* get_cryptgenrandom_seed */

#ifdef WIN32

#define HAVE_CRYPTGENRANDOM 1

#include <windows.h>
#pragma comment(lib, "advapi32.lib")

static int get_cryptgenrandom_seed()
{
    DEBUG_SEED("get_cryptgenrandom_seed");
    
    HCRYPTPROV hProvider = 0;
    int r;
    
    if (!CryptAcquireContextW(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT)) {
        fprintf(stderr, "error CryptAcquireContextW");
        exit(1);
    }
    
    if (!CryptGenRandom(hProvider, sizeof(r), (BYTE*)&r)) {
        fprintf(stderr, "error CryptGenRandom");
        exit(1);
    }
    
    CryptReleaseContext(hProvider, 0);
    
    return r;
}

#endif


/* get_time_seed */

#include <time.h>

static int get_time_seed()
{
    DEBUG_SEED("get_time_seed");
    
    return (int)time(NULL) * 433494437;
}


/* json_c_get_random_seed */

int json_c_get_random_seed()
{
#if HAVE_RDRAND
    if (has_rdrand()) return get_rdrand_seed();
#endif
#if HAVE_DEV_RANDOM
    if (has_dev_urandom()) return get_dev_random_seed();
#endif
#if HAVE_CRYPTGENRANDOM
    return get_cryptgenrandom_seed();
#endif
    return get_time_seed();
}
//-------libjson.c-------


/* dummy source file for compatibility purposes */

#if defined(HAVE_CDEFS_H)
#include <sys/cdefs.h>
#endif

#ifndef __warn_references

#if defined(__GNUC__)  && defined (HAS_GNU_WARNING_LONG)

#define __warn_references(sym,msg)                  \
  __asm__(".section .gnu" #sym ",\n\t.ascii \"" msg "\"\n\t.text");

#else
#define __warn_references(sym,msg)    /* nothing */
#endif

#endif 



__warn_references(json_object_get, "Warning: please link against libjson-c instead of libjson");

/*        __asm__(".section .gnu.warning." __STRING(sym)  \
            " ; .ascii \"" msg "\" ; .text") */
//-------json_c_version.c-------

/*
 * Copyright (c) 2012 Eric Haszlakiewicz
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 */




const char *json_c_version(void)
{
	return JSON_C_VERSION;
}

int json_c_version_num(void)
{
	return JSON_C_VERSION_NUM;
}

//-------json_util.c-------

/*
 * $Id: json_util.c,v 1.4 2006/01/30 23:07:57 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */


#undef realloc

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif /* HAVE_SYS_STAT_H */

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif /* HAVE_FCNTL_H */

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif /* HAVE_UNISTD_H */

#ifdef WIN32
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <io.h>
#endif /* defined(WIN32) */

#if !defined(HAVE_OPEN) && defined(WIN32)
# define open _open
#endif

#if !defined(HAVE_SNPRINTF) && defined(_MSC_VER)
  /* MSC has the version as _snprintf */
# define snprintf _snprintf
#elif !defined(HAVE_SNPRINTF)
# error You do not have snprintf on your system.
#endif /* HAVE_SNPRINTF */








static int sscanf_is_broken = 0;
static int sscanf_is_broken_testdone = 0;
static void sscanf_is_broken_test(void);

struct json_object* json_object_from_file(const char *filename)
{
  struct printbuf *pb;
  struct json_object *obj;
  char buf[JSON_FILE_BUF_SIZE];
  int fd, ret;

  if((fd = open(filename, O_RDONLY)) < 0) {
    MC_ERROR("json_object_from_file: error opening file %s: %s\n",
	     filename, strerror(errno));
    return NULL;
  }
  if(!(pb = printbuf_new())) {
    close(fd);
    MC_ERROR("json_object_from_file: printbuf_new failed\n");
    return NULL;
  }
  while((ret = read(fd, buf, JSON_FILE_BUF_SIZE)) > 0) {
    printbuf_memappend(pb, buf, ret);
  }
  close(fd);
  if(ret < 0) {
    MC_ERROR("json_object_from_file: error reading file %s: %s\n",
	     filename, strerror(errno));
    printbuf_free(pb);
    return NULL;
  }
  obj = json_tokener_parse(pb->buf);
  printbuf_free(pb);
  return obj;
}

/* extended "format and write to file" function */

int json_object_to_file_ext(const char *filename, struct json_object *obj, int flags)
{
  const char *json_str;
  int fd, ret;
  unsigned int wpos, wsize;

  if(!obj) {
    MC_ERROR("json_object_to_file: object is null\n");
    return -1;
  }

  if((fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT, 0644)) < 0) {
    MC_ERROR("json_object_to_file: error opening file %s: %s\n",
	     filename, strerror(errno));
    return -1;
  }

  if(!(json_str = json_object_to_json_string_ext(obj,flags))) {
    close(fd);
    return -1;
  }

  wsize = (unsigned int)(strlen(json_str) & UINT_MAX); /* CAW: probably unnecessary, but the most 64bit safe */
  wpos = 0;
  while(wpos < wsize) {
    if((ret = write(fd, json_str + wpos, wsize-wpos)) < 0) {
      close(fd);
      MC_ERROR("json_object_to_file: error writing file %s: %s\n",
	     filename, strerror(errno));
      return -1;
    }

	/* because of the above check for ret < 0, we can safely cast and add */
    wpos += (unsigned int)ret;
  }

  close(fd);
  return 0;
}

// backwards compatible "format and write to file" function

int json_object_to_file(const char *filename, struct json_object *obj)
{
  return json_object_to_file_ext(filename, obj, JSON_C_TO_STRING_PLAIN);
}

int json_parse_double(const char *buf, double *retval)
{
  return (sscanf(buf, "%lf", retval)==1 ? 0 : 1);
}

/*
 * Not all implementations of sscanf actually work properly.
 * Check whether the one we're currently using does, and if
 * it's broken, enable the workaround code.
 */
static void sscanf_is_broken_test()
{
	int64_t num64;
	int ret_errno, is_int64_min, ret_errno2, is_int64_max;

	(void)sscanf(" -01234567890123456789012345", "%" SCNd64, &num64);
	ret_errno = errno;
	is_int64_min = (num64 == INT64_MIN);

	(void)sscanf(" 01234567890123456789012345", "%" SCNd64, &num64);
	ret_errno2 = errno;
	is_int64_max = (num64 == INT64_MAX);

	if (ret_errno != ERANGE || !is_int64_min ||
	    ret_errno2 != ERANGE || !is_int64_max)
	{
		MC_DEBUG("sscanf_is_broken_test failed, enabling workaround code\n");
		sscanf_is_broken = 1;
	}
}

int json_parse_int64(const char *buf, int64_t *retval)
{
	int64_t num64;
	const char *buf_sig_digits;
	int orig_has_neg;
	int saved_errno;

	if (!sscanf_is_broken_testdone)
	{
		sscanf_is_broken_test();
		sscanf_is_broken_testdone = 1;
	}

	// Skip leading spaces
	while (isspace((int)*buf) && *buf)
		buf++;

	errno = 0; // sscanf won't always set errno, so initialize
	if (sscanf(buf, "%" SCNd64, &num64) != 1)
	{
		MC_DEBUG("Failed to parse, sscanf != 1\n");
		return 1;
	}

	saved_errno = errno;
	buf_sig_digits = buf;
	orig_has_neg = 0;
	if (*buf_sig_digits == '-')
	{
		buf_sig_digits++;
		orig_has_neg = 1;
	}

	// Not all sscanf implementations actually work
	if (sscanf_is_broken && saved_errno != ERANGE)
	{
		char buf_cmp[100];
		char *buf_cmp_start = buf_cmp;
		int recheck_has_neg = 0;
		int buf_cmp_len;

		// Skip leading zeros, but keep at least one digit
		while (buf_sig_digits[0] == '0' && buf_sig_digits[1] != '\0')
			buf_sig_digits++;
		if (num64 == 0) // assume all sscanf impl's will parse -0 to 0
			orig_has_neg = 0; // "-0" is the same as just plain "0"

		snprintf(buf_cmp_start, sizeof(buf_cmp), "%" PRId64, num64);
		if (*buf_cmp_start == '-')
		{
			recheck_has_neg = 1;
			buf_cmp_start++;
		}
		// No need to skip leading spaces or zeros here.

		buf_cmp_len = strlen(buf_cmp_start);
		/**
		 * If the sign is different, or
		 * some of the digits are different, or
		 * there is another digit present in the original string
		 * then we have NOT successfully parsed the value.
		 */
		if (orig_has_neg != recheck_has_neg ||
		    strncmp(buf_sig_digits, buf_cmp_start, strlen(buf_cmp_start)) != 0 ||
			((int)strlen(buf_sig_digits) != buf_cmp_len &&
			 isdigit((int)buf_sig_digits[buf_cmp_len])
		    )
		   )
		{
			saved_errno = ERANGE;
		}
	}

	// Not all sscanf impl's set the value properly when out of range.
	// Always do this, even for properly functioning implementations,
	// since it shouldn't slow things down much.
	if (saved_errno == ERANGE)
	{
		if (orig_has_neg)
			num64 = INT64_MIN;
		else
			num64 = INT64_MAX;
	}
	*retval = num64;
	return 0;
}

#ifndef HAVE_REALLOC
void* rpl_realloc(void* p, size_t n)
{
	if (n == 0)
		n = 1;
	if (p == 0)
		return malloc(n);
	return realloc(p, n);
}
#endif

#define NELEM(a)        (sizeof(a) / sizeof(a[0]))
static const char* json_type_name[] = {
  /* If you change this, be sure to update the enum json_type definition too */
  "null",
  "boolean",
  "double",
  "int",
  "object",
  "array",
  "string",
};

const char *json_type_to_name(enum json_type o_type)
{
	int o_type_int = (int)o_type;
	if (o_type_int < 0 || o_type_int >= (int)NELEM(json_type_name))
	{
		MC_ERROR("json_type_to_name: type %d is out of range [0,%d]\n", o_type, NELEM(json_type_name));
		return NULL;
	}
	return json_type_name[o_type];
}

//-------json_tokener.c-------

/*
 * $Id: json_tokener.c,v 1.20 2006/07/25 03:24:50 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 *
 * Copyright (c) 2008-2009 Yahoo! Inc.  All rights reserved.
 * The copyrights to the contents of this file are licensed under the MIT License
 * (http://www.opensource.org/licenses/mit-license.php)
 */



#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>









#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif /* HAVE_LOCALE_H */

#define jt_hexdigit(x) (((x) <= '9') ? (x) - '0' : ((x) & 7) + 9)

#if !HAVE_STRDUP && defined(_MSC_VER)
  /* MSC has the version as _strdup */
# define strdup _strdup
#elif !HAVE_STRDUP
# error You do not have strdup on your system.
#endif /* HAVE_STRDUP */

#if !HAVE_STRNCASECMP && defined(_MSC_VER)
  /* MSC has the version as _strnicmp */
# define strncasecmp _strnicmp
#elif !HAVE_STRNCASECMP
# error You do not have strncasecmp on your system.
#endif /* HAVE_STRNCASECMP */

/* Use C99 NAN by default; if not available, nan("") should work too. */
#ifndef NAN
#define NAN nan("")
#endif /* !NAN */

static const char json_null_str[] = "null";
static const int json_null_str_len = sizeof(json_null_str) - 1;
static const char json_inf_str[] = "Infinity";
static const int json_inf_str_len = sizeof(json_inf_str) - 1;
static const char json_nan_str[] = "NaN";
static const int json_nan_str_len = sizeof(json_nan_str) - 1;
static const char json_true_str[] = "true";
static const int json_true_str_len = sizeof(json_true_str) - 1;
static const char json_false_str[] = "false";
static const int json_false_str_len = sizeof(json_false_str) - 1;

static const char* json_tokener_errors[] = {
  "success",
  "continue",
  "nesting too deep",
  "unexpected end of data",
  "unexpected character",
  "null expected",
  "boolean expected",
  "number expected",
  "array value separator ',' expected",
  "quoted object property name expected",
  "object property name separator ':' expected",
  "object value separator ',' expected",
  "invalid string sequence",
  "expected comment",
  "buffer size overflow"
};

const char *json_tokener_error_desc(enum json_tokener_error jerr)
{
	int jerr_int = (int)jerr;
	if (jerr_int < 0 || jerr_int >= (int)(sizeof(json_tokener_errors) / sizeof(json_tokener_errors[0])))
		return "Unknown error, invalid json_tokener_error value passed to json_tokener_error_desc()";
	return json_tokener_errors[jerr];
}

enum json_tokener_error json_tokener_get_error(json_tokener *tok)
{
	return tok->err;
}

/* Stuff for decoding unicode sequences */
#define IS_HIGH_SURROGATE(uc) (((uc) & 0xFC00) == 0xD800)
#define IS_LOW_SURROGATE(uc)  (((uc) & 0xFC00) == 0xDC00)
#define DECODE_SURROGATE_PAIR(hi,lo) ((((hi) & 0x3FF) << 10) + ((lo) & 0x3FF) + 0x10000)
static unsigned char utf8_replacement_char[3] = { 0xEF, 0xBF, 0xBD };

struct json_tokener* json_tokener_new_ex(int depth)
{
  struct json_tokener *tok;

  tok = (struct json_tokener*)calloc(1, sizeof(struct json_tokener));
  if (!tok) return NULL;
  tok->stack = (struct json_tokener_srec *)calloc(depth, sizeof(struct json_tokener_srec));
  if (!tok->stack) {
    free(tok);
    return NULL;
  }
  tok->pb = printbuf_new();
  tok->max_depth = depth;
  json_tokener_reset(tok);
  return tok;
}

struct json_tokener* json_tokener_new(void)
{
  return json_tokener_new_ex(JSON_TOKENER_DEFAULT_DEPTH);
}

void json_tokener_free(struct json_tokener *tok)
{
  json_tokener_reset(tok);
  if (tok->pb) printbuf_free(tok->pb);
  if (tok->stack) free(tok->stack);
  free(tok);
}

static void json_tokener_reset_level(struct json_tokener *tok, int depth)
{
  tok->stack[depth].state = json_tokener_state_eatws;
  tok->stack[depth].saved_state = json_tokener_state_start;
  json_object_put(tok->stack[depth].current);
  tok->stack[depth].current = NULL;
  free(tok->stack[depth].obj_field_name);
  tok->stack[depth].obj_field_name = NULL;
}

void json_tokener_reset(struct json_tokener *tok)
{
  int i;
  if (!tok)
    return;

  for(i = tok->depth; i >= 0; i--)
    json_tokener_reset_level(tok, i);
  tok->depth = 0;
  tok->err = json_tokener_success;
}

struct json_object* json_tokener_parse(const char *str)
{
    enum json_tokener_error jerr_ignored;
    struct json_object* obj;
    obj = json_tokener_parse_verbose(str, &jerr_ignored);
    return obj;
}

struct json_object* json_tokener_parse_verbose(const char *str, enum json_tokener_error *error)
{
    struct json_tokener* tok;
    struct json_object* obj;

    tok = json_tokener_new();
    if (!tok)
      return NULL;
    obj = json_tokener_parse_ex(tok, str, -1);
    *error = tok->err;
    if(tok->err != json_tokener_success) {
		if (obj != NULL)
			json_object_put(obj);
        obj = NULL;
    }

    json_tokener_free(tok);
    return obj;
}

#define state  tok->stack[tok->depth].state
#define saved_state  tok->stack[tok->depth].saved_state
#define current tok->stack[tok->depth].current
#define obj_field_name tok->stack[tok->depth].obj_field_name

/* Optimization:
 * json_tokener_parse_ex() consumed a lot of CPU in its main loop,
 * iterating character-by character.  A large performance boost is
 * achieved by using tighter loops to locally handle units such as
 * comments and strings.  Loops that handle an entire token within
 * their scope also gather entire strings and pass them to
 * printbuf_memappend() in a single call, rather than calling
 * printbuf_memappend() one char at a time.
 *
 * PEEK_CHAR() and ADVANCE_CHAR() macros are used for code that is
 * common to both the main loop and the tighter loops.
 */

/* PEEK_CHAR(dest, tok) macro:
 *   Peeks at the current char and stores it in dest.
 *   Returns 1 on success, sets tok->err and returns 0 if no more chars.
 *   Implicit inputs:  str, len vars
 */
#define PEEK_CHAR(dest, tok)                                                  \
  (((tok)->char_offset == len) ?                                          \
   (((tok)->depth == 0 && state == json_tokener_state_eatws && saved_state == json_tokener_state_finish) ? \
    (((tok)->err = json_tokener_success), 0)                              \
    :                                                                   \
    (((tok)->err = json_tokener_continue), 0)                             \
    ) :                                                                 \
   (((dest) = *str), 1)                                                 \
   )

/* ADVANCE_CHAR() macro:
 *   Incrementes str & tok->char_offset.
 *   For convenience of existing conditionals, returns the old value of c (0 on eof)
 *   Implicit inputs:  c var
 */
#define ADVANCE_CHAR(str, tok) \
  ( ++(str), ((tok)->char_offset)++, c)


/* End optimization macro defs */


struct json_object* json_tokener_parse_ex(struct json_tokener *tok,
					  const char *str, int len)
{
  struct json_object *obj = NULL;
  char c = '\1';
#ifdef HAVE_SETLOCALE
  char *oldlocale=NULL, *tmplocale;

  tmplocale = setlocale(LC_NUMERIC, NULL);
  if (tmplocale) oldlocale = strdup(tmplocale);
  setlocale(LC_NUMERIC, "C");
#endif

  tok->char_offset = 0;
  tok->err = json_tokener_success;

  /* this interface is presently not 64-bit clean due to the int len argument
     and the internal printbuf interface that takes 32-bit int len arguments
     so the function limits the maximum string size to INT32_MAX (2GB).
     If the function is called with len == -1 then strlen is called to check
     the string length is less than INT32_MAX (2GB) */
  if ((len < -1) || (len == -1 && strlen(str) > INT32_MAX)) {
    tok->err = json_tokener_error_size;
    return NULL;
  }

  while (PEEK_CHAR(c, tok)) {

  redo_char:
    switch(state) {

    case json_tokener_state_eatws:
      /* Advance until we change state */
      while (isspace((int)c)) {
	if ((!ADVANCE_CHAR(str, tok)) || (!PEEK_CHAR(c, tok)))
	  goto out;
      }
      if(c == '/' && !(tok->flags & JSON_TOKENER_STRICT)) {
	printbuf_reset(tok->pb);
	printbuf_memappend_fast(tok->pb, &c, 1);
	state = json_tokener_state_comment_start;
      } else {
	state = saved_state;
	goto redo_char;
      }
      break;

    case json_tokener_state_start:
      switch(c) {
      case '{':
	state = json_tokener_state_eatws;
	saved_state = json_tokener_state_object_field_start;
	current = json_object_new_object();
	break;
      case '[':
	state = json_tokener_state_eatws;
	saved_state = json_tokener_state_array;
	current = json_object_new_array();
	break;
      case 'I':
      case 'i':
	state = json_tokener_state_inf;
	printbuf_reset(tok->pb);
	tok->st_pos = 0;
	goto redo_char;
      case 'N':
      case 'n':
	state = json_tokener_state_null; // or NaN
	printbuf_reset(tok->pb);
	tok->st_pos = 0;
	goto redo_char;
      case '\'':
        if (tok->flags & JSON_TOKENER_STRICT) {
            /* in STRICT mode only double-quote are allowed */
            tok->err = json_tokener_error_parse_unexpected;
            goto out;
        }
      case '"':
	state = json_tokener_state_string;
	printbuf_reset(tok->pb);
	tok->quote_char = c;
	break;
      case 'T':
      case 't':
      case 'F':
      case 'f':
	state = json_tokener_state_boolean;
	printbuf_reset(tok->pb);
	tok->st_pos = 0;
	goto redo_char;
#if defined(__GNUC__)
	  case '0' ... '9':
#else
	  case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
#endif
      case '-':
	state = json_tokener_state_number;
	printbuf_reset(tok->pb);
	tok->is_double = 0;
	goto redo_char;
      default:
	tok->err = json_tokener_error_parse_unexpected;
	goto out;
      }
      break;

    case json_tokener_state_finish:
      if(tok->depth == 0) goto out;
      obj = json_object_get(current);
      json_tokener_reset_level(tok, tok->depth);
      tok->depth--;
      goto redo_char;

    case json_tokener_state_inf: /* aka starts with 'i' */
      {
	int size_inf;
	int is_negative = 0;

	printbuf_memappend_fast(tok->pb, &c, 1);
	size_inf = json_min(tok->st_pos+1, json_inf_str_len);
	char *infbuf = tok->pb->buf;
	if (*infbuf == '-')
	{
		infbuf++;
		is_negative = 1;
	}
	if ((!(tok->flags & JSON_TOKENER_STRICT) &&
	          strncasecmp(json_inf_str, infbuf, size_inf) == 0) ||
	         (strncmp(json_inf_str, infbuf, size_inf) == 0)
	        )
	{
		if (tok->st_pos == json_inf_str_len)
		{
			current = json_object_new_double(is_negative ? -INFINITY : INFINITY); 
			saved_state = json_tokener_state_finish;
			state = json_tokener_state_eatws;
			goto redo_char;
		}
	} else {
		tok->err = json_tokener_error_parse_unexpected;
		goto out;
	}
	tok->st_pos++;
      }
      break;
    case json_tokener_state_null: /* aka starts with 'n' */
      {
	int size;
	int size_nan;
	printbuf_memappend_fast(tok->pb, &c, 1);
	size = json_min(tok->st_pos+1, json_null_str_len);
	size_nan = json_min(tok->st_pos+1, json_nan_str_len);
	if((!(tok->flags & JSON_TOKENER_STRICT) &&
	  strncasecmp(json_null_str, tok->pb->buf, size) == 0)
	  || (strncmp(json_null_str, tok->pb->buf, size) == 0)
	  ) {
	  if (tok->st_pos == json_null_str_len) {
	    current = NULL;
	    saved_state = json_tokener_state_finish;
	    state = json_tokener_state_eatws;
	    goto redo_char;
	  }
	}
	else if ((!(tok->flags & JSON_TOKENER_STRICT) &&
	          strncasecmp(json_nan_str, tok->pb->buf, size_nan) == 0) ||
	         (strncmp(json_nan_str, tok->pb->buf, size_nan) == 0)
	        )
	{
		if (tok->st_pos == json_nan_str_len)
		{
			current = json_object_new_double(NAN);
			saved_state = json_tokener_state_finish;
			state = json_tokener_state_eatws;
			goto redo_char;
		}
	} else {
	  tok->err = json_tokener_error_parse_null;
	  goto out;
	}
	tok->st_pos++;
      }
      break;

    case json_tokener_state_comment_start:
      if(c == '*') {
	state = json_tokener_state_comment;
      } else if(c == '/') {
	state = json_tokener_state_comment_eol;
      } else {
	tok->err = json_tokener_error_parse_comment;
	goto out;
      }
      printbuf_memappend_fast(tok->pb, &c, 1);
      break;

    case json_tokener_state_comment:
              {
          /* Advance until we change state */
          const char *case_start = str;
          while(c != '*') {
            if (!ADVANCE_CHAR(str, tok) || !PEEK_CHAR(c, tok)) {
              printbuf_memappend_fast(tok->pb, case_start, str-case_start);
              goto out;
            }
          }
          printbuf_memappend_fast(tok->pb, case_start, 1+str-case_start);
          state = json_tokener_state_comment_end;
        }
            break;

    case json_tokener_state_comment_eol:
      {
	/* Advance until we change state */
	const char *case_start = str;
	while(c != '\n') {
	  if (!ADVANCE_CHAR(str, tok) || !PEEK_CHAR(c, tok)) {
	    printbuf_memappend_fast(tok->pb, case_start, str-case_start);
	    goto out;
	  }
	}
	printbuf_memappend_fast(tok->pb, case_start, str-case_start);
	MC_DEBUG("json_tokener_comment: %s\n", tok->pb->buf);
	state = json_tokener_state_eatws;
      }
      break;

    case json_tokener_state_comment_end:
      printbuf_memappend_fast(tok->pb, &c, 1);
      if(c == '/') {
	MC_DEBUG("json_tokener_comment: %s\n", tok->pb->buf);
	state = json_tokener_state_eatws;
      } else {
	state = json_tokener_state_comment;
      }
      break;

    case json_tokener_state_string:
      {
	/* Advance until we change state */
	const char *case_start = str;
	while(1) {
	  if(c == tok->quote_char) {
	    printbuf_memappend_fast(tok->pb, case_start, str-case_start);
	    current = json_object_new_string_len(tok->pb->buf, tok->pb->bpos);
	    saved_state = json_tokener_state_finish;
	    state = json_tokener_state_eatws;
	    break;
	  } else if(c == '\\') {
	    printbuf_memappend_fast(tok->pb, case_start, str-case_start);
	    saved_state = json_tokener_state_string;
	    state = json_tokener_state_string_escape;
	    break;
	  }
	  if (!ADVANCE_CHAR(str, tok) || !PEEK_CHAR(c, tok)) {
	    printbuf_memappend_fast(tok->pb, case_start, str-case_start);
	    goto out;
	  }
	}
      }
      break;

    case json_tokener_state_string_escape:
      switch(c) {
      case '"':
      case '\\':
      case '/':
	printbuf_memappend_fast(tok->pb, &c, 1);
	state = saved_state;
	break;
      case 'b':
      case 'n':
      case 'r':
      case 't':
      case 'f':
	if(c == 'b') printbuf_memappend_fast(tok->pb, "\b", 1);
	else if(c == 'n') printbuf_memappend_fast(tok->pb, "\n", 1);
	else if(c == 'r') printbuf_memappend_fast(tok->pb, "\r", 1);
	else if(c == 't') printbuf_memappend_fast(tok->pb, "\t", 1);
	else if(c == 'f') printbuf_memappend_fast(tok->pb, "\f", 1);
	state = saved_state;
	break;
      case 'u':
	tok->ucs_char = 0;
	tok->st_pos = 0;
	state = json_tokener_state_escape_unicode;
	break;
      default:
	tok->err = json_tokener_error_parse_string;
	goto out;
      }
      break;

    case json_tokener_state_escape_unicode:
	{
          unsigned int got_hi_surrogate = 0;

	  /* Handle a 4-byte sequence, or two sequences if a surrogate pair */
	  while(1) {
	    if(strchr(json_hex_chars, c)) {
	      tok->ucs_char += ((unsigned int)jt_hexdigit(c) << ((3-tok->st_pos++)*4));
	      if(tok->st_pos == 4) {
		unsigned char unescaped_utf[4];

                if (got_hi_surrogate) {
		  if (IS_LOW_SURROGATE(tok->ucs_char)) {
                    /* Recalculate the ucs_char, then fall thru to process normally */
                    tok->ucs_char = DECODE_SURROGATE_PAIR(got_hi_surrogate, tok->ucs_char);
                  } else {
                    /* Hi surrogate was not followed by a low surrogate */
                    /* Replace the hi and process the rest normally */
		    printbuf_memappend_fast(tok->pb, (char*)utf8_replacement_char, 3);
                  }
                  got_hi_surrogate = 0;
                }

		if (tok->ucs_char < 0x80) {
		  unescaped_utf[0] = tok->ucs_char;
		  printbuf_memappend_fast(tok->pb, (char*)unescaped_utf, 1);
		} else if (tok->ucs_char < 0x800) {
		  unescaped_utf[0] = 0xc0 | (tok->ucs_char >> 6);
		  unescaped_utf[1] = 0x80 | (tok->ucs_char & 0x3f);
		  printbuf_memappend_fast(tok->pb, (char*)unescaped_utf, 2);
		} else if (IS_HIGH_SURROGATE(tok->ucs_char)) {
                  /* Got a high surrogate.  Remember it and look for the
                   * the beginning of another sequence, which should be the
                   * low surrogate.
                   */
                  got_hi_surrogate = tok->ucs_char;
                  /* Not at end, and the next two chars should be "\u" */
                  if ((tok->char_offset+1 != len) &&
                      (tok->char_offset+2 != len) &&
                      (str[1] == '\\') &&
                      (str[2] == 'u'))
                  {
                /* Advance through the 16 bit surrogate, and move on to the
                 * next sequence. The next step is to process the following
                 * characters.
                 */
	            if( !ADVANCE_CHAR(str, tok) || !ADVANCE_CHAR(str, tok) ) {
                    printbuf_memappend_fast(tok->pb, (char*)utf8_replacement_char, 3);
                }
                    /* Advance to the first char of the next sequence and
                     * continue processing with the next sequence.
                     */
	            if (!ADVANCE_CHAR(str, tok) || !PEEK_CHAR(c, tok)) {
	              printbuf_memappend_fast(tok->pb, (char*)utf8_replacement_char, 3);
	              goto out;
                    }
	            tok->ucs_char = 0;
                    tok->st_pos = 0;
                    continue; /* other json_tokener_state_escape_unicode */
                  } else {
                    /* Got a high surrogate without another sequence following
                     * it.  Put a replacement char in for the hi surrogate
                     * and pretend we finished.
                     */
		    printbuf_memappend_fast(tok->pb, (char*)utf8_replacement_char, 3);
                  }
		} else if (IS_LOW_SURROGATE(tok->ucs_char)) {
                  /* Got a low surrogate not preceded by a high */
		  printbuf_memappend_fast(tok->pb, (char*)utf8_replacement_char, 3);
                } else if (tok->ucs_char < 0x10000) {
		  unescaped_utf[0] = 0xe0 | (tok->ucs_char >> 12);
		  unescaped_utf[1] = 0x80 | ((tok->ucs_char >> 6) & 0x3f);
		  unescaped_utf[2] = 0x80 | (tok->ucs_char & 0x3f);
		  printbuf_memappend_fast(tok->pb, (char*)unescaped_utf, 3);
		} else if (tok->ucs_char < 0x110000) {
		  unescaped_utf[0] = 0xf0 | ((tok->ucs_char >> 18) & 0x07);
		  unescaped_utf[1] = 0x80 | ((tok->ucs_char >> 12) & 0x3f);
		  unescaped_utf[2] = 0x80 | ((tok->ucs_char >> 6) & 0x3f);
		  unescaped_utf[3] = 0x80 | (tok->ucs_char & 0x3f);
		  printbuf_memappend_fast(tok->pb, (char*)unescaped_utf, 4);
		} else {
                  /* Don't know what we got--insert the replacement char */
		  printbuf_memappend_fast(tok->pb, (char*)utf8_replacement_char, 3);
                }
		state = saved_state;
		break;
	      }
	    } else {
	      tok->err = json_tokener_error_parse_string;
	      goto out;
	    }
	  if (!ADVANCE_CHAR(str, tok) || !PEEK_CHAR(c, tok)) {
            if (got_hi_surrogate) /* Clean up any pending chars */
	      printbuf_memappend_fast(tok->pb, (char*)utf8_replacement_char, 3);
	    goto out;
	  }
	}
      }
      break;

    case json_tokener_state_boolean:
      {
	int size1, size2;
	printbuf_memappend_fast(tok->pb, &c, 1);
	size1 = json_min(tok->st_pos+1, json_true_str_len);
	size2 = json_min(tok->st_pos+1, json_false_str_len);
	if((!(tok->flags & JSON_TOKENER_STRICT) &&
	  strncasecmp(json_true_str, tok->pb->buf, size1) == 0)
	  || (strncmp(json_true_str, tok->pb->buf, size1) == 0)
	  ) {
	  if(tok->st_pos == json_true_str_len) {
	    current = json_object_new_boolean(1);
	    saved_state = json_tokener_state_finish;
	    state = json_tokener_state_eatws;
	    goto redo_char;
	  }
	} else if((!(tok->flags & JSON_TOKENER_STRICT) &&
	  strncasecmp(json_false_str, tok->pb->buf, size2) == 0)
	  || (strncmp(json_false_str, tok->pb->buf, size2) == 0)) {
	  if(tok->st_pos == json_false_str_len) {
	    current = json_object_new_boolean(0);
	    saved_state = json_tokener_state_finish;
	    state = json_tokener_state_eatws;
	    goto redo_char;
	  }
	} else {
	  tok->err = json_tokener_error_parse_boolean;
	  goto out;
	}
	tok->st_pos++;
      }
      break;

    case json_tokener_state_number:
      {
	/* Advance until we change state */
	const char *case_start = str;
	int case_len=0;
	while(c && strchr(json_number_chars, c)) {
	  ++case_len;
	  if(c == '.' || c == 'e' || c == 'E')
	    tok->is_double = 1;
	  if (!ADVANCE_CHAR(str, tok) || !PEEK_CHAR(c, tok)) {
	    printbuf_memappend_fast(tok->pb, case_start, case_len);
	    goto out;
	  }
	}
        if (case_len>0)
          printbuf_memappend_fast(tok->pb, case_start, case_len);

	// Check for -Infinity
	if (tok->pb->buf[0] == '-' && case_len == 1 &&
	    (c == 'i' || c == 'I'))
	{
		state = json_tokener_state_inf;
		goto redo_char;
	}
      }
      {
	int64_t num64;
	double  numd;
	if (!tok->is_double && json_parse_int64(tok->pb->buf, &num64) == 0) {
		if (num64 && tok->pb->buf[0]=='0' && (tok->flags & JSON_TOKENER_STRICT)) {
			/* in strict mode, number must not start with 0 */
			tok->err = json_tokener_error_parse_number;
			goto out;
		}
		current = json_object_new_int64(num64);
	}
	else if(tok->is_double && json_parse_double(tok->pb->buf, &numd) == 0)
	{
          current = json_object_new_double_s(numd, tok->pb->buf);
        } else {
          tok->err = json_tokener_error_parse_number;
          goto out;
        }
        saved_state = json_tokener_state_finish;
        state = json_tokener_state_eatws;
        goto redo_char;
      }
      break;

    case json_tokener_state_array_after_sep:
    case json_tokener_state_array:
      if(c == ']') {
		if (state == json_tokener_state_array_after_sep &&
			(tok->flags & JSON_TOKENER_STRICT))
		{
			tok->err = json_tokener_error_parse_unexpected;
			goto out;
		}
	saved_state = json_tokener_state_finish;
	state = json_tokener_state_eatws;
      } else {
	if(tok->depth >= tok->max_depth-1) {
	  tok->err = json_tokener_error_depth;
	  goto out;
	}
	state = json_tokener_state_array_add;
	tok->depth++;
	json_tokener_reset_level(tok, tok->depth);
	goto redo_char;
      }
      break;

    case json_tokener_state_array_add:
      json_object_array_add(current, obj);
      saved_state = json_tokener_state_array_sep;
      state = json_tokener_state_eatws;
      goto redo_char;

    case json_tokener_state_array_sep:
      if(c == ']') {
	saved_state = json_tokener_state_finish;
	state = json_tokener_state_eatws;
      } else if(c == ',') {
	saved_state = json_tokener_state_array_after_sep;
	state = json_tokener_state_eatws;
      } else {
	tok->err = json_tokener_error_parse_array;
	goto out;
      }
      break;

    case json_tokener_state_object_field_start:
    case json_tokener_state_object_field_start_after_sep:
      if(c == '}') {
		if (state == json_tokener_state_object_field_start_after_sep &&
		    (tok->flags & JSON_TOKENER_STRICT))
		{
			tok->err = json_tokener_error_parse_unexpected;
			goto out;
		}
	saved_state = json_tokener_state_finish;
	state = json_tokener_state_eatws;
      } else if (c == '"' || c == '\'') {
	tok->quote_char = c;
	printbuf_reset(tok->pb);
	state = json_tokener_state_object_field;
      } else {
	tok->err = json_tokener_error_parse_object_key_name;
	goto out;
      }
      break;

    case json_tokener_state_object_field:
      {
	/* Advance until we change state */
	const char *case_start = str;
	while(1) {
	  if(c == tok->quote_char) {
	    printbuf_memappend_fast(tok->pb, case_start, str-case_start);
	    obj_field_name = strdup(tok->pb->buf);
	    saved_state = json_tokener_state_object_field_end;
	    state = json_tokener_state_eatws;
	    break;
	  } else if(c == '\\') {
	    printbuf_memappend_fast(tok->pb, case_start, str-case_start);
	    saved_state = json_tokener_state_object_field;
	    state = json_tokener_state_string_escape;
	    break;
	  }
	  if (!ADVANCE_CHAR(str, tok) || !PEEK_CHAR(c, tok)) {
	    printbuf_memappend_fast(tok->pb, case_start, str-case_start);
	    goto out;
	  }
	}
      }
      break;

    case json_tokener_state_object_field_end:
      if(c == ':') {
	saved_state = json_tokener_state_object_value;
	state = json_tokener_state_eatws;
      } else {
	tok->err = json_tokener_error_parse_object_key_sep;
	goto out;
      }
      break;

    case json_tokener_state_object_value:
      if(tok->depth >= tok->max_depth-1) {
	tok->err = json_tokener_error_depth;
	goto out;
      }
      state = json_tokener_state_object_value_add;
      tok->depth++;
      json_tokener_reset_level(tok, tok->depth);
      goto redo_char;

    case json_tokener_state_object_value_add:
      json_object_object_add(current, obj_field_name, obj);
      free(obj_field_name);
      obj_field_name = NULL;
      saved_state = json_tokener_state_object_sep;
      state = json_tokener_state_eatws;
      goto redo_char;

    case json_tokener_state_object_sep:
      if(c == '}') {
	saved_state = json_tokener_state_finish;
	state = json_tokener_state_eatws;
      } else if(c == ',') {
	saved_state = json_tokener_state_object_field_start_after_sep;
	state = json_tokener_state_eatws;
      } else {
	tok->err = json_tokener_error_parse_object_value_sep;
	goto out;
      }
      break;

    }
    if (!ADVANCE_CHAR(str, tok))
      goto out;
  } /* while(POP_CHAR) */

 out:
  if (c &&
     (state == json_tokener_state_finish) &&
     (tok->depth == 0) &&
     (tok->flags & JSON_TOKENER_STRICT)) {
      /* unexpected char after JSON data */
      tok->err = json_tokener_error_parse_unexpected;
  }
  if (!c) { /* We hit an eof char (0) */
    if(state != json_tokener_state_finish &&
       saved_state != json_tokener_state_finish)
      tok->err = json_tokener_error_parse_eof;
  }

#ifdef HAVE_SETLOCALE
  setlocale(LC_NUMERIC, oldlocale);
  if (oldlocale) free(oldlocale);
#endif

  if (tok->err == json_tokener_success)
  {
    json_object *ret = json_object_get(current);
	int ii;

	/* Partially reset, so we parse additional objects on subsequent calls. */
    for(ii = tok->depth; ii >= 0; ii--)
      json_tokener_reset_level(tok, ii);
    return ret;
  }

  MC_DEBUG("json_tokener_parse_ex: error %s at offset %d\n",
	   json_tokener_errors[tok->err], tok->char_offset);
  return NULL;
}

void json_tokener_set_flags(struct json_tokener *tok, int flags)
{
	tok->flags = flags;
}
//-------json_object_iterator.c-------

/**
*******************************************************************************
* @file json_object_iterator.c
*
* Copyright (c) 2009-2012 Hewlett-Packard Development Company, L.P.
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the MIT license. See COPYING for details.
*
* @brief  json-c forces clients to use its private data
*         structures for JSON Object iteration.  This API
*         implementation corrects that by abstracting the
*         private json-c details.
*
*******************************************************************************
*/

#include <stddef.h>






/**
 * How It Works
 *
 * For each JSON Object, json-c maintains a linked list of zero
 * or more lh_entry (link-hash entry) structures inside the
 * Object's link-hash table (lh_table).
 *
 * Each lh_entry structure on the JSON Object's linked list
 * represents a single name/value pair.  The "next" field of the
 * last lh_entry in the list is set to NULL, which terminates
 * the list.
 *
 * We represent a valid iterator that refers to an actual
 * name/value pair via a pointer to the pair's lh_entry
 * structure set as the iterator's opaque_ field.
 *
 * We follow json-c's current pair list representation by
 * representing a valid "end" iterator (one that refers past the
 * last pair) with a NULL value in the iterator's opaque_ field.
 *
 * A JSON Object without any pairs in it will have the "head"
 * field of its lh_table structure set to NULL.  For such an
 * object, json_object_iter_begin will return an iterator with
 * the opaque_ field set to NULL, which is equivalent to the
 * "end" iterator.
 *
 * When iterating, we simply update the iterator's opaque_ field
 * to point to the next lh_entry structure in the linked list.
 * opaque_ will become NULL once we iterate past the last pair
 * in the list, which makes the iterator equivalent to the "end"
 * iterator.
 */

/// Our current representation of the "end" iterator;
///
/// @note May not always be NULL
static const void* kObjectEndIterValue = NULL;

/**
 * ****************************************************************************
 */
struct json_object_iterator
json_object_iter_begin(struct json_object* obj)
{
    struct json_object_iterator iter;
    struct lh_table* pTable;

    /// @note json_object_get_object will return NULL if passed NULL
    ///       or a non-json_type_object instance
    pTable = json_object_get_object(obj);
    JASSERT(NULL != pTable);

    /// @note For a pair-less Object, head is NULL, which matches our
    ///       definition of the "end" iterator
    iter.opaque_ = pTable->head;
    return iter;
}

/**
 * ****************************************************************************
 */
struct json_object_iterator
json_object_iter_end(const struct json_object* obj)
{
    struct json_object_iterator iter;

    JASSERT(NULL != obj);
    JASSERT(json_object_is_type(obj, json_type_object));

    iter.opaque_ = kObjectEndIterValue;

    return iter;
}

/**
 * ****************************************************************************
 */
void
json_object_iter_next(struct json_object_iterator* iter)
{
    JASSERT(NULL != iter);
    JASSERT(kObjectEndIterValue != iter->opaque_);

    iter->opaque_ = ((struct lh_entry *)iter->opaque_)->next;
}


/**
 * ****************************************************************************
 */
const char*
json_object_iter_peek_name(const struct json_object_iterator* iter)
{
    JASSERT(NULL != iter);
    JASSERT(kObjectEndIterValue != iter->opaque_);

    return (const char*)(((struct lh_entry *)iter->opaque_)->k);
}


/**
 * ****************************************************************************
 */
struct json_object*
json_object_iter_peek_value(const struct json_object_iterator* iter)
{
    JASSERT(NULL != iter);
    JASSERT(kObjectEndIterValue != iter->opaque_);

    return (struct json_object*)(((struct lh_entry *)iter->opaque_)->v);
}


/**
 * ****************************************************************************
 */
json_bool
json_object_iter_equal(const struct json_object_iterator* iter1,
                       const struct json_object_iterator* iter2)
{
    JASSERT(NULL != iter1);
    JASSERT(NULL != iter2);

    return (iter1->opaque_ == iter2->opaque_);
}


/**
 * ****************************************************************************
 */
struct json_object_iterator
json_object_iter_init_default(void)
{
    struct json_object_iterator iter;

    /**
     * @note Make this a negative, invalid value, such that
     *       accidental access to it would likely be trapped by the
     *       hardware as an invalid address.
     */
    iter.opaque_ = NULL;

    return iter;
}
//-------json_object.c-------

/*
 * $Id: json_object.c,v 1.17 2006/07/25 03:24:50 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 * Copyright (c) 2009 Hewlett-Packard Development Company, L.P.
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <errno.h>











#if !defined(HAVE_STRDUP) && defined(_MSC_VER)
  /* MSC has the version as _strdup */
# define strdup _strdup
#elif !defined(HAVE_STRDUP)
# error You do not have strdup on your system.
#endif /* HAVE_STRDUP */

#if !defined(HAVE_SNPRINTF) && defined(_MSC_VER)
  /* MSC has the version as _snprintf */
# define snprintf _snprintf
#elif !defined(HAVE_SNPRINTF)
# error You do not have snprintf on your system.
#endif /* HAVE_SNPRINTF */

// Don't define this.  It's not thread-safe.
/* #define REFCOUNT_DEBUG 1 */

const char *json_number_chars = "0123456789.+-eE";
const char *json_hex_chars = "0123456789abcdefABCDEF";

static void json_object_generic_delete(struct json_object* jso);
static struct json_object* json_object_new(enum json_type o_type);

static json_object_to_json_string_fn json_object_object_to_json_string;
static json_object_to_json_string_fn json_object_boolean_to_json_string;
static json_object_to_json_string_fn json_object_int_to_json_string;
static json_object_to_json_string_fn json_object_double_to_json_string;
static json_object_to_json_string_fn json_object_string_to_json_string;
static json_object_to_json_string_fn json_object_array_to_json_string;


/* ref count debugging */

#ifdef REFCOUNT_DEBUG

static struct lh_table *json_object_table;

static void json_object_init(void) __attribute__ ((constructor));
static void json_object_init(void) {
	MC_DEBUG("json_object_init: creating object table\n");
	json_object_table = lh_kptr_table_new(128, "json_object_table", NULL);
}

static void json_object_fini(void) __attribute__ ((destructor));
static void json_object_fini(void)
{
	struct lh_entry *ent;
	if (MC_GET_DEBUG())
	{
		if (json_object_table->count)
		{
			MC_DEBUG("json_object_fini: %d referenced objects at exit\n",
			   json_object_table->count);
			lh_foreach(json_object_table, ent)
			{
				struct json_object* obj = (struct json_object*)ent->v;
				MC_DEBUG("\t%s:%p\n", json_type_to_name(obj->o_type), obj);
			}
		}
	}
	MC_DEBUG("json_object_fini: freeing object table\n");
	lh_table_free(json_object_table);
}
#endif /* REFCOUNT_DEBUG */


/* string escaping */

static int json_escape_str(struct printbuf *pb, char *str, int len)
{
	int pos = 0, start_offset = 0;
	unsigned char c;
	while (len--)
	{
		c = str[pos];
		switch(c)
		{
		case '\b':
		case '\n':
		case '\r':
		case '\t':
		case '\f':
		case '"':
		case '\\':
		case '/':
			if(pos - start_offset > 0)
				printbuf_memappend(pb, str + start_offset, pos - start_offset);

			if(c == '\b') printbuf_memappend(pb, "\\b", 2);
			else if(c == '\n') printbuf_memappend(pb, "\\n", 2);
			else if(c == '\r') printbuf_memappend(pb, "\\r", 2);
			else if(c == '\t') printbuf_memappend(pb, "\\t", 2);
			else if(c == '\f') printbuf_memappend(pb, "\\f", 2);
			else if(c == '"') printbuf_memappend(pb, "\\\"", 2);
			else if(c == '\\') printbuf_memappend(pb, "\\\\", 2);
			else if(c == '/') printbuf_memappend(pb, "\\/", 2);

			start_offset = ++pos;
			break;
		default:
			if(c < ' ')
			{
				if(pos - start_offset > 0)
				printbuf_memappend(pb, str + start_offset, pos - start_offset);
				sprintbuf(pb, "\\u00%c%c",
				json_hex_chars[c >> 4],
				json_hex_chars[c & 0xf]);
				start_offset = ++pos;
			} else
				pos++;
		}
	}
	if (pos - start_offset > 0)
		printbuf_memappend(pb, str + start_offset, pos - start_offset);
	return 0;
}


/* reference counting */

extern struct json_object* json_object_get(struct json_object *jso)
{
	if (jso)
		jso->_ref_count++;
	return jso;
}

int json_object_put(struct json_object *jso)
{
	if(jso)
	{
		jso->_ref_count--;
		if(!jso->_ref_count)
		{
			if (jso->_user_delete)
				jso->_user_delete(jso, jso->_userdata);
			jso->_delete(jso);
			return 1;
		}
	}
	return 0;
}


/* generic object construction and destruction parts */

static void json_object_generic_delete(struct json_object* jso)
{
#ifdef REFCOUNT_DEBUG
	MC_DEBUG("json_object_delete_%s: %p\n",
	   json_type_to_name(jso->o_type), jso);
	lh_table_delete(json_object_table, jso);
#endif /* REFCOUNT_DEBUG */
	printbuf_free(jso->_pb);
	free(jso);
}

static struct json_object* json_object_new(enum json_type o_type)
{
	struct json_object *jso;

	jso = (struct json_object*)calloc(sizeof(struct json_object), 1);
	if (!jso)
		return NULL;
	jso->o_type = o_type;
	jso->_ref_count = 1;
	jso->_delete = &json_object_generic_delete;
#ifdef REFCOUNT_DEBUG
	lh_table_insert(json_object_table, jso, jso);
	MC_DEBUG("json_object_new_%s: %p\n", json_type_to_name(jso->o_type), jso);
#endif /* REFCOUNT_DEBUG */
	return jso;
}


/* type checking functions */

int json_object_is_type(struct json_object *jso, enum json_type type)
{
	if (!jso)
		return (type == json_type_null);
	return (jso->o_type == type);
}

enum json_type json_object_get_type(struct json_object *jso)
{
	if (!jso)
		return json_type_null;
	return jso->o_type;
}

/* set a custom conversion to string */

void json_object_set_serializer(json_object *jso,
	json_object_to_json_string_fn to_string_func,
	void *userdata,
	json_object_delete_fn *user_delete)
{
	// First, clean up any previously existing user info
	if (jso->_user_delete)
	{
		jso->_user_delete(jso, jso->_userdata);
	}
	jso->_userdata = NULL;
	jso->_user_delete = NULL;

	if (to_string_func == NULL)
	{
		// Reset to the standard serialization function
		switch(jso->o_type)
		{
		case json_type_null:
			jso->_to_json_string = NULL;
			break;
		case json_type_boolean:
			jso->_to_json_string = &json_object_boolean_to_json_string;
			break;
		case json_type_double:
			jso->_to_json_string = &json_object_double_to_json_string;
			break;
		case json_type_int:
			jso->_to_json_string = &json_object_int_to_json_string;
			break;
		case json_type_object:
			jso->_to_json_string = &json_object_object_to_json_string;
			break;
		case json_type_array:
			jso->_to_json_string = &json_object_array_to_json_string;
			break;
		case json_type_string:
			jso->_to_json_string = &json_object_string_to_json_string;
			break;
		}
		return;
	}

	jso->_to_json_string = to_string_func;
	jso->_userdata = userdata;
	jso->_user_delete = user_delete;
}


/* extended conversion to string */

const char* json_object_to_json_string_ext(struct json_object *jso, int flags)
{
	if (!jso)
		return "null";

	if ((!jso->_pb) && !(jso->_pb = printbuf_new()))
		return NULL;

	printbuf_reset(jso->_pb);

	if(jso->_to_json_string(jso, jso->_pb, 0, flags) < 0)
		return NULL;

	return jso->_pb->buf;
}

/* backwards-compatible conversion to string */

const char* json_object_to_json_string(struct json_object *jso)
{
	return json_object_to_json_string_ext(jso, JSON_C_TO_STRING_SPACED);
}

static void indent(struct printbuf *pb, int level, int flags)
{
	if (flags & JSON_C_TO_STRING_PRETTY)
	{
		printbuf_memset(pb, -1, ' ', level * 2);
	}
}

/* json_object_object */

static int json_object_object_to_json_string(struct json_object* jso,
					     struct printbuf *pb,
					     int level,
						 int flags)
{
	int had_children = 0;
	struct json_object_iter iter;

	sprintbuf(pb, "{" /*}*/);
	if (flags & JSON_C_TO_STRING_PRETTY)
		sprintbuf(pb, "\n");
	json_object_object_foreachC(jso, iter)
	{
		if (had_children)
		{
			sprintbuf(pb, ",");
			if (flags & JSON_C_TO_STRING_PRETTY)
				sprintbuf(pb, "\n");
		}
		had_children = 1;
		if (flags & JSON_C_TO_STRING_SPACED)
			sprintbuf(pb, " ");
		indent(pb, level+1, flags);
		sprintbuf(pb, "\"");
		json_escape_str(pb, iter.key, strlen(iter.key));
		if (flags & JSON_C_TO_STRING_SPACED)
			sprintbuf(pb, "\": ");
		else
			sprintbuf(pb, "\":");
		if(iter.val == NULL)
			sprintbuf(pb, "null");
		else
			iter.val->_to_json_string(iter.val, pb, level+1,flags);
	}
	if (flags & JSON_C_TO_STRING_PRETTY)
	{
		if (had_children)
			sprintbuf(pb, "\n");
		indent(pb,level,flags);
	}
	if (flags & JSON_C_TO_STRING_SPACED)
		return sprintbuf(pb, /*{*/ " }");
	else
		return sprintbuf(pb, /*{*/ "}");
}


static void json_object_lh_entry_free(struct lh_entry *ent)
{
	free(ent->k);
	json_object_put((struct json_object*)ent->v);
}

static void json_object_object_delete(struct json_object* jso)
{
	lh_table_free(jso->o.c_object);
	json_object_generic_delete(jso);
}

struct json_object* json_object_new_object(void)
{
	struct json_object *jso = json_object_new(json_type_object);
	if (!jso)
		return NULL;
	jso->_delete = &json_object_object_delete;
	jso->_to_json_string = &json_object_object_to_json_string;
	jso->o.c_object = lh_kchar_table_new(JSON_OBJECT_DEF_HASH_ENTRIES,
					NULL, &json_object_lh_entry_free);
	if (!jso->o.c_object)
	{
		json_object_generic_delete(jso);
		errno = ENOMEM;
		return NULL;
	}
	return jso;
}

struct lh_table* json_object_get_object(struct json_object *jso)
{
	if (!jso)
		return NULL;
	switch(jso->o_type)
	{
	case json_type_object:
		return jso->o.c_object;
	default:
		return NULL;
	}
}

void json_object_object_add(struct json_object* jso, const char *key,
			    struct json_object *val)
{
	// We lookup the entry and replace the value, rather than just deleting
	// and re-adding it, so the existing key remains valid.
	json_object *existing_value = NULL;
	struct lh_entry *existing_entry;
	existing_entry = lh_table_lookup_entry(jso->o.c_object, (void*)key);
	if (!existing_entry)
	{
		lh_table_insert(jso->o.c_object, strdup(key), val);
		return;
	}
	existing_value = (json_object *)(void *)existing_entry->v;
	if (existing_value)
		json_object_put(existing_value);
	existing_entry->v = val;
}

int json_object_object_length(struct json_object *jso)
{
	return lh_table_length(jso->o.c_object);
}

struct json_object* json_object_object_get(struct json_object* jso, const char *key)
{
	struct json_object *result = NULL;
	json_object_object_get_ex(jso, key, &result);
	return result;
}

json_bool json_object_object_get_ex(struct json_object* jso, const char *key, struct json_object **value)
{
	if (value != NULL)
		*value = NULL;

	if (NULL == jso)
		return FALSE;

	switch(jso->o_type)
	{
	case json_type_object:
		return lh_table_lookup_ex(jso->o.c_object, (void*)key, (void**)value);
	default:
		if (value != NULL)
			*value = NULL;
		return FALSE;
	}
}

void json_object_object_del(struct json_object* jso, const char *key)
{
	lh_table_delete(jso->o.c_object, key);
}


/* json_object_boolean */

static int json_object_boolean_to_json_string(struct json_object* jso,
					      struct printbuf *pb,
					      int level,
						  int flags)
{
	if (jso->o.c_boolean)
		return sprintbuf(pb, "true");
	else
		return sprintbuf(pb, "false");
}

struct json_object* json_object_new_boolean(json_bool b)
{
	struct json_object *jso = json_object_new(json_type_boolean);
	if (!jso)
		return NULL;
	jso->_to_json_string = &json_object_boolean_to_json_string;
	jso->o.c_boolean = b;
	return jso;
}

json_bool json_object_get_boolean(struct json_object *jso)
{
	if (!jso)
		return FALSE;
	switch(jso->o_type)
	{
	case json_type_boolean:
		return jso->o.c_boolean;
	case json_type_int:
		return (jso->o.c_int64 != 0);
	case json_type_double:
		return (jso->o.c_double != 0);
	case json_type_string:
		return (jso->o.c_string.len != 0);
	default:
		return FALSE;
	}
}


/* json_object_int */

static int json_object_int_to_json_string(struct json_object* jso,
					  struct printbuf *pb,
					  int level,
					  int flags)
{
	//return sprintbuf(pb, "%"PRId64, jso->o.c_int64);
	return sprintbuf(pb, "%" PRId64, jso->o.c_int64);
}

struct json_object* json_object_new_int(int32_t i)
{
	struct json_object *jso = json_object_new(json_type_int);
	if (!jso)
		return NULL;
	jso->_to_json_string = &json_object_int_to_json_string;
	jso->o.c_int64 = i;
	return jso;
}

int32_t json_object_get_int(struct json_object *jso)
{
  int64_t cint64;
  enum json_type o_type;

  if(!jso) return 0;

  o_type = jso->o_type;
  cint64 = jso->o.c_int64;

  if (o_type == json_type_string)
  {
	/*
	 * Parse strings into 64-bit numbers, then use the
	 * 64-to-32-bit number handling below.
	 */
	if (json_parse_int64(jso->o.c_string.str, &cint64) != 0)
		return 0; /* whoops, it didn't work. */
	o_type = json_type_int;
  }

  switch(o_type) {
  case json_type_int:
	/* Make sure we return the correct values for out of range numbers. */
	if (cint64 <= INT32_MIN)
		return INT32_MIN;
	else if (cint64 >= INT32_MAX)
		return INT32_MAX;
	else
		return (int32_t)cint64;
  case json_type_double:
    return (int32_t)jso->o.c_double;
  case json_type_boolean:
    return jso->o.c_boolean;
  default:
    return 0;
  }
}

struct json_object* json_object_new_int64(int64_t i)
{
	struct json_object *jso = json_object_new(json_type_int);
	if (!jso)
		return NULL;
	jso->_to_json_string = &json_object_int_to_json_string;
	jso->o.c_int64 = i;
	return jso;
}

int64_t json_object_get_int64(struct json_object *jso)
{
	int64_t cint;

	if (!jso)
		return 0;
	switch(jso->o_type)
	{
	case json_type_int:
		return jso->o.c_int64;
	case json_type_double:
		return (int64_t)jso->o.c_double;
	case json_type_boolean:
		return jso->o.c_boolean;
	case json_type_string:
		if (json_parse_int64(jso->o.c_string.str, &cint) == 0)
			return cint;
	default:
		return 0;
	}
}


/* json_object_double */

static int json_object_double_to_json_string(struct json_object* jso,
					     struct printbuf *pb,
					     int level,
						 int flags)
{
  char buf[128], *p, *q;
  int size;
  /* Although JSON RFC does not support
     NaN or Infinity as numeric values
     ECMA 262 section 9.8.1 defines
     how to handle these cases as strings */
  if(isnan(jso->o.c_double))
    size = snprintf(buf, sizeof(buf), "NaN");
  else if(isinf(jso->o.c_double))
    if(jso->o.c_double > 0)
      size = snprintf(buf, sizeof(buf), "Infinity");
    else
      size = snprintf(buf, sizeof(buf), "-Infinity");
  else
    size = snprintf(buf, sizeof(buf), "%.17g", jso->o.c_double);

  p = strchr(buf, ',');
  if (p) {
    *p = '.';
  } else {
    p = strchr(buf, '.');
  }
  if (p && (flags & JSON_C_TO_STRING_NOZERO)) {
    /* last useful digit, always keep 1 zero */
    p++;
    for (q=p ; *q ; q++) {
      if (*q!='0') p=q;
    }
    /* drop trailing zeroes */
    *(++p) = 0;
    size = p-buf;
  }
  printbuf_memappend(pb, buf, size);
  return size;
}

struct json_object* json_object_new_double(double d)
{
	struct json_object *jso = json_object_new(json_type_double);
	if (!jso)
		return NULL;
	jso->_to_json_string = &json_object_double_to_json_string;
	jso->o.c_double = d;
	return jso;
}

struct json_object* json_object_new_double_s(double d, const char *ds)
{
	struct json_object *jso = json_object_new_double(d);
	if (!jso)
		return NULL;

	char *new_ds = strdup(ds);
	if (!new_ds)
	{
		json_object_generic_delete(jso);
		errno = ENOMEM;
		return NULL;
	}
	json_object_set_serializer(jso, json_object_userdata_to_json_string,
	    new_ds, json_object_free_userdata);
	return jso;
}

int json_object_userdata_to_json_string(struct json_object *jso,
	struct printbuf *pb, int level, int flags)
{
	int userdata_len = strlen((const char *) jso->_userdata);
	printbuf_memappend(pb, (const char *) jso->_userdata, userdata_len);
	return userdata_len;
}

void json_object_free_userdata(struct json_object *jso, void *userdata)
{
	free(userdata);
}

double json_object_get_double(struct json_object *jso)
{
  double cdouble;
  char *errPtr = NULL;

  if(!jso) return 0.0;
  switch(jso->o_type) {
  case json_type_double:
    return jso->o.c_double;
  case json_type_int:
    return jso->o.c_int64;
  case json_type_boolean:
    return jso->o.c_boolean;
  case json_type_string:
    errno = 0;
    cdouble = strtod(jso->o.c_string.str,&errPtr);

    /* if conversion stopped at the first character, return 0.0 */
    if (errPtr == jso->o.c_string.str)
        return 0.0;

    /*
     * Check that the conversion terminated on something sensible
     *
     * For example, { "pay" : 123AB } would parse as 123.
     */
    if (*errPtr != '\0')
        return 0.0;

    /*
     * If strtod encounters a string which would exceed the
     * capacity of a double, it returns +/- HUGE_VAL and sets
     * errno to ERANGE. But +/- HUGE_VAL is also a valid result
     * from a conversion, so we need to check errno.
     *
     * Underflow also sets errno to ERANGE, but it returns 0 in
     * that case, which is what we will return anyway.
     *
     * See CERT guideline ERR30-C
     */
    if ((HUGE_VAL == cdouble || -HUGE_VAL == cdouble) &&
        (ERANGE == errno))
            cdouble = 0.0;
    return cdouble;
  default:
    return 0.0;
  }
}


/* json_object_string */

static int json_object_string_to_json_string(struct json_object* jso,
					     struct printbuf *pb,
					     int level,
						 int flags)
{
	sprintbuf(pb, "\"");
	json_escape_str(pb, jso->o.c_string.str, jso->o.c_string.len);
	sprintbuf(pb, "\"");
	return 0;
}

static void json_object_string_delete(struct json_object* jso)
{
	free(jso->o.c_string.str);
	json_object_generic_delete(jso);
}

struct json_object* json_object_new_string(const char *s)
{
	struct json_object *jso = json_object_new(json_type_string);
	if (!jso)
		return NULL;
	jso->_delete = &json_object_string_delete;
	jso->_to_json_string = &json_object_string_to_json_string;
	jso->o.c_string.str = strdup(s);
	if (!jso->o.c_string.str)
	{
		json_object_generic_delete(jso);
		errno = ENOMEM;
		return NULL;
	}
	jso->o.c_string.len = strlen(s);
	return jso;
}

struct json_object* json_object_new_string_len(const char *s, int len)
{
	struct json_object *jso = json_object_new(json_type_string);
	if (!jso)
		return NULL;
	jso->_delete = &json_object_string_delete;
	jso->_to_json_string = &json_object_string_to_json_string;
	jso->o.c_string.str = (char*)malloc(len + 1);
	if (!jso->o.c_string.str)
	{
		json_object_generic_delete(jso);
		errno = ENOMEM;
		return NULL;
	}
	memcpy(jso->o.c_string.str, (void *)s, len);
	jso->o.c_string.str[len] = '\0';
	jso->o.c_string.len = len;
	return jso;
}

const char* json_object_get_string(struct json_object *jso)
{
	if (!jso)
		return NULL;
	switch(jso->o_type)
	{
	case json_type_string:
		return jso->o.c_string.str;
	default:
		return json_object_to_json_string(jso);
	}
}

int json_object_get_string_len(struct json_object *jso)
{
	if (!jso)
		return 0;
	switch(jso->o_type)
	{
	case json_type_string:
		return jso->o.c_string.len;
	default:
		return 0;
	}
}


/* json_object_array */

static int json_object_array_to_json_string(struct json_object* jso,
                                            struct printbuf *pb,
                                            int level,
                                            int flags)
{
	int had_children = 0;
	int ii;
	sprintbuf(pb, "[");
	if (flags & JSON_C_TO_STRING_PRETTY)
		sprintbuf(pb, "\n");
	for(ii=0; ii < json_object_array_length(jso); ii++)
	{
		struct json_object *val;
		if (had_children)
		{
			sprintbuf(pb, ",");
			if (flags & JSON_C_TO_STRING_PRETTY)
				sprintbuf(pb, "\n");
		}
		had_children = 1;
		if (flags & JSON_C_TO_STRING_SPACED)
			sprintbuf(pb, " ");
		indent(pb, level + 1, flags);
		val = json_object_array_get_idx(jso, ii);
		if(val == NULL)
			sprintbuf(pb, "null");
		else
			val->_to_json_string(val, pb, level+1, flags);
	}
	if (flags & JSON_C_TO_STRING_PRETTY)
	{
		if (had_children)
			sprintbuf(pb, "\n");
		indent(pb,level,flags);
	}

	if (flags & JSON_C_TO_STRING_SPACED)
		return sprintbuf(pb, " ]");
	else
		return sprintbuf(pb, "]");
}

static void json_object_array_entry_free(void *data)
{
	json_object_put((struct json_object*)data);
}

static void json_object_array_delete(struct json_object* jso)
{
	array_list_free(jso->o.c_array);
	json_object_generic_delete(jso);
}

struct json_object* json_object_new_array(void)
{
	struct json_object *jso = json_object_new(json_type_array);
	if (!jso)
		return NULL;
	jso->_delete = &json_object_array_delete;
	jso->_to_json_string = &json_object_array_to_json_string;
	jso->o.c_array = array_list_new(&json_object_array_entry_free);
	return jso;
}

struct array_list* json_object_get_array(struct json_object *jso)
{
	if (!jso)
		return NULL;
	switch(jso->o_type)
	{
	case json_type_array:
		return jso->o.c_array;
	default:
		return NULL;
	}
}

void json_object_array_sort(struct json_object *jso, int(*sort_fn)(const void *, const void *))
{
	array_list_sort(jso->o.c_array, sort_fn);
}

int json_object_array_length(struct json_object *jso)
{
	return array_list_length(jso->o.c_array);
}

int json_object_array_add(struct json_object *jso,struct json_object *val)
{
	return array_list_add(jso->o.c_array, val);
}

int json_object_array_put_idx(struct json_object *jso, int idx,
			      struct json_object *val)
{
	return array_list_put_idx(jso->o.c_array, idx, val);
}

struct json_object* json_object_array_get_idx(struct json_object *jso,
					      int idx)
{
	return (struct json_object*)array_list_get_idx(jso->o.c_array, idx);
}

//-------../driver.c-------

#include <stdio.h>
#include <string.h>
//#include <vector>
//#include <functional>
//#include "json-c/json.h"
//#include "json-c/json_object.h"
//#include "json-c/printbuf.h"
//#include <json-c/json.h>
//#include <json-c/json_object.h>

//requires v.1.12 of json-c.
//#IMPORTANT: v. in ubuntu repo is 1.11

typedef json_object Jsono;

//#include <jsoncpp/json/writer.h>
//using namespace std::vector;


//compilation:
//clang++ -ljson-c hello_json.cpp

/*
template <typename T>
Json::Value vec2jsonCallback(vector<T> in, 
                             function<Json::Value(T&)> const& f) {
    Json::Value root = Json::arrayValue;
    for (int i=0; i<in.size(); i++) {
        root[i] = f(in[i]);
    }
    return root;
} 

*/

/*template <typename T>
Json::Value vec2json(vector<T> in) {
    Json::Value root = Json::arrayValue;
    for (int i=0; i<in.size(); i++) {
        root[i] = in[i];
    }
    return root;
    } */

struct OtterStruct {
    int storeThis;
    int discardThis;
    /* Json::Value toJson() {
        Json::Value root = this->storeThis;
        return root;
        } */
};


extern int dummy(struct json_object *jso, struct printbuf *pb, int level, int flags) {
    sprintbuf(pb, json_object_get_string(jso));
    return 0;
};
//extern int dummyfree(struct json_object *jso, void * userdata) {
//    sprintbuf(pb, json_object_get_string(jso));
//    return 0;
//};

int mainx() {

    struct OtterStruct a, b;
    a.storeThis = 20;
    a.discardThis = 200;
    b.storeThis = 30;
    b.discardThis = 501;
    
    const char *one, *two, *three, *four, *five;
    //auto 
    //function<Json::Value(OtterStruct&)> OtterStruct2json = 
    //    &OtterStruct::toJson;
    /* function<Json::Value(OtterStruct&)> Otter2jsonLambda = 
        [&](OtterStruct x) {
        Json::Value root = x.storeThis;
        return root;
        }; */

    /*[&](OtterStruct ss) { 
        Json::Value root = ss.storeThis;
        return root;
        }; */

        //JSONNode root;
    json_object * root;
    json_object * tmp1, * tmp2;
    root = json_object_new_object();
    json_object_object_add(root, "taste", json_object_new_string("mushroom")); 
    json_object_object_get_ex(root, "taste", &tmp1);
    one = json_object_get_string(tmp1);
       
    
    //--
    json_object * maple = json_object_new_object();
    json_object_object_add(maple, "syrup", json_object_new_string("tasty"));
    json_object_object_add(root, "maple", maple);
    json_object_object_get_ex(root, "maple", &tmp1);
    //APPROACH 1
    //notice if you use a nonexistent key here (such an any string except syrup)
    // you get a segfault
    //json_object_object_get_ex(tmp1, "syrup", &tmp2);
    //two = json_object_get_string(tmp2);

    //APPROACH 2
    //costly, but if you use a nonexistent key here you don't get a segfault.
    
    
    int foundVal = 0;
    json_object_object_foreach(tmp1, key, val) {
        if (strcmp(key, "syrup") == 0) {
            two = json_object_get_string(val);    
            foundVal = 1;
        }
    }
    // in a real program we'd encase this in its own function
    // and add this or some other means of graceful response on key not found.
    // it's all about that graceful response.
    //if (!foundval) throw Error(key)

    //--
    json_object_object_add(root, "zeroString", json_object_new_string("0"));
    json_object_object_add(root, "zeroInt", json_object_new_double(0));
    double d = 12.3;
    json_object * d1 = json_object_new_double(d);
    json_object_object_add(root, "double: raw", d1);
    



 
    json_object * d2 = json_object_new_string("12.3");
    json_object_object_add(root, "double: neatly displayed", d2);
    //    const char *ds = new char[5];

    json_object_set_serializer(d2, dummy,
                               0x0, json_object_free_userdata);

    json_object_object_add(root, "emptyString", json_object_new_string(""));
    json_object * someboolean = json_object_new_boolean(1);
    printf("boolean should be 1: %i\n", json_object_get_boolean(someboolean));
    json_object_object_add(root, "shouldBeTrue", someboolean);
    json_object_object_add(root, "shouldBeMap", json_object_new_object()); 
    json_object * intArr = json_object_new_array();
    json_object_object_add(root, "one string, two doubles", intArr);
    json_object_array_put_idx(intArr, 0, json_object_new_string("20")); 
    json_object_array_put_idx(intArr, 1, json_object_new_double(40));
    json_object_array_add(intArr, json_object_new_double(60));
    json_object * middleInt = json_object_array_get_idx(intArr, 1);
    printf("middle numArray value is 40.0000...: %f\n", json_object_get_double(middleInt));
    printf("truncated to int is 40: %i\n", json_object_get_int(middleInt));
    printf("numArray length should be 3: %d\n", json_object_array_length(intArr));
    printf("num of keys in root: %d\n", json_object_object_length(root));
    //---


    json_object_object_add(root, "this key is", json_object_new_string("extraneous"));
    json_object_object_del(root, "this key is");

    
    /*
    int somelist[] = {4,5,6};
    root["someInts"] = vec2json<int>({4,5,6});
    string da = "4";
    string db = "5";
    string dc = "6";
    vector<string> dv;
    dv.push_back(da);
    dv.push_back(db);
    dv.push_back(dc);
    root["someStrs"] = vec2json<double>({2.0, 3.02, 4.0});

    */

    //both should be [20,30]
    //root["otterVals"] = vec2jsonCallback({a, b},
    //                                     OtterStruct2json);
    /*   root["otterValsLambda"] = vec2jsonCallback({a, b},
                                         Otter2jsonLambda);
    root["otterValsInline"] = vec2jsonCallback({a, b},
                                   (function<Json::Value(OtterStruct&)>)
                                         [&](OtterStruct x) {
                                             Json::Value root = x.storeThis;
                                             return root;
                                             });*/

    //for more cool functions, like removeMember or setComment,
    //see the documentation for json::Value
    //at /usr/share/doc/libjsoncpp-apishtml/
    //vector<string> names = root.getMemberNames();

    /*    
    Json::FastWriter fw;
    four = fw.write(root);

    Json::StyledWriter sw;
    five = sw.write(root);
    */
    
    
    printf("%s\n",one);
    printf("%s\n",two);
    printf("%s\n",(json_object_to_json_string_ext(root,  JSON_C_TO_STRING_SPACED)));
    return 0;
}

int main() {
    mainx();
}
