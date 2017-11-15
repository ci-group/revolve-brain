#include "NEATLearner.h"
#include "brain/learner/cppneat/CPPNCrossover.h"

#include <iostream>
#include <fstream>

#include <yaml-cpp/yaml.h>

namespace CPPNEAT
{
  NEATLearner::NEATLearner(
          MutatorPtr mutator,
          const std::string &_innovations,
          const std::string &_parent1,
          const std::string &_parent2,
          NEATLearner::LearningConfiguration conf
  )
          : active_brain_(nullptr)
          , generation_number(0)
          , total_brains_evaluated(0)
          , mutator(mutator)
          , mutator_path(_innovations)
          , is_asexual_(conf.asexual)
          , initial_structural_mutations_(conf.initial_structural_mutations)
          , num_children_(conf.num_children)
          , population_size_(conf.pop_size)
          , tournament_size_(conf.tournament_size)
          , weight_mutation_probability(conf.weight_mutation_probability)
          , weight_mutation_sigma(conf.weight_mutation_sigma)
          , param_mutation_probability(conf.param_mutation_probability)
          , param_mutation_sigma(conf.param_mutation_sigma)
          , structural_augmentation_probability(conf.structural_augmentation_probability)
          , structural_removal_probability(conf.structural_removal_probability)
          , max_generations(conf.max_generations)
          , speciation_threshold(conf.speciation_threshold)
          , repeat_evaluations(conf.repeat_evaluations)
          , start_from_(conf.start_from)
          , interspecies_mate_probability(conf.interspecies_mate_probability)
  {
    std::random_device rd;
    generator.seed(rd());
    if (population_size_ < 2)
    {
      population_size_ = 2;
    }
    if (tournament_size_ > population_size_)
    {
      tournament_size_ = population_size_;
    }
    if (tournament_size_ < 2)
    {
      tournament_size_ = 2;
    }
    if (_parent1 not_eq "none")
    {
      mutator->LoadInnovationsFromFirst(_parent1);
    }
    if (_parent2 not_eq "none")
    {
      mutator->LoadInnovationsFromSecond(_parent2);
    }
    if (start_from_ != nullptr)
    {
      std::cout << "generating initial population from starting network"
                << std::endl;
      initialise(std::vector< GeneticEncodingPtr >());
    }
    else
    {
      std::cout << "no starting network given, initialise has to be called"
                << std::endl;
    }
    this->mutator->make_starting_genotype_known(start_from_);
    this->displayConfiguration();
  }

  void NEATLearner::initialise(std::vector< GeneticEncodingPtr > init_genotypes)
  {
    if (init_genotypes.empty())
    {
      brain_population_ = InitCppns();
    }
    else
    {
      std::cout << "initialised with starting population" << std::endl;
      std::cout << "overwriting current population if present" << std::endl;
      brain_population_ = init_genotypes;
    }
    evaluation_queue_.clear();
    for (const auto &brain : brain_population_)
    {
      evaluation_queue_.push_back(brain);
    }
    active_brain_ = evaluation_queue_.back();
    evaluation_queue_.pop_back();
  }

