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

#ifndef REVOLVEBRAIN_BRAIN_CONTROLLER_EXTNN_BIASNEURON_H_
#define REVOLVEBRAIN_BRAIN_CONTROLLER_EXTNN_BIASNEURON_H_

#include <map>
#include <string>

#include "ENeuron.h"
#include "NeuralConnection.h"

namespace revolve
{
  namespace brain
  {
    class BiasNeuron
            : public Neuron
    {
      public:
      /// \brief  Constructor for a bias neuron
      /// \param _id: string to identify the neuron
      /// \param _parameters: parameters that specify the behavior of the neuron
      /// \return pointer to the bias neuron
      BiasNeuron(
              const std::string &_id,
              const std::map< std::string, double > &_parameters);

      /// \brief  Method to calculate the output of the neuron
      /// \param t: current time
      /// \return the output of the neuron at time t
      double Output(const double t) override;

      /// \brief
      std::map< std::string, double > Parameters() override;

      /// \brief
      void SetParameters(std::map< std::string, double > _parameters) override;

      /// \brief
      std::string Type() override;

      /// \brief bias of the neuron
      protected:
      double bias_;
    };
  }
}

#endif  //  REVOLVEBRAIN_BRAIN_CONTROLLER_EXTNN_BIASNEURON_H_
