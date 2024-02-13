#ifndef SERVAL_SDK__EXTENSION_HPP
#define SERVAL_SDK__EXTENSION_HPP

#include "../api.hpp"

namespace serval {
    class ExtensionEvents;
}


class serval::ExtensionEvents {
public:
    virtual ~ExtensionEvents () {}

    /**
     * @brief The extension has been loaded
     * 
     * @param api 
     */
    virtual void onLoad (serval::Init& api) = 0;

    /**
     * @brief The extension is being reloaded
     * 
     * @param api 
     */
    virtual void onReload (serval::Init& api) = 0;

    /**
     * @brief The extension is about to be unloaded
     * 
     * @param api 
     */
    virtual void onUnload (serval::Init& api) = 0;
};

#endif