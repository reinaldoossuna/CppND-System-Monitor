#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {

  float diffIdle;
  float diffTotal;
  float diffUsage;

  actIdle = LinuxParser::IdleJiffies();
  actTotal = LinuxParser::Jiffies();

  diffIdle = actIdle - prevIdle;
  diffTotal = actTotal - prevTotal;

  diffUsage = (diffTotal - diffIdle)/diffTotal;

  prevIdle = actIdle;
  prevTotal = actTotal;

  return diffUsage;
}
