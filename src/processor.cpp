#include "processor.h"
#include "linux_parser.h"
#include <unistd.h>


float Processor::Utilization() { 
  long int prev_idle_jiffies = LinuxParser::IdleJiffies();
  long int prev_active_jiffies = LinuxParser::ActiveJiffies();
  long int prev_total_jiffies = prev_idle_jiffies +
                                  prev_active_jiffies;
  sleep(1);
  long int curr_idle_jiffies = LinuxParser::IdleJiffies();
  long int curr_active_jiffies = LinuxParser::ActiveJiffies();
  long int curr_total_jiffies = curr_idle_jiffies +
                                  curr_active_jiffies;
  long int delta_total = curr_total_jiffies - prev_total_jiffies;
  long int delta_idle = curr_idle_jiffies - prev_idle_jiffies;
  return  (float)(delta_total - delta_idle)/(float)delta_total;
}