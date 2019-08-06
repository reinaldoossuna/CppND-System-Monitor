#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

namespace LinuxParser {
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_ ,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};

const std::vector<CPUStates> ActiveStates{kUser_,kNice_,kSystem_,kIRQ_,kSoftIRQ_,kSteal_};
const std::vector<CPUStates> IdleStates{kIdle_,kIOwait_};

std::vector<std::string> CpuUtilization(std::string = "cpu");
long Jiffies();
long ActiveJiffies();
long ActiveJiffies(int pid);
long IdleJiffies();

// Processes
enum processStats {
  kUtime = 13,
  kStime,
  kCutime,
  kCstime,
  kStartTime = 21
};

const std::vector<processStats> allProcessStats {kUtime,kStime,kCutime,kCstime};
std::string Command(int pid);
long Ram(int pid);   
std::string Uid(int pid);
std::string User(int pid);
long int UpTime(int pid);

};  // namespace LinuxParser

#endif