  std::vector< GeneticEncodingPtr > NEATLearner::BrainsFromYaml(
          std::string yaml_path,
          int offset)
  {
    if (offset != -1)
    {
      int innovation_counter = offset;
      auto yaml_file = YAML::LoadFile(yaml_path);
      if (yaml_file.IsNull())
      {
        std::cerr << "Failed to load the yaml file." << std::endl;
        return std::vector< GeneticEncodingPtr >();
      }
      std::vector< GeneticEncodingPtr > genotypes;
      for (size_t first = 0; first < yaml_file.size(); first++)
      {
        std::map< int, int > old_to_new;
        GeneticEncodingPtr newGenome(new GeneticEncoding(true));
        for (size_t counter = 0; counter < yaml_file[first]["brain"]["layers"].size(); counter++)
        {
          auto layer = yaml_file[first]["brain"]["layers"][counter];
          layer = layer["layer_" + std::to_string(counter + 1)];
          bool is_new_layer = true;
          for (const auto &neuron_node : layer)
          {
            auto neuron_id = neuron_node["nid"].as< std::string >();
            auto neuron_type = static_cast<Neuron::Ntype>(neuron_node["ntype"].as< int >());
            auto neuron_layer = static_cast<Neuron::Layer>(neuron_node["nlayer"].as< int >());
            int innov_numb;
            if (old_to_new.find(neuron_node["in_no"].as< int >()) == old_to_new.end())
            {
              innov_numb = innovation_counter++;
              old_to_new[neuron_node["in_no"].as< int >()] = innov_numb;
            }
            else
            {
              innov_numb = old_to_new[neuron_node["in_no"].as< int >()];
            }
            std::map< std::string, double > neuron_params;
            std::vector< std::string > params;
            for (const auto &spec_pair : mutator->BrainSpec())
            {
              for (const auto &param_spec : spec_pair.second.param_specs)
              {
                if (std::find(params.begin(),
                              params.end(),
                              param_spec.name) == params.end())
                {
                  params.push_back(param_spec.name);
                }
              }
            }
            auto params_node = neuron_node["params"];
            for (const auto &param_name : params)
            {
              YAML::Node param_node = params_node[param_name];
              if (param_node.IsDefined())
              {
                neuron_params[param_name] = param_node.as< double >();
              }
            }
            NeuronPtr new_neuron(new Neuron(
                    neuron_id,
                    neuron_layer,
                    neuron_type,
                    neuron_params));
            NeuronGenePtr new_neuron_gene(new NeuronGene(
                    new_neuron,
                    innov_numb,
                    true));
            newGenome->add_neuron_gene(
                    new_neuron_gene,
                    counter,
                    is_new_layer);
            is_new_layer = false;
          }
        }
        for (size_t i = 0; i < yaml_file[first]["brain"]["connection_genes"].size(); i++)
        {
          auto connection = yaml_file[first]["brain"]["connection_genes"][i]["con_1"];
          auto mark_to = old_to_new[connection["to"].as< int >()];
          auto mark_from = old_to_new[connection["from"].as< int >()];
          auto weight = connection["weight"].as< double >();
          int innov_numb;
          if (old_to_new.find(connection["in_no"].as< int >()) == old_to_new.end())
          {
            innov_numb = innovation_counter++;
            old_to_new[connection["in_no"].as< int >()] = innov_numb;
          }
          else
          {
            innov_numb = old_to_new[connection["in_no"].as< int >()];
          }
          ConnectionGenePtr newConnection(new ConnectionGene(
                  mark_to,
                  mark_from,
                  weight,
                  innov_numb,
                  true,
                  ""));
          newGenome->add_connection_gene(newConnection);
        }
        genotypes.push_back(newGenome);
      }
      return genotypes;
    }
    else
    {
      auto yaml_file = YAML::LoadFile(yaml_path);
      if (yaml_file.IsNull())
      {
        std::cout << "Failed to load the yaml file." << std::endl;
        return std::vector< GeneticEncodingPtr >();
      }
      std::vector< GeneticEncodingPtr > genotypes;
      for (size_t first = 0; first < yaml_file.size(); first++)
      {
        GeneticEncodingPtr newGenome(new GeneticEncoding(true));
        for (size_t counter = 0; counter < yaml_file[first]["brain"]["layers"].size(); counter++)
        {
          auto layer = yaml_file[first]["brain"]["layers"][counter];
          layer = layer["layer_" + std::to_string(counter + 1)];
          bool is_new_layer = true;
          for (const auto &neuron_node : layer)
          {
            auto neuron_id = neuron_node["nid"].as< std::string >();
            auto neuron_type = static_cast<Neuron::Ntype>(neuron_node["ntype"].as< int >());
            auto neuron_layer = static_cast<Neuron::Layer>(neuron_node["nlayer"].as< int >());
            auto innov_numb = neuron_node["in_no"].as< int >();
            std::map< std::string, double > neuron_params;
            std::vector< std::string > params;
            for (const auto &spec_pair : mutator->BrainSpec())
            {
              for (const auto &param_spec : spec_pair.second.param_specs)
              {
                if (std::find(params.begin(),
                              params.end(),
                              param_spec.name) == params.end())
                {
                  params.push_back(param_spec.name);
                }
              }
            }
            YAML::Node params_node = neuron_node["params"];
            for (const auto &param_name : params)
            {
              YAML::Node param_node = params_node[param_name];
              if (param_node.IsDefined())
              {
                neuron_params[param_name] = param_node.as< double >();
              }
            }
            NeuronPtr new_neuron(new Neuron(
                    neuron_id,
                    neuron_layer,
                    neuron_type,
                    neuron_params));
            NeuronGenePtr new_neuron_gene(new NeuronGene(
                    new_neuron,
                    innov_numb,
                    true,
                    yaml_path,
                    first));
            newGenome->add_neuron_gene(
                    new_neuron_gene,
                    counter,
                    is_new_layer);
            is_new_layer = false;
          }
        }
        for (unsigned int i = 0; i < yaml_file[first]["brain"]["connection_genes"].size(); i++)
        {
          auto connection = yaml_file[first]["brain"]["connection_genes"][i]["con_1"];
          auto to = connection["to"].as< int >();
          auto from = connection["from"].as< int >();
          auto weight = connection["weight"].as< double >();
          auto innov_numb = connection["in_no"].as< int >();
          ConnectionGenePtr newConnection(new ConnectionGene(
                  to,
                  from,
                  weight,
                  innov_numb,
                  true,
                  yaml_path,
                  first,
                  ""));
          newGenome->add_connection_gene(newConnection);
        }
        genotypes.push_back(newGenome);
      }
      return genotypes;
    }
  }

  std::vector< GeneticEncodingPtr > NEATLearner::InitCppns()
  {
    std::vector< GeneticEncodingPtr > init_pop;
    size_t i = 0;
    while (i++ < population_size_)
    {
      GeneticEncodingPtr mutated_genotype = start_from_->copy();
#ifdef CPPNEAT_DEBUG
      if (not mutated_genotype->is_valid()) {
          std::cerr << "copying caused invalid genotype" << std::endl;
      }
#endif
      for (size_t j = 0; j < initial_structural_mutations_
                 && initial_structural_mutations_ > 0; j++)
      {
        apply_structural_mutation(mutated_genotype);
      }

      mutator->MutateWeights(mutated_genotype, 1, weight_mutation_sigma);
      mutator->MutateNeuronParams(mutated_genotype, 1, param_mutation_sigma);

      init_pop.push_back(mutated_genotype);
    }
    return init_pop;
  }

  void NEATLearner::reportFitness(
          std::string id,
          GeneticEncodingPtr genotype,
          double fitness)
  {
    std::cout << "Evalutation over\n"
              << "Evaluated " << ++total_brains_evaluated << " brains \n"
              << "Last fitness: " << fitness << std::endl;
    this->writeGenome(id  + ".policy", genotype);

    // Make a record of best CPPNs
    this->bestBrains_.insert({fitness, genotype});
    while (bestBrains_.size() > 10)
    {
      auto last = std::prev(bestBrains_.end());
      bestBrains_.erase(last);
    }

    fitness_buffer.push_back(fitness);
    if (fitness_buffer.size() == repeat_evaluations)
    {
      double sum = 0;
      for (double add : fitness_buffer)
      {
        sum += add;
      }
      double average_fitness = sum / repeat_evaluations;
      brain_fitness[active_brain_] = average_fitness;
      brain_velocity[active_brain_] = average_fitness;

      if (evaluation_queue_.empty())
      {
        share_fitness();

        produce_new_generation();
        std::reverse(evaluation_queue_.begin(), evaluation_queue_.end());
        generation_number++;
      }
      active_brain_ = evaluation_queue_.back();
      evaluation_queue_.pop_back();
      fitness_buffer.clear();
      if (generation_number >= max_generations)
      {
        std::cout << "Maximum number of generations reached" << std::endl;
        this->RecordBest(id);

        if (mutator_path != "none")
        {
          mutator->RecordInnovations(mutator_path);
        }
        std::exit(0);
      }
    }
  }

  GeneticEncodingPtr NEATLearner::currentGenotype()
  {
    return active_brain_;
  }

  void NEATLearner::writeGenome(
          std::string robot_name,
          GeneticEncodingPtr genome)
  {
    std::ofstream outputFile;
    outputFile.open(robot_name, std::ios::app | std::ios::out | std::ios::ate);
    outputFile << "- evaluation: " << total_brains_evaluated << std::endl;
    outputFile << "  brain:" << std::endl;
    outputFile << "    connection_genes:" << std::endl;
    auto connection_genes = genome->connections_;
    int n_cons = 1;
    for (const auto &connection : connection_genes)
    {
      outputFile << "      - con_" << n_cons << ":" << std::endl;
      outputFile
              << "            in_no: "
              << connection->InnovationNumber()
              << std::endl;
      outputFile << "            from: " << connection->from_ << std::endl;
      outputFile << "            to: " << connection->to_ << std::endl;
      outputFile << "            weight: " << connection->weight << std::endl;
      outputFile
              << "            parent_name: "
              << connection->get_parent_name()
              << std::endl;
      outputFile
              << "            parent_index: "
              << connection->get_parent_index()
              << std::endl;
    }
    outputFile << "    layers:" << std::endl;
    auto layers = genome->layers_;
    int n_layer = 1;
    for (const auto &it : layers)
    {
      outputFile << "      - layer_" << n_layer << ":" << std::endl;
      for (const auto &it2 : it)
      {
        auto neuron = it2->neuron;
        auto neuron_params = neuron->neuron_params;
        outputFile << "          - nid: " << neuron->neuron_id << std::endl;
        outputFile << "            ntype: " << neuron->type_ << std::endl;
        outputFile << "            nlayer: " << neuron->layer << std::endl;
        outputFile
                << "            in_no: "
                << it2->InnovationNumber()
                << std::endl;
        outputFile
                << "            parent_name: "
                << it2->get_parent_name()
                << std::endl;
        outputFile
                << "            parent_index: "
                << it2->get_parent_index()
                << std::endl;
        outputFile << "            params:" << std::endl;
        for (const auto &np : neuron_params)
        {
          outputFile
                  << "              "
                  << np.first
                  << ": "
                  << np.second
                  << std::endl;
        }
      }
      n_layer++;
    }
    outputFile.close();
  }

  void NEATLearner::RecordBest(const std::string &_name)
  {
    for (auto const &it : this->bestBrains_)
    {
      this->writeGenome(_name + ".best", it.second);
    }
  }

  void NEATLearner::share_fitness()
  {
    //speciate
    auto old_species = species;
    species.clear();
    //choose representative from previous generation (or do nothing for first run)
    for (const auto &sppair : old_species)
    {
      std::uniform_int_distribution< int > choose(
              0,
              sppair.second.size() - 1);
      GeneticEncodingPtr representative = sppair.second[choose(generator)];
      species.insert({representative,
                      std::vector< GeneticEncodingPtr >()});
    }

    for (const auto &cur_brain : brain_velocity)
    {
      bool added = false;
      //search for matching species
      for (const auto &sppair : species)
      {
        //TODO:: coefficients
        auto dissimilarity = GeneticEncoding::Dissimilarity(
                sppair.first,
                cur_brain.first,
                1,
                1,
                0.4);
        if (dissimilarity < speciation_threshold)
        {
          added = true;
          species[sppair.first].push_back(cur_brain.first);
          break;
        }
      }
      //add new species in case of no matches
      if (not added)
      {
        species.insert({cur_brain.first,
                        std::vector< GeneticEncodingPtr >(1, cur_brain.first)});
      }
    }
    old_species = species;
    species.clear();
    //only keep species which are not empty
    for (const auto &sppair : old_species)
    {
      if (sppair.second.empty())
      {
        species.insert(sppair);
      }
    }
    //actual sharing
    std::map< GeneticEncodingPtr, double > new_fitness;
    for (const auto &sppair : species)
    {
      for (const auto &brain : sppair.second)
      {
        new_fitness[brain] = brain_velocity[brain] / sppair.second.size();
      }
    }
    brain_fitness = new_fitness;
  }

