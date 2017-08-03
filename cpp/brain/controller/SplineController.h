/*
* Copyright (C) 2017 Vrije Universiteit Amsterdam
*
* Licensed under the Apache License, Version 2.0 (the "License");
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Description: TODO: <Add brief description about file purpose>
* Author: TODO <Add proper author>
*
*/

#ifndef REVOLVEBRAIN_BRAIN_CONTROLLER_SPLINECONTROLLER_H_
#define REVOLVEBRAIN_BRAIN_CONTROLLER_SPLINECONTROLLER_H_

#include <vector>

#include "BaseController.h"

namespace revolve {
namespace brain {

class BaseLearner;

class SplineController
        : public BaseController
{
public: // typedefs
    typedef std::vector<double> Spline;
    typedef std::vector<Spline> Policy;
    typedef std::shared_ptr<Policy> PolicyPtr;

    friend class BaseLearner;

// METHODS-FUNCTIONS ----------------------------------------------------------
public: // STATIC METHODS
    /**
    * Generate interpolated spline based on number of sampled control points in 'source_y'
    /// \param source_y: set of control points over which interpolation is generated
    /// \param destination_y: set of interpolated control points (default 100 points)
    */
    static void
    Interpolate_cubic(Policy *const source_y,
                      Policy *destination_y);

    static SplineController *
    GenerateRandomController(double noise_sigma,
                             unsigned int n_actuators,
                             unsigned int n_spline_points,
                             unsigned int interpolation_cache_size);

    static SplineController *
    GenerateRandomController(double noise_sigma,
                             unsigned int n_actuators,
                             unsigned int n_spline_points)
    {
      return GenerateRandomController(noise_sigma,
                                      n_actuators,
                                      n_spline_points,
                                      INTERPOLATION_CACHE_SIZE);
    }

public: // methods
    explicit SplineController(unsigned int n_actuators,
                              unsigned int n_spline_points,
                              unsigned int interpolation_cache_size);

    explicit SplineController(unsigned int n_actuators,
                              unsigned int n_spline_points);

    virtual ~SplineController();

    virtual void
    update(const std::vector<ActuatorPtr> &actuators,
           const std::vector<SensorPtr> &sensors,
           double t,
           double step) override;

    virtual void
    generateOutput(const double time,
                   double *output_vector);

    /**
     * Generate cache policy
     */
    void
    update_cache();

// VARIABLES-CONSTANTS --------------------------------------------------------
public: // STATIC CONSTANTS
    // cycle lenght in seconds
    static const double CYCLE_LENGTH;// = 5 seconds
    /** Default value for the number of `interpolation_cache` data points
     * (default value for `interpolation_cache_size`)
     */
    static const unsigned int INTERPOLATION_CACHE_SIZE;// = 100

protected: // consts
    // number of actuators the spline is expeting to write into
    const unsigned int n_actuators;
    // number of spline points
    const unsigned int n_spline_points;
    // Number of `interpolation_cache` data points
    const unsigned int interpolation_cache_size;

protected: // variables
    // Pointer to the current policy
    PolicyPtr policy;
    // Pointer to the interpolated current_policy_ (default 100 points)
    PolicyPtr interpolation_cache = nullptr;
    double cycle_start_time;
};

}
}

#endif  //  REVOLVEBRAIN_BRAIN_CONTROLLER_SPLINECONTROLLER_H_
