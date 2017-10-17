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

#ifndef CPP_NEAT_ACCNEAT_SRC_INNOVGENOME_INNOVGENOME_H_
#define CPP_NEAT_ACCNEAT_SRC_INNOVGENOME_INNOVGENOME_H_

#pragma once

#include <string>
#include <vector>

#include "genome.h"
#include "innovlinkgene.h"
#include "innovnodegene.h"
#include "innovnodelookup.h"
#include "innovation.h"

namespace NEAT
{
  class InnovGenome
          : public Genome
  {
    public:
    /// \brief
    std::vector<Trait> traits;

    /// \brief
    std::vector<InnovNodeGene> nodes;

    /// \brief
    std::vector<InnovLinkGene> links;

    /// \brief Return id of final InnovNodeGene in InnovGenome
    int LastNodeId();

    /// \brief Return last innovation number in InnovGenome
    real_t LastGeneInnovationNumber();

    /// \brief
    InnovGenome(const InnovGenome &other)
            : InnovGenome(other.robot_name)
    {}

    /// \brief
    InnovGenome(const std::string &robot_name) : InnovGenome(&robot_name)
    {}

    /// \brief
    InnovGenome(const std::string *robot_name);

    /// \brief
    InnovGenome(rng_t rng,
                size_t ntraits,
                size_t ninputs,
                size_t noutputs,
                size_t nhidden,
                const std::string &robot_name);

    virtual Genome &operator=(const Genome &other) override;

    /// \brief Destructor kills off all lists (including the trait vector)
    virtual ~InnovGenome();

    /// \brief Save this genome to specified file
    virtual void save(std::ostream &out) const override;

    /// \brief Load genome from specified file
    virtual bool load(std::istream &in) override;

    void duplicate_into(InnovGenome *offspring) const;

    InnovGenome &operator=(const InnovGenome &other);

    /// \brief For debugging: A number of tests can be run on a genome to check
    /// its integrity
    /// Note: Some of these tests do not indicate a bug, but rather are meant
    /// to be used to detect specific system states
    virtual void verify() override;

    /// \brief
    virtual Stats get_stats() override;

    // ******* MUTATORS *******

    /// \brief Perturb params in one trait
    void mutate_random_trait();

    /// \brief Change random link's trait. Repeat times times
    void mutate_link_trait(int times);

    /// \brief Change random node's trait times times
    void mutate_node_trait(int times);

    /// \brief Add Gaussian noise to linkweights either GAUSSIAN or COLDGAUSSIAN
    /// (from zero)
    void mutate_link_weights(real_t power,
                             real_t rate,
                             mutator mut_type);

    /// \brief toggle links on or off
    void mutate_toggle_enable(int times);

    /// \brief Find first disabled gene and enable it
    void mutate_gene_reenable();

    // These last kinds of mutations return false if they fail
    //   They can fail under certain conditions,  being unable
    //   to find a suitable place to make the mutation.
    //   Generally, if they fail, they can be called again if desired.

    /// \brief Mutate genome by adding a node respresentation
    bool mutate_add_node(CreateInnovationFunc create_innov,
                         bool delete_split_link);

    /// \brief
    void mutate_delete_node();

    /// \brief
    void mutate_delete_link();

    /// \brief Mutate the genome by adding a new link between 2
    /// random InnovNodeGenes
    bool mutate_add_link(CreateInnovationFunc create_innov,
                         int tries);

    // ****** MATING METHODS *****
    static void mate(InnovGenome *genome1,
                     InnovGenome *genome2,
                     InnovGenome *offspring,
                     real_t fitness1,
                     real_t fitness2);

    /// \brief For every point in each InnovGenome where each InnovGenome shares
    ///   the innovation number, the InnovLinkGene is chosen randomly from
    ///   either parent.  If one parent has an innovation absent in
    ///   the other, the baby will inherit the innovation
    ///   Interspecies mating leads to all genes being inherited.
    ///   Otherwise, excess genes come from most fit parent.
    static void mate_multipoint(InnovGenome *genome1,
                                InnovGenome *genome2,
                                InnovGenome *offspring,
                                real_t fitness1,
                                real_t fitness2);

    /// \brief This method mates like multipoint but instead of selecting one
    ///   or the other when the innovation numbers match, it averages their
    ///   weights
    static void mate_multipoint_avg(InnovGenome *genome1,
                                    InnovGenome *genome2,
                                    InnovGenome *offspring,
                                    real_t fitness1,
                                    real_t fitness2);

    // ******** COMPATIBILITY CHECKING METHODS ********

    /// \brief This function gives a measure of compatibility between
    ///   two InnovGenomes by computing a linear combination of 3
    ///   characterizing variables of their compatibilty.
    ///   The 3 variables represent PERCENT DISJOINT GENES,
    ///   PERCENT EXCESS GENES, MUTATIONAL DIFFERENCE WITHIN
    ///   MATCHING GENES.  So the formula for compatibility
    ///   is:  disjoint_coeff*pdg+excess_coeff*peg+mutdiff_coeff*mdmg.
    ///   The 3 coefficients are global system parameters
    real_t compatibility(InnovGenome *g);

    /// \brief
    real_t trait_compare(Trait *t1,
                         Trait *t2);

    /// \brief Randomize the trait pointers of all the node and connection genes
    void randomize_traits();

    /// \brief
    Trait &get_trait(const InnovNodeGene &node);

    /// \brief
    Trait &get_trait(const InnovLinkGene &gene);

    /// \brief
    InnovNodeGene *get_node(int id);

    /// \brief
    node_size_t get_node_index(int id);

    /// \brief
    virtual void init_phenotype(class Network &net) override;

    public:
    void reset();

    static bool linklist_cmp(const InnovLinkGene &a,
                             const InnovLinkGene &b)
    {
      return a.innovation_num < b.innovation_num;
    }

    /// \brief Inserts a InnovNodeGene into a given ordered list of
    /// InnovNodeGenes in order
    static void add_node(std::vector<InnovNodeGene> &nlist,
                         const InnovNodeGene n);

    /// \brief Adds a new gene that has been created through a mutation in the
    /// *correct order* into the list of links in the genome
    static void add_link(std::vector<InnovLinkGene> &glist,
                         const InnovLinkGene &g);

    private:
    /// \brief
    InnovLinkGene *find_link(int in_node_id,
                             int out_node_id,
                             bool is_recurrent);

    /// \brief
    void delete_if_orphaned_hidden_node(int node_id);

    /// \brief
    void delete_link(InnovLinkGene *link);

    /// \brief
    InnovNodeLookup node_lookup;
  };
}

#endif
