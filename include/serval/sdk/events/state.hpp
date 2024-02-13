#ifndef SERVAL_SDK__EVENTS_STATE_HPP
#define SERVAL_SDK__EVENTS_STATE_HPP

#ifndef MIND_ENGINE_STATE_H_
#define MIND_ENGINE_STATE_H_

#include "../types.hpp"

namespace serval {
    class StateEvents;

    class Runtime;
    class Attributes;
}


/**
 * @brief Public interface for game State events
 * 
 */
class serval::StateEvents {
public:
    virtual ~StateEvents() {}

    /**
     * @brief Event handler called when the scene is entered
     * (becomes the active scene)
     * 
     * @param api A reference to the runtime API
     * @param attributes Attributes set for the game state
     */
    virtual void onEnter (serval::Runtime& api, const serval::Attributes& attributes) = 0;

    /**
     * @brief Event handler called when the scene is left
     * (another scene becomes the acitve scene when this one was previously active)
     * 
     * @param api A reference to the runtime API
     */
    virtual void onLeave (serval::Runtime& api) = 0;

    /**
     * @brief Event handler called when game loading is about to begin
     * Derived classes should also call the base method before their implementation
     * 
     * @param api A reference to the runtime API
     */
    virtual void onLoadBegin (serval::Runtime& api) = 0;

    /**
     * @brief Event handler called when the game is being loaded
     * Derived classes should also call the base method before their implementation
     * 
     * @param api A reference to the runtime API
     * @param reader
     */
    virtual void onLoad (serval::Runtime& api, serval::Reader& reader) = 0;

    /**
     * @brief Event handler called when game loading has ended
     * Derived classes should also call the base method after their implementation
     * 
     * @param api A reference to the runtime API
     */
    virtual void onLoadEnd (serval::Runtime& api) = 0;

    /**
     * @brief Event handler called when game saving is about to begin
     * Derived classes should also call the base method before their implementation
     * 
     * @param api A reference to the runtime API
     */
    virtual void onSaveBegin (serval::Runtime& api) = 0;

    /**
     * @brief Event handler called when the game is being saved
     * Derived classes should also call the base method before their implementation
     * 
     * @param api A reference to the runtime API
     * @param writer
     */
    virtual void onSave (serval::Runtime& api, serval::Writer& writer) = 0;
    
    /**
     * @brief Event handler called when game saving has ended
     * Derived classes should also call the base method after their implementation
     * 
     * @param api A reference to the runtime API
     */
    virtual void onSaveEnd (serval::Runtime& api) = 0;
};

#endif

#endif