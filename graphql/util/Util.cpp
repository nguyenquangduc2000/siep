

#include "Util.h"

using namespace std;

Util::Util(){}

Util::~Util() {}

long Util::get_cur_time() {
    timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec*1000 + tv.tv_usec/1000);
}

//NOTICE: we had better watch the virtual memory usage, i.e., vsize.
//https://blog.csdn.net/zyboy2000/article/details/50456764
//The unit of return values are both KB.
void Util::process_mem_usage(double& vm_usage, double& resident_set) {
   using std::ios_base;
   using std::ifstream;
   using std::string;

   vm_usage     = 0.0;
   resident_set = 0.0;

   // 'file' stat seems to give the most reliable results
   ifstream stat_stream("/proc/self/stat",ios_base::in);

   // dummy vars for leading entries in stat that we don't care about
   string pid, comm, state, ppid, pgrp, session, tty_nr;
   string tpgid, flags, minflt, cminflt, majflt, cmajflt;
   string utime, stime, cutime, cstime, priority, nice;
   string O, itrealvalue, starttime;

   // the two fields we want
   unsigned long vsize;  // for virtual memory, the virtual size (B)
   long rss;    //for physical memory, number of real pages
   //In /proc/$pid/status, these corresponding to VmSize and VmRSS

   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
               >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
               >> utime >> stime >> cutime >> cstime >> priority >> nice
               >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

   stat_stream.close();

   long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
   vm_usage     = vsize / 1024.0;
   resident_set = rss * page_size_kb;
}


void myTimeout(int signo)
{
    switch(signo)
    {
        case SIGALRM:
            printf("This query runs time out!\n");
            exit(1);
        default:
            break;
    }
}

void 
Util::timeLimit(int seconds)
{
    struct itimerval tick;
    //signal(SIGALRM, exit);
    signal(SIGALRM, myTimeout);
    memset(&tick, 0, sizeof(tick));
    //Timeout to run first time
    tick.it_value.tv_sec = seconds;
    tick.it_value.tv_usec = 0;
    //After first, the Interval time for clock
    tick.it_interval.tv_sec = seconds;
    tick.it_interval.tv_usec = 0;
    if(setitimer(ITIMER_REAL, &tick, NULL) < 0)
        printf("Set timer failed!\n");
}

void 
Util::noTimeLimit()
{
    struct itimerval tick;
    memset(&tick, 0, sizeof(tick));
    if(setitimer(ITIMER_REAL, &tick, NULL) < 0)
        printf("Withdraw timer failed!\n");
}


