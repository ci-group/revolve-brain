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

#ifndef REVOLVE_BRAIN_RLPOWER_PYTHON_H
#define REVOLVE_BRAIN_RLPOWER_PYTHON_H

#include <string>

#include <boost/python/object.hpp>
#include <boost/python/list.hpp>

#include "brain/RLPower.h"

namespace revolve
{
  namespace brain
  {
    class RLPower_python
            : public revolve::brain::RLPower
    {
      public:
      /// \brief
      RLPower_python(std::string robot_name,
                     const boost::python::object &conf,
                     revolve::brain::EvaluatorPtr evaluator,
                     size_t n_actuators,
                     size_t n_sensors);

      /// \brief
      void update(boost::python::list &actuators,
                  const boost::python::list &sensors,
                  double t,
                  double step);

      /// \brief
      static RLPower::Config create_config(const boost::python::object &conf);
    };
  }
}

#endif  //  REVOLVE_BRAIN_RLPOWER_PYTHON_H
