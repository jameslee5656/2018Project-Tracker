// This file contains helper functions that are automatically created from
// templates.

#include "nuitka/prelude.hpp"

extern PyObject *callPythonFunction( PyObject *func, PyObject **args, int count );


PyObject *CALL_FUNCTION_WITH_ARGS1( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 1; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        Nuitka_FunctionObject *function = (Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 1 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 1; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 1 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 1 * sizeof(PyObject *) );
            memcpy( python_pars + 1, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 1 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        Nuitka_MethodObject *method = (Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 1 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 1; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 1 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 1 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 1, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 1 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called );

        if ( flags & METH_NOARGS )
        {
#if 1 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (1 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 1 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (1 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 1 );

            PyObject *result;

            assert( flags && METH_VARARGS );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            if ( flags && METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            1
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 1 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS2( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 2; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        Nuitka_FunctionObject *function = (Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 2 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 2; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 2 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 2 * sizeof(PyObject *) );
            memcpy( python_pars + 2, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 2 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        Nuitka_MethodObject *method = (Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 2 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 2; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 2 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 2 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 2, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 2 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called );

        if ( flags & METH_NOARGS )
        {
#if 2 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (2 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 2 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (2 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 2 );

            PyObject *result;

            assert( flags && METH_VARARGS );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            if ( flags && METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            2
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 2 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS3( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 3; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        Nuitka_FunctionObject *function = (Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 3 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 3; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 3 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 3 * sizeof(PyObject *) );
            memcpy( python_pars + 3, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 3 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        Nuitka_MethodObject *method = (Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 3 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 3; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 3 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 3 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 3, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 3 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called );

        if ( flags & METH_NOARGS )
        {
#if 3 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (3 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 3 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (3 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 3 );

            PyObject *result;

            assert( flags && METH_VARARGS );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            if ( flags && METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            3
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 3 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS4( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 4; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        Nuitka_FunctionObject *function = (Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 4 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 4; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 4 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 4 * sizeof(PyObject *) );
            memcpy( python_pars + 4, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 4 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        Nuitka_MethodObject *method = (Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 4 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 4; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 4 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 4 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 4, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 4 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called );

        if ( flags & METH_NOARGS )
        {
#if 4 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (4 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 4 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (4 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 4 );

            PyObject *result;

            assert( flags && METH_VARARGS );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            if ( flags && METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            4
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 4 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS5( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 5; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        Nuitka_FunctionObject *function = (Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 5 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 5; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 5 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 5 * sizeof(PyObject *) );
            memcpy( python_pars + 5, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 5 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        Nuitka_MethodObject *method = (Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 5 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 5; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 5 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 5 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 5, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 5 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called );

        if ( flags & METH_NOARGS )
        {
#if 5 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (5 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 5 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (5 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 5 );

            PyObject *result;

            assert( flags && METH_VARARGS );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            if ( flags && METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            5
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 5 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}
/* Code to register embedded modules for meta path based loading if any. */

#include "nuitka/unfreezing.hpp"

/* Table for lookup to find compiled or bytecode modules included in this
 * binary or module, or put along this binary as extension modules. We do
 * our own loading for each of these.
 */
MOD_INIT_DECL( __main__ );
MOD_INIT_DECL( base );
MOD_INIT_DECL( constants );
static struct Nuitka_MetaPathBasedLoaderEntry meta_path_loader_entries[] =
{
    { (char *)"__main__", MOD_INIT_NAME( __main__ ), NULL, 0, NUITKA_COMPILED_MODULE },
    { (char *)"_bz2", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_codecs_cn", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_codecs_hk", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_codecs_iso2022", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_codecs_jp", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_codecs_kr", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_codecs_tw", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_crypt", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_csv", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_ctypes", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_curses", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_curses_panel", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_dbm", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_decimal", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_gdbm", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_hashlib", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_json", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_lsprof", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_lzma", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_multibytecodec", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_multiprocessing", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_opcode", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_sqlite3", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_ssl", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_tkinter", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"audioop", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"base", MOD_INIT_NAME( base ), NULL, 0, NUITKA_COMPILED_MODULE },
    { (char *)"constants", MOD_INIT_NAME( constants ), NULL, 0, NUITKA_COMPILED_MODULE },
    { (char *)"mmap", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"readline", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"termios", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"__future__", NULL, &constant_bin[ 6219 ], 4292, NUITKA_BYTECODE_FLAG },
    { (char *)"_dummy_thread", NULL, &constant_bin[ 10511 ], 5034, NUITKA_BYTECODE_FLAG },
    { (char *)"_markupbase", NULL, &constant_bin[ 15545 ], 8868, NUITKA_BYTECODE_FLAG },
    { (char *)"_osx_support", NULL, &constant_bin[ 24413 ], 10458, NUITKA_BYTECODE_FLAG },
    { (char *)"_pyio", NULL, &constant_bin[ 34871 ], 75982, NUITKA_BYTECODE_FLAG },
    { (char *)"_sitebuiltins", NULL, &constant_bin[ 110853 ], 3642, NUITKA_BYTECODE_FLAG },
    { (char *)"_strptime", NULL, &constant_bin[ 114495 ], 15766, NUITKA_BYTECODE_FLAG },
    { (char *)"_sysconfigdata", NULL, &constant_bin[ 130261 ], 239, NUITKA_BYTECODE_FLAG },
    { (char *)"_sysconfigdata_m", NULL, &constant_bin[ 130500 ], 21068, NUITKA_BYTECODE_FLAG },
    { (char *)"_threading_local", NULL, &constant_bin[ 151568 ], 6908, NUITKA_BYTECODE_FLAG },
    { (char *)"aifc", NULL, &constant_bin[ 158476 ], 27674, NUITKA_BYTECODE_FLAG },
    { (char *)"argparse", NULL, &constant_bin[ 186150 ], 65770, NUITKA_BYTECODE_FLAG },
    { (char *)"asynchat", NULL, &constant_bin[ 251920 ], 8453, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio", NULL, &constant_bin[ 260373 ], 840, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"asyncio.base_events", NULL, &constant_bin[ 261213 ], 38358, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.base_subprocess", NULL, &constant_bin[ 299571 ], 9760, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.compat", NULL, &constant_bin[ 309331 ], 767, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.constants", NULL, &constant_bin[ 310098 ], 215, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.coroutines", NULL, &constant_bin[ 310313 ], 8655, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.events", NULL, &constant_bin[ 318968 ], 24411, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.futures", NULL, &constant_bin[ 343379 ], 15728, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.locks", NULL, &constant_bin[ 359107 ], 15636, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.log", NULL, &constant_bin[ 374743 ], 217, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.proactor_events", NULL, &constant_bin[ 374960 ], 17646, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.protocols", NULL, &constant_bin[ 392606 ], 6064, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.queues", NULL, &constant_bin[ 398670 ], 8840, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.selector_events", NULL, &constant_bin[ 407510 ], 30570, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.sslproto", NULL, &constant_bin[ 438080 ], 21186, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.streams", NULL, &constant_bin[ 459266 ], 20794, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.subprocess", NULL, &constant_bin[ 480060 ], 7083, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.tasks", NULL, &constant_bin[ 487143 ], 21078, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.test_utils", NULL, &constant_bin[ 508221 ], 16283, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.transports", NULL, &constant_bin[ 524504 ], 12110, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.unix_events", NULL, &constant_bin[ 536614 ], 31327, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncore", NULL, &constant_bin[ 567941 ], 17145, NUITKA_BYTECODE_FLAG },
    { (char *)"bdb", NULL, &constant_bin[ 585086 ], 18532, NUITKA_BYTECODE_FLAG },
    { (char *)"binhex", NULL, &constant_bin[ 603618 ], 13400, NUITKA_BYTECODE_FLAG },
    { (char *)"bisect", NULL, &constant_bin[ 617018 ], 2807, NUITKA_BYTECODE_FLAG },
    { (char *)"cProfile", NULL, &constant_bin[ 619825 ], 4579, NUITKA_BYTECODE_FLAG },
    { (char *)"calendar", NULL, &constant_bin[ 624404 ], 27500, NUITKA_BYTECODE_FLAG },
    { (char *)"cgi", NULL, &constant_bin[ 651904 ], 29838, NUITKA_BYTECODE_FLAG },
    { (char *)"cgitb", NULL, &constant_bin[ 681742 ], 10983, NUITKA_BYTECODE_FLAG },
    { (char *)"chunk", NULL, &constant_bin[ 692725 ], 5192, NUITKA_BYTECODE_FLAG },
    { (char *)"cmd", NULL, &constant_bin[ 697917 ], 13381, NUITKA_BYTECODE_FLAG },
    { (char *)"code", NULL, &constant_bin[ 711298 ], 9799, NUITKA_BYTECODE_FLAG },
    { (char *)"codeop", NULL, &constant_bin[ 721097 ], 6427, NUITKA_BYTECODE_FLAG },
    { (char *)"colorsys", NULL, &constant_bin[ 727524 ], 3614, NUITKA_BYTECODE_FLAG },
    { (char *)"compileall", NULL, &constant_bin[ 731138 ], 8710, NUITKA_BYTECODE_FLAG },
    { (char *)"concurrent", NULL, &constant_bin[ 739848 ], 112, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"concurrent.futures", NULL, &constant_bin[ 739960 ], 660, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"concurrent.futures._base", NULL, &constant_bin[ 740620 ], 20888, NUITKA_BYTECODE_FLAG },
    { (char *)"concurrent.futures.process", NULL, &constant_bin[ 761508 ], 16321, NUITKA_BYTECODE_FLAG },
    { (char *)"concurrent.futures.thread", NULL, &constant_bin[ 777829 ], 3886, NUITKA_BYTECODE_FLAG },
    { (char *)"configparser", NULL, &constant_bin[ 781715 ], 48145, NUITKA_BYTECODE_FLAG },
    { (char *)"contextlib", NULL, &constant_bin[ 829860 ], 10888, NUITKA_BYTECODE_FLAG },
    { (char *)"copy", NULL, &constant_bin[ 840748 ], 8088, NUITKA_BYTECODE_FLAG },
    { (char *)"crypt", NULL, &constant_bin[ 848836 ], 2401, NUITKA_BYTECODE_FLAG },
    { (char *)"csv", NULL, &constant_bin[ 851237 ], 12901, NUITKA_BYTECODE_FLAG },
    { (char *)"ctypes", NULL, &constant_bin[ 864138 ], 17492, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"ctypes._endian", NULL, &constant_bin[ 881630 ], 2040, NUITKA_BYTECODE_FLAG },
    { (char *)"ctypes.util", NULL, &constant_bin[ 883670 ], 7157, NUITKA_BYTECODE_FLAG },
    { (char *)"curses", NULL, &constant_bin[ 890827 ], 1898, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"curses.ascii", NULL, &constant_bin[ 892725 ], 4196, NUITKA_BYTECODE_FLAG },
    { (char *)"curses.has_key", NULL, &constant_bin[ 896921 ], 4747, NUITKA_BYTECODE_FLAG },
    { (char *)"curses.panel", NULL, &constant_bin[ 901668 ], 207, NUITKA_BYTECODE_FLAG },
    { (char *)"curses.textpad", NULL, &constant_bin[ 901875 ], 6254, NUITKA_BYTECODE_FLAG },
    { (char *)"datetime", NULL, &constant_bin[ 908129 ], 55476, NUITKA_BYTECODE_FLAG },
    { (char *)"dbm", NULL, &constant_bin[ 963605 ], 4409, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"dbm.dumb", NULL, &constant_bin[ 968014 ], 8058, NUITKA_BYTECODE_FLAG },
    { (char *)"dbm.gnu", NULL, &constant_bin[ 976072 ], 325, NUITKA_BYTECODE_FLAG },
    { (char *)"dbm.ndbm", NULL, &constant_bin[ 976397 ], 186, NUITKA_BYTECODE_FLAG },
    { (char *)"decimal", NULL, &constant_bin[ 976583 ], 171256, NUITKA_BYTECODE_FLAG },
    { (char *)"difflib", NULL, &constant_bin[ 1147839 ], 62246, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils", NULL, &constant_bin[ 1210085 ], 379, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"distutils.archive_util", NULL, &constant_bin[ 1210464 ], 6784, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.bcppcompiler", NULL, &constant_bin[ 1217248 ], 7218, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.ccompiler", NULL, &constant_bin[ 1224466 ], 34780, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.cmd", NULL, &constant_bin[ 1259246 ], 15608, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command", NULL, &constant_bin[ 1274854 ], 544, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"distutils.command.bdist", NULL, &constant_bin[ 1275398 ], 4021, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.bdist_dumb", NULL, &constant_bin[ 1279419 ], 3941, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.bdist_rpm", NULL, &constant_bin[ 1283360 ], 14189, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.bdist_wininst", NULL, &constant_bin[ 1297549 ], 9138, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build", NULL, &constant_bin[ 1306687 ], 4259, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build_clib", NULL, &constant_bin[ 1310946 ], 5379, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build_ext", NULL, &constant_bin[ 1316325 ], 18080, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build_py", NULL, &constant_bin[ 1334405 ], 11433, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build_scripts", NULL, &constant_bin[ 1345838 ], 4688, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.check", NULL, &constant_bin[ 1350526 ], 5214, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.clean", NULL, &constant_bin[ 1355740 ], 2340, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.config", NULL, &constant_bin[ 1358080 ], 10986, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install", NULL, &constant_bin[ 1369066 ], 16155, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_data", NULL, &constant_bin[ 1385221 ], 2517, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_egg_info", NULL, &constant_bin[ 1387738 ], 3685, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_headers", NULL, &constant_bin[ 1391423 ], 1824, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_lib", NULL, &constant_bin[ 1393247 ], 5762, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_scripts", NULL, &constant_bin[ 1399009 ], 2349, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.register", NULL, &constant_bin[ 1401358 ], 9173, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.sdist", NULL, &constant_bin[ 1410531 ], 14097, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.upload", NULL, &constant_bin[ 1424628 ], 5656, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.config", NULL, &constant_bin[ 1430284 ], 3777, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.core", NULL, &constant_bin[ 1434061 ], 7083, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.cygwinccompiler", NULL, &constant_bin[ 1441144 ], 9186, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.debug", NULL, &constant_bin[ 1450330 ], 184, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.dep_util", NULL, &constant_bin[ 1450514 ], 2826, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.dir_util", NULL, &constant_bin[ 1453340 ], 6593, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.dist", NULL, &constant_bin[ 1459933 ], 36672, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.errors", NULL, &constant_bin[ 1496605 ], 5690, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.extension", NULL, &constant_bin[ 1502295 ], 7377, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.fancy_getopt", NULL, &constant_bin[ 1509672 ], 11464, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.file_util", NULL, &constant_bin[ 1521136 ], 6269, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.filelist", NULL, &constant_bin[ 1527405 ], 10081, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.log", NULL, &constant_bin[ 1537486 ], 2451, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.msvccompiler", NULL, &constant_bin[ 1539937 ], 15914, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.spawn", NULL, &constant_bin[ 1555851 ], 5340, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.sysconfig", NULL, &constant_bin[ 1561191 ], 13570, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.text_file", NULL, &constant_bin[ 1574761 ], 8865, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.unixccompiler", NULL, &constant_bin[ 1583626 ], 7678, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.util", NULL, &constant_bin[ 1591304 ], 16500, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.version", NULL, &constant_bin[ 1607804 ], 7746, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.versionpredicate", NULL, &constant_bin[ 1615550 ], 5342, NUITKA_BYTECODE_FLAG },
    { (char *)"doctest", NULL, &constant_bin[ 1620892 ], 79710, NUITKA_BYTECODE_FLAG },
    { (char *)"dummy_threading", NULL, &constant_bin[ 1700602 ], 1172, NUITKA_BYTECODE_FLAG },
    { (char *)"email", NULL, &constant_bin[ 1701774 ], 1734, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"email._encoded_words", NULL, &constant_bin[ 1703508 ], 6019, NUITKA_BYTECODE_FLAG },
    { (char *)"email._header_value_parser", NULL, &constant_bin[ 1709527 ], 87237, NUITKA_BYTECODE_FLAG },
    { (char *)"email._parseaddr", NULL, &constant_bin[ 1796764 ], 13711, NUITKA_BYTECODE_FLAG },
    { (char *)"email._policybase", NULL, &constant_bin[ 1810475 ], 15122, NUITKA_BYTECODE_FLAG },
    { (char *)"email.base64mime", NULL, &constant_bin[ 1825597 ], 3342, NUITKA_BYTECODE_FLAG },
    { (char *)"email.charset", NULL, &constant_bin[ 1828939 ], 12029, NUITKA_BYTECODE_FLAG },
    { (char *)"email.contentmanager", NULL, &constant_bin[ 1840968 ], 8080, NUITKA_BYTECODE_FLAG },
    { (char *)"email.encoders", NULL, &constant_bin[ 1849048 ], 1717, NUITKA_BYTECODE_FLAG },
    { (char *)"email.errors", NULL, &constant_bin[ 1850765 ], 6248, NUITKA_BYTECODE_FLAG },
    { (char *)"email.feedparser", NULL, &constant_bin[ 1857013 ], 11723, NUITKA_BYTECODE_FLAG },
    { (char *)"email.generator", NULL, &constant_bin[ 1868736 ], 13570, NUITKA_BYTECODE_FLAG },
    { (char *)"email.header", NULL, &constant_bin[ 1882306 ], 17656, NUITKA_BYTECODE_FLAG },
    { (char *)"email.headerregistry", NULL, &constant_bin[ 1899962 ], 22590, NUITKA_BYTECODE_FLAG },
    { (char *)"email.iterators", NULL, &constant_bin[ 1922552 ], 2001, NUITKA_BYTECODE_FLAG },
    { (char *)"email.message", NULL, &constant_bin[ 1924553 ], 39241, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime", NULL, &constant_bin[ 1963794 ], 112, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"email.mime.application", NULL, &constant_bin[ 1963906 ], 1434, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.audio", NULL, &constant_bin[ 1965340 ], 2651, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.base", NULL, &constant_bin[ 1967991 ], 991, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.image", NULL, &constant_bin[ 1968982 ], 1893, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.message", NULL, &constant_bin[ 1970875 ], 1311, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.multipart", NULL, &constant_bin[ 1972186 ], 1526, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.nonmultipart", NULL, &constant_bin[ 1973712 ], 768, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.text", NULL, &constant_bin[ 1974480 ], 1336, NUITKA_BYTECODE_FLAG },
    { (char *)"email.parser", NULL, &constant_bin[ 1975816 ], 5883, NUITKA_BYTECODE_FLAG },
    { (char *)"email.policy", NULL, &constant_bin[ 1981699 ], 9740, NUITKA_BYTECODE_FLAG },
    { (char *)"email.quoprimime", NULL, &constant_bin[ 1991439 ], 8082, NUITKA_BYTECODE_FLAG },
    { (char *)"email.utils", NULL, &constant_bin[ 1999521 ], 10431, NUITKA_BYTECODE_FLAG },
    { (char *)"filecmp", NULL, &constant_bin[ 2009952 ], 9059, NUITKA_BYTECODE_FLAG },
    { (char *)"fileinput", NULL, &constant_bin[ 2019011 ], 13808, NUITKA_BYTECODE_FLAG },
    { (char *)"fnmatch", NULL, &constant_bin[ 2032819 ], 3104, NUITKA_BYTECODE_FLAG },
    { (char *)"formatter", NULL, &constant_bin[ 2035923 ], 18784, NUITKA_BYTECODE_FLAG },
    { (char *)"fractions", NULL, &constant_bin[ 2054707 ], 19984, NUITKA_BYTECODE_FLAG },
    { (char *)"ftplib", NULL, &constant_bin[ 2074691 ], 30064, NUITKA_BYTECODE_FLAG },
    { (char *)"getopt", NULL, &constant_bin[ 2104755 ], 6673, NUITKA_BYTECODE_FLAG },
    { (char *)"getpass", NULL, &constant_bin[ 2111428 ], 4532, NUITKA_BYTECODE_FLAG },
    { (char *)"gettext", NULL, &constant_bin[ 2115960 ], 13147, NUITKA_BYTECODE_FLAG },
    { (char *)"glob", NULL, &constant_bin[ 2129107 ], 4175, NUITKA_BYTECODE_FLAG },
    { (char *)"gzip", NULL, &constant_bin[ 2133282 ], 17553, NUITKA_BYTECODE_FLAG },
    { (char *)"hashlib", NULL, &constant_bin[ 2150835 ], 6331, NUITKA_BYTECODE_FLAG },
    { (char *)"hmac", NULL, &constant_bin[ 2157166 ], 5104, NUITKA_BYTECODE_FLAG },
    { (char *)"html", NULL, &constant_bin[ 2162270 ], 3605, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"html.entities", NULL, &constant_bin[ 2165875 ], 55413, NUITKA_BYTECODE_FLAG },
    { (char *)"html.parser", NULL, &constant_bin[ 2221288 ], 12208, NUITKA_BYTECODE_FLAG },
    { (char *)"http", NULL, &constant_bin[ 2233496 ], 6707, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"http.client", NULL, &constant_bin[ 2240203 ], 33627, NUITKA_BYTECODE_FLAG },
    { (char *)"http.cookiejar", NULL, &constant_bin[ 2273830 ], 58313, NUITKA_BYTECODE_FLAG },
    { (char *)"http.cookies", NULL, &constant_bin[ 2332143 ], 17201, NUITKA_BYTECODE_FLAG },
    { (char *)"http.server", NULL, &constant_bin[ 2349344 ], 34540, NUITKA_BYTECODE_FLAG },
    { (char *)"imaplib", NULL, &constant_bin[ 2383884 ], 44690, NUITKA_BYTECODE_FLAG },
    { (char *)"imghdr", NULL, &constant_bin[ 2428574 ], 4471, NUITKA_BYTECODE_FLAG },
    { (char *)"imp", NULL, &constant_bin[ 2433045 ], 10428, NUITKA_BYTECODE_FLAG },
    { (char *)"importlib.abc", NULL, &constant_bin[ 2443473 ], 11727, NUITKA_BYTECODE_FLAG },
    { (char *)"importlib.util", NULL, &constant_bin[ 2455200 ], 9613, NUITKA_BYTECODE_FLAG },
    { (char *)"ipaddress", NULL, &constant_bin[ 2464813 ], 66508, NUITKA_BYTECODE_FLAG },
    { (char *)"json", NULL, &constant_bin[ 2531321 ], 12227, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"json.decoder", NULL, &constant_bin[ 2543548 ], 10647, NUITKA_BYTECODE_FLAG },
    { (char *)"json.encoder", NULL, &constant_bin[ 2554195 ], 11970, NUITKA_BYTECODE_FLAG },
    { (char *)"json.scanner", NULL, &constant_bin[ 2566165 ], 2185, NUITKA_BYTECODE_FLAG },
    { (char *)"json.tool", NULL, &constant_bin[ 2568350 ], 1655, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3", NULL, &constant_bin[ 2570005 ], 109, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"lib2to3.btm_matcher", NULL, &constant_bin[ 2570114 ], 5289, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.btm_utils", NULL, &constant_bin[ 2575403 ], 6759, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixer_base", NULL, &constant_bin[ 2582162 ], 6499, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixer_util", NULL, &constant_bin[ 2588661 ], 13325, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes", NULL, &constant_bin[ 2601986 ], 115, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"lib2to3.fixes.fix_apply", NULL, &constant_bin[ 2602101 ], 1658, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_asserts", NULL, &constant_bin[ 2603759 ], 1335, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_basestring", NULL, &constant_bin[ 2605094 ], 660, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_buffer", NULL, &constant_bin[ 2605754 ], 812, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_callable", NULL, &constant_bin[ 2606566 ], 1317, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_dict", NULL, &constant_bin[ 2607883 ], 3641, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_except", NULL, &constant_bin[ 2611524 ], 3012, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_exec", NULL, &constant_bin[ 2614536 ], 1237, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_execfile", NULL, &constant_bin[ 2615773 ], 1817, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_exitfunc", NULL, &constant_bin[ 2617590 ], 2432, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_filter", NULL, &constant_bin[ 2620022 ], 2067, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_funcattrs", NULL, &constant_bin[ 2622089 ], 979, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_future", NULL, &constant_bin[ 2623068 ], 787, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_getcwdu", NULL, &constant_bin[ 2623855 ], 790, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_has_key", NULL, &constant_bin[ 2624645 ], 3139, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_idioms", NULL, &constant_bin[ 2627784 ], 4119, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_import", NULL, &constant_bin[ 2631903 ], 2985, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_imports", NULL, &constant_bin[ 2634888 ], 4695, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_imports2", NULL, &constant_bin[ 2639583 ], 540, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_input", NULL, &constant_bin[ 2640123 ], 976, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_intern", NULL, &constant_bin[ 2641099 ], 1022, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_isinstance", NULL, &constant_bin[ 2642121 ], 1647, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_itertools", NULL, &constant_bin[ 2643768 ], 1611, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_itertools_imports", NULL, &constant_bin[ 2645379 ], 1757, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_long", NULL, &constant_bin[ 2647136 ], 708, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_map", NULL, &constant_bin[ 2647844 ], 2813, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_metaclass", NULL, &constant_bin[ 2650657 ], 5920, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_methodattrs", NULL, &constant_bin[ 2656577 ], 951, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_ne", NULL, &constant_bin[ 2657528 ], 819, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_next", NULL, &constant_bin[ 2658347 ], 3274, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_nonzero", NULL, &constant_bin[ 2661621 ], 944, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_numliterals", NULL, &constant_bin[ 2662565 ], 1075, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_operator", NULL, &constant_bin[ 2663640 ], 4458, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_paren", NULL, &constant_bin[ 2668098 ], 1406, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_print", NULL, &constant_bin[ 2669504 ], 2554, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_raise", NULL, &constant_bin[ 2672058 ], 2409, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_raw_input", NULL, &constant_bin[ 2674467 ], 797, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_reduce", NULL, &constant_bin[ 2675264 ], 1130, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_reload", NULL, &constant_bin[ 2676394 ], 1022, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_renames", NULL, &constant_bin[ 2677416 ], 2084, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_repr", NULL, &constant_bin[ 2679500 ], 865, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_set_literal", NULL, &constant_bin[ 2680365 ], 1773, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_standarderror", NULL, &constant_bin[ 2682138 ], 717, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_sys_exc", NULL, &constant_bin[ 2682855 ], 1463, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_throw", NULL, &constant_bin[ 2684318 ], 1913, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_tuple_params", NULL, &constant_bin[ 2686231 ], 5024, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_types", NULL, &constant_bin[ 2691255 ], 1961, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_unicode", NULL, &constant_bin[ 2693216 ], 1621, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_urllib", NULL, &constant_bin[ 2694837 ], 6483, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_ws_comma", NULL, &constant_bin[ 2701320 ], 1183, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_xrange", NULL, &constant_bin[ 2702503 ], 2686, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_xreadlines", NULL, &constant_bin[ 2705189 ], 1139, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_zip", NULL, &constant_bin[ 2706328 ], 1193, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.main", NULL, &constant_bin[ 2707521 ], 9121, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.patcomp", NULL, &constant_bin[ 2716642 ], 6305, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2", NULL, &constant_bin[ 2722947 ], 147, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"lib2to3.pgen2.driver", NULL, &constant_bin[ 2723094 ], 4436, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.grammar", NULL, &constant_bin[ 2727530 ], 5674, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.literals", NULL, &constant_bin[ 2733204 ], 1718, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.parse", NULL, &constant_bin[ 2734922 ], 6683, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.pgen", NULL, &constant_bin[ 2741605 ], 10984, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.token", NULL, &constant_bin[ 2752589 ], 1998, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.tokenize", NULL, &constant_bin[ 2754587 ], 15933, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pygram", NULL, &constant_bin[ 2770520 ], 1232, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pytree", NULL, &constant_bin[ 2771752 ], 26842, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.refactor", NULL, &constant_bin[ 2798594 ], 22776, NUITKA_BYTECODE_FLAG },
    { (char *)"logging", NULL, &constant_bin[ 2821370 ], 61374, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"logging.config", NULL, &constant_bin[ 2882744 ], 25386, NUITKA_BYTECODE_FLAG },
    { (char *)"logging.handlers", NULL, &constant_bin[ 2908130 ], 45060, NUITKA_BYTECODE_FLAG },
    { (char *)"lzma", NULL, &constant_bin[ 2953190 ], 12449, NUITKA_BYTECODE_FLAG },
    { (char *)"macpath", NULL, &constant_bin[ 2965639 ], 6116, NUITKA_BYTECODE_FLAG },
    { (char *)"macurl2path", NULL, &constant_bin[ 2971755 ], 2057, NUITKA_BYTECODE_FLAG },
    { (char *)"mailbox", NULL, &constant_bin[ 2973812 ], 69770, NUITKA_BYTECODE_FLAG },
    { (char *)"mailcap", NULL, &constant_bin[ 3043582 ], 6478, NUITKA_BYTECODE_FLAG },
    { (char *)"mimetypes", NULL, &constant_bin[ 3050060 ], 16620, NUITKA_BYTECODE_FLAG },
    { (char *)"modulefinder", NULL, &constant_bin[ 3066680 ], 17233, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing", NULL, &constant_bin[ 3083913 ], 536, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"multiprocessing.connection", NULL, &constant_bin[ 3084449 ], 27186, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.context", NULL, &constant_bin[ 3111635 ], 13485, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.dummy", NULL, &constant_bin[ 3125120 ], 4020, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"multiprocessing.dummy.connection", NULL, &constant_bin[ 3129140 ], 2640, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.forkserver", NULL, &constant_bin[ 3131780 ], 6963, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.heap", NULL, &constant_bin[ 3138743 ], 6740, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.managers", NULL, &constant_bin[ 3145483 ], 35484, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.pool", NULL, &constant_bin[ 3180967 ], 22824, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.popen_fork", NULL, &constant_bin[ 3203791 ], 2354, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.popen_forkserver", NULL, &constant_bin[ 3206145 ], 2517, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.popen_spawn_posix", NULL, &constant_bin[ 3208662 ], 2288, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.process", NULL, &constant_bin[ 3210950 ], 8765, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.queues", NULL, &constant_bin[ 3219715 ], 9851, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.reduction", NULL, &constant_bin[ 3229566 ], 7738, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.resource_sharer", NULL, &constant_bin[ 3237304 ], 5607, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.semaphore_tracker", NULL, &constant_bin[ 3242911 ], 3695, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.sharedctypes", NULL, &constant_bin[ 3246606 ], 7461, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.spawn", NULL, &constant_bin[ 3254067 ], 7011, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.synchronize", NULL, &constant_bin[ 3261078 ], 12278, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.util", NULL, &constant_bin[ 3273356 ], 10319, NUITKA_BYTECODE_FLAG },
    { (char *)"netrc", NULL, &constant_bin[ 3283675 ], 4227, NUITKA_BYTECODE_FLAG },
    { (char *)"nntplib", NULL, &constant_bin[ 3287902 ], 36050, NUITKA_BYTECODE_FLAG },
    { (char *)"ntpath", NULL, &constant_bin[ 3323952 ], 14787, NUITKA_BYTECODE_FLAG },
    { (char *)"nturl2path", NULL, &constant_bin[ 3338739 ], 1668, NUITKA_BYTECODE_FLAG },
    { (char *)"numbers", NULL, &constant_bin[ 3340407 ], 12640, NUITKA_BYTECODE_FLAG },
    { (char *)"optparse", NULL, &constant_bin[ 3353047 ], 51299, NUITKA_BYTECODE_FLAG },
    { (char *)"pathlib", NULL, &constant_bin[ 3404346 ], 44166, NUITKA_BYTECODE_FLAG },
    { (char *)"pdb", NULL, &constant_bin[ 3448512 ], 49312, NUITKA_BYTECODE_FLAG },
    { (char *)"pickletools", NULL, &constant_bin[ 3497824 ], 70095, NUITKA_BYTECODE_FLAG },
    { (char *)"pipes", NULL, &constant_bin[ 3567919 ], 8329, NUITKA_BYTECODE_FLAG },
    { (char *)"pkgutil", NULL, &constant_bin[ 3576248 ], 17455, NUITKA_BYTECODE_FLAG },
    { (char *)"plat-arm-linux-gnueabihf.CDROM", NULL, &constant_bin[ 3593703 ], 6117, NUITKA_BYTECODE_FLAG },
    { (char *)"plat-arm-linux-gnueabihf.DLFCN", NULL, &constant_bin[ 3599820 ], 2237, NUITKA_BYTECODE_FLAG },
    { (char *)"plat-arm-linux-gnueabihf.IN", NULL, &constant_bin[ 3602057 ], 24844, NUITKA_BYTECODE_FLAG },
    { (char *)"plat-arm-linux-gnueabihf.TYPES", NULL, &constant_bin[ 3626901 ], 4843, NUITKA_BYTECODE_FLAG },
    { (char *)"plat-arm-linux-gnueabihf._sysconfigdata_m", NULL, &constant_bin[ 130500 ], 21068, NUITKA_BYTECODE_FLAG },
    { (char *)"platform", NULL, &constant_bin[ 3631744 ], 32271, NUITKA_BYTECODE_FLAG },
    { (char *)"plistlib", NULL, &constant_bin[ 3664015 ], 29793, NUITKA_BYTECODE_FLAG },
    { (char *)"poplib", NULL, &constant_bin[ 3693808 ], 13902, NUITKA_BYTECODE_FLAG },
    { (char *)"pprint", NULL, &constant_bin[ 3707710 ], 17452, NUITKA_BYTECODE_FLAG },
    { (char *)"profile", NULL, &constant_bin[ 3725162 ], 15058, NUITKA_BYTECODE_FLAG },
    { (char *)"pstats", NULL, &constant_bin[ 3740220 ], 23540, NUITKA_BYTECODE_FLAG },
    { (char *)"pty", NULL, &constant_bin[ 3763760 ], 4177, NUITKA_BYTECODE_FLAG },
    { (char *)"py_compile", NULL, &constant_bin[ 3767937 ], 6851, NUITKA_BYTECODE_FLAG },
    { (char *)"pyclbr", NULL, &constant_bin[ 3774788 ], 9072, NUITKA_BYTECODE_FLAG },
    { (char *)"pydoc", NULL, &constant_bin[ 3783860 ], 90781, NUITKA_BYTECODE_FLAG },
    { (char *)"pydoc_data", NULL, &constant_bin[ 3874641 ], 112, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"pydoc_data.topics", NULL, &constant_bin[ 3874753 ], 387339, NUITKA_BYTECODE_FLAG },
    { (char *)"queue", NULL, &constant_bin[ 4262092 ], 9167, NUITKA_BYTECODE_FLAG },
    { (char *)"random", NULL, &constant_bin[ 4271259 ], 18958, NUITKA_BYTECODE_FLAG },
    { (char *)"rlcompleter", NULL, &constant_bin[ 4290217 ], 5755, NUITKA_BYTECODE_FLAG },
    { (char *)"runpy", NULL, &constant_bin[ 4295972 ], 8238, NUITKA_BYTECODE_FLAG },
    { (char *)"sched", NULL, &constant_bin[ 4304210 ], 6339, NUITKA_BYTECODE_FLAG },
    { (char *)"selectors", NULL, &constant_bin[ 4310549 ], 18801, NUITKA_BYTECODE_FLAG },
    { (char *)"shelve", NULL, &constant_bin[ 4329350 ], 9913, NUITKA_BYTECODE_FLAG },
    { (char *)"shlex", NULL, &constant_bin[ 4339263 ], 7311, NUITKA_BYTECODE_FLAG },
    { (char *)"shutil", NULL, &constant_bin[ 4346574 ], 32709, NUITKA_BYTECODE_FLAG },
    { (char *)"signal", NULL, &constant_bin[ 4379283 ], 2750, NUITKA_BYTECODE_FLAG },
    { (char *)"site", NULL, &constant_bin[ 4382033 ], 15011, NUITKA_BYTECODE_FLAG },
    { (char *)"sitecustomize", NULL, &constant_bin[ 4397044 ], 205, NUITKA_BYTECODE_FLAG },
    { (char *)"smtpd", NULL, &constant_bin[ 4397249 ], 29281, NUITKA_BYTECODE_FLAG },
    { (char *)"smtplib", NULL, &constant_bin[ 4426530 ], 37004, NUITKA_BYTECODE_FLAG },
    { (char *)"sndhdr", NULL, &constant_bin[ 4463534 ], 6878, NUITKA_BYTECODE_FLAG },
    { (char *)"socket", NULL, &constant_bin[ 4470412 ], 23009, NUITKA_BYTECODE_FLAG },
    { (char *)"socketserver", NULL, &constant_bin[ 4493421 ], 23169, NUITKA_BYTECODE_FLAG },
    { (char *)"sqlite3", NULL, &constant_bin[ 4516590 ], 142, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"sqlite3.dbapi2", NULL, &constant_bin[ 4516732 ], 2677, NUITKA_BYTECODE_FLAG },
    { (char *)"sqlite3.dump", NULL, &constant_bin[ 4519409 ], 2018, NUITKA_BYTECODE_FLAG },
    { (char *)"ssl", NULL, &constant_bin[ 4521427 ], 35739, NUITKA_BYTECODE_FLAG },
    { (char *)"statistics", NULL, &constant_bin[ 4557166 ], 17071, NUITKA_BYTECODE_FLAG },
    { (char *)"string", NULL, &constant_bin[ 4574237 ], 8583, NUITKA_BYTECODE_FLAG },
    { (char *)"subprocess", NULL, &constant_bin[ 4582820 ], 46748, NUITKA_BYTECODE_FLAG },
    { (char *)"sunau", NULL, &constant_bin[ 4629568 ], 18174, NUITKA_BYTECODE_FLAG },
    { (char *)"symbol", NULL, &constant_bin[ 4647742 ], 2703, NUITKA_BYTECODE_FLAG },
    { (char *)"symtable", NULL, &constant_bin[ 4650445 ], 10990, NUITKA_BYTECODE_FLAG },
    { (char *)"sysconfig", NULL, &constant_bin[ 4661435 ], 17033, NUITKA_BYTECODE_FLAG },
    { (char *)"tabnanny", NULL, &constant_bin[ 4678468 ], 7674, NUITKA_BYTECODE_FLAG },
    { (char *)"tarfile", NULL, &constant_bin[ 4686142 ], 68938, NUITKA_BYTECODE_FLAG },
    { (char *)"telnetlib", NULL, &constant_bin[ 4755080 ], 19204, NUITKA_BYTECODE_FLAG },
    { (char *)"tempfile", NULL, &constant_bin[ 4774284 ], 26993, NUITKA_BYTECODE_FLAG },
    { (char *)"textwrap", NULL, &constant_bin[ 4801277 ], 14178, NUITKA_BYTECODE_FLAG },
    { (char *)"this", NULL, &constant_bin[ 4815455 ], 1289, NUITKA_BYTECODE_FLAG },
    { (char *)"timeit", NULL, &constant_bin[ 4816744 ], 10983, NUITKA_BYTECODE_FLAG },
    { (char *)"tkinter", NULL, &constant_bin[ 4827727 ], 184340, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"tkinter.colorchooser", NULL, &constant_bin[ 5012067 ], 1180, NUITKA_BYTECODE_FLAG },
    { (char *)"tkinter.commondialog", NULL, &constant_bin[ 5013247 ], 1252, NUITKA_BYTECODE_FLAG },
    { (char *)"tkinter.constants", NULL, &constant_bin[ 5014499 ], 1780, NUITKA_BYTECODE_FLAG },
    { (char *)"tkinter.dialog", NULL, &constant_bin[ 5016279 ], 1598, NUITKA_BYTECODE_FLAG },
    { (char *)"tkinter.dnd", NULL, &constant_bin[ 5017877 ], 11803, NUITKA_BYTECODE_FLAG },
    { (char *)"tkinter.filedialog", NULL, &constant_bin[ 5029680 ], 13403, NUITKA_BYTECODE_FLAG },
    { (char *)"tkinter.font", NULL, &constant_bin[ 5043083 ], 6653, NUITKA_BYTECODE_FLAG },
    { (char *)"tkinter.messagebox", NULL, &constant_bin[ 5049736 ], 3253, NUITKA_BYTECODE_FLAG },
    { (char *)"tkinter.scrolledtext", NULL, &constant_bin[ 5052989 ], 2312, NUITKA_BYTECODE_FLAG },
    { (char *)"tkinter.simpledialog", NULL, &constant_bin[ 5055301 ], 11367, NUITKA_BYTECODE_FLAG },
    { (char *)"tkinter.tix", NULL, &constant_bin[ 5066668 ], 88850, NUITKA_BYTECODE_FLAG },
    { (char *)"tkinter.ttk", NULL, &constant_bin[ 5155518 ], 58210, NUITKA_BYTECODE_FLAG },
    { (char *)"trace", NULL, &constant_bin[ 5213728 ], 23912, NUITKA_BYTECODE_FLAG },
    { (char *)"tracemalloc", NULL, &constant_bin[ 5237640 ], 17051, NUITKA_BYTECODE_FLAG },
    { (char *)"tty", NULL, &constant_bin[ 5254691 ], 1119, NUITKA_BYTECODE_FLAG },
    { (char *)"turtle", NULL, &constant_bin[ 5255810 ], 138833, NUITKA_BYTECODE_FLAG },
    { (char *)"typing", NULL, &constant_bin[ 5394643 ], 61326, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest", NULL, &constant_bin[ 5455969 ], 3086, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"unittest.case", NULL, &constant_bin[ 5459055 ], 50602, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.loader", NULL, &constant_bin[ 5509657 ], 15099, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.main", NULL, &constant_bin[ 5524756 ], 7651, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.mock", NULL, &constant_bin[ 5532407 ], 65704, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.result", NULL, &constant_bin[ 5598111 ], 7747, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.runner", NULL, &constant_bin[ 5605858 ], 7491, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.signals", NULL, &constant_bin[ 5613349 ], 2350, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.suite", NULL, &constant_bin[ 5615699 ], 9886, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.util", NULL, &constant_bin[ 5625585 ], 5200, NUITKA_BYTECODE_FLAG },
    { (char *)"urllib", NULL, &constant_bin[ 5630785 ], 108, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"urllib.error", NULL, &constant_bin[ 5630893 ], 2896, NUITKA_BYTECODE_FLAG },
    { (char *)"urllib.parse", NULL, &constant_bin[ 5633789 ], 30071, NUITKA_BYTECODE_FLAG },
    { (char *)"urllib.request", NULL, &constant_bin[ 5663860 ], 76363, NUITKA_BYTECODE_FLAG },
    { (char *)"urllib.response", NULL, &constant_bin[ 5740223 ], 3368, NUITKA_BYTECODE_FLAG },
    { (char *)"urllib.robotparser", NULL, &constant_bin[ 5743591 ], 6686, NUITKA_BYTECODE_FLAG },
    { (char *)"uu", NULL, &constant_bin[ 5750277 ], 3928, NUITKA_BYTECODE_FLAG },
    { (char *)"uuid", NULL, &constant_bin[ 5754205 ], 21647, NUITKA_BYTECODE_FLAG },
    { (char *)"venv", NULL, &constant_bin[ 5775852 ], 16752, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"wave", NULL, &constant_bin[ 5792604 ], 18979, NUITKA_BYTECODE_FLAG },
    { (char *)"webbrowser", NULL, &constant_bin[ 5811583 ], 17006, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref", NULL, &constant_bin[ 5828589 ], 706, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"wsgiref.handlers", NULL, &constant_bin[ 5829295 ], 17152, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref.headers", NULL, &constant_bin[ 5846447 ], 8126, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref.simple_server", NULL, &constant_bin[ 5854573 ], 5699, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref.util", NULL, &constant_bin[ 5860272 ], 5563, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref.validate", NULL, &constant_bin[ 5865835 ], 15626, NUITKA_BYTECODE_FLAG },
    { (char *)"xdrlib", NULL, &constant_bin[ 5881461 ], 8939, NUITKA_BYTECODE_FLAG },
    { (char *)"xml", NULL, &constant_bin[ 5890400 ], 680, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.dom", NULL, &constant_bin[ 5891080 ], 5748, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.dom.NodeFilter", NULL, &constant_bin[ 5896828 ], 988, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.domreg", NULL, &constant_bin[ 5897816 ], 2913, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.expatbuilder", NULL, &constant_bin[ 5900729 ], 29803, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.minicompat", NULL, &constant_bin[ 5930532 ], 2941, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.minidom", NULL, &constant_bin[ 5933473 ], 61355, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.pulldom", NULL, &constant_bin[ 5994828 ], 11554, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.xmlbuilder", NULL, &constant_bin[ 6006382 ], 14773, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.etree", NULL, &constant_bin[ 6021155 ], 111, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.etree.ElementInclude", NULL, &constant_bin[ 6021266 ], 1725, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.etree.ElementPath", NULL, &constant_bin[ 6022991 ], 6688, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.etree.ElementTree", NULL, &constant_bin[ 6029679 ], 47821, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.etree.cElementTree", NULL, &constant_bin[ 6077500 ], 155, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.parsers", NULL, &constant_bin[ 6077655 ], 287, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.parsers.expat", NULL, &constant_bin[ 6077942 ], 328, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax", NULL, &constant_bin[ 6078270 ], 3323, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.sax._exceptions", NULL, &constant_bin[ 6081593 ], 5657, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax.expatreader", NULL, &constant_bin[ 6087250 ], 13313, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax.handler", NULL, &constant_bin[ 6100563 ], 12479, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax.saxutils", NULL, &constant_bin[ 6113042 ], 13723, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax.xmlreader", NULL, &constant_bin[ 6126765 ], 17503, NUITKA_BYTECODE_FLAG },
    { (char *)"xmlrpc", NULL, &constant_bin[ 6144268 ], 108, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xmlrpc.client", NULL, &constant_bin[ 6144376 ], 36780, NUITKA_BYTECODE_FLAG },
    { (char *)"xmlrpc.server", NULL, &constant_bin[ 6181156 ], 31192, NUITKA_BYTECODE_FLAG },
    { (char *)"zipapp", NULL, &constant_bin[ 6212348 ], 6000, NUITKA_BYTECODE_FLAG },
    { (char *)"zipfile", NULL, &constant_bin[ 6218348 ], 49903, NUITKA_BYTECODE_FLAG },
    { NULL, NULL, 0 }
};

void setupMetaPathBasedLoader( void )
{
    static bool init_done = false;

    if ( init_done == false )
    {
        registerMetaPathBasedUnfreezer( meta_path_loader_entries );
        init_done = true;
    }
}
