
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

//--------------------------------------------------------------------------------------------

void bipolar_disorder_model(const state_type& x, state_type& dxdt, const double t)
{
    dxdt[0] = (0.16 / (0.16 + x[1] * x[1])) * ((2 * x[3]) / (1 + 2 * x[3])) - x[0] / (1 + 2 * x[0]);
    dxdt[1] = (0.16 / (0.16 + x[0] * x[0])) * ((2 * x[2]) / (1 + 2 * x[2])) - x[1] / (1 + 2 * x[1]);
    dxdt[2] = b * (x[1] - x[3]);
    dxdt[3] = b * (x[0] - x[2]);
}

//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------

float k1 = 0.35;
float k2 = 250;
float k3 = 0.035;
float k4 = 20;
float k5 = 5.35;
float k6 = 0.0001;
float k7 = 0.8;
float k_7 = 0.1;
float k8 = 0.825;

void peroxidase(const state_type& x, state_type& dxdt, const double t)
{
    dxdt[0] = -k3 * x[0] * x[1] * x[3] + k7 - k_7 * x[0];
    dxdt[1] = -k3 * x[0] * x[1] * x[3] - k1 * x[1] * x[2] + k8;
    dxdt[2] = k1 * x[1] * x[2] - 2 * k2 * x[2] * x[2] + 3 * k3 * x[0] * x[1] * x[3] - k4 * x[2] + k6;
    dxdt[3] = -k3 * x[0] * x[1] * x[3] + 2 * k2 * x[2] * x[2] - k5 * x[3];
}

//--------------------------------------------------------------------------------------------

void test(const state_type& x, state_type& dxdt, const double t) {
    dxdt[0] = 0.0f;
    dxdt[1] = 0.1f;
    dxdt[2] = 0.1f;
    dxdt[3] = 0.1f;
}

//--------------------------------------------------------------------------------------------

void test_2(const state_type& x, state_type& dxdt, const double t) {

    float k1 = -2;
    float k2 = 1;
    float k3 = 0.2f;
    float k4 = 1;

    dxdt[0] = x[0]*(1-x[1])+k1*x[2];
    dxdt[1] = k2*(pow(x[0],2)-1)*x[1];
    dxdt[2] = k3*(1-x[1])*x[3];
    dxdt[3] = k4*x[2];
}

enum System {Lorenz = 'l', Bipolar = 'b', PO_Reaction = 'p'};

std::vector<double> Trajectory_generator::integrate(std::vector<float>& vars, const char* system, double integration_end, double step_size) {

    std::vector<state_type> x_vec;
    std::vector<double> times;
    state_type initial(4);

    //TODO rework system parameters
    R = vars.at(0);
    sigma = vars.at(1);
    b = vars.at(2);

    initial[0] = vars.at(3); // start at x=...
    initial[1] = vars.at(4);
    initial[2] = vars.at(5);
    initial[3] = vars.at(6);

    if (system == "Lorenz") {
        size_t steps = boost::numeric::odeint::integrate(lorenz_system, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
    }
    if (system == "PO_Reaction") {
        size_t steps = boost::numeric::odeint::integrate(peroxidase, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
    }
    if (system == "test") {
        size_t steps = boost::numeric::odeint::integrate(test, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
    }
    if (system == "test_2") {
        size_t steps = boost::numeric::odeint::integrate(test_2, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
    }

    std::vector<double> coordinates;
    for (int i = 0; i < times.size(); i++) {
        coordinates.push_back(i);
        coordinates.push_back(x_vec[i][0]);
        coordinates.push_back(x_vec[i][1]);
        coordinates.push_back(x_vec[i][2]);
        if(system != "Lorenz")
            coordinates.push_back(x_vec[i][3]);
        else
            coordinates.push_back(i);
    }

    return coordinates;
}





