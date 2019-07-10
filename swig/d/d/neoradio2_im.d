/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.12
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

module neoradio2_im;
static import tango.stdc.config;

// Exception throwing support currently requires Tango, but there is no reason
// why it could not support Phobos.
static import tango.core.Exception;
static import tango.core.Thread;
static import tango.stdc.stringz;
static import tango.stdc.stringz;


private {
  version(linux) {
    version = Nix;
  } else version(darwin) {
    version = Nix;
  } else version(OSX) {
    version = Nix;
  } else version(FreeBSD) {
    version = Nix;
    version = freebsd;
  } else version(freebsd) {
    version = Nix;
  } else version(Unix) {
    version = Nix;
  } else version(Posix) {
    version = Nix;
  }

  version(Tango) {
    static import tango.stdc.string;
    static import tango.stdc.stringz;

    version (PhobosCompatibility) {
    } else {
      alias char[] string;
      alias wchar[] wstring;
      alias dchar[] dstring;
    }
  } else {
    version(D_Version2) {
      static import std.conv;
    }
    static import std.string;
    static import std.c.string;
  }

  version(D_Version2) {
    mixin("alias const(char)* CCPTR;");
  } else {
    alias char* CCPTR;
  }

  CCPTR swigToCString(string str) {
    version(Tango) {
      return tango.stdc.stringz.toStringz(str);
    } else {
      return std.string.toStringz(str);
    }
  }

  string swigToDString(CCPTR cstr) {
    version(Tango) {
      return tango.stdc.stringz.fromStringz(cstr);
    } else {
      version(D_Version2) {
        mixin("return std.conv.to!string(cstr);");
      } else {
        return std.c.string.toString(cstr);
      }
    }
  }
}

class SwigSwigSharedLibLoadException : Exception {
  this(in string[] libNames, in string[] reasons) {
    string msg = "Failed to load one or more shared libraries:";
    foreach(i, n; libNames) {
      msg ~= "\n\t" ~ n ~ " - ";
      if(i < reasons.length)
        msg ~= reasons[i];
      else
        msg ~= "Unknown";
    }
    super(msg);
  }
}

class SwigSymbolLoadException : Exception {
  this(string SwigSharedLibName, string symbolName) {
    super("Failed to load symbol " ~ symbolName ~ " from shared library " ~ SwigSharedLibName);
    _symbolName = symbolName;
  }

  string symbolName() {
    return _symbolName;
  }

private:
  string _symbolName;
}

