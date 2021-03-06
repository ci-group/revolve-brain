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

#include <algorithm>

#include "network/network.h"
#include "multinnspeciesorganism.h"
#include "multinnspecies.h"

using namespace NEAT;
using namespace std;

MultiNNSpeciesOrganism::MultiNNSpeciesOrganism(
        const MultiNNSpeciesOrganism &other)
{
  this->genome = env->genome_manager->make_default();
  this->net = unique_ptr< Network >(Network::create());
  other.copy_into(*this);
}

MultiNNSpeciesOrganism::MultiNNSpeciesOrganism(const Genome &genome)
{
  this->genome = env->genome_manager->make_default();
  *this->genome = genome;
  this->net = unique_ptr< Network >(Network::create());
  init(0);
}

MultiNNSpeciesOrganism::~MultiNNSpeciesOrganism()
{
}

void MultiNNSpeciesOrganism::init(int gen)
{
  Organism::init(gen);

  species = nullptr;  // Start it in no MultiNNSpecies
  adjusted_fitness = 0.0;
  expected_offspring = 0;
  eliminate = false;
  champion = false;
  super_champ_offspring = 0;
}

void MultiNNSpeciesOrganism::copy_into(Organism &dst_) const
{
  Organism::copy_into(dst_);

  MultiNNSpeciesOrganism *dst = dynamic_cast<MultiNNSpeciesOrganism *>(&dst_);

#define copy(field) dst->field = this->field;

  copy(species);
  copy(adjusted_fitness);
  copy(expected_offspring);
  copy(eliminate);
  copy(champion);
  copy(super_champ_offspring);

#undef copy
}
