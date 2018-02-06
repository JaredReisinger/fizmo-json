// extern void dbg(const char *msg);

#define trace(args...) trace_impl(__func__, __FILE__, __LINE__, args)

extern void trace_impl(const char *funcname, const char *fmt, ...);
