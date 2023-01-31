#define P1_SWITCH 0 // sig[P1_SWITCH] == 1 to resume P1, ==0 to pause P1 (from S)
#define P1_THREAD_COUNT 1 // sig[P1_THREAD_COUNT] = total number of threads currently running in P1
#define P1_ENDED 2 // sig[P1_END] == 1 P1 ended, else ==0

#define P2_SWITCH 3 // sig[P2_SWITCH] == 1 to resume P2, ==0 to pause P2 (from S)
#define P2_THREAD_COUNT 4 // sig[P2_THREAD_COUNT] = total number of threads currently running in P2
#define P2_ENDED 5 // sig[P2_END] == 1 P2 ended, else ==0

#define ON 1
#define OFF 0

#define YES 1
#define NO 0

#define QUANTA 1000