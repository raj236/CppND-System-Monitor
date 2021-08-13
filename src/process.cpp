#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { return pid; }

float Process::CpuUtilization() { return  cpuUtilization;}

string Process::Command() { return command; }

string Process::Ram() { return ram; }

string Process::User() { return user;}

long int Process::UpTime() { return uptime; }

bool Process::operator<(Process const& a) const { return this->pid < a.pid; }

Process::Process(int pid_,std::string user_,std::string command_,
         float cpuUtilization_,std::string ram_,long int uptime_) :
         pid(pid_),user(user_),command(command_),
         cpuUtilization(cpuUtilization_),ram(ram_),uptime(uptime_) {}