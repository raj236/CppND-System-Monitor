#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { 
  Processor *p = new Processor();
  return *p;
}


vector<Process>& System::Processes() { 
  std::vector<int> pids = LinuxParser::Pids();
  vector<Process> *processes_v = new vector<Process>;
  for(unsigned int i = 0; i < pids.size(); i++)
  {
    float cpuUtilization = (LinuxParser::ActiveJiffies(pids[i])/sysconf(_SC_CLK_TCK));
    cpuUtilization =   cpuUtilization/LinuxParser::UpTime(pids[i]);
    std::string user = LinuxParser::User(pids[i]);
    std::string cmd = LinuxParser::Command(pids[i]);
    std::string ram = LinuxParser::Ram(pids[i]);
    if(!ram.empty()  && !cmd.empty() && !user.empty())
    {
      Process process(pids[i],
                      LinuxParser::User(pids[i]),
                      LinuxParser::Command(pids[i]),
                      cpuUtilization,
                      LinuxParser::Ram(pids[i]),
                      LinuxParser::UpTime(pids[i]));
      processes_v->push_back(process);
    }
  }
  return *processes_v;
}

std::string System::Kernel() { 
  return LinuxParser::Kernel();
}


float System::MemoryUtilization() { 
  return LinuxParser::MemoryUtilization();
}

std::string System::OperatingSystem() { 
  return LinuxParser::OperatingSystem();
}

int System::RunningProcesses() { 
  return LinuxParser::RunningProcesses();
}

int System::TotalProcesses() { 
  return LinuxParser::TotalProcesses();
}

long int System::UpTime() { 
  return LinuxParser::UpTime();
}