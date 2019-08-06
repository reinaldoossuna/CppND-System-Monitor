#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// DONE: Return this process's ID
int Process::Pid() { return pid; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization(){ 
  cpu = LinuxParser::ActiveJiffies(pid);
  return cpu; }

// DONE: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid);}

// TODO: Return this process's memory utilization
string Process::Ram() { return std::to_string(LinuxParser::Ram(pid)); }

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  return cpu < a.cpu;
}
