/*
 * Specifies a utility `Contoller` base class, which is supposed to control the robot according to a given genome.
 */

#ifndef REVOLVE_BRAIN_SPLIT_BRAIN_CONTROLLER_H_
#define REVOLVE_BRAIN_SPLIT_BRAIN_CONTROLLER_H_

#include "../actuator.h"
#include "../sensor.h"
#include <vector>


namespace revolve {
namespace brain {

template <typename G>
class Controller {
public:
    virtual ~Controller() {};
    
    /**
     * Get method for the genome
     * @return current genome
     */
    virtual G getGenome() = 0;
    
    /**
     * Set method for the genome
     * @param newGenome: new genome to use instead of the old one
     */
    virtual void setGenome(G newGenome) = 0;
    
    /**
     * Update step called for the controller.
     *
     * @param actuators List of actuators
     * @param sensors List of sensors
     * @param t Current simulation time
     * @param step Actuation step size in seconds
     */
    virtual void update(const std::vector< ActuatorPtr > & actuators,
                        const std::vector< SensorPtr > & sensors,
                        double t,
                        double step) = 0;
    
    
};

}
}

#endif /* REVOLVE_BRAIN_SPLIT_BRAIN_CONTROLLER_H_ */
