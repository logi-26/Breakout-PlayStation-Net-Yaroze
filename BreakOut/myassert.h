// Assertion: use printout to siocons console
// Use #define switch ASSERTIONS_ON;

#ifndef ASSERTIONS_ON

	#define assert(ignore)
#else
	#define assert(ex) \
        ((ex) ? 1 : \
              (printf("Assertion failure! " #ex " At line %d of file `%s'.\n", \
               __LINE__, __FILE__), exit (1), 0))
#endif