private {
  version(Nix) {
    version(freebsd) {
      // the dl* functions are in libc on FreeBSD
    }
    else {
      pragma(lib, "dl");
    }

    version(Tango) {
      import tango.sys.Common;
    } else version(linux) {
      import std.c.linux.linux;
    } else {
      extern(C) {
        const RTLD_NOW = 2;

        void *dlopen(CCPTR file, int mode);
        int dlclose(void* handle);
        void *dlsym(void* handle, CCPTR name);
        CCPTR dlerror();
      }
    }

    alias void* SwigSharedLibHandle;

    SwigSharedLibHandle swigLoadSharedLib(string libName) {
      return dlopen(swigToCString(libName), RTLD_NOW);
    }

    void swigUnloadSharedLib(SwigSharedLibHandle hlib) {
      dlclose(hlib);
    }

    void* swigGetSymbol(SwigSharedLibHandle hlib, string symbolName) {
      return dlsym(hlib, swigToCString(symbolName));
    }

    string swigGetErrorStr() {
      CCPTR err = dlerror();
      if (err is null) {
        return "Unknown Error";
      }
      return swigToDString(err);
    }
  } else version(Windows) {
    alias ushort WORD;
    alias uint DWORD;
    alias CCPTR LPCSTR;
    alias void* HMODULE;
    alias void* HLOCAL;
    alias int function() FARPROC;
    struct VA_LIST {}

    extern (Windows) {
      HMODULE LoadLibraryA(LPCSTR);
      FARPROC GetProcAddress(HMODULE, LPCSTR);
      void FreeLibrary(HMODULE);
      DWORD GetLastError();
      DWORD FormatMessageA(DWORD, in void*, DWORD, DWORD, LPCSTR, DWORD, VA_LIST*);
      HLOCAL LocalFree(HLOCAL);
    }

    DWORD MAKELANGID(WORD p, WORD s) {
      return (((cast(WORD)s) << 10) | cast(WORD)p);
    }

    enum {
      LANG_NEUTRAL                    = 0,
      SUBLANG_DEFAULT                 = 1,
      FORMAT_MESSAGE_ALLOCATE_BUFFER  = 256,
      FORMAT_MESSAGE_IGNORE_INSERTS   = 512,
      FORMAT_MESSAGE_FROM_SYSTEM      = 4096
    }

    alias HMODULE SwigSharedLibHandle;

    SwigSharedLibHandle swigLoadSharedLib(string libName) {
      return LoadLibraryA(swigToCString(libName));
    }

    void swigUnloadSharedLib(SwigSharedLibHandle hlib) {
      FreeLibrary(hlib);
    }

    void* swigGetSymbol(SwigSharedLibHandle hlib, string symbolName) {
      return GetProcAddress(hlib, swigToCString(symbolName));
    }

    string swigGetErrorStr() {
      DWORD errcode = GetLastError();

      LPCSTR msgBuf;
      DWORD i = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        null,
        errcode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        cast(LPCSTR)&msgBuf,
        0,
        null);

      string text = swigToDString(msgBuf);
      LocalFree(cast(HLOCAL)msgBuf);

      if (i >= 2) {
        i -= 2;
      }
      return text[0 .. i];
    }
  } else {
    static assert(0, "Operating system not supported by the wrapper loading code.");
  }

  final class SwigSharedLib {
    void load(string[] names) {
      if (_hlib !is null) return;

      string[] failedLibs;
      string[] reasons;

      foreach(n; names) {
        _hlib = swigLoadSharedLib(n);
        if (_hlib is null) {
          failedLibs ~= n;
          reasons ~= swigGetErrorStr();
          continue;
        }
        _name = n;
        break;
      }

      if (_hlib is null) {
        throw new SwigSwigSharedLibLoadException(failedLibs, reasons);
      }
    }

    void* loadSymbol(string symbolName, bool doThrow = true) {
      void* sym = swigGetSymbol(_hlib, symbolName);
      if(doThrow && (sym is null)) {
        throw new SwigSymbolLoadException(_name, symbolName);
      }
      return sym;
    }

    void unload() {
      if(_hlib !is null) {
        swigUnloadSharedLib(_hlib);
        _hlib = null;
      }
    }

  private:
    string _name;
    SwigSharedLibHandle _hlib;
  }
}

static this() {
  string[] possibleFileNames;
  version (Posix) {
    version (OSX) {
      possibleFileNames ~= ["libneoradio2_wrap.dylib", "libneoradio2_wrap.bundle"];
    }
    possibleFileNames ~= ["libneoradio2_wrap.so"];
  } else version (Windows) {
    possibleFileNames ~= ["neoradio2_wrap.dll", "libneoradio2_wrap.so"];
  } else {
    static assert(false, "Operating system not supported by the wrapper loading code.");
  }

  auto library = new SwigSharedLib;
  library.load(possibleFileNames);

  string bindCode(string functionPointer, string symbol) {
    return functionPointer ~ " = cast(typeof(" ~ functionPointer ~
      "))library.loadSymbol(`" ~ symbol ~ "`);";
  }

  //#if !defined(SWIG_D_NO_EXCEPTION_HELPER)
  mixin(bindCode("swigRegisterExceptionCallbacksneoradio2", "SWIGRegisterExceptionCallbacks_neoradio2"));
  //#endif // SWIG_D_NO_EXCEPTION_HELPER
  //#if !defined(SWIG_D_NO_STRING_HELPER)
  mixin(bindCode("swigRegisterStringCallbackneoradio2", "SWIGRegisterStringCallback_neoradio2"));
  //#endif // SWIG_D_NO_STRING_HELPER
  
  mixin(bindCode("NEORADIO2_SUCCESS_get", "D_NEORADIO2_SUCCESS_get"));
  mixin(bindCode("NEORADIO2_FAILURE_get", "D_NEORADIO2_FAILURE_get"));
  mixin(bindCode("NEORADIO2_ERR_WBLOCK_get", "D_NEORADIO2_ERR_WBLOCK_get"));
  mixin(bindCode("NEORADIO2_ERR_INPROGRESS_get", "D_NEORADIO2_ERR_INPROGRESS_get"));
  mixin(bindCode("NEORADIO2_ERR_FAILURE_get", "D_NEORADIO2_ERR_FAILURE_get"));
  mixin(bindCode("NEORADIO2_MAX_DEVS_get", "D_NEORADIO2_MAX_DEVS_get"));
  mixin(bindCode("Neoradio2DeviceInfo_name_set", "D_Neoradio2DeviceInfo_name_set"));
  mixin(bindCode("Neoradio2DeviceInfo_name_get", "D_Neoradio2DeviceInfo_name_get"));
  mixin(bindCode("Neoradio2DeviceInfo_serial_str_set", "D_Neoradio2DeviceInfo_serial_str_set"));
  mixin(bindCode("Neoradio2DeviceInfo_serial_str_get", "D_Neoradio2DeviceInfo_serial_str_get"));
  mixin(bindCode("Neoradio2DeviceInfo_vendor_id_set", "D_Neoradio2DeviceInfo_vendor_id_set"));
  mixin(bindCode("Neoradio2DeviceInfo_vendor_id_get", "D_Neoradio2DeviceInfo_vendor_id_get"));
  mixin(bindCode("Neoradio2DeviceInfo_product_id_set", "D_Neoradio2DeviceInfo_product_id_set"));
  mixin(bindCode("Neoradio2DeviceInfo_product_id_get", "D_Neoradio2DeviceInfo_product_id_get"));
  mixin(bindCode("Neoradio2DeviceInfo__reserved_set", "D_Neoradio2DeviceInfo__reserved_set"));
  mixin(bindCode("Neoradio2DeviceInfo__reserved_get", "D_Neoradio2DeviceInfo__reserved_get"));
  mixin(bindCode("new_Neoradio2DeviceInfo", "D_new_Neoradio2DeviceInfo"));
  mixin(bindCode("delete_Neoradio2DeviceInfo", "D_delete_Neoradio2DeviceInfo"));
}

