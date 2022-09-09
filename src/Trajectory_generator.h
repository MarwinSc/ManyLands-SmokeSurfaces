#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <boost/numeric/odeint.hpp>

typedef std::vector< double > state_type;

class Trajectory_generator {
public:
    std::vector<double> integrate(std::vector<float>& vars, const char* system, double integration_end = 100.0f, double step_size = 1.0f, bool adaptive = true);

private:

    struct push_back_state_and_time
    {
        std::vector< state_type >& m_states;
        std::vector< double >& m_times;

        push_back_state_and_time(std::vector< state_type >& states, std::vector< double >& times)
            : m_states(states), m_times(times) { }

        void operator()(const state_type& x, double t)
        {
            m_states.push_back(x);
            m_times.push_back(t);
        }
    };

    struct write_state
    {
        void operator()(const state_type& x) const
        {
            std::cout << x[0] << "\t" << x[1] << "\n";
        }
    };

    void writeTrajectoryToDisc(const std::vector<double>& coordinates) {
        std::ofstream dataFile;
        dataFile.open("data.txt");
        for (int i = 0; i < coordinates.size(); i++) {
            dataFile << std::setprecision(18) << std::scientific << coordinates.at(i);
            if ((i + 1) % 5 == 0) {
                dataFile << "\n";
            }
            else {
                dataFile << "\t";
            }
        }
    }

};

