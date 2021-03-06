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

#ifndef CPP_NEAT_ACCNEAT_SRC_NETWORK_CUDA_CUDAUTIL_H_
#define CPP_NEAT_ACCNEAT_SRC_NETWORK_CUDA_CUDAUTIL_H_

#pragma once

#include <cstdio>
#include <iostream>
#include <vector>
#include "neattypes.h"

#define __net_eval_decl __host__ __device__

using namespace NEAT;

#define xcuda(stmt) {                                                         \
        cudaError_t err = stmt;                                               \
        if (err not_eq cudaSuccess) {                                         \
            std::cerr << __FILE__ << ":" << __LINE__ << ": Failed to run " << \
            #stmt << ". Reason: " << cudaGetErrorString(err) << std::endl;    \
            abort();                                                          \
        }                                                                     \
    }

#define p(msg) std::cout << "[cuda]: " << msg << std::endl

#define errif(STMT, MSG...) if ( STMT ) { fprintf(stderr, "[%s:%d] '%s' ",    \
                                          __FILE__, __LINE__, #STMT);         \
                                          fprintf(stderr, MSG);               \
                                          fprintf(stderr, "\n");              \
                                          abort(); }

#define require(STMT) if ( !(STMT) ) { fprintf(stderr,                        \
                                      "ASSERTION ERROR! [%s:%d] '%s'\n",      \
                                      __FILE__, __LINE__, #STMT);             \
                                      abort(); }

static uchar *alloc_host(uint size)
{
  uchar *result;
  xcuda(cudaMallocHost(reinterpret_cast<void **>(&result), size));
  return result;
}

static uchar *alloc_dev(uint size)
{
  uchar *result;
  xcuda(cudaMalloc(reinterpret_cast<void **>(&result), size));
  return result;
}

template < typename T >
static void free_host(
        accneat_inout
        T *&buf,
        bool tolerate_shutdown = false)
{
  if (buf)
  {
    cudaError_t err = cudaFreeHost(buf);

    if ((err == cudaSuccess)
        or (tolerate_shutdown and (err == cudaErrorCudartUnloading)))
    {
      buf = 0;
    }
    else
    {
      std::cerr << "Failed freeing cuda host buffer" << std::endl;
      abort();
    }
  }
}

template < typename T >
static void free_dev(T *&buf)
{
  if (buf)
  {
    xcuda(cudaFree(buf));
    buf = 0;
  }
}

template < typename T >
static void grow_buffers(
        accneat_inout
        T *&h_buf,
        accneat_inout
        T *&d_buf,
        accneat_inout
        uint &capacity,
        accneat_in uint newlen)
{
  if (newlen > capacity)
  {
    free_host(h_buf);
    free_dev(d_buf);

    capacity = newlen;

    h_buf = alloc_host(newlen);
    d_buf = alloc_dev(newlen);
  }
}

#endif
