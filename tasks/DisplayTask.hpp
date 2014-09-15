/* Generated from orogen/lib/orogen/templates/tasks/DisplayTask.hpp */

#ifndef UCM_SCHILLING_DISPLAYTASK_HPP
#define UCM_SCHILLING_DISPLAYTASK_HPP

#include <boost/circular_buffer.hpp>
#include "ucm_schilling/DisplayTaskBase.hpp"

namespace ucm_schilling {
    class Driver;
    /*! \class DisplayTask
     * \brief The task context provides and requires services. It uses an ExecutionEngine to perform its functions.
     * Essential interfaces are operations, data flow ports and properties. These interfaces have been defined using the oroGen specification.
     * In order to modify the interfaces you should (re)use oroGen and rely on the associated workflow.
     * 
     * \details
     * The name of a DisplayTaskContext is primarily defined via:
     \verbatim
     deployment 'deployment_name'
         task('custom_task_name','ucm_schilling::DisplayTask')
     end
     \endverbatim
     *  It can be dynamically adapted when the deployment is called with a prefix argument. 
     */
    class DisplayTask : public DisplayTaskBase
    {
	friend class DisplayTaskBase;
    protected:
      Driver* mDriver;



    public:
        /** DisplayTaskContext constructor for DisplayTask
         * \param name Name of the task. This name needs to be unique to make it identifiable via nameservices.
         * \param initial_state The initial DisplayTaskState of the DisplayTaskContext. Default is Stopped state.
         */
        DisplayTask(std::string const& name = "ucm_schilling::DisplayTask");

        /** DisplayTaskContext constructor for DisplayTask
         * \param name Name of the task. This name needs to be unique to make it identifiable for nameservices. 
         * \param engine The RTT Execution engine to be used for this task, which serialises the execution of all commands, programs, state machines and incoming events for a task. 
         * 
         */
        DisplayTask(std::string const& name, RTT::ExecutionEngine* engine);

        /** Default deconstructor of DisplayTask
         */
	~DisplayTask();

        /** This hook is called by Orocos when the state machine transitions
         * from PreOperational to Stopped. If it returns false, then the
         * component will stay in PreOperational. Otherwise, it goes into
         * Stopped.
         *
         * It is meaningful only if the #needs_configuration has been specified
         * in the task context definition with (for example):
         \verbatim
         task_context "DisplayTaskName" do
           needs_configuration
           ...
         end
         \endverbatim
         */
        bool configureHook();

        /** This hook is called by Orocos when the state machine transitions
         * from Stopped to Running. If it returns false, then the component will
         * stay in Stopped. Otherwise, it goes into Running and updateHook()
         * will be called.
         */
        bool startHook();

        /** This hook is called by Orocos when the component is in the Running
         * state, at each activity step. Here, the activity gives the "ticks"
         * when the hook should be called.
         *
         * The error(), exception() and fatal() calls, when called in this hook,
         * allow to get into the associated RunTimeError, Exception and
         * FatalError states. 
         *
         * In the first case, updateHook() is still called, and recover() allows
         * you to go back into the Running state.  In the second case, the
         * errorHook() will be called instead of updateHook(). In Exception, the
         * component is stopped and recover() needs to be called before starting
         * it again. Finally, FatalError cannot be recovered.
         */
        void updateHook();

        /** This hook is called by Orocos when the component is in the
         * RunTimeError state, at each activity step. See the discussion in
         * updateHook() about triggering options.
         *
         * Call recover() to go back in the Runtime state.
         */
         void errorHook();

        /** This hook is called by Orocos when the state machine transitions
         * from Running to Stopped after stop() has been called.
         */
        void stopHook();

        /** This hook is called by Orocos when the state machine transitions
         * from Stopped to PreOperational, requiring the call to configureHook()
         * before calling start() again.
         */
         void cleanupHook();
	 
	 void statusCheck(const UcmStatus& status);
	 
	 void processIO();
	 
	 void run();

	 void init_gui_request();
	 void set_calib_values(const ucm_schilling::Config & arg);

    private:
	 boost::circular_buffer<UcmData> *mRingBuffer;
     int mBufferTimeMin;
     int mBufferFrequency;
    };
}

#endif

