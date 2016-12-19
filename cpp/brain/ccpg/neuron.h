#ifndef REVOLVE_GAZEBO_BRAIN_NEURON_H_
#define REVOLVE_GAZEBO_BRAIN_NEURON_H_

//#include "Types.h"
#include "brain/brain.h"
#include <cstdlib>
#include <utility>
#include <map>
#include <string>

#include <boost/shared_ptr.hpp>

//Types.h
namespace revolve {
namespace brain {
	class Motor;
	class VirtualSensor;
	class Brain;
	class MotorFactory;
	class SensorFactory;
	class ExtendedNeuralNetwork;
	
	class Neuron;
	class NeuralConnection;

	typedef boost::shared_ptr< Brain > BrainPtr;
	typedef boost::shared_ptr<ExtendedNeuralNetwork> NeuralNetworkPtr;
	typedef boost::shared_ptr< MotorFactory > MotorFactoryPtr;
	typedef boost::shared_ptr< SensorFactory > SensorFactoryPtr;

	typedef boost::shared_ptr<Neuron> NeuronPtr;
	typedef boost::shared_ptr<NeuralConnection> NeuralConnectionPtr;
}
}


namespace revolve {
namespace brain {

class Neuron
{
public:
  	/**
	* Constructor for a neuron.
	* @param id: string to identify the neuron
	* @return pointer to the neuron
	*/
	Neuron(const std::string &id);
	virtual ~Neuron() {};
	
	/**
	* Method to calculate the output of the neuron
	* @param t: current time
	* @return the output of the neuron at time t
	*/
	virtual double CalculateOutput(double t) = 0;

	/**
	 * Add an incoming connection to the neuron.
	 * @param socketName: name of the socket the connection "arrives" at
	 * @param connection: name of the connection to be added
	 */
	void AddIncomingConnection(const std::string &socketName,
				   NeuralConnectionPtr connection);
	
	/**
	 * Deletes all incoming connections 
	 */
	void DeleteIncomingConections();
	
	
	/**
	 * @return the current output of the neuron
	 */
	double GetOutput() const;

	/**
	 * Method to set the input of the neuron
	 * @param value: the value the input should be set to
	 */
	virtual void SetInput(double value) {};
	
	/**
	 * Calculate the output and save it for when FlipState is called
	 * @param t: current time
	 */
	void Update(double t);

	/**
	 * Set the output to the already calculated new output
	 */
	void FlipState();

	/**
	 * Compute the id for the socket of the next imcoming neural connection
	 * @return id for the socket of the next imcoming neural connection
	 */
	std::string GetSocketId() const;

	/**
	 * @return id of the neuron
	 */
	const std::string &Id() const;

protected:

	std::vector<std::pair<std::string, NeuralConnectionPtr> > incomingConnections_; //vector of the incoming connections and the name of their socket
	double output_; //current output
	double newOutput_; //calculated output for when FlipState is called

	std::string id_; //id of the neuron

};

}
}
#endif // REVOLVE_GAZEBO_BRAIN_NEURON_H_