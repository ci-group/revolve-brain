#!/usr/bin/env python3

import random
import revolve_brain_python
import unittest

from actuator_test import Actuator
from sensor_test import Sensor


class Evaluator(revolve_brain_python.Evaluator):
    def __init__(self):
        super().__init__()
        self.i = 0

    def start(self):
        self.i = 0

    def fitness(self):
        self.i += 1
        return random.random()


class RLPowerTestInstance(unittest.TestCase):
    def setUp(self):
        # Called before the first testfunction is executedù
        revolve_brain_python.AsyncNeat.Init()
        pass

    def test_evaluator(self):
        self.evaluator = Evaluator()
        self.assertIsInstance(self.evaluator, Evaluator)
        self.assertIsInstance(self.evaluator, revolve_brain_python.Evaluator)

        self.evaluator.start()
        self.evaluator.fitness()

    def test_isInstantiated(self):
        self.evaluator = Evaluator()

        self.controller = revolve_brain_python.RLPower(self.evaluator, 12, 12)
        self.assertIsInstance(self.controller, revolve_brain_python.RLPower)
        self.assertIsInstance(self.controller, revolve_brain_python.Brain)

    def tearDown(self):
        # Called after the last testfunction was executed
        revolve_brain_python.AsyncNeat.CleanUp()
        pass


class RLPowerTestRun(unittest.TestCase):
    def setUp(self):
        revolve_brain_python.AsyncNeat.Init()
        # Called before the first testfunction is executed
        self.evaluator = Evaluator()
        self.assertIsInstance(self.evaluator, Evaluator)

        self.actuators = [Actuator() for i in range(0, 2)]
        self.sensors = [Sensor(i) for i in range(0, 2)]

        self.controller = revolve_brain_python.SUPGBrain(
                self.evaluator,
                [
                    [-1, 1, 1],
                    [-.5, .5, 1],
                ],
                self.actuators,
                self.sensors,
        )
        self.assertIsInstance(self.controller, revolve_brain_python.Brain)

    def test_runsOnce(self):
        self.time = 1
        step = 0.1
        self.controller.update(self.actuators, self.sensors, self.time, step)

    def test_runsMultiple(self):
        self.time = 1
        step = 0.1
        import time

        start = time.time()
        for i in range(0, 100000):
            self.controller.update(self.actuators, self.sensors, self.time, step)
            self.time += step
        end = time.time()
        print("made it in {} seconds".format(end - start))

    def tearDown(self):
        # Called after the last testfunction was executed
        revolve_brain_python.AsyncNeat.CleanUp()
        pass


if __name__ == "__main__":
    unittest.main()
