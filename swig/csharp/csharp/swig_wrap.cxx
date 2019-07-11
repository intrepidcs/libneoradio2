/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.12
 *
 * This file is not intended to be easily readable and contains a number of
 * coding conventions designed to improve portability and efficiency. Do not make
 * changes to this file unless you know what you are doing--modify the SWIG
 * interface file instead.
 * ----------------------------------------------------------------------------- */


#ifndef SWIGCSHARP
#define SWIGCSHARP
#endif



#ifdef __cplusplus
/* SwigValueWrapper is described in swig.swg */
template<typename T> class SwigValueWrapper {
  struct SwigMovePointer {
    T *ptr;
    SwigMovePointer(T *p) : ptr(p) { }
    ~SwigMovePointer() { delete ptr; }
    SwigMovePointer& operator=(SwigMovePointer& rhs) { T* oldptr = ptr; ptr = 0; delete oldptr; ptr = rhs.ptr; rhs.ptr = 0; return *this; }
  } pointer;
  SwigValueWrapper& operator=(const SwigValueWrapper<T>& rhs);
  SwigValueWrapper(const SwigValueWrapper<T>& rhs);
public:
  SwigValueWrapper() : pointer(0) { }
  SwigValueWrapper& operator=(const T& t) { SwigMovePointer tmp(new T(t)); pointer = tmp; return *this; }
  operator T&() const { return *pointer.ptr; }
  T *operator&() { return pointer.ptr; }
};

template <typename T> T SwigValueInit() {
  return T();
}
#endif

/* -----------------------------------------------------------------------------
 *  This section contains generic SWIG labels for method/variable
 *  declarations/attributes, and other compiler dependent labels.
 * ----------------------------------------------------------------------------- */

/* template workaround for compilers that cannot correctly implement the C++ standard */
#ifndef SWIGTEMPLATEDISAMBIGUATOR
# if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x560)
#  define SWIGTEMPLATEDISAMBIGUATOR template
# elif defined(__HP_aCC)
/* Needed even with `aCC -AA' when `aCC -V' reports HP ANSI C++ B3910B A.03.55 */
/* If we find a maximum version that requires this, the test would be __HP_aCC <= 35500 for A.03.55 */
#  define SWIGTEMPLATEDISAMBIGUATOR template
# else
#  define SWIGTEMPLATEDISAMBIGUATOR
# endif
#endif

/* inline attribute */
#ifndef SWIGINLINE
# if defined(__cplusplus) || (defined(__GNUC__) && !defined(__STRICT_ANSI__))
#   define SWIGINLINE inline
# else
#   define SWIGINLINE
# endif
#endif

/* attribute recognised by some compilers to avoid 'unused' warnings */
#ifndef SWIGUNUSED
# if defined(__GNUC__)
#   if !(defined(__cplusplus)) || (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))
#     define SWIGUNUSED __attribute__ ((__unused__))
#   else
#     define SWIGUNUSED
#   endif
# elif defined(__ICC)
#   define SWIGUNUSED __attribute__ ((__unused__))
# else
#   define SWIGUNUSED
# endif
#endif

#ifndef SWIG_MSC_UNSUPPRESS_4505
# if defined(_MSC_VER)
#   pragma warning(disable : 4505) /* unreferenced local function has been removed */
# endif
#endif

#ifndef SWIGUNUSEDPARM
# ifdef __cplusplus
#   define SWIGUNUSEDPARM(p)
# else
#   define SWIGUNUSEDPARM(p) p SWIGUNUSED
# endif
#endif

/* internal SWIG method */
#ifndef SWIGINTERN
# define SWIGINTERN static SWIGUNUSED
#endif

/* internal inline SWIG method */
#ifndef SWIGINTERNINLINE
# define SWIGINTERNINLINE SWIGINTERN SWIGINLINE
#endif

/* exporting methods */
#if defined(__GNUC__)
#  if (__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#    ifndef GCC_HASCLASSVISIBILITY
#      define GCC_HASCLASSVISIBILITY
#    endif
#  endif
#endif

