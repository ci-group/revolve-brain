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

#ifndef CPP_NEAT_ACCNEAT_SRC_NETWORK_CPU_CPUNETWORK_H_
#define CPP_NEAT_ACCNEAT_SRC_NETWORK_CPU_CPUNETWORK_H_

#pragma once

#include <vector>

#include "network/network.h"

namespace NEAT
{
  ///
  /// CLASS CpuNetwork
  ///
  class CpuNetwork
          : public Network
  {
    private:
    NetDims dims;
    std::vector< NetNode > nodes;
    std::vector< NetLink > links;
    std::vector< real_t > activations;

    public:
    CpuNetwork()
    {}

    virtual ~CpuNetwork()
    {}

    void activate(size_t ncycles);

    std::vector< real_t > &get_activations(
            accneat_out
            std::vector< real_t > &result);

    void set_activations(accneat_in std::vector< real_t > &newacts);

    void clear_noninput();

    void load_sensor(
            size_t isensor,
            real_t activation);

    real_t *Outputs();

    virtual void configure(
            const NetDims &dims,
            NetNode *nodes,
            NetLink *links);

    virtual NetDims get_dims()
    {
      return dims;
    }
  };
}

#endif
