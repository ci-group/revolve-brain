#!/usr/bin/env python3

import revolve_brain_python
import unittest


class ImportTest(unittest.TestCase):
    def test_import(self):
        self.assertIsNotNone(revolve_brain_python.Evaluator)
        self.assertIsNotNone(revolve_brain_python.Brain)
        self.assertIsNotNone(revolve_brain_python.NeuralNetwork)
        self.assertIsNotNone(revolve_brain_python.RLPower)


if __name__ == "__main__":
    unittest.main()
