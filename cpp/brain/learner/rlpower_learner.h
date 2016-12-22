//
// Created by Milan Jelisavcic on 28/03/16.
//

#ifndef REVOLVE_BRAIN_RLPOWER_LEARNER_H
#define REVOLVE_BRAIN_RLPOWER_LEARNER_H

#include "learner.h"

#include <cmath>
#include <string>
#include <vector>

#include <boost/thread/mutex.hpp>


namespace revolve {
    namespace brain {

        class RLPowerLearner : public Learner<std::vector<std::vector<double>>> {

        public:
	    struct Config;
            typedef std::vector<double> Spline;
            typedef std::vector<Spline> Policy;
            typedef std::shared_ptr<Policy> PolicyPtr;

            /**
             * The RLPower constructor reads out configuration file, deretmines which algorithm type to apply and
             * initialises new policy.
             * @param modelName: name of a robot
             * @param brain: configuration file
             * @param evaluator: pointer to fitness evaluatior
             * @param n_actuators: number of actuators
             * @param n_sensors: number of sensors
             * @return pointer to the RLPower class object
             */
            RLPowerLearner(std::string modelName,
			   Config brain,
			   unsigned int n_weight_vectors);

            virtual ~RLPowerLearner();
	    
	    static const unsigned int MAX_EVALUATIONS;// = 1000; // max number of evaluations
            static const unsigned int MAX_RANKED_POLICIES;// = 10; // max length of policies vector
            static const unsigned int INTERPOLATION_CACHE_SIZE;// = 100; // number of data points for the interpolation cache
            static const unsigned int INITIAL_SPLINE_SIZE;// = 3; // number of initially sampled spline points
            static const unsigned int UPDATE_STEP;// = 100; // after # generations, it increases the number of spline points
            static const double EVALUATION_RATE;// = 30.0; // evaluation time for each policy
            static const double SIGMA_START_VALUE;// = 0.8; // starting value for sigma
            static const double SIGMA_TAU_CORRECTION;// = 0.2;

            static const double CYCLE_LENGTH;// = 5; // seconds
            static const double SIGMA_DECAY_SQUARED;// = 0.98; // sigma decay

            struct Config {
                std::string algorithm_type;
                unsigned int interpolation_spline_size;
		double evaluation_rate;
                unsigned int max_evaluations;
                unsigned int max_ranked_policies;
                double noise_sigma;
                double sigma_tau_correction;
                unsigned int source_y_size;
                unsigned int update_step;
                std::string policy_load_path;
            };

        protected:
            /**
             * Ranked list of used splines
             */
            class PolicySave {
            public:
                PolicyPtr policy_;
                double fitness_;

                PolicySave(double fitness, PolicyPtr &p) :
                        policy_(p),
                        fitness_(fitness) {}

                bool operator>(const PolicySave &ps) const {
                    return this->fitness_ > ps.fitness_;
                }
            };
	   

        private:
            /**
             * Generate new policy
             */
            void generateInitPolicy();

	    virtual void reportFitness(std::string id, 
				       Policy genome,
				       double fitness);
	    
	    virtual Policy getNewGenome(std::string id);

            /**
             * Load saved policy from JSON file
             */
            void loadPolicy(std::string const policy_path);
	    
	    /**
             * Generate interpolated spline based on number of sampled control points in 'source_y'
             * @param source_y: set of control points over which interpolation is generated
             * @param destination_y: set of interpolated control points (default 100 points)
             */
            void interpolateCubic(Policy *const source_y,
                                  Policy *destination_y);
	    
	    /**
             * Writes current spline to file
             */
            void writeCurrent();

            /**
             * Writes best 10 splines to file
             */
            void writeElite();

            /**
             * Increment number of sampling points for policy
             */
            void increaseSplinePoints();

            /**
             * Randomly select two policies and return the one with higher fitness
             * @return an iterator from 'ranked_policies_' map
             */
            std::map<double, RLPowerLearner::PolicyPtr>::iterator binarySelection();


            PolicyPtr current_policy_ = NULL; // Pointer to the current policy

            unsigned int interpolation_spline_size_; // Number of 'interpolation_cache_' sample points
            unsigned int generation_counter_; // Number of current generation
            unsigned int max_ranked_policies_; // Maximal number of stored ranked policies
            unsigned int max_evaluations_; // Maximal number of evaluations
            unsigned int n_weight_vectors_; // Number of actuators
            unsigned int source_y_size_; //
            unsigned int step_rate_; //
            unsigned int update_step_; // Number of evaluations after which sampling size increases

            double cycle_start_time_;
            double noise_sigma_; // Noise in generatePolicy() function
            double sigma_tau_correction_; // Tau deviation for self-adaptive sigma

            std::string robot_name_; // Name of the robot
            std::string algorithm_type_; // Type of the used algorithm
            std::string policy_load_path_; // Load path for previously saved policies
            std::map<double, PolicyPtr, std::greater<double>> ranked_policies_; // Container for best ranked policies
        };

    }
}

#endif //REVOLVE_BRAIN_RLPOWER_LEARNER_H
