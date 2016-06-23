/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Task.hpp"
#include <ucm_schilling/Driver.hpp>
#include <ucm_schilling/UcmRaw.hpp>
#include <base_schilling/Error.hpp>
#include <base/Logging.hpp>

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
}

 bool Task::startHook()
 {
    if (! TaskBase::startHook())
	return false;
    return true;
 }

 void Task::updateHook()
 {
   run();
 }
 
 void Task::errorHook()
 {
   run();
   TaskBase::errorHook();
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
  bool bDevError = false;
  if(status.status & UCM_STAT_FAIL_EEPROM){
    _log_message.write(LogMessage(Alarm, UCMSTR_EEPROM, UCMALARM_EEPROM));
    bDevError = true;
  }
#ifdef ACT_UCM_AD
  if(status.status & UCM_STAT_FAIL_AD){
    _log_message.write(LogMessage(Alarm, UCMSTR_AD, UCMALARM_AD));
    bDevError = true;
  }
#endif
#ifdef ACT_UCM_DA
  if(status.status & UCM_STAT_FAIL_DA){
    _log_message.write(LogMessage(Alarm, UCMSTR_DA, UCMALARM_DA));
    bDevError = true;
  }
#endif
  if(bDevError){
    error(DEV_ERROR);
  }
  else{
    if(DEV_ERROR == state()){
      recover();
    }
  }
}

void Task::processIO()
{
  mDriver->collectData();
  _ucm_samples.write(mDriver->getData());
  UcmStatus status = mDriver->getStatus();
  statusCheck(status);
  _ucm_status.write(status);
}

void Task::run()
{
    try{
      switch(state()){
	case RUNNING: {
	  processIO();
	  state(MONITORING);
	  break;
	}
	case MONITORING:{
	  processIO();
	  break;
	}
	default: break;
      }      
    } catch(std::runtime_error &e){
      LOG_DEBUG("exception %s",e.what());
      _log_message.write(LogMessage(e));
      exception(IO_TIMEOUT);  
    }

}

