/* Generated from orogen/lib/orogen/templates/tasks/DisplayTask.cpp */

#include "DisplayTask.hpp"
#include <ucm_schilling/Driver.hpp>
#include <ucm_schilling/UcmRaw.hpp>
#include <base_schilling/Error.hpp>
#include <base-logging/Logging.hpp>

#define ACT_UCM_AD

using namespace ucm_schilling;
using namespace oro_marum;

DisplayTask::DisplayTask(std::string const& name)
    : DisplayTaskBase(name),
    mDriver(0)
{
}

DisplayTask::DisplayTask(std::string const& name, RTT::ExecutionEngine* engine)
    : DisplayTaskBase(name, engine),
    mDriver(0)
{
}

DisplayTask::~DisplayTask()
{
  delete mDriver;
}



/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See DisplayTask.hpp for more detailed
// documentation about them.

bool DisplayTask::configureHook()
{
  delete mDriver;
  ucm_schilling::Config configuration = _config.get();
  mDriver = new ucm_schilling::Driver(configuration);
  if (!_io_port.get().empty())
  {
    mDriver->open(_io_port.get());
  }
  setDriver(mDriver);

  mBufferTimeMin = _bufferTimeMin.get();
  mBufferFrequency = _bufferFrequency.get();
  mRingBuffer = new boost::circular_buffer<UcmData>(mBufferTimeMin*mBufferFrequency);

  if (! DisplayTaskBase::configureHook())
    return false;
  return true;
}

 bool DisplayTask::startHook()
 {
	mDriver->setReadTimeout(_io_read_timeout.get());
	mDriver->setWriteTimeout(_io_write_timeout.get());
    if (! DisplayTaskBase::startHook())
	return false;
    return true;
 }

 void DisplayTask::updateHook()
 {
   run();
 }
 
 void DisplayTask::errorHook()
 {
   run();
   DisplayTaskBase::errorHook();
 }

 void DisplayTask::stopHook()
 {
   DisplayTaskBase::stopHook();
 }

void DisplayTask::cleanupHook()
{
    if(mDriver){
      mDriver->close();
    }
    DisplayTaskBase::cleanupHook();
}

void DisplayTask::statusCheck(const UcmStatus& status)
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

void DisplayTask::processIO()
{
  mDriver->collectData();
  UcmData data = mDriver->getData();
  _ucm_samples.write(data);
  mRingBuffer->push_back(data);

  UcmStatus status = mDriver->getStatus();
  statusCheck(status);
  _ucm_status.write(status);
}

void DisplayTask::run()
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

void DisplayTask::init_gui_request(){
	ucm_schilling::UcmData *dataP = mRingBuffer->linearize();
	unsigned int size = mRingBuffer->size();
	std::vector<ucm_schilling::UcmData> ucmData = std::vector<ucm_schilling::UcmData>(dataP, (dataP + size-1));
	const ucm_schilling::UcmInitData ucmInit(mBufferTimeMin,mBufferFrequency,ucmData);
	_ucm_init.write(ucmInit);
}

void DisplayTask::set_calib_values(const ucm_schilling::Config & arg){
	_config.set(arg);
	mDriver->setConfig(arg);
}