//#if !defined(SWIG_D_NO_EXCEPTION_HELPER)
extern(C) void function(
  SwigExceptionCallback exceptionCallback,
  SwigExceptionCallback illegalArgumentCallback,
  SwigExceptionCallback illegalElementCallback,
  SwigExceptionCallback ioCallback,
  SwigExceptionCallback noSuchElementCallback) swigRegisterExceptionCallbacksneoradio2;
//#endif // SWIG_D_NO_EXCEPTION_HELPER

//#if !defined(SWIG_D_NO_STRING_HELPER)
extern(C) void function(SwigStringCallback callback) swigRegisterStringCallbackneoradio2;
//#endif // SWIG_D_NO_STRING_HELPER


template SwigOperatorDefinitions() {
  public override int opEquals(Object o) {
    if (auto rhs = cast(typeof(this))o) {
      if (swigCPtr == rhs.swigCPtr) return 1;
      static if (is(typeof(swigOpEquals(rhs)))) {
        return swigOpEquals(rhs) ? 1 : 0;
      } else {
        return 0; 
      }
    }
    return super.opEquals(o);
  }


  public override int opCmp(Object o) {
    static if (is(typeof(swigOpLt(typeof(this).init) &&
        swigOpEquals(typeof(this).init)))) {
      if (auto rhs = cast(typeof(this))o) {
        if (swigOpLt(rhs)) {
          return -1;
        } else if (swigOpEquals(rhs)) {
          return 0;
        } else {
          return 1;
        }
      }
    }
    return super.opCmp(o);
  }

  public typeof(this) opPostInc(T = int)(T unused = 0) {
    static assert(
      is(typeof(swigOpInc(int.init))),
      "opPostInc called on " ~ typeof(this).stringof ~ ", but no postfix " ~
        "increment operator exists in the corresponding C++ class."
    );
    return swigOpInc(int.init);
  }

  public typeof(this) opPostDec(T = int)(T unused = 0) {
    static assert(
      is(typeof(swigOpDec(int.init))),
      "opPostInc called on " ~ typeof(this).stringof ~ ", but no postfix " ~
        "decrement operator exists in the corresponding C++ class."
    );
    return swigOpDec(int.init);
  }


}


private class SwigExceptionHelper {
  static this() {
    swigRegisterExceptionCallbacksneoradio2(
      &setException,
      &setIllegalArgumentException,
      &setIllegalElementException,
      &setIOException,
      &setNoSuchElementException);
  }