#ifndef SWIGEXPORT
# if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
#   if defined(STATIC_LINKED)
#     define SWIGEXPORT
#   else
#     define SWIGEXPORT __declspec(dllexport)
#   endif
# else
#   if defined(__GNUC__) && defined(GCC_HASCLASSVISIBILITY)
#     define SWIGEXPORT __attribute__ ((visibility("default")))
#   else
#     define SWIGEXPORT
#   endif
# endif
#endif

/* calling conventions for Windows */
#ifndef SWIGSTDCALL
# if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
#   define SWIGSTDCALL __stdcall
# else
#   define SWIGSTDCALL
# endif
#endif

/* Deal with Microsoft's attempt at deprecating C standard runtime functions */
#if !defined(SWIG_NO_CRT_SECURE_NO_DEPRECATE) && defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
# define _CRT_SECURE_NO_DEPRECATE
#endif

/* Deal with Microsoft's attempt at deprecating methods in the standard C++ library */
#if !defined(SWIG_NO_SCL_SECURE_NO_DEPRECATE) && defined(_MSC_VER) && !defined(_SCL_SECURE_NO_DEPRECATE)
# define _SCL_SECURE_NO_DEPRECATE
#endif

/* Deal with Apple's deprecated 'AssertMacros.h' from Carbon-framework */
#if defined(__APPLE__) && !defined(__ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES)
# define __ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES 0
#endif

/* Intel's compiler complains if a variable which was never initialised is
 * cast to void, which is a common idiom which we use to indicate that we
 * are aware a variable isn't used.  So we just silence that warning.
 * See: https://github.com/swig/swig/issues/192 for more discussion.
 */
#ifdef __INTEL_COMPILER
# pragma warning disable 592
#endif


#include <stdlib.h>
#include <string.h>
#include <stdio.h>


/* Support for throwing C# exceptions from C/C++. There are two types: 
 * Exceptions that take a message and ArgumentExceptions that take a message and a parameter name. */
typedef enum {
  SWIG_CSharpApplicationException,
  SWIG_CSharpArithmeticException,
  SWIG_CSharpDivideByZeroException,
  SWIG_CSharpIndexOutOfRangeException,
  SWIG_CSharpInvalidCastException,
  SWIG_CSharpInvalidOperationException,
  SWIG_CSharpIOException,
  SWIG_CSharpNullReferenceException,
  SWIG_CSharpOutOfMemoryException,
  SWIG_CSharpOverflowException,
  SWIG_CSharpSystemException
} SWIG_CSharpExceptionCodes;

typedef enum {
  SWIG_CSharpArgumentException,
  SWIG_CSharpArgumentNullException,
  SWIG_CSharpArgumentOutOfRangeException
} SWIG_CSharpExceptionArgumentCodes;

typedef void (SWIGSTDCALL* SWIG_CSharpExceptionCallback_t)(const char *);
typedef void (SWIGSTDCALL* SWIG_CSharpExceptionArgumentCallback_t)(const char *, const char *);

typedef struct {
  SWIG_CSharpExceptionCodes code;
  SWIG_CSharpExceptionCallback_t callback;
} SWIG_CSharpException_t;

typedef struct {
  SWIG_CSharpExceptionArgumentCodes code;
  SWIG_CSharpExceptionArgumentCallback_t callback;
} SWIG_CSharpExceptionArgument_t;

static SWIG_CSharpException_t SWIG_csharp_exceptions[] = {
  { SWIG_CSharpApplicationException, NULL },
  { SWIG_CSharpArithmeticException, NULL },
  { SWIG_CSharpDivideByZeroException, NULL },
  { SWIG_CSharpIndexOutOfRangeException, NULL },
  { SWIG_CSharpInvalidCastException, NULL },
  { SWIG_CSharpInvalidOperationException, NULL },
  { SWIG_CSharpIOException, NULL },
  { SWIG_CSharpNullReferenceException, NULL },
  { SWIG_CSharpOutOfMemoryException, NULL },
  { SWIG_CSharpOverflowException, NULL },
  { SWIG_CSharpSystemException, NULL }
};

static SWIG_CSharpExceptionArgument_t SWIG_csharp_exceptions_argument[] = {
  { SWIG_CSharpArgumentException, NULL },
  { SWIG_CSharpArgumentNullException, NULL },
  { SWIG_CSharpArgumentOutOfRangeException, NULL }
};

