#ifndef SERVAL_SDK__TIMELINE_HPP
#define SERVAL_SDK__TIMELINE_HPP

#include "types.hpp"

namespace serval {
    class Timeline;
}

/**
 * @brief Public interface for Timelines
 * 
 */
class serval::Timeline {
public:
    virtual ~Timeline() {}

    /**
     * @brief If the timeline is paused
     * 
     * @return true The timeline is paused
     * @return false The timeline is running
     */
    virtual bool paused () const = 0;

    /**
     * @brief Elapsed seconds since the timeline was started
     * 
     * @return serval::Scalar 
     */
    virtual serval::Scalar elapsed () const = 0;

    /**
     * @brief Calculate the time scaled by local_scale()
     * 
     * @param time Time to scale
     * @return serval::Scalar Scaled time_delta
     */
    virtual serval::Scalar scale (serval::Scalar time) const = 0;

    /**
     * @brief Elapsed seconds since last update (scaled by local_scale())
     * 
     * @return serval::Scalar 
     */
    virtual serval::Scalar delta () const = 0;

    /**
     * @brief The current timeline's scale factor
     * 
     * @return serval::Scalar 
     */
    virtual serval::Scalar local_scale () const = 0;

    /**
     * @brief The timeline's scale factor after applying all parent scaling
     * 
     * @return serval::Scalar 
     */
    virtual serval::Scalar absolute_scale () const = 0;
};

#endif