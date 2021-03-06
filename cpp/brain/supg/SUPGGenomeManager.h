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

#ifndef REVOLVEBRAIN_BRAIN_SUPGG_SUPGGENOMEMANAGER_H_
#define REVOLVEBRAIN_BRAIN_SUPGG_SUPGGENOMEMANAGER_H_

#include <string>
#include <vector>

#include "innovgenome/innovgenomemanager.h"

class SUPGGenomeManager
        : public NEAT::InnovGenomeManager
{
  public:
  SUPGGenomeManager(const std::string &robot_name);

  virtual std::vector< std::unique_ptr< NEAT::Genome>>
  create_seed_generation(
          size_t ngenomes,
          NEAT::rng_t rng,
          size_t ntraits,
          size_t ninputs,
          size_t noutputs,
          size_t nhidden,
          const std::string &robot_name) override;
};

#endif  //  REVOLVEBRAIN_BRAIN_SUPGG_SUPGGENOMEMANAGER_H_
