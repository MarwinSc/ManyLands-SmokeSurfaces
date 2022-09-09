
#include "Trajectory_generator.h"



//--------------------------------------------------------------------------------------------


void lorenz_system(const state_type& x, state_type& dxdt, const double t)
{
    double sigma = 10.0;
    double R = 28.0;
    double b = 8.0 / 3.0;

	dxdt[0] = sigma * (x[1] - x[0]);
	dxdt[1] = R * x[0] - x[1] - x[0] * x[2];
	dxdt[2] = -b * x[2] + x[0] * x[1];
}

//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------

void bipolar_disorder_model(const state_type& x, state_type& dxdt, const double t)
{
    float eps = 0.01;

    dxdt[0] = (0.16 / (0.16 + x[1] * x[1])) * ((2 * x[3]) / (1 + 2 * x[3])) - x[0] / (1 + 2 * x[0]);
    dxdt[1] = (0.16 / (0.16 + x[0] * x[0])) * ((2 * x[2]) / (1 + 2 * x[2])) - x[1] / (1 + 2 * x[1]);
    dxdt[2] = eps * (x[0] - x[3]);
    dxdt[3] = eps * (x[1] - x[2]);
}

//--------------------------------------------------------------------------------------------

void nf_kb_pathway(const state_type& x, state_type& dxdt, const double t) {

    float kNin = 5.4;
    float kIin = 0.018;
    float kIout = 0.012;
    float kNIout = 0.83;
    float kt = 1.03;
    float kt1 = 0.24;
    float kf = 30;
    float kfn = 30;
    float kb = 0.03;
    float kbn = 0.03;
    float alpha = 1.05 * 0.5;
    float gamma = 0.017;
    float k3 = 0.00001;
    float k1 = 0.00001;
    
    dxdt[0] = kNin * k3 * (1 - x[0]) / (k3 + x[2]) - (kNIout * x[3] * x[0]) / (k1 + x[0]);
    dxdt[1] = kt * x[0] * x[0] * x[0] - gamma * x[1];
    dxdt[2] = kt1 * x[1] - alpha * (1 - x[0]) * x[2] / (k3 + x[2]) - kIin * x[2] + (kIout * x[3] * k1) / (k1 + x[0]);
    dxdt[3] = kIin * x[2] - kIout * x[3] * k1 / (k1 + x[0]) - kNIout * x[3] * x[0] / (k1 + x[0]);
}

//--------------------------------------------------------------------------------------------

void peroxidase(const state_type& x, state_type& dxdt, const double t)
{
    float k1 = 0.35;
    float k2 = 250;
    float k3 = 0.035;
    float k4 = 20;
    float k5 = 5.35;
    float k6 = 0.0001;
    float k7 = 0.8;
    float k_7 = 0.1;
    float k8 = 0.825;

    dxdt[0] = -k3 * x[0] * x[1] * x[3] + k7 - k_7 * x[0];
    dxdt[1] = -k3 * x[0] * x[1] * x[3] - k1 * x[1] * x[2] + k8;
    dxdt[2] = k1 * x[1] * x[2] - 2 * k2 * x[2] * x[2] + 3 * k3 * x[0] * x[1] * x[3] - k4 * x[2] + k6;
    dxdt[3] = -k3 * x[0] * x[1] * x[3] + 2 * k2 * x[2] * x[2] - k5 * x[3];
}

//--------------------------------------------------------------------------------------------

void test(const state_type& x, state_type& dxdt, const double t) {
    dxdt[0] = x[0];
    dxdt[1] = x[1];
    dxdt[2] = x[2];
    dxdt[3] = x[3];
}

//--------------------------------------------------------------------------------------------

void bouali_1(const state_type& x, state_type& dxdt, const double t) {

    float k1 = -2;
    float k2 = 1;
    float k3 = 0.2f;
    float k4 = 1;

    dxdt[0] = x[0]*(1-x[1])+k1*x[2];
    dxdt[1] = k2*(pow(x[0],2)-1)*x[1];
    dxdt[2] = k3*(1-x[1])*x[3];
    dxdt[3] = k4*x[2];
}

void pendulum(const state_type& x, state_type& dxdt, const double t) {

    float a = 0.1;

    dxdt[0] = x[1];
    dxdt[1] = -sin(1.5*x[0]) - a * x[1];
    dxdt[2] = 0.1;
    dxdt[3] = 0.1;
}

void huang(const state_type& x, state_type& dxdt, const double t) {

    float a = 6;
    float b = 11; 
    float c = 5;

    dxdt[0] = a * (x[1] - x[0]);
    dxdt[1] = x[0] * x[2] + x[3];
    dxdt[2] = b - x[0] * x[1];
    dxdt[3] = x[1] * x[2] - c * x[3];
}

void bouali_2(const state_type& x, state_type& dxdt, const double t) {
    float k1 = 1.0;
    float k2 = 0.7;
    float k3 = -0.1;
    float k4 = 1.0;
    float k5 = -0.2;

    dxdt[0] = x[0] * (1 - x[1]) + k1 * x[2];
    dxdt[1] = k2 * (pow(x[0], 2) - 1) * x[1];
    dxdt[2] = k3 * x[0] + (1 - x[1]) * x[2] + k4 * x[3];
    dxdt[3] = k5 * x[0] * x[1];
}

