
#include "Trajectory_generator.h"



//--------------------------------------------------------------------------------------------

double sigma = 10.0;
double R = 28.0;
double b = 8.0 / 3.0;
void lorenz_system(const state_type& x, state_type& dxdt, const double t)
{
	dxdt[0] = sigma * (x[1] - x[0]);
	dxdt[1] = R * x[0] - x[1] - x[0] * x[2];
	dxdt[2] = -b * x[2] + x[0] * x[1];
}

//--------------------------------------------------------------------------------------------

std::vector<double> Trajectory_generator::lorenz(std::vector<float> &vars) {

	std::vector<double> coordinates = integrateSystem(vars);
	return coordinates;

}

std::vector<double> Trajectory_generator::integrateSystem(std::vector<float>& vars) {

    std::vector<state_type> x_vec;
    std::vector<double> times;
    state_type initial(3);

    R = vars.at(0);
    sigma = vars.at(1);
    b = vars.at(2);

    initial[0] = vars.at(3); // start at x=...
    initial[1] = vars.at(4);
    initial[2] = vars.at(5);

    size_t steps = boost::numeric::odeint::integrate(lorenz_system, initial, 0.0, 10.0, 0.1, push_back_state_and_time(x_vec, times));

    return create3DTrajectoryWithHomogenCoord(x_vec, times);

}

std::vector<double> Trajectory_generator::create3DTrajectoryWithHomogenCoord(const std::vector<state_type>& coord, const std::vector<double>& times) {

    std::vector<double> coordinates;
    for (int i = 0; i < times.size(); i++) {
        coordinates.push_back(i);
        coordinates.push_back(coord[i][0]);
        coordinates.push_back(coord[i][1]);
        coordinates.push_back(coord[i][2]);
        coordinates.push_back(i);
    }
    return coordinates;
}



