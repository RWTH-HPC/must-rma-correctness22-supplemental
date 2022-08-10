#include <stdio.h>
#include <stdlib.h>

#if (defined __linux__) || (defined linux) || (defined __linux)
#include <dlfcn.h>
#endif

namespace __tsan{
class ReportDesc;
typedef bool (*OnReportFp)(const ReportDesc*, bool);

bool OnReport(const ReportDesc *rep, bool suppressed)
{
  static OnReportFp f = nullptr;
  static bool tried = false;

  if (!tried) {
    void* handle = nullptr;
#if (defined __APPLE__) || (defined __MACH__)
    handle = RTLD_DEFAULT;
#elif (defined __linux__) || (defined linux) || (defined __linux)
    #define _GNU_SOURCE 1
    handle = RTLD_NEXT;
#else
    return suppressed;
#endif

    dlerror();
    f = (OnReportFp)dlsym(handle, "OnReport");
    char* load_error = dlerror();
#ifdef MUST_DEBUG
    if (load_error)
      fprintf(stderr, "Could not load OnReport symbol: %s", load_error);
    else if (f)
      fprintf(stderr, "Successfully loaded OnReport symbol.\n");
    else if (!f)
      fprintf(stderr, "Could not load OnReport symbol: dlsym returned nullptr.\n");
#endif
    tried = true;
  }

  if (f)
    return (*f)(rep, suppressed);

  return suppressed;
}
} /* namespace __tsan */