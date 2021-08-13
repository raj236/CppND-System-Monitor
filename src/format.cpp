#include <string>

#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) { 
  string time;
  int hour = seconds/3600;
  time = std::to_string(hour);
  if(hour < 10) time = '0'+time; 
  seconds = seconds - hour*3600;
  int minutes = seconds/60;
  if(minutes < 10) time += std::string("::0")+std::to_string(minutes); 
  else time += std::string("::")+ std::to_string(minutes); 
  seconds = seconds - minutes*60;
  if(seconds < 10) time +=std::string("::0")+std::to_string(seconds); 
  else time += std::string("::")+ std::to_string(seconds); 
  return time;
}