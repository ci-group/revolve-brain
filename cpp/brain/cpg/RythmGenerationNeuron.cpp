/*
* Copyright (C) 2015-2017 Vrije Universiteit Amsterdam
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
* Author: Matteo De Carlo
* Date: November 1, 2016
*
*/

#include <cmath>
#include <vector>

#include "RythmGenerationNeuron.h"

using namespace revolve::brain::cpg;
namespace rbc = revolve::brain::cpg;

/////////////////////////////////////////////////
RythmGenerationNeuron::RythmGenerationNeuron(
        real_t weight,
        std::vector< real_t > weight_neigbours,
        real_t c,
        real_t amplitude,
        real_t offset
)
        : phi(1)
        , weight_neigbours(weight_neigbours)
{
  setWeight(weight);
  setC(c);
  setAmplitude(amplitude);
  setOffset(offset);
}

/////////////////////////////////////////////////
RythmGenerationNeuron::~RythmGenerationNeuron()
{
}

/////////////////////////////////////////////////
std::vector< real_t > RythmGenerationNeuron::update(
        std::vector< real_t > inputs,
        real_t delta_time)
{
  // reading neuron inputs
  if (inputs.size() not_eq 1 + weight_neigbours.size())
  {
    std::stringstream ss;
    ss << "input size should be 1 + " << weight_neigbours.size()
       << ", instead is " << inputs.size();
    throw invalid_input_exception(ss.str());
  }
  //  real_t otherPhi = inputs[0];

  // creating output value from previous phi
  real_t _output = output();

  // updating phi to the new value
  // NOTE we update phi after getting the output value because this way both
  // phi and other phi are of the same cycle.
  phi = nextPhi(inputs, delta_time);

  return {_output, phi};
}

/////////////////////////////////////////////////
real_t RythmGenerationNeuron::nextPhi(
        const std::vector< real_t > &inputs,
        real_t delta_time) const
{
  static const real_t PI = std::acos(-1);  // define PI

  const real_t thisPhi = this->phi;
  const real_t otherPhi = inputs[0];

  // (2 * pi * c') + w * sin(otherPhi-thisPhi)
  real_t deltaPhi = 2 * PI * this->c
                    + this->weight * std::sin(otherPhi - thisPhi);

  for (size_t i = 1; i < inputs.size(); ++i)
  {
    const real_t otherPhi = inputs[i];
    deltaPhi += weight_neigbours[i - 1] * std::sin(otherPhi - thisPhi);
  }

  // consider delta_time
  deltaPhi *= delta_time;

  // create phi(t+1)
  real_t new_phi = thisPhi + deltaPhi;

  // TODO compensate for phi error
  //    if (new_phi > PI*2)
  //        new_phi -= PI*2;
  //    else if (new_phi < -PI*2)
  //        new_phi += PI*2;
  return new_phi;
}

/////////////////////////////////////////////////
real_t RythmGenerationNeuron::output() const
{
  // A * cos(phi) + o
  return (this->amplitude * std::cos(this->phi)) + this->offset;
}

// GETTERS AND SETTERS --------------------------------------------------------

/////////////////////////////////////////////////
real_t RythmGenerationNeuron::Phi() const
{
  return phi;
}

/////////////////////////////////////////////////
real_t RythmGenerationNeuron::Weight() const
{
  return weight;
}

/////////////////////////////////////////////////
real_t RythmGenerationNeuron::WeightNeighbour(size_t index) const
{
  return weight_neigbours[index];
}

/////////////////////////////////////////////////
void RythmGenerationNeuron::setWeight(real_t weight)
{
  if (weight < WEIGHT_MIN or weight > WEIGHT_MAX)
  {
    throw invalid_parameter("weight", weight, WEIGHT_MIN, WEIGHT_MAX);
  }
  RythmGenerationNeuron::weight = weight;
}

// the weight that determines the shape [0, 1.0]
/////////////////////////////////////////////////
void RythmGenerationNeuron::setWeightNeighbour(
        real_t weight,
        size_t index)
{
  if (weight < WEIGHT_MIN or weight > WEIGHT_MAX)
  {
    throw invalid_parameter("weight", weight, WEIGHT_MIN, WEIGHT_MAX);
  }
  RythmGenerationNeuron::weight_neigbours[index] = weight;
}

/////////////////////////////////////////////////
real_t RythmGenerationNeuron::C() const
{
  return c;
}

/////////////////////////////////////////////////
void RythmGenerationNeuron::setC(real_t c)
{
  if (c < C_MIN or c > C_MAX)
  {
    throw invalid_parameter("c", c, C_MIN, C_MAX);
  }
  RythmGenerationNeuron::c = c;
}

/////////////////////////////////////////////////
real_t RythmGenerationNeuron::Amplitude() const
{
  return amplitude;
}

/////////////////////////////////////////////////
void RythmGenerationNeuron::setAmplitude(real_t amplitude)
{
  if (amplitude < AMPLITUDE_MIN or amplitude > AMPLITUDE_MAX)
  {
    throw invalid_parameter("amplitude",
                            amplitude,
                            AMPLITUDE_MIN,
                            AMPLITUDE_MAX);
  }
  RythmGenerationNeuron::amplitude = amplitude;
}

/////////////////////////////////////////////////
real_t RythmGenerationNeuron::Offset() const
{
  return offset;
}

/////////////////////////////////////////////////
void RythmGenerationNeuron::setOffset(real_t offset)
{
  if (offset < OFFSET_MIN or offset > OFFSET_MAX)
  {
    throw invalid_parameter("offset", offset, OFFSET_MIN, OFFSET_MAX);
  }
  RythmGenerationNeuron::offset = offset;
}


// FROM PERCENTAGE ------------------------------------------------------------
/////////////////////////////////////////////////
real_t rbc::RythmGenerationNeuron::calculateWeightFromPercentage(
        real_t weight) const
{
  return percentage_from_range(weight, WEIGHT_MIN, WEIGHT_MAX);
}

/////////////////////////////////////////////////
real_t rbc::RythmGenerationNeuron::calculateWeightNeighbourFromPercentage(
        real_t /*weight*/,
        size_t index) const
{
  return percentage_from_range(weight_neigbours[index], WEIGHT_MIN, WEIGHT_MAX);
}

/////////////////////////////////////////////////
real_t rbc::RythmGenerationNeuron::calculateCFromPercentage(
        real_t c) const
{
  return percentage_from_range(c, C_MIN, C_MAX);
}

/////////////////////////////////////////////////
real_t rbc::RythmGenerationNeuron::calculateAmplitudeFromPercentage(
        real_t amplitude) const
{
  return percentage_from_range(amplitude, AMPLITUDE_MIN, AMPLITUDE_MAX);
}

/////////////////////////////////////////////////
real_t
rbc::RythmGenerationNeuron::calculateOffsetFromPercentage(real_t offset) const
{
  return percentage_from_range(offset, OFFSET_MIN, OFFSET_MAX);
}


// TO PERCENTAGE --------------------------------------------------------------
/////////////////////////////////////////////////
real_t rbc::RythmGenerationNeuron::calculateWeightPercentage(
        real_t weight) const
{
  return percentage_of_range(weight, WEIGHT_MIN, WEIGHT_MAX);
}

/////////////////////////////////////////////////
real_t rbc::RythmGenerationNeuron::calculateWeightNeighbourPercentage(
        real_t /*weight*/,
        size_t index) const
{
  return percentage_of_range(weight_neigbours[index], WEIGHT_MIN, WEIGHT_MAX);
}

/////////////////////////////////////////////////
real_t rbc::RythmGenerationNeuron::calculateCPercentage(
        real_t c) const
{
  return percentage_of_range(c, C_MIN, C_MAX);
}

/////////////////////////////////////////////////
real_t rbc::RythmGenerationNeuron::calculateAmplitudePercentage(
        real_t amplitude) const
{
  return percentage_of_range(amplitude, AMPLITUDE_MIN, AMPLITUDE_MAX);
}

/////////////////////////////////////////////////
real_t rbc::RythmGenerationNeuron::calculateOffsetPercentage(
        real_t offset) const
{
  return percentage_of_range(offset, OFFSET_MIN, OFFSET_MAX);
}

/////////////////////////////////////////////////
real_t RythmGenerationNeuron::setWeightPercentage(real_t weight)
{
  this->weight = calculateWeightFromPercentage(weight);
  return this->weight;
}

/////////////////////////////////////////////////
real_t RythmGenerationNeuron::setWeightNeighbourPercentage(
        real_t weight,
        size_t index)
{
  this->weight_neigbours[index] = calculateWeightFromPercentage(weight);
  return this->weight_neigbours[index];
}

/////////////////////////////////////////////////
real_t RythmGenerationNeuron::setCPercentage(real_t c)
{
  this->c = calculateCFromPercentage(c);
  return this->c;
}

/////////////////////////////////////////////////
real_t RythmGenerationNeuron::setAmplitudePercentage(real_t amplitude)
{
  this->amplitude = calculateAmplitudeFromPercentage(amplitude);
  return this->amplitude;
}

/////////////////////////////////////////////////
real_t RythmGenerationNeuron::setOffsetPercentage(real_t offset)
{
  this->offset = calculateOffsetFromPercentage(offset);
  return this->offset;
}