void bouali_3(const state_type& x, state_type& dxdt, const double t) {

    float k1 = 3.0;
    float k2 = 0.1;
    float k3 = 1.0;
    float k4 = 0.02;
    float k5 = -0.01;


    dxdt[0] = k1 * x[0] * (1 - x[1]) - k2 * x[2];
    dxdt[1] = -(k3 - pow(x[0], 2)) * x[1];
    dxdt[2] = -k4 * x[3];
    dxdt[3] = -k5 * x[2];
}

void toxin(const state_type& x, state_type& dxdt, const double t) {

    float epsilon = 0.0783;
    float beta = 100;
    float n = 2;
    float kappa = 0.407;
    float alpha = 6.74;
    float gamma = 7.38;
    float delta_a = 1.16;
    float delta_c = 1.96;
    float delta_AT = 0.1 * (delta_a / epsilon);

    dxdt[0] = 1 / epsilon - x[0] - (beta - 1) * (pow(x[2], n) / pow(x[2], n) + pow(kappa, n)) * x[0];
    dxdt[1] = -alpha * x[1] * x[2] + gamma * x[0] - delta_a * x[1];
    dxdt[2] = -alpha * x[1] * x[2] + epsilon * (gamma * x[0] + delta_AT * x[3] - delta_c * x[2]);
    dxdt[3] = alpha * x[1] * x[2] - epsilon * (delta_c + delta_AT) * x[3];
}

std::vector<double> Trajectory_generator::integrate(std::vector<float>& vars, const char* system, double integration_end, double step_size, bool adaptive) {

    std::vector<state_type> x_vec;
    std::vector<double> times;
    state_type initial(4);

    initial[0] = vars.at(0); // start at x=...
    initial[1] = vars.at(1);
    initial[2] = vars.at(2);
    initial[3] = vars.at(3);

    //adaptive trajectories will have different stepsize -> the streamsurface can get less representative if the step size differs a lot
    if (!adaptive) {

        boost::numeric::odeint::runge_kutta4<state_type> stepper;

        if (system == "Lorenz") {
            integrate_const(stepper, lorenz_system, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
        }
        if (system == "PO_Reaction") {
            integrate_const(stepper, peroxidase, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
        }
        if (system == "Bipolar") {
            integrate_const(stepper, bipolar_disorder_model, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
        }
        if (system == "NF_KB_Pathway") {
            integrate_const(stepper, nf_kb_pathway, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
        }
        if (system == "test") {
            integrate_const(stepper, test, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
        }
        if (system == "bouali_1") {
            integrate_const(stepper, bouali_1, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
        }
        if (system == "pendulum") {
            integrate_const(stepper, pendulum, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
        }
        if (system == "huang") {
            integrate_const(stepper, huang, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
        }
        if (system == "bouali_2") {
            integrate_const(stepper, bouali_2, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
        }
        if (system == "bouali_3") {
            integrate_const(stepper, bouali_3, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
        }
        if (system == "toxin") {
            integrate_const(stepper, toxin, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
        }
    }
    else {
        if (system == "Lorenz") {
            size_t steps = boost::numeric::odeint::integrate(lorenz_system, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
        }
        if (system == "PO_Reaction") {
            size_t steps = boost::numeric::odeint::integrate(peroxidase, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
        }
        if (system == "Bipolar") {
            size_t steps = boost::numeric::odeint::integrate(bipolar_disorder_model, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
        }
        if (system == "NF_KB_Pathway") {
            size_t steps = boost::numeric::odeint::integrate(nf_kb_pathway, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
        }
        if (system == "test") {
            size_t steps = boost::numeric::odeint::integrate(test, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
        }
        if (system == "bouali_1") {
            size_t steps = boost::numeric::odeint::integrate(bouali_1, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
        }
        if (system == "pendulum") {
            size_t steps = boost::numeric::odeint::integrate(pendulum, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
        }
        if (system == "huang") {
            size_t steps = boost::numeric::odeint::integrate(huang, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
        }
        if (system == "bouali_2") {
            size_t steps = boost::numeric::odeint::integrate(bouali_2, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
        }
        if (system == "bouali_3") {
            size_t steps = boost::numeric::odeint::integrate(bouali_3, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
        }
        if (system == "toxin") {
            size_t steps = boost::numeric::odeint::integrate(toxin, initial, 0.0, integration_end, step_size, push_back_state_and_time(x_vec, times));
        }
    }

    std::vector<double> coordinates;

    for (int i = 0; i < times.size(); i++) {
        coordinates.push_back(times[i]);
        coordinates.push_back(x_vec[i][0]);
        coordinates.push_back(x_vec[i][1]);
        coordinates.push_back(x_vec[i][2]);
        if(system != "Lorenz")
            coordinates.push_back(x_vec[i][3]);
        else
            coordinates.push_back(i);
    }

    std::cout << "Calculated Trajectory with length: " << coordinates.size() / 5 << "\n";
    
    return coordinates;
}





