#include <dirent.h>
#include <unistd.h> // sysconf(_SC_CLK_TCK)
#include <filesystem>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids {};

  for (auto& p : std::filesystem::directory_iterator(kProcDirectory)) {
    if (p.is_directory()) {
      std::string s = p.path().filename().c_str();
      // Is every character of the name a digit?
      if (std::all_of(s.begin(), s.end(), isdigit))
        pids.push_back(std::stoi(s));
    }
  }
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  string value;

  float memtotal;
  float memfree;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::stringstream linestream(line);

      linestream >> key >> value;
      if (key == "MemTotal:") memtotal = std::stof(value);
      if (key == "MemFree:") memfree = std::stof(value);
    }
  }
  return (memtotal - memfree) / memtotal;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  string uptimeSEC;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptimeSEC;
  }
  return stol(uptimeSEC);
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return ActiveJiffies() + IdleJiffies(); }

// TODO: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  std::string line;
  long total_clock = 0;
  long total_sec = 0;

  long uptime = UpTime();
  long seconds = 0;

  std::ifstream filestream(kProcDirectory + "/" +
                           std::to_string(pid) +
                           kStatFilename);

  if (filestream.is_open()) {
    std::getline(filestream,line);
    std::istringstream iss(line);
    std::vector<std::string> v(std::istream_iterator<std::string> {iss},
                                      std::istream_iterator<std::string>());

    for(auto state: allProcessStats) {
      total_clock += std::stol( v[state] );
    }


    seconds = uptime - ( std::stol( v[kStartTime] )/ sysconf(_SC_CLK_TCK));

    total_sec = (total_clock/ sysconf(_SC_CLK_TCK));

    return 100 * (total_sec / seconds);
    
  }
  
  return -1;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::vector<std::string> cpuUtilization = LinuxParser::CpuUtilization();
  long activeJiffies = 0;
  for(CPUStates s: ActiveStates){
    activeJiffies += std::stof(cpuUtilization[s]);
  }
  return activeJiffies;
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  std::vector<std::string> cpuUtilization = LinuxParser::CpuUtilization();
  long idleJiffies = 0;
  for(CPUStates s: IdleStates){
    idleJiffies += std::stof(cpuUtilization[s]);
  }
  return idleJiffies;
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization(std::string cpuN) {
  std::string line;
  std::string key;
  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    while(std::getline(filestream,line)){
      std::istringstream iss(line);
      iss >> key;
      if (key == cpuN) {
        std::vector<std::string> results(std::istream_iterator<std::string> {iss},
                                        std::istream_iterator<std::string>());
        return results;
      }
    }
  }
  return std::vector<std::string> {};
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return stoi(value);
        }
      }
    }
  }
  return -1;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return stoi(value);
        }
      }
    }
  }
  return -1;
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  std::string line;
  std::string cmd;

  std::ifstream filestream(kProcDirectory + "/" 
                           + std::to_string(pid) +
                           kCmdlineFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      return line;
    }
  }
  return string{};
}

// DONE: Read and return the memory used by a process
long LinuxParser::Ram(int pid) {
  string line;
  string key;
  string value;

  std::ifstream filestream(kProcDirectory + "/" + 
                          std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::stringstream linestream(line);

      linestream >> key >> value;
      if (key == "VmSize:") return std::stol(value)/1000;// transform from kb to Mb
    }
  }
  return -1;
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  std::string line;
  std::string key;
  std::string value;

  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::stringstream linestream(line);
      linestream >> key >> value;

      if (key == "Uid:") return value;
    }
  }
  return string{};
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  std::string uid = Uid(pid);

  std::string line;
  std::string key;
  std::string value;

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::stringstream linestream(line);
      linestream >> value >> key >> key;
      if (key == uid) {
        return value;
      }
    }
  }
  return string();
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  long uptime_clockticks;
  std::string line;

  std::ifstream filestream(kProcDirectory + "/" +
                           std::to_string(pid) +
                           kStatFilename);

  if(filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream iss(line);

    std::vector<std::string> v(std::istream_iterator<std::string> {iss},
        std::istream_iterator<std::string> ());

    uptime_clockticks = std::stol(v[processStats::kStartTime]);

    return uptime_clockticks / sysconf(_SC_CLK_TCK); //return value in seconds
  }
  return -1;
}