static void SWIGUNUSED SWIG_CSharpSetPendingException(SWIG_CSharpExceptionCodes code, const char *msg) {
  SWIG_CSharpExceptionCallback_t callback = SWIG_csharp_exceptions[SWIG_CSharpApplicationException].callback;
  if ((size_t)code < sizeof(SWIG_csharp_exceptions)/sizeof(SWIG_CSharpException_t)) {
    callback = SWIG_csharp_exceptions[code].callback;
  }
  callback(msg);
}

static void SWIGUNUSED SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpExceptionArgumentCodes code, const char *msg, const char *param_name) {
  SWIG_CSharpExceptionArgumentCallback_t callback = SWIG_csharp_exceptions_argument[SWIG_CSharpArgumentException].callback;
  if ((size_t)code < sizeof(SWIG_csharp_exceptions_argument)/sizeof(SWIG_CSharpExceptionArgument_t)) {
    callback = SWIG_csharp_exceptions_argument[code].callback;
  }
  callback(msg, param_name);
}


#ifdef __cplusplus
extern "C" 
#endif
SWIGEXPORT void SWIGSTDCALL SWIGRegisterExceptionCallbacks_neoradio2(
                                                SWIG_CSharpExceptionCallback_t applicationCallback,
                                                SWIG_CSharpExceptionCallback_t arithmeticCallback,
                                                SWIG_CSharpExceptionCallback_t divideByZeroCallback, 
                                                SWIG_CSharpExceptionCallback_t indexOutOfRangeCallback, 
                                                SWIG_CSharpExceptionCallback_t invalidCastCallback,
                                                SWIG_CSharpExceptionCallback_t invalidOperationCallback,
                                                SWIG_CSharpExceptionCallback_t ioCallback,
                                                SWIG_CSharpExceptionCallback_t nullReferenceCallback,
                                                SWIG_CSharpExceptionCallback_t outOfMemoryCallback, 
                                                SWIG_CSharpExceptionCallback_t overflowCallback, 
                                                SWIG_CSharpExceptionCallback_t systemCallback) {
  SWIG_csharp_exceptions[SWIG_CSharpApplicationException].callback = applicationCallback;
  SWIG_csharp_exceptions[SWIG_CSharpArithmeticException].callback = arithmeticCallback;
  SWIG_csharp_exceptions[SWIG_CSharpDivideByZeroException].callback = divideByZeroCallback;
  SWIG_csharp_exceptions[SWIG_CSharpIndexOutOfRangeException].callback = indexOutOfRangeCallback;
  SWIG_csharp_exceptions[SWIG_CSharpInvalidCastException].callback = invalidCastCallback;
  SWIG_csharp_exceptions[SWIG_CSharpInvalidOperationException].callback = invalidOperationCallback;
  SWIG_csharp_exceptions[SWIG_CSharpIOException].callback = ioCallback;
  SWIG_csharp_exceptions[SWIG_CSharpNullReferenceException].callback = nullReferenceCallback;
  SWIG_csharp_exceptions[SWIG_CSharpOutOfMemoryException].callback = outOfMemoryCallback;
  SWIG_csharp_exceptions[SWIG_CSharpOverflowException].callback = overflowCallback;
  SWIG_csharp_exceptions[SWIG_CSharpSystemException].callback = systemCallback;
}

#ifdef __cplusplus
extern "C" 
#endif
SWIGEXPORT void SWIGSTDCALL SWIGRegisterExceptionArgumentCallbacks_neoradio2(
                                                SWIG_CSharpExceptionArgumentCallback_t argumentCallback,
                                                SWIG_CSharpExceptionArgumentCallback_t argumentNullCallback,
                                                SWIG_CSharpExceptionArgumentCallback_t argumentOutOfRangeCallback) {
  SWIG_csharp_exceptions_argument[SWIG_CSharpArgumentException].callback = argumentCallback;
  SWIG_csharp_exceptions_argument[SWIG_CSharpArgumentNullException].callback = argumentNullCallback;
  SWIG_csharp_exceptions_argument[SWIG_CSharpArgumentOutOfRangeException].callback = argumentOutOfRangeCallback;
}


/* Callback for returning strings to C# without leaking memory */
typedef char * (SWIGSTDCALL* SWIG_CSharpStringHelperCallback)(const char *);
static SWIG_CSharpStringHelperCallback SWIG_csharp_string_callback = NULL;


#ifdef __cplusplus
extern "C" 
#endif
SWIGEXPORT void SWIGSTDCALL SWIGRegisterStringCallback_neoradio2(SWIG_CSharpStringHelperCallback callback) {
  SWIG_csharp_string_callback = callback;
}


/* Contract support */

#define SWIG_contract_assert(nullreturn, expr, msg) if (!(expr)) {SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentOutOfRangeException, msg, ""); return nullreturn; } else



#include "../libneoradio2.h"
#include "../libneoradio2common.h"

extern void neoradio2_set_blocking(int blocking, long long ms_timeout);
extern int neoradio2_find(Neoradio2DeviceInfo* devices, unsigned int* device_count);
extern int neoradio2_is_blocking();
extern int neoradio2_open(neoradio2_handle* handle, Neoradio2DeviceInfo* device);
extern int neoradio2_is_opened(neoradio2_handle* handle, int* is_opened);
extern int neoradio2_close(neoradio2_handle* handle);
extern int neoradio2_is_closed(neoradio2_handle* handle, int* is_closed);
extern int neoradio2_chain_is_identified(neoradio2_handle* handle, int* is_identified);
extern int neoradio2_chain_identify(neoradio2_handle* handle);
extern int neoradio2_app_is_started(neoradio2_handle* handle, int device, int bank, int* is_started);
extern int neoradio2_app_start(neoradio2_handle* handle, int device, int bank);
extern int neoradio2_enter_bootloader(neoradio2_handle* handle, int device, int bank);
extern int neoradio2_get_serial_number(neoradio2_handle* handle, int device, int bank, unsigned int* serial_number);
extern int neoradio2_get_manufacturer_date(neoradio2_handle* handle, int device, int bank, int* year, int* month, int* day);
extern int neoradio2_get_firmware_version(neoradio2_handle* handle, int device, int bank, int* major, int* minor);
extern int neoradio2_get_hardware_revision(neoradio2_handle* handle, int device, int bank, int* major, int* minor);
extern int neoradio2_get_device_type(neoradio2_handle* handle, int device, int bank, unsigned int* device_type);
extern int neoradio2_request_pcbsn(neoradio2_handle* handle, int device, int bank);
extern int neoradio2_get_pcbsn(neoradio2_handle* handle, int device, int bank, char* pcb_sn);
extern int neoradio2_request_sensor_data(neoradio2_handle* handle, int device, int bank, int enable_cal);
extern int neoradio2_read_sensor_float(neoradio2_handle* handle, int device, int bank, float* value);
extern int neoradio2_read_sensor_array(neoradio2_handle* handle, int device, int bank, int* arr, int* arr_size);
extern int neoradio2_write_sensor(neoradio2_handle* handle, int device, int bank, int mask, int value);
extern int neoradio2_write_sensor_successful(neoradio2_handle* handle, int device, int bank);
extern int neoradio2_request_settings(neoradio2_handle* handle, int device, int bank);
extern int neoradio2_read_settings(neoradio2_handle* handle, int device, int bank, neoRADIO2_settings* settings);
extern int neoradio2_write_settings(neoradio2_handle* handle, int device, int bank, neoRADIO2_settings* settings);
extern int neoradio2_write_settings_successful(neoradio2_handle* handle, int device, int bank);
extern int neoradio2_get_chain_count(neoradio2_handle* handle, int* count, int identify);
extern int neoradio2_request_calibration(neoradio2_handle* handle, int device, int bank, neoRADIO2frame_calHeader* header);
extern int neoradio2_read_calibration_array(neoradio2_handle* handle, int device, int bank, neoRADIO2frame_calHeader* header, float* arr, int* arr_size);
extern int neoradio2_request_calibration_points(neoradio2_handle* handle, int device, int bank, neoRADIO2frame_calHeader* header);
extern int neoradio2_read_calibration_points_array(neoradio2_handle* handle, int device, int bank, neoRADIO2frame_calHeader* header, float* arr, int* arr_size);
extern int neoradio2_write_calibration(neoradio2_handle* handle, int device, int bank, neoRADIO2frame_calHeader* header, float* arr, int arr_size);
extern int neoradio2_write_calibration_successful(neoradio2_handle* handle, int device, int bank);
extern int neoradio2_write_calibration_points(neoradio2_handle* handle, int device, int bank, neoRADIO2frame_calHeader* header, float* arr, int arr_size);
extern int neoradio2_write_calibration_points_successful(neoradio2_handle* handle, int device, int bank);
extern int neoradio2_store_calibration(neoradio2_handle* handle, int device, int bank);
extern int neoradio2_is_calibration_stored(neoradio2_handle* handle, int device, int bank, int* stored);
extern int neoradio2_get_calibration_is_valid(neoradio2_handle* handle, int device, int bank, int* is_valid);