  bool fitness_cmp(
          std::pair< GeneticEncodingPtr, double > genotype1,
          std::pair< GeneticEncodingPtr, double > genotype2)
  {
    return genotype1.second > genotype2.second;
  }

  void NEATLearner::produce_new_generation()
  {
    //calculate number of children for each species
    double overall_fitness = 0;
    std::map< GeneticEncodingPtr, double > species_fitness;
    for (const auto &sppair : species)
    {
      double cur_sum = 0;
      for (const auto &brain : sppair.second)
      {
        cur_sum += brain_fitness[brain];
      }
      species_fitness[sppair.first] = cur_sum;
      overall_fitness += cur_sum;
    }
    std::map< GeneticEncodingPtr, int > species_offspring;
    size_t cur_children = 0;
    double average_fitness = overall_fitness / num_children_;
    GeneticEncodingPtr best = species_fitness.begin()->first;
    double treshold = 0;
    for (const auto &fitness : species_fitness)
    {
      while (treshold + fitness.second >= (cur_children + 1) * average_fitness)
      {
        species_offspring[fitness.first]++;
        cur_children++;
      }
      treshold += fitness.second;
      if (species_fitness[best] < fitness.second)
      {
        best = fitness.first;
      }
    }
    //should not happen, but might (>= for double flawed)
    if (cur_children < num_children_)
    {
      assert(cur_children == num_children_ - 1);
      species_offspring[best]++;
    }
    //reproduce
    std::uniform_real_distribution< double > uniform(0, 1);
    for (const auto &sppair : species)
    {
      std::vector< std::pair< GeneticEncodingPtr, double>> fitness_pairs;
      for (const auto &brain : sppair.second)
      {
        fitness_pairs.push_back({brain, brain_fitness[brain]});
      }
      std::sort(fitness_pairs.begin(), fitness_pairs.end(), fitness_cmp);

      std::vector< std::pair< GeneticEncodingPtr, GeneticEncodingPtr>>
              parent_pairs;
      int i = 0;
      while (i++ < species_offspring[sppair.first])
      {
        if (sppair.second.size() == 1
            or uniform(generator) < interspecies_mate_probability)
        {
          //if there is only one individual in species or we are below
          // threshold probability we want interspecies mating
          std::uniform_int_distribution< int > choose(0, sppair.second.size() - 1);
          GeneticEncodingPtr mom = sppair.second[choose(generator)];
          std::uniform_int_distribution< int > chooose(0, species.size() - 1);
          int until = chooose(generator);
          auto species_iterator = species.begin();
          for (int k = 0; k < until; k++)
          {
            ++species_iterator;
          }
          auto  bachelors = (species_iterator)->second;
          std::vector< std::pair< GeneticEncodingPtr, double>> to_sort;
          for (GeneticEncodingPtr brain : bachelors)
          {
            to_sort.push_back({brain, brain_fitness[brain]});
          }
          std::sort(to_sort.begin(), to_sort.end(), fitness_cmp);

          GeneticEncodingPtr dad = to_sort[0].first;
          parent_pairs.push_back(brain_fitness[mom] > brain_fitness[dad] ?
                                 std::make_pair(mom, dad) :
                                 std::make_pair(dad, mom));
        }
        else
        {
          auto selected = select_for_tournament(fitness_pairs, 2);
          parent_pairs.push_back(selected);
        }
      }
      for (const auto &parents : parent_pairs)
      {
        GeneticEncodingPtr
                child_genotype = produce_child(parents.first, parents.second);
        evaluation_queue_.push_back(child_genotype);
      }
    }
    //elitism
    std::vector< std::pair< GeneticEncodingPtr, double>> velocity_pairs;
    for (const auto &it : brain_velocity)
    {
      velocity_pairs.push_back({it.first, it.second});
    }
    std::sort(velocity_pairs.begin(),
              velocity_pairs.end(),
              fitness_cmp);
    size_t i = 0;
    while (i < (population_size_ - num_children_))
    {
      evaluation_queue_.push_back(velocity_pairs[i].first);
      i++;
    }
    //remove old fitness
    brain_fitness.clear();
    brain_velocity.clear();
    //debug
    std::cout << "Produced new generation with: " << std::endl;
    std::cout << "* " << species.size() << " species with sizes: " << std::endl;
    auto species_iterator = species.begin();
    for (unsigned int j = 0; j < species.size(); j++)
    {
      std::cout << "** " << (species_iterator++)->second.size() << std::endl;
    }
    std::cout
            << "* overall number of individuals in queue: "
            << evaluation_queue_.size()
            << std::endl;
  }

  GeneticEncodingPtr
  NEATLearner::produce_child(
          GeneticEncodingPtr parent1,
          GeneticEncodingPtr parent2)
  {
    GeneticEncodingPtr child_genotype;
    if (is_asexual_)
    {
      child_genotype = parent1->copy();
    }
    else
    {
      child_genotype = Crossover::crossover(
              parent1,
              parent2);
    }

    mutator->MutateWeights(child_genotype,
                           weight_mutation_probability,
                           weight_mutation_sigma);

    mutator->MutateNeuronParams(child_genotype,
                                param_mutation_probability,
                                param_mutation_sigma);

    apply_structural_mutation(child_genotype);

    return child_genotype;
  }

  void
  NEATLearner::apply_structural_mutation(GeneticEncodingPtr genotype)
  {
    std::uniform_real_distribution< double > uniform(0, 1);
    this->mutator->MutateStructure(
            genotype,
            structural_augmentation_probability);

    if (uniform(generator) < structural_removal_probability)
    {
      if (uniform(generator) < 0.5)
      {
        mutator->RemoveConnection(genotype);
      }
      else
      {
        mutator->RemoveNeuron(genotype);
      }
    }
  }

  std::pair< GeneticEncodingPtr, GeneticEncodingPtr > NEATLearner::select_for_tournament(
          std::vector< std::pair< GeneticEncodingPtr, double > > candidates,
          unsigned int tourn_size)
  {
    std::shuffle(candidates.begin(),
                 candidates.end(),
                 generator);
    candidates = std::vector< std::pair< GeneticEncodingPtr, double>>(
            candidates.begin(),
            candidates.begin() + tourn_size);
    std::sort(candidates.begin(),
              candidates.end(),
              fitness_cmp);
    return {candidates[0].first, candidates[1].first};
  }

