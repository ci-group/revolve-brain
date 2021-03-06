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

#include <vector>

#include "staticexperiment.h"

using namespace NEAT;

static struct XorInit
{
  XorInit()
  {
    create_static_experiment("xor",
                             []()
                             {
                               const real_t T = 1.0;
                               const real_t F = 0.0;
                               const real_t weight = 1.0;

                               std::vector< Test > tests = {
                                       {{
                                                {{F, F}, {F}, weight},
                                        }},
                                       {{
                                                {{F, T}, {T}, weight},
                                        }},
                                       {{
                                                {{T, F}, {T}, weight},
                                        }},
                                       {{
                                                {{T, T}, {F}, weight}
                                        }}
                               };

                               return tests;
                             });
  }
} init;
