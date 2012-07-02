/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Task.hpp"
#include <ucm_schilling/Driver.hpp>
#include <ucm_schilling/UcmRaw.hpp>
#include <act_schilling/Error.hpp>
#include <base/logging.h>

#define ACT_UCM_AD

using namespace ucm_schilling;
using namespace oro_marum;

Task::Task(std::string const& name)
    : TaskBase(name),
    mDriver(0)
{
}

Task::Task(std::string const& name, RTT::ExecutionEngine* engine)
    : TaskBase(name, engine),
    mDriver(0)
{
}

Task::~Task()
{
  delete mDriver;
}



/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See Task.hpp for more detailed
// documentation about them.

 bool Task::configureHook()
 {
    try{
      delete mDriver;
      ucm_schilling::Config configuration = _config.get();
      mDriver = new ucm_schilling::Driver(configuration);
      if (!_io_port.get().empty())
      {
	  mDriver->open(_io_port.get());
      }

      setDriver(mDriver);
  
      if (! TaskBase::configureHook())
	  return false;
      return true;
    } catch(std::runtime_error &e){
      LOG_DEBUG("exception %s",e.what());
      error(COMM_ERROR);      
    }
 }

 bool Task::startHook()
 {
    try{
      if (! TaskBase::startHook())
	  return false;
      return true;
    } catch(std::runtime_error &e){
      LOG_DEBUG("exception %s",e.what());
      error(COMM_ERROR);      
    }
 }

 void Task::updateHook()
 {
    try{
      mDriver->collectData();
      _ucm_samples.write(mDriver->getData());
      statusCheck(mDriver->getStatus());
      TaskBase::updateHook();
    } catch(std::runtime_error &e){
      LOG_DEBUG("exception %s",e.what());
      error(COMM_ERROR);      
    }
 }
 
 void Task::errorHook()
 {
   TaskBase::errorHook();
   recover();
 }

 void Task::stopHook()
 {
   TaskBase::stopHook();
 }

void Task::cleanupHook()
{
    if(mDriver){
      mDriver->close();
    }
    TaskBase::cleanupHook();
}

void Task::statusCheck(const UcmStatus& status)
{
  if(status.status & UCM_STAT_FAIL_EEPROM){
    _log_message.write(LogMessage(Alarm, UCMSTR_EEPROM, UCMALARM_EEPROM));
  }
#ifdef ACT_UCM_AD
  if(status.status & UCM_STAT_FAIL_AD){
    _log_message.write(LogMessage(Alarm, UCMSTR_AD, UCMALARM_AD));
  }
#endif
#ifdef ACT_UCM_DA
  if(status.status & UCM_STAT_FAIL_DA){
    _log_message.write(LogMessage(Alarm, UCMSTR_DA, UCMALARM_DA));
  }
#endif
}