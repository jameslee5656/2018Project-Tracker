
#include "nuitka/prelude.hpp"

// Sentinel PyObject to be used for all our call iterator endings. It will
// become a PyCObject pointing to NULL. It's address is unique, and that's
// enough for us to use it as sentinel value.
PyObject *_sentinel_value = NULL;

PyObject *const_int_0;
PyObject *const_float_0_5;
PyObject *const_int_pos_1;
PyObject *const_int_pos_2;
PyObject *const_int_pos_3;
PyObject *const_int_pos_5;
PyObject *const_int_pos_8;
PyObject *const_str_empty;
PyObject *const_dict_empty;
PyObject *const_int_pos_10;
PyObject *const_int_pos_20;
PyObject *const_bytes_chr_2;
PyObject *const_bytes_chr_3;
PyObject *const_bytes_empty;
PyObject *const_str_plain_b;
PyObject *const_str_plain_x;
PyObject *const_tuple_empty;
PyObject *const_str_plain_end;
PyObject *const_str_plain_int;
PyObject *const_str_plain_len;
PyObject *const_str_plain_base;
PyObject *const_str_plain_data;
PyObject *const_str_plain_file;
PyObject *const_str_plain_info;
PyObject *const_str_plain_iter;
PyObject *const_str_plain_open;
PyObject *const_str_plain_read;
PyObject *const_str_plain_repr;
PyObject *const_str_plain_self;
PyObject *const_str_plain_send;
PyObject *const_str_plain_site;
PyObject *const_str_plain_time;
PyObject *const_str_plain_type;
PyObject *const_str_plain_HEART;
PyObject *const_str_plain_UUIDS;
PyObject *const_str_plain_bytes;
PyObject *const_str_plain_close;
PyObject *const_str_plain_debug;
PyObject *const_str_plain_index;
PyObject *const_str_plain_level;
PyObject *const_str_plain_print;
PyObject *const_str_plain_range;
PyObject *const_str_plain_sleep;
PyObject *const_str_plain_steps;
PyObject *const_str_plain_throw;
PyObject *const_str_plain_tuple;
PyObject *const_str_plain_types;
PyObject *const_str_plain_write;
PyObject *const_str_plain_append;
PyObject *const_str_plain_device;
PyObject *const_str_plain_encode;
PyObject *const_str_plain_format;
PyObject *const_str_plain_object;
PyObject *const_str_plain_serial;
PyObject *const_str_plain_AUTH_OK;
PyObject *const_str_plain_MESSAGE;
PyObject *const_str_plain_MiBand2;
PyObject *const_str_plain___all__;
PyObject *const_str_plain___cmp__;
PyObject *const_str_plain___doc__;
PyObject *const_str_plain_compile;
PyObject *const_str_plain_inspect;
PyObject *const_str_plain_logging;
PyObject *const_str_plain_message;
PyObject *const_str_plain_timeout;
PyObject *const_str_plain___dict__;
PyObject *const_str_plain___exit__;
PyObject *const_str_plain___file__;
PyObject *const_str_plain___init__;
PyObject *const_str_plain___iter__;
PyObject *const_str_plain___main__;
PyObject *const_str_plain___name__;
PyObject *const_str_plain___path__;
PyObject *const_str_plain_datetime;
PyObject *const_str_plain_setLevel;
PyObject *const_str_plain_RAW_ACCEL;
PyObject *const_str_plain_RAW_HEART;
PyObject *const_str_plain___class__;
PyObject *const_str_plain___enter__;
PyObject *const_str_plain_constants;
PyObject *const_str_plain_getLogger;
PyObject *const_str_plain_get_steps;
PyObject *const_str_plain_metaclass;
PyObject *const_str_plain___cached__;
PyObject *const_str_plain___import__;
PyObject *const_str_plain___loader__;
PyObject *const_str_plain___module__;
PyObject *const_str_plain_initialize;
PyObject *const_str_plain_send_alert;
PyObject *const_str_plain_ALERT_TYPES;
PyObject *const_str_plain_AUTH_FAILED;
PyObject *const_str_plain_AUTH_STATES;
PyObject *const_str_plain_QUEUE_TYPES;
PyObject *const_str_plain___package__;
PyObject *const_str_plain___prepare__;
PyObject *const_tuple_int_pos_3_tuple;
PyObject *const_str_plain___builtins__;
PyObject *const_str_plain___internal__;
PyObject *const_str_plain___qualname__;
PyObject *const_str_plain_authenticate;
PyObject *const_str_plain_SERVICE_ALERT;
PyObject *const_str_plain___metaclass__;
PyObject *const_slice_int_pos_2_none_none;
PyObject *const_slice_int_pos_3_none_none;
PyObject *const_str_plain_DefaultDelegate;
PyObject *const_str_plain_SERVICE_MIBAND1;
PyObject *const_str_plain_SERVICE_MIBAND2;
PyObject *const_slice_int_0_int_pos_2_none;
PyObject *const_str_plain_REQUEST_RN_ERROR;
PyObject *const_tuple_str_plain_self_tuple;
PyObject *const_str_plain_CHARACTERISTIC_HZ;
PyObject *const_str_plain_KEY_SENDING_FAILED;
PyObject *const_str_plain_SERVICE_HEART_RATE;
PyObject *const_str_plain_accel_raw_callback;
PyObject *const_str_plain_heart_raw_callback;
PyObject *const_str_plain_CHARACTERISTIC_AUTH;
PyObject *const_str_plain_SERVICE_DEVICE_INFO;
PyObject *const_str_plain_CHARACTERISTIC_ALERT;
PyObject *const_str_plain_CHARACTERISTIC_FETCH;
PyObject *const_str_plain_CHARACTERISTIC_STEPS;
PyObject *const_str_plain_CHARACTERISTIC_SENSOR;
PyObject *const_str_plain_CHARACTERISTIC_SERIAL;
PyObject *const_str_plain_ENCRIPTION_KEY_FAILED;
PyObject *const_str_plain_CHARACTERISTIC_BATTERY;
PyObject *const_str_plain_heart_measure_callback;
PyObject *const_str_plain_CHARACTERISTIC_REVISION;
PyObject *const_str_plain_NOTIFICATION_DESCRIPTOR;
PyObject *const_str_plain_start_raw_data_realtime;
PyObject *const_str_plain_CHARACTERISTIC_CURRENT_TIME;
PyObject *const_str_plain_CHARACTERISTIC_ACTIVITY_DATA;
PyObject *const_str_plain_CHARACTERISTIC_CONFIGURATION;
PyObject *const_str_plain_CHARACTERISTIC_HRDW_REVISION;
PyObject *const_str_digest_1ba84b494402f7fe2a3198d92061a521;
PyObject *const_str_digest_25731c733fd74e8333aa29126ce85686;
PyObject *const_str_digest_45e4dde2057b0bf276d6a84f4c917d27;
PyObject *const_str_digest_a55b3b0a258f9102b5da9e5c1e03be49;
PyObject *const_str_digest_adc474dd61fbd736d69c1bac5d9712e0;
PyObject *const_str_plain_CHARACTERISTIC_HEART_RATE_CONTROL;
PyObject *const_str_plain_CHARACTERISTIC_HEART_RATE_MEASURE;

