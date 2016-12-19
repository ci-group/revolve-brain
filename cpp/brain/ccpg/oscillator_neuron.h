#ifndef REVOLVE_GAZEBO_BRAIN_OSCILLATORNEURON_H_
#define REVOLVE_GAZEBO_BRAIN_OSCILLATORNEURON_H_

#include "neuron.h"
#include "neural_connection.h"

namespace revolve {
namespace brain {

class OscillatorNeuron : public Neuron
{
public:
	/**
	* Constructor for an oscilator neuron
	* @param id: string to identify the neuron
	* @param params: parameters that specify the behavior of the neuron
	* @return pointer to the oscilator neuron
	*/
	OscillatorNeuron(const std::string &id,
			 const std::map<std::string, double> &params);
	
	/**
	* Method to calculate the output of the neuron
	* @param t: current time
	* @return the output of the neuron at time t
	*/
	virtual double CalculateOutput(double t);

protected:
	double period_; //period of the oscillation of the neuron
	double phaseOffset_; //current phase offset of the oscillation of the neuron
	double gain_; //gain of the neuron
};

}
}

#endif // REVOLVE_GAZEBO_BRAIN_OSCILLATORNEURON_H_