  static void setException(char* message) {
    auto exception = new object.Exception(tango.stdc.stringz.fromStringz(message).dup);
    SwigPendingException.set(exception);
  }

  static void setIllegalArgumentException(char* message) {
    auto exception = new tango.core.Exception.IllegalArgumentException(tango.stdc.stringz.fromStringz(message).dup);
    SwigPendingException.set(exception);
  }

  static void setIllegalElementException(char* message) {
    auto exception = new tango.core.Exception.IllegalElementException(tango.stdc.stringz.fromStringz(message).dup);
    SwigPendingException.set(exception);
  }

  static void setIOException(char* message) {
    auto exception = new tango.core.Exception.IOException(tango.stdc.stringz.fromStringz(message).dup);
    SwigPendingException.set(exception);
  }

  static void setNoSuchElementException(char* message) {
    auto exception = new tango.core.Exception.NoSuchElementException(tango.stdc.stringz.fromStringz(message).dup);
    SwigPendingException.set(exception);
  }
}

package class SwigPendingException {
public:
  static this() {
    m_sPendingException = new ThreadLocalData(null);
  }

  static bool isPending() {
    return m_sPendingException.val !is null;
  }

  static void set(object.Exception e) {
    auto pending = m_sPendingException.val;
    if (pending !is null) {
      e.next = pending;
      throw new object.Exception("FATAL: An earlier pending exception from C/C++ " ~
        "code was missed and thus not thrown (" ~ pending.classinfo.name ~ ": " ~
        pending.msg ~ ")!", e);
    }
    m_sPendingException.val = e;
  }

  static object.Exception retrieve() {
    auto e = m_sPendingException.val;
    m_sPendingException.val = null;
    return e;
  }

private:
  // The reference to the pending exception (if any) is stored thread-local.
  alias tango.core.Thread.ThreadLocal!(object.Exception) ThreadLocalData;
  static ThreadLocalData m_sPendingException;
}
alias void function(char* message) SwigExceptionCallback;


private class SwigStringHelper {
  static this() {
    swigRegisterStringCallbackneoradio2(&createString);
  }

  static char* createString(char* cString) {
    // We are effectively dup'ing the string here.
    return tango.stdc.stringz.toStringz(tango.stdc.stringz.fromStringz(cString));
  }
}
alias char* function(char* cString) SwigStringCallback;


template SwigExternC(T) {
  static if (is(typeof(*(T.init)) R == return)) {
    static if (is(typeof(*(T.init)) P == function)) {
      alias extern(C) R function(P) SwigExternC;
    }
  }
}

SwigExternC!(int function()) NEORADIO2_SUCCESS_get;
SwigExternC!(int function()) NEORADIO2_FAILURE_get;
SwigExternC!(int function()) NEORADIO2_ERR_WBLOCK_get;
SwigExternC!(int function()) NEORADIO2_ERR_INPROGRESS_get;
SwigExternC!(int function()) NEORADIO2_ERR_FAILURE_get;
SwigExternC!(int function()) NEORADIO2_MAX_DEVS_get;
SwigExternC!(void function(void* jarg1, char* jarg2)) Neoradio2DeviceInfo_name_set;
SwigExternC!(char* function(void* jarg1)) Neoradio2DeviceInfo_name_get;
SwigExternC!(void function(void* jarg1, char* jarg2)) Neoradio2DeviceInfo_serial_str_set;
SwigExternC!(char* function(void* jarg1)) Neoradio2DeviceInfo_serial_str_get;
SwigExternC!(void function(void* jarg1, int jarg2)) Neoradio2DeviceInfo_vendor_id_set;
SwigExternC!(int function(void* jarg1)) Neoradio2DeviceInfo_vendor_id_get;
SwigExternC!(void function(void* jarg1, int jarg2)) Neoradio2DeviceInfo_product_id_set;
SwigExternC!(int function(void* jarg1)) Neoradio2DeviceInfo_product_id_get;
SwigExternC!(void function(void* jarg1, void* jarg2)) Neoradio2DeviceInfo__reserved_set;
SwigExternC!(void* function(void* jarg1)) Neoradio2DeviceInfo__reserved_get;
SwigExternC!(void* function()) new_Neoradio2DeviceInfo;
SwigExternC!(void function(void* jarg1)) delete_Neoradio2DeviceInfo;