#if defined(_WIN32) && defined(_NUITKA_EXE)
#include <Windows.h>
const unsigned char* constant_bin;
struct __initResourceConstants
{
    __initResourceConstants()
    {
        constant_bin = (const unsigned char*)LockResource(
            LoadResource(
                NULL,
                FindResource(NULL, MAKEINTRESOURCE(3), RT_RCDATA)
            )
        );
    }
} __initResourceConstants_static_initializer;
#else
extern "C" const unsigned char constant_bin[];
#endif

static void _createGlobalConstants( void )
{
    NUITKA_MAY_BE_UNUSED PyObject *exception_type, *exception_value;
    NUITKA_MAY_BE_UNUSED PyTracebackObject *exception_tb;

#ifdef _MSC_VER
    // Prevent unused warnings in case of simple programs, the attribute
    // NUITKA_MAY_BE_UNUSED doesn't work for MSVC.
    (void *)exception_type; (void *)exception_value; (void *)exception_tb;
#endif

    const_int_0 = PyLong_FromUnsignedLong( 0ul );
    const_float_0_5 = UNSTREAM_FLOAT( &constant_bin[ 5182 ] );
    const_int_pos_1 = PyLong_FromUnsignedLong( 1ul );
    const_int_pos_2 = PyLong_FromUnsignedLong( 2ul );
    const_int_pos_3 = PyLong_FromUnsignedLong( 3ul );
    const_int_pos_5 = PyLong_FromUnsignedLong( 5ul );
    const_int_pos_8 = PyLong_FromUnsignedLong( 8ul );
    const_str_empty = UNSTREAM_STRING( &constant_bin[ 0 ], 0, 0 );
    const_dict_empty = _PyDict_NewPresized( 0 );
    assert( PyDict_Size( const_dict_empty ) == 0 );
    const_int_pos_10 = PyLong_FromUnsignedLong( 10ul );
    const_int_pos_20 = PyLong_FromUnsignedLong( 20ul );
    const_bytes_chr_2 = UNSTREAM_BYTES( &constant_bin[ 1676 ], 1 );
    const_bytes_chr_3 = UNSTREAM_BYTES( &constant_bin[ 1888 ], 1 );
    const_bytes_empty = UNSTREAM_BYTES( &constant_bin[ 0 ], 0 );
    const_str_plain_b = UNSTREAM_STRING( &constant_bin[ 85 ], 1, 1 );
    const_str_plain_x = UNSTREAM_STRING( &constant_bin[ 617 ], 1, 1 );
    const_tuple_empty = PyTuple_New( 0 );
    const_str_plain_end = UNSTREAM_STRING( &constant_bin[ 741 ], 3, 1 );
    const_str_plain_int = UNSTREAM_STRING( &constant_bin[ 614 ], 3, 1 );
    const_str_plain_len = UNSTREAM_STRING( &constant_bin[ 3798 ], 3, 1 );
    const_str_plain_base = UNSTREAM_STRING( &constant_bin[ 1890 ], 4, 1 );
    const_str_plain_data = UNSTREAM_STRING( &constant_bin[ 193 ], 4, 1 );
    const_str_plain_file = UNSTREAM_STRING( &constant_bin[ 5190 ], 4, 1 );
    const_str_plain_info = UNSTREAM_STRING( &constant_bin[ 2646 ], 4, 1 );
    const_str_plain_iter = UNSTREAM_STRING( &constant_bin[ 5194 ], 4, 1 );
    const_str_plain_open = UNSTREAM_STRING( &constant_bin[ 461 ], 4, 1 );
    const_str_plain_read = UNSTREAM_STRING( &constant_bin[ 2 ], 4, 1 );
    const_str_plain_repr = UNSTREAM_STRING( &constant_bin[ 5198 ], 4, 1 );
    const_str_plain_self = UNSTREAM_STRING( &constant_bin[ 1307 ], 4, 1 );
    const_str_plain_send = UNSTREAM_STRING( &constant_bin[ 1200 ], 4, 1 );
    const_str_plain_site = UNSTREAM_STRING( &constant_bin[ 5202 ], 4, 1 );
    const_str_plain_time = UNSTREAM_STRING( &constant_bin[ 20 ], 4, 1 );
    const_str_plain_type = UNSTREAM_STRING( &constant_bin[ 36 ], 4, 1 );
    const_str_plain_HEART = UNSTREAM_STRING( &constant_bin[ 5206 ], 5, 1 );
    const_str_plain_UUIDS = UNSTREAM_STRING( &constant_bin[ 5211 ], 5, 1 );
    const_str_plain_bytes = UNSTREAM_STRING( &constant_bin[ 4197 ], 5, 1 );
    const_str_plain_close = UNSTREAM_STRING( &constant_bin[ 5216 ], 5, 1 );
    const_str_plain_debug = UNSTREAM_STRING( &constant_bin[ 3093 ], 5, 1 );
    const_str_plain_index = UNSTREAM_STRING( &constant_bin[ 5221 ], 5, 1 );
    const_str_plain_level = UNSTREAM_STRING( &constant_bin[ 479 ], 5, 1 );
    const_str_plain_print = UNSTREAM_STRING( &constant_bin[ 5226 ], 5, 1 );
    const_str_plain_range = UNSTREAM_STRING( &constant_bin[ 3449 ], 5, 1 );
    const_str_plain_sleep = UNSTREAM_STRING( &constant_bin[ 3036 ], 5, 1 );
    const_str_plain_steps = UNSTREAM_STRING( &constant_bin[ 3352 ], 5, 1 );
    const_str_plain_throw = UNSTREAM_STRING( &constant_bin[ 5231 ], 5, 1 );
    const_str_plain_tuple = UNSTREAM_STRING( &constant_bin[ 5236 ], 5, 1 );
    const_str_plain_types = UNSTREAM_STRING( &constant_bin[ 5241 ], 5, 1 );
    const_str_plain_write = UNSTREAM_STRING( &constant_bin[ 1079 ], 5, 1 );
    const_str_plain_append = UNSTREAM_STRING( &constant_bin[ 5246 ], 6, 1 );
    const_str_plain_device = UNSTREAM_STRING( &constant_bin[ 149 ], 6, 1 );
    const_str_plain_encode = UNSTREAM_STRING( &constant_bin[ 5252 ], 6, 1 );
    const_str_plain_format = UNSTREAM_STRING( &constant_bin[ 1084 ], 6, 1 );
    const_str_plain_object = UNSTREAM_STRING( &constant_bin[ 4356 ], 6, 1 );
    const_str_plain_serial = UNSTREAM_STRING( &constant_bin[ 1904 ], 6, 1 );
    const_str_plain_AUTH_OK = UNSTREAM_STRING( &constant_bin[ 5258 ], 7, 1 );
    const_str_plain_MESSAGE = UNSTREAM_STRING( &constant_bin[ 5265 ], 7, 1 );
    const_str_plain_MiBand2 = UNSTREAM_STRING( &constant_bin[ 1365 ], 7, 1 );
    const_str_plain___all__ = UNSTREAM_STRING( &constant_bin[ 4448 ], 7, 1 );
    const_str_plain___cmp__ = UNSTREAM_STRING( &constant_bin[ 5272 ], 7, 1 );
    const_str_plain___doc__ = UNSTREAM_STRING( &constant_bin[ 5279 ], 7, 1 );
    const_str_plain_compile = UNSTREAM_STRING( &constant_bin[ 5286 ], 7, 1 );
    const_str_plain_inspect = UNSTREAM_STRING( &constant_bin[ 5293 ], 7, 1 );
    const_str_plain_logging = UNSTREAM_STRING( &constant_bin[ 1005 ], 7, 1 );
    const_str_plain_message = UNSTREAM_STRING( &constant_bin[ 441 ], 7, 1 );
    const_str_plain_timeout = UNSTREAM_STRING( &constant_bin[ 5300 ], 7, 1 );
    const_str_plain___dict__ = UNSTREAM_STRING( &constant_bin[ 5307 ], 8, 1 );
    const_str_plain___exit__ = UNSTREAM_STRING( &constant_bin[ 5315 ], 8, 1 );
    const_str_plain___file__ = UNSTREAM_STRING( &constant_bin[ 5323 ], 8, 1 );
    const_str_plain___init__ = UNSTREAM_STRING( &constant_bin[ 403 ], 8, 1 );
    const_str_plain___iter__ = UNSTREAM_STRING( &constant_bin[ 5331 ], 8, 1 );
    const_str_plain___main__ = UNSTREAM_STRING( &constant_bin[ 5339 ], 8, 1 );
    const_str_plain___name__ = UNSTREAM_STRING( &constant_bin[ 5347 ], 8, 1 );
    const_str_plain___path__ = UNSTREAM_STRING( &constant_bin[ 5355 ], 8, 1 );
    const_str_plain_datetime = UNSTREAM_STRING( &constant_bin[ 2729 ], 8, 1 );
    const_str_plain_setLevel = UNSTREAM_STRING( &constant_bin[ 5363 ], 8, 1 );
    const_str_plain_RAW_ACCEL = UNSTREAM_STRING( &constant_bin[ 5371 ], 9, 1 );
    const_str_plain_RAW_HEART = UNSTREAM_STRING( &constant_bin[ 5380 ], 9, 1 );
    const_str_plain___class__ = UNSTREAM_STRING( &constant_bin[ 5389 ], 9, 1 );
    const_str_plain___enter__ = UNSTREAM_STRING( &constant_bin[ 5398 ], 9, 1 );
    const_str_plain_constants = UNSTREAM_STRING( &constant_bin[ 4815 ], 9, 1 );
    const_str_plain_getLogger = UNSTREAM_STRING( &constant_bin[ 5407 ], 9, 1 );
    const_str_plain_get_steps = UNSTREAM_STRING( &constant_bin[ 3890 ], 9, 1 );
    const_str_plain_metaclass = UNSTREAM_STRING( &constant_bin[ 5416 ], 9, 1 );
    const_str_plain___cached__ = UNSTREAM_STRING( &constant_bin[ 5425 ], 10, 1 );
    const_str_plain___import__ = UNSTREAM_STRING( &constant_bin[ 5435 ], 10, 1 );
    const_str_plain___loader__ = UNSTREAM_STRING( &constant_bin[ 5445 ], 10, 1 );
    const_str_plain___module__ = UNSTREAM_STRING( &constant_bin[ 5455 ], 10, 1 );
    const_str_plain_initialize = UNSTREAM_STRING( &constant_bin[ 1715 ], 10, 1 );
    const_str_plain_send_alert = UNSTREAM_STRING( &constant_bin[ 2713 ], 10, 1 );
    const_str_plain_ALERT_TYPES = UNSTREAM_STRING( &constant_bin[ 5465 ], 11, 1 );
    const_str_plain_AUTH_FAILED = UNSTREAM_STRING( &constant_bin[ 5476 ], 11, 1 );
    const_str_plain_AUTH_STATES = UNSTREAM_STRING( &constant_bin[ 5487 ], 11, 1 );
    const_str_plain_QUEUE_TYPES = UNSTREAM_STRING( &constant_bin[ 5498 ], 11, 1 );
    const_str_plain___package__ = UNSTREAM_STRING( &constant_bin[ 5509 ], 11, 1 );
    const_str_plain___prepare__ = UNSTREAM_STRING( &constant_bin[ 5520 ], 11, 1 );
    const_tuple_int_pos_3_tuple = PyTuple_New( 1 );
    PyTuple_SET_ITEM( const_tuple_int_pos_3_tuple, 0, const_int_pos_3 ); Py_INCREF( const_int_pos_3 );
    const_str_plain___builtins__ = UNSTREAM_STRING( &constant_bin[ 5531 ], 12, 1 );
    const_str_plain___internal__ = UNSTREAM_STRING( &constant_bin[ 5543 ], 12, 1 );
    const_str_plain___qualname__ = UNSTREAM_STRING( &constant_bin[ 5555 ], 12, 1 );
    const_str_plain_authenticate = UNSTREAM_STRING( &constant_bin[ 3547 ], 12, 1 );
    const_str_plain_SERVICE_ALERT = UNSTREAM_STRING( &constant_bin[ 4995 ], 13, 1 );
    const_str_plain___metaclass__ = UNSTREAM_STRING( &constant_bin[ 5567 ], 13, 1 );
    const_slice_int_pos_2_none_none = PySlice_New( const_int_pos_2, Py_None, Py_None );
    const_slice_int_pos_3_none_none = PySlice_New( const_int_pos_3, Py_None, Py_None );
    const_str_plain_DefaultDelegate = UNSTREAM_STRING( &constant_bin[ 3990 ], 15, 1 );
    const_str_plain_SERVICE_MIBAND1 = UNSTREAM_STRING( &constant_bin[ 5580 ], 15, 1 );
    const_str_plain_SERVICE_MIBAND2 = UNSTREAM_STRING( &constant_bin[ 5595 ], 15, 1 );
    const_slice_int_0_int_pos_2_none = PySlice_New( const_int_0, const_int_pos_2, Py_None );
    const_str_plain_REQUEST_RN_ERROR = UNSTREAM_STRING( &constant_bin[ 5610 ], 16, 1 );
    const_tuple_str_plain_self_tuple = PyTuple_New( 1 );
    PyTuple_SET_ITEM( const_tuple_str_plain_self_tuple, 0, const_str_plain_self ); Py_INCREF( const_str_plain_self );
    const_str_plain_CHARACTERISTIC_HZ = UNSTREAM_STRING( &constant_bin[ 5626 ], 17, 1 );
    const_str_plain_KEY_SENDING_FAILED = UNSTREAM_STRING( &constant_bin[ 5643 ], 18, 1 );
    const_str_plain_SERVICE_HEART_RATE = UNSTREAM_STRING( &constant_bin[ 5661 ], 18, 1 );
    const_str_plain_accel_raw_callback = UNSTREAM_STRING( &constant_bin[ 5679 ], 18, 1 );
    const_str_plain_heart_raw_callback = UNSTREAM_STRING( &constant_bin[ 5697 ], 18, 1 );
    const_str_plain_CHARACTERISTIC_AUTH = UNSTREAM_STRING( &constant_bin[ 5715 ], 19, 1 );
    const_str_plain_SERVICE_DEVICE_INFO = UNSTREAM_STRING( &constant_bin[ 5734 ], 19, 1 );
    const_str_plain_CHARACTERISTIC_ALERT = UNSTREAM_STRING( &constant_bin[ 5753 ], 20, 1 );
    const_str_plain_CHARACTERISTIC_FETCH = UNSTREAM_STRING( &constant_bin[ 5773 ], 20, 1 );
    const_str_plain_CHARACTERISTIC_STEPS = UNSTREAM_STRING( &constant_bin[ 5793 ], 20, 1 );
    const_str_plain_CHARACTERISTIC_SENSOR = UNSTREAM_STRING( &constant_bin[ 5813 ], 21, 1 );
    const_str_plain_CHARACTERISTIC_SERIAL = UNSTREAM_STRING( &constant_bin[ 5834 ], 21, 1 );
    const_str_plain_ENCRIPTION_KEY_FAILED = UNSTREAM_STRING( &constant_bin[ 5855 ], 21, 1 );
    const_str_plain_CHARACTERISTIC_BATTERY = UNSTREAM_STRING( &constant_bin[ 5876 ], 22, 1 );
    const_str_plain_heart_measure_callback = UNSTREAM_STRING( &constant_bin[ 5898 ], 22, 1 );
    const_str_plain_CHARACTERISTIC_REVISION = UNSTREAM_STRING( &constant_bin[ 5920 ], 23, 1 );
    const_str_plain_NOTIFICATION_DESCRIPTOR = UNSTREAM_STRING( &constant_bin[ 5943 ], 23, 1 );
    const_str_plain_start_raw_data_realtime = UNSTREAM_STRING( &constant_bin[ 3759 ], 23, 1 );
    const_str_plain_CHARACTERISTIC_CURRENT_TIME = UNSTREAM_STRING( &constant_bin[ 5966 ], 27, 1 );
    const_str_plain_CHARACTERISTIC_ACTIVITY_DATA = UNSTREAM_STRING( &constant_bin[ 5993 ], 28, 1 );
    const_str_plain_CHARACTERISTIC_CONFIGURATION = UNSTREAM_STRING( &constant_bin[ 6021 ], 28, 1 );
    const_str_plain_CHARACTERISTIC_HRDW_REVISION = UNSTREAM_STRING( &constant_bin[ 6049 ], 28, 1 );
    const_str_digest_1ba84b494402f7fe2a3198d92061a521 = UNSTREAM_STRING( &constant_bin[ 6077 ], 18, 0 );
    const_str_digest_25731c733fd74e8333aa29126ce85686 = UNSTREAM_STRING( &constant_bin[ 1421 ], 2, 0 );
    const_str_digest_45e4dde2057b0bf276d6a84f4c917d27 = UNSTREAM_STRING( &constant_bin[ 4355 ], 7, 0 );
    const_str_digest_a55b3b0a258f9102b5da9e5c1e03be49 = UNSTREAM_STRING( &constant_bin[ 6095 ], 11, 0 );
    const_str_digest_adc474dd61fbd736d69c1bac5d9712e0 = UNSTREAM_STRING( &constant_bin[ 6106 ], 47, 0 );
    const_str_plain_CHARACTERISTIC_HEART_RATE_CONTROL = UNSTREAM_STRING( &constant_bin[ 6153 ], 33, 1 );
    const_str_plain_CHARACTERISTIC_HEART_RATE_MEASURE = UNSTREAM_STRING( &constant_bin[ 6186 ], 33, 1 );

#if _NUITKA_EXE
    /* Set the "sys.executable" path to the original CPython executable. */
    PySys_SetObject(
        (char *)"executable",
        const_str_digest_1ba84b494402f7fe2a3198d92061a521
    );
#endif
}

// In debug mode we can check that the constants were not tampered with in any
// given moment. We typically do it at program exit, but we can add extra calls
// for sanity.
#ifndef __NUITKA_NO_ASSERT__
void checkGlobalConstants( void )
{

}
#endif


void createGlobalConstants( void )
{
    if ( _sentinel_value == NULL )
    {
#if PYTHON_VERSION < 300
        _sentinel_value = PyCObject_FromVoidPtr( NULL, NULL );
#else
        // The NULL value is not allowed for a capsule, so use something else.
        _sentinel_value = PyCapsule_New( (void *)27, "sentinel", NULL );
#endif
        assert( _sentinel_value );

        _createGlobalConstants();
    }
}