  void NEATLearner::displayConfiguration()
  {
    std::cout
            << "\033[1;33m"
            << "-----------------------------------------------------------------------------------------------------------"
            << "\033[0m"
            << std::endl;
    std::cout.width(35);
    std::cout
            << std::right
            << "\033[1;31m"
            << "Starting NEAT learner with the following parameters"
            << "\033[0m"
            << std::endl;
    std::cout.width(100);
    std::cout
            << std::right
            << "Asexual reproduction: "
            << "\033[1;36m"
            << is_asexual_
            << "\033[0m"
            << std::endl;
    std::cout.width(100);
    std::cout
            << std::right
            << "Population size: "
            << "\033[1;36m"
            << population_size_
            << "\033[0m"
            << std::endl;
    std::cout.width(100);
    std::cout
            << std::right
            << "Tournament size: (currently not applicable, always 2) "
            << "\033[1;36m"
            << tournament_size_
            << "\033[0m"
            << std::endl;
    std::cout.width(100);
    std::cout
            << std::right
            << "Number of children: (rest of new individuals are elite of previous generation) "
            << "\033[1;36m"
            << num_children_
            << "\033[0m"
            << std::endl;
    std::cout.width(100);
    std::cout
            << std::right
            << "Weight mutation probability: "
            << "\033[1;36m"
            << weight_mutation_probability
            << "\033[0m"
            << std::endl;
    std::cout.width(100);
    std::cout
            << std::right
            << "Weight mutation sigma: "
            << "\033[1;36m"
            << weight_mutation_sigma
            << "\033[0m"
            << std::endl;
    std::cout.width(100);
    std::cout
            << std::right
            << "Param mutation probability: "
            << "\033[1;36m"
            << param_mutation_probability
            << "\033[0m"
            << std::endl;
    std::cout.width(100);
    std::cout
            << std::right
            << "Param mutation sigma: "
            << "\033[1;36m"
            << param_mutation_sigma
            << "\033[0m"
            << std::endl;
    std::cout.width(100);
    std::cout
            << std::right
            << "Probability of structural additions: "
            << "\033[1;36m"
            << structural_augmentation_probability
            << "\033[0m"
            << std::endl;
    std::cout.width(100);
    std::cout
            << std::right
            << "Probability of structural removal: "
            << "\033[1;36m"
            << structural_removal_probability
            << "\033[0m"
            << std::endl;
    std::cout.width(100);
    std::cout
            << std::right
            << "Number of generations: "
            << "\033[1;36m"
            << max_generations
            << "\033[0m"
            << std::endl;
    std::cout.width(100);
    std::cout
            << std::right
            << "Speciation threshold: (maximum dissimilarity to be in same species) "
            << "\033[1;36m"
            << speciation_threshold
            << "\033[0m"
            << std::endl;
    std::cout.width(100);
    std::cout
            << std::right
            << "How often do we evaluate before we take the average fitness: "
            << "\033[1;36m"
            << repeat_evaluations
            << "\033[0m"
            << std::endl;
    std::cout.width(100);
    std::cout
            << std::right
            << "How many initial structural mutations do we apply in case a starting genome was given: "
            << "\033[1;36m"
            << initial_structural_mutations_
            << "\033[0m"
            << std::endl;
    std::cout.width(100);
    std::cout
            << std::right
            << "Probability to mate outside of species while there is more than one member inside the species: "
            << "\033[1;36m"
            << interspecies_mate_probability
            << "\033[0m"
            << std::endl;
    std::cout
            << "\033[1;33m"
            << "-----------------------------------------------------------------------------------------------------------"
            << "\033[0m"
            << std::endl;
  }

  const bool NEATLearner::ASEXUAL = false;

  const int NEATLearner::POP_SIZE = 50;

  const int NEATLearner::TOURNAMENT_SIZE = 40;

  const int NEATLearner::NUM_CHILDREN = 45;

  const double NEATLearner::WEIGHT_MUTATION_PROBABILITY = 0.8;

  const double NEATLearner::WEIGHT_MUTATION_SIGMA = 5.0;

  const double NEATLearner::PARAM_MUTATION_PROBABILITY = 0.8;

  const double NEATLearner::PARAM_MUTATION_SIGMA = 0.25;

  const double NEATLearner::STRUCTURAL_AUGMENTATION_PROBABILITY = 0.8;

  const double NEATLearner::STRUCTURAL_REMOVAL_PROBABILITY = 0;

  const int NEATLearner::MAX_GENERATIONS = 20;

  const double NEATLearner::SPECIATION_TRESHOLD = 0.03;

  const int NEATLearner::REPEAT_EVALUATIONS = 1;

  const int NEATLearner::INITIAL_STRUCTURAL_MUTATIONS = 1;

  const double NEATLearner::INTERSPECIES_MATE_PROBABILITY = 0.001;

}
