/*
 Copyright 2001 The University of Texas at Austin

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "trait.h"
#include <sstream>

using namespace NEAT;

Trait::Trait()
{
  for (size_t count = 0; count < NUM_TRAIT_PARAMS; count++)
    params[count] = 0;
  trait_id = 0;
}

Trait::Trait(int id,
             real_t p1,
             real_t p2,
             real_t p3,
             real_t p4,
             real_t p5,
             real_t p6,
             real_t p7,
             real_t p8,
             real_t p9)
{
  trait_id = id;
  params[0] = p1;
  params[1] = p2;
  params[2] = p3;
  params[3] = p4;
  params[4] = p5;
  params[5] = p6;
  params[6] = p7;
  params[7] = 0;
}

Trait::Trait(const Trait &t)
{
  for (int count = 0; count < NUM_TRAIT_PARAMS; count++)
    params[count] = (t.params)[count];

  trait_id = t.trait_id;
}


Trait::Trait(Trait *t)
{
  for (int count = 0; count < NUM_TRAIT_PARAMS; count++)
    params[count] = (t->params)[count];

  trait_id = t->trait_id;
}


Trait::Trait(const char *argline)
{

  std::stringstream ss(argline);
  //Read in trait id
  //   std::string curword;
  //char delimiters[] = " \n";
  //int curwordnum = 0;

  //strcpy(curword, env->getUnit(argline, curwordnum++, delimiters));

//    trait_id = atoi(curword);
  ss >> trait_id;

  //std::cout << ss.str() << " trait_id: " << trait_id << std::endl;

  //IS THE STOPPING CONDITION CORRECT?  ALERT
  for (int count = 0; count < NUM_TRAIT_PARAMS; count++) {
    //strcpy(curword, env->getUnit(argline, curwordnum++, delimiters));
    //params[count] = atof(curword);
    ss >> params[count];
    //iFile>>params[count];
  }

}

Trait::Trait(const Trait &t1,
             const Trait &t2)
{
  trait_id = t1.trait_id;
  for (int count = 0; count < NUM_TRAIT_PARAMS; count++)
    params[count] = (t1.params[count] + t2.params[count]) / 2.0;
}

void Trait::print_to_file(std::ostream &outFile) const {
  outFile << "trait " << trait_id << " ";
  for (int count = 0; count < NUM_TRAIT_PARAMS; count++)
    outFile << params[count] << " ";

  outFile << std::endl;
}

void
Trait::mutate(rng_t &rng)
{
  for (int count = 0; count < NUM_TRAIT_PARAMS; count++) {
    if (rng.prob() > env->trait_param_mut_prob) {
      params[count] += (rng.posneg() * rng.prob()) * env->trait_mutation_power;
      if (params[count] < 0) params[count] = 0;
      if (params[count] > 1.0) params[count] = 1.0;
    }
  }
}

bool YAML::convert<NEAT::Trait>::decode(const YAML::Node &node, NEAT::Trait &rhs)
{
  rhs.trait_id = node["id"].as<int>();
  Node yaml_params = node["params"];

  if (not yaml_params.IsSequence() || yaml_params.size() != NUM_TRAIT_PARAMS) {
    return false;
  }

  for (int count = 0; count < NUM_TRAIT_PARAMS; count++)
    rhs.params[count] = yaml_params[count].as<real_t>();

  return true;
}

YAML::Node YAML::convert<NEAT::Trait>::encode(const NEAT::Trait &rhs)
{
  Node node;
  node["id"] = rhs.trait_id;
  Node yaml_params = node["params"];
  for (int count = 0; count < NUM_TRAIT_PARAMS; count++)
    yaml_params.push_back(rhs.params[count]);

  return node;
}