extern int neoradio2_request_calibration_info(neoradio2_handle* handle, int device, int bank);
extern int neoradio2_read_calibration_info(neoradio2_handle* handle, int device, int bank, neoRADIO2frame_calHeader* header);


extern int neoradio2_toggle_led(neoradio2_handle* handle, int device, int bank, int ms);
extern int neoradio2_toggle_led_successful(neoradio2_handle* handle, int device, int bank);


extern int neoradio2_get_status(neoradio2_handle* handle, int device, int bank, int bitfield, StatusType type, CommandStatus* status);



#ifdef __cplusplus
extern "C" {
#endif

SWIGEXPORT int SWIGSTDCALL CSharp_NEORADIO2_SUCCESS_get() {
  int jresult ;
  int result;
  
  result = (int)(0);
  jresult = result; 
  return jresult;
}


SWIGEXPORT int SWIGSTDCALL CSharp_NEORADIO2_FAILURE_get() {
  int jresult ;
  int result;
  
  result = (int)(1);
  jresult = result; 
  return jresult;
}


SWIGEXPORT int SWIGSTDCALL CSharp_NEORADIO2_ERR_WBLOCK_get() {
  int jresult ;
  int result;
  
  result = (int)(2);
  jresult = result; 
  return jresult;
}


SWIGEXPORT int SWIGSTDCALL CSharp_NEORADIO2_ERR_INPROGRESS_get() {
  int jresult ;
  int result;
  
  result = (int)(3);
  jresult = result; 
  return jresult;
}


SWIGEXPORT int SWIGSTDCALL CSharp_NEORADIO2_ERR_FAILURE_get() {
  int jresult ;
  int result;
  
  result = (int)(4);
  jresult = result; 
  return jresult;
}


SWIGEXPORT int SWIGSTDCALL CSharp_NEORADIO2_MAX_DEVS_get() {
  int jresult ;
  int result;
  
  result = (int)(8);
  jresult = result; 
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_Neoradio2DeviceInfo_name_set(void * jarg1, char * jarg2) {
  _Neoradio2DeviceInfo *arg1 = (_Neoradio2DeviceInfo *) 0 ;
  char *arg2 ;
  
  arg1 = (_Neoradio2DeviceInfo *)jarg1; 
  arg2 = (char *)jarg2; 
  {
    if(arg2) {
      strncpy((char*)arg1->name, (const char *)arg2, 64-1);
      arg1->name[64-1] = 0;
    } else {
      arg1->name[0] = 0;
    }
  }
}


SWIGEXPORT char * SWIGSTDCALL CSharp_Neoradio2DeviceInfo_name_get(void * jarg1) {
  char * jresult ;
  _Neoradio2DeviceInfo *arg1 = (_Neoradio2DeviceInfo *) 0 ;
  char *result = 0 ;
  
  arg1 = (_Neoradio2DeviceInfo *)jarg1; 
  result = (char *)(char *) ((arg1)->name);
  jresult = SWIG_csharp_string_callback((const char *)result); 
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_Neoradio2DeviceInfo_serial_str_set(void * jarg1, char * jarg2) {
  _Neoradio2DeviceInfo *arg1 = (_Neoradio2DeviceInfo *) 0 ;
  char *arg2 ;
  
  arg1 = (_Neoradio2DeviceInfo *)jarg1; 
  arg2 = (char *)jarg2; 
  {
    if(arg2) {
      strncpy((char*)arg1->serial_str, (const char *)arg2, 64-1);
      arg1->serial_str[64-1] = 0;
    } else {
      arg1->serial_str[0] = 0;
    }
  }
}


SWIGEXPORT char * SWIGSTDCALL CSharp_Neoradio2DeviceInfo_serial_str_get(void * jarg1) {
  char * jresult ;
  _Neoradio2DeviceInfo *arg1 = (_Neoradio2DeviceInfo *) 0 ;
  char *result = 0 ;
  
  arg1 = (_Neoradio2DeviceInfo *)jarg1; 
  result = (char *)(char *) ((arg1)->serial_str);
  jresult = SWIG_csharp_string_callback((const char *)result); 
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_Neoradio2DeviceInfo_vendor_id_set(void * jarg1, int jarg2) {
  _Neoradio2DeviceInfo *arg1 = (_Neoradio2DeviceInfo *) 0 ;
  int arg2 ;
  
  arg1 = (_Neoradio2DeviceInfo *)jarg1; 
  arg2 = (int)jarg2; 
  if (arg1) (arg1)->vendor_id = arg2;
}


SWIGEXPORT int SWIGSTDCALL CSharp_Neoradio2DeviceInfo_vendor_id_get(void * jarg1) {
  int jresult ;
  _Neoradio2DeviceInfo *arg1 = (_Neoradio2DeviceInfo *) 0 ;
  int result;
  
  arg1 = (_Neoradio2DeviceInfo *)jarg1; 
  result = (int) ((arg1)->vendor_id);
  jresult = result; 
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_Neoradio2DeviceInfo_product_id_set(void * jarg1, int jarg2) {
  _Neoradio2DeviceInfo *arg1 = (_Neoradio2DeviceInfo *) 0 ;
  int arg2 ;
  
  arg1 = (_Neoradio2DeviceInfo *)jarg1; 
  arg2 = (int)jarg2; 
  if (arg1) (arg1)->product_id = arg2;
}


SWIGEXPORT int SWIGSTDCALL CSharp_Neoradio2DeviceInfo_product_id_get(void * jarg1) {
  int jresult ;
  _Neoradio2DeviceInfo *arg1 = (_Neoradio2DeviceInfo *) 0 ;
  int result;
  
  arg1 = (_Neoradio2DeviceInfo *)jarg1; 
  result = (int) ((arg1)->product_id);
  jresult = result; 
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_Neoradio2DeviceInfo__reserved_set(void * jarg1, void * jarg2) {
  _Neoradio2DeviceInfo *arg1 = (_Neoradio2DeviceInfo *) 0 ;
  uint8_t *arg2 ;
  
  arg1 = (_Neoradio2DeviceInfo *)jarg1; 
  arg2 = (uint8_t *)jarg2; 
  {
    size_t ii;
    uint8_t *b = (uint8_t *) arg1->_reserved;
    for (ii = 0; ii < (size_t)32; ii++) b[ii] = *((uint8_t *) arg2 + ii);
  }
}


SWIGEXPORT void * SWIGSTDCALL CSharp_Neoradio2DeviceInfo__reserved_get(void * jarg1) {
  void * jresult ;
  _Neoradio2DeviceInfo *arg1 = (_Neoradio2DeviceInfo *) 0 ;
  uint8_t *result = 0 ;
  
  arg1 = (_Neoradio2DeviceInfo *)jarg1; 
  result = (uint8_t *)(uint8_t *) ((arg1)->_reserved);
  jresult = result; 
  return jresult;
}


SWIGEXPORT void * SWIGSTDCALL CSharp_new_Neoradio2DeviceInfo() {
  void * jresult ;
  _Neoradio2DeviceInfo *result = 0 ;
  
  result = (_Neoradio2DeviceInfo *)new _Neoradio2DeviceInfo();
  jresult = (void *)result; 
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_delete_Neoradio2DeviceInfo(void * jarg1) {
  _Neoradio2DeviceInfo *arg1 = (_Neoradio2DeviceInfo *) 0 ;
  
  arg1 = (_Neoradio2DeviceInfo *)jarg1; 
  delete arg1;
}


#ifdef __cplusplus
}
#endif
