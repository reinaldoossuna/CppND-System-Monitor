#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"


using std::set;
using std::size_t;
using std::string;
using std::vector;

System::System(void){
  Update();
}

void System::Clean(){
  processes_.clear();
  pids.clear();
}

void System::Update(){

  Clean();

  pids = LinuxParser::Pids();

  for(auto pid:pids){
    processes_.push_back(Process(pid));
  }
  std::sort(processes_.begin(),processes_.end());
  //std::reverse(processes_.begin(),processes_.end());
}

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() { 
  Update();
  return processes_; }

// DONE: Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// DONE: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// DONE: Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// DONE: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// DONE: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }
