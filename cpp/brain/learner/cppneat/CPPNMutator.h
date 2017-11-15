#ifndef REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_MUTATOR_H_
#define REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_MUTATOR_H_

#include <map>
#include <string>
#include <vector>
#include <random>

#include "GeneticEncoding.h"

//class responsible for mutation
namespace CPPNEAT
{
  class Mutator
  {
    public:

    Mutator(
            std::map< Neuron::Ntype, Neuron::NeuronTypeSpec > brain_spec,
            double new_connection_sigma,
            int innovation_number,
            int max_attempts,
            std::vector< Neuron::Ntype > addable_neurons,
            bool layered);

    static std::vector< Neuron::Ntype > AddableTypes(
            std::map< Neuron::Ntype, Neuron::NeuronTypeSpec > _specifications);

    void make_starting_genotype_known(GeneticEncodingPtr _genotype);

    void LoadInnovationsFromFirst(const std::string &_path);
    void LoadInnovationsFromSecond(const std::string &_path);

    void RecordInnovations(const std::string &_path);

    void MutateNeuronParams(
            GeneticEncodingPtr genotype,
            double probability,
            double sigma);

    void MutateWeights(
            GeneticEncodingPtr genotype,
            double probability,
            double sigma);

    void MutateStructure(
            GeneticEncodingPtr genotype,
            double probability);

    bool MutateConnection(
            GeneticEncodingPtr genotype,
            double sigma);

    void MutateNeuron(
            GeneticEncodingPtr genotype,
            double sigma);

    void RemoveConnection(GeneticEncodingPtr genotype);

    void RemoveNeuron(GeneticEncodingPtr genotype);

    int AddNeuron(
            NeuronPtr neuron,
            GeneticEncodingPtr genotype,
            ConnectionGenePtr split);

    int AddConnection(
            int mark_from,
            int mark_to,
            double weight,
            GeneticEncodingPtr genotype,
            std::string socket);


    std::map< Neuron::Ntype, Neuron::NeuronTypeSpec > BrainSpec()
    {
      return brain_spec;
    };

    void set_current_innovation_number(int innov_numb)
    {
      this->innovationNumber_ = innov_numb;
    };

    private:
    std::map< std::pair< int, int >, size_t > connectionInnovations_;

    //<mark_from, mark_to> -> innovation_number
    //contains all connections that ever existed!
    /*<innovation_num_of_splited_connection, added_neurons_type> ->
     * -> added_neurons_innovation_nums
     * since we want the ability to have multiple neurons of the same type
     * added between two neurons. It is necessary that we check if a new
     * neuron has already been added to the current genotype. If so we add
     * another neuron in order to store all the innovation numbers we need a
     * vector.
     */
    std::map< std::pair< int, Neuron::Ntype >, std::vector< int>> neuronInnovations_;
    //contains only neurons that have been added by structural mutation
    std::map< Neuron::Ntype, Neuron::NeuronTypeSpec > brain_spec;
    double sigma_;
    int innovationNumber_;
    int max_attempts;
    std::vector< Neuron::Ntype > addable_neurons;
//    bool is_layered_;
    std::mt19937 generator;
  };
}

#endif  //NEAT_MUTATOR_H_