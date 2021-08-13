#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <assert.h>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}


vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() { 
  string line;
  int memTotal = 0;
  int memFree = 0;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    string dummy;
    /* Adding a while loop, in case expansion is needed
       to read more lines */
    while (std::getline(stream, line))
    {
      if(line.find("MemTotal") != std::string::npos)
      {
        std::istringstream linestream(line);
        linestream >> dummy >> memTotal;
      }
      else if(line.find("MemFree") != std::string::npos)
      {
        std::istringstream linestream(line);
        linestream >> dummy >> memFree;
      }
   } 
    
  }
  return (float)(memTotal - memFree)/(float)memTotal;
}

long LinuxParser::UpTime() { 
  long int uptime = 0;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    string line;
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >>uptime;
  }
  return uptime;
}


long LinuxParser::Jiffies() { 
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}


long LinuxParser::ActiveJiffies(int pid) { 
  const int skip_idx = 13;
  long jiffies = 0;
  long scanned_jiffies = 0;
  std::ifstream stream(kProcDirectory + std::to_string(pid)+ kStatFilename);
  if (stream.is_open()) {
    string pid_stat;
    std::getline(stream, pid_stat);
    std::istringstream pid_stat_stream(pid_stat);
    std::string token;
    for(int i = 0; i < skip_idx; i++){
        pid_stat_stream >> token;
    }
    //#14 Utime
    pid_stat_stream >> scanned_jiffies;
    jiffies += scanned_jiffies;
    
    //#15 stime
    pid_stat_stream >> scanned_jiffies;
    jiffies += scanned_jiffies;
 

    //#16 Cutime
    pid_stat_stream >> scanned_jiffies;
    jiffies += scanned_jiffies;
    
    //#17 Cstime
    pid_stat_stream >> scanned_jiffies;
    jiffies += scanned_jiffies;
  }
  return jiffies;
}


long LinuxParser::ActiveJiffies() { 
  vector<string> cpu_utilization = LinuxParser::CpuUtilization();
  assert(cpu_utilization.size() == kGuestNice_+1);
  return stol(cpu_utilization[kUser_]) + stol(cpu_utilization[kNice_]) +
         stol(cpu_utilization[kSystem_]) + stol(cpu_utilization[kIRQ_]) +
         stol(cpu_utilization[kSoftIRQ_]) + stol(cpu_utilization[kSteal_]);
}


long LinuxParser::IdleJiffies() { 
  vector<string> cpu_utilization = LinuxParser::CpuUtilization();
  assert(cpu_utilization.size() == kGuestNice_+1);
  return stol(cpu_utilization[kIdle_]) + stol(cpu_utilization[kIOwait_]);
}


vector<string> LinuxParser::CpuUtilization() { 
  vector<string> cpu_utilization;
  std::ifstream fstream(kProcDirectory + kStatFilename);
  if (fstream.is_open()) {
    std::string value_str;
    std::string cpu;
    std::string line;
    std::getline(fstream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    linestream >> value_str;
    cpu_utilization.push_back(value_str);
    linestream >> value_str;
    cpu_utilization.push_back(value_str);
    linestream >> value_str;
    cpu_utilization.push_back(value_str);
    linestream >> value_str;
    cpu_utilization.push_back(value_str);
    linestream >> value_str;
    cpu_utilization.push_back(value_str);
    linestream >> value_str;
    cpu_utilization.push_back(value_str);
    linestream >> value_str;
    cpu_utilization.push_back(value_str);
    linestream >> value_str;
    cpu_utilization.push_back(value_str);
    linestream >> value_str;
    cpu_utilization.push_back(value_str);
    linestream >> value_str;
    cpu_utilization.push_back(value_str);
    
  }
  return cpu_utilization;

}

int LinuxParser::TotalProcesses() { 
  int total_proceesses = 0;
  std::ifstream fstream(kProcDirectory + kStatFilename);
  if (fstream.is_open()) {
    std::string line;
    while (std::getline(fstream, line))
    {
      if(line.rfind("processes",0) == 0)
      {
        std::string dummy;
        std::istringstream linestream(line);
        linestream >> dummy >> total_proceesses;
        break;
      }
    } 
  }
  return total_proceesses;
}

int LinuxParser::RunningProcesses() { 
  int num_of_proceesses = 0;
  std::ifstream fstream(kProcDirectory + kStatFilename);
  if (fstream.is_open()) {
    std::string line;
    while (std::getline(fstream, line))
    {
      if(line.rfind("procs_running",0) == 0)
      {
        std::string dummy;
        std::istringstream linestream(line);
        linestream >> dummy >> num_of_proceesses;
        break;
      }
    } 
  }
  return num_of_proceesses;
}

string LinuxParser::Command(int pid) { 
  std::ifstream fstream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  std::string cmdline = string();
  if (fstream.is_open()) {
    std::getline(fstream, cmdline);
  }
  return cmdline;
}

string LinuxParser::Ram(int pid) { 
  long ram = 0;
  std::ifstream fstream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (fstream.is_open()) {
    std::string line;
    while (std::getline(fstream, line))
    {
      if(line.rfind("VmSize:",0) == 0)
      {
        std::string dummy;
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream linestream(line);
        linestream >> dummy >> ram;
        break;
      }
    } 
  }
  return std::to_string(ram/1000)+ " MB";
}


string LinuxParser::Uid(int pid) 
{ 
  std::string uid = string();
  std::ifstream fstream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (fstream.is_open()) {
    std::string line;
    while (std::getline(fstream, line))
    {
      if(line.rfind("Uid:",0) == 0)
      {
        std::string dummy;
        std::istringstream linestream(line);
        linestream >> dummy >> uid;
        break;
      }
    } 
  }
  return uid;
}


string LinuxParser::User(int pid) { 
  std::string user = string();
  std::string pwd;
  std::string current_uid;
  std::ifstream fstream(kPasswordPath);
  std::string uid = LinuxParser::Uid(pid);
  if (fstream.is_open()) {
    std::string line;
    while (std::getline(fstream, line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> pwd >> current_uid;
      if(current_uid == uid)
      {
        break;
      }
    } 
  }
  return user;
}


long LinuxParser::UpTime(int pid) { 
  const int pid_start_time_idx = 22;
  long pid_start_time = 0;
  std::ifstream stream(kProcDirectory + std::to_string(pid)+ kStatFilename);
  if (stream.is_open()) {
    string pid_stat;
    std::getline(stream, pid_stat);
    std::istringstream pid_stat_stream(pid_stat);
    std::string token;
    for(int i = 0; i < pid_start_time_idx; i++){
        pid_stat_stream >> token;
    }
    pid_start_time = stol(token)/sysconf(_SC_CLK_TCK);
  }
  return LinuxParser::UpTime() - pid_start_time;
}
