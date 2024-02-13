#ifndef SERVAL_SDK__EVENTS_SYSTEM_HPP
#define SERVAL_SDK__EVENTS_SYSTEM_HPP

#include "../types.hpp"

namespace serval {
    class SystemEvents;

    class Timeline;
    class SystemSetup;
    class Runtime;
}

/**
 * @brief Public interface for a system's events
 * 
 */
class serval::SystemEvents {
public:
    virtual ~SystemEvents() {}

    /**
     * @brief Event handler called after the system is first created
     * 
     * @param api The SystemSetup api object
     */
    virtual void onCreate (serval::SystemSetup& api) = 0;
    
    /**
     * @brief Event handler called before the system is destroyed
     * 
     */
    virtual void onDestroy (serval::SystemSetup& api) = 0;

    /**
     * @brief Event handler called when the system should reset its internal state
     * 
     */
    virtual void onReset (serval::SystemSetup& api) = 0;

    /**
     * @brief Event handler called when the system has been marked as actively running
     * 
     */
    virtual void onActivate (serval::Runtime& api) = 0;

    /**
     * @brief Event handler called when the system has been marked as no longer actively running
     * 
     */
    virtual void onDeactivate (serval::Runtime& api) = 0;

    /**
     * @brief Event handler called when the game is being loaded
     * Called on both active and inactive systems
     * 
     * @param api A reference to the runtime API
     * @param reader
     */
    virtual void onLoad (serval::Runtime& api, serval::Reader& reader) = 0;

    /**
     * @brief Event handler called when the game is being saved
     * Called on both active and inactive systems
     * 
     * @param api A reference to the runtime API
     * @param writer
     */
    virtual void onSave (serval::Runtime& api, serval::Writer& writer) = 0;
};

#endif