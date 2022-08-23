#include "Scene.h"
// local
#include "Mesh.h"
// std
#include <fstream>
// boost
#include <boost/algorithm/string.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/assignment.hpp>

#include "Trajectory_generator.h"
#include "Drawable_Streamsurface.h"

//******************************************************************************
// Scene
//******************************************************************************

Scene::Scene(std::shared_ptr<Scene_state> state)
    : state_(state)
{
}

//******************************************************************************
// load_ode
//******************************************************************************

void Scene::load_ode(
    const std::vector<std::string>& fnames,
    float cuve_min_rad,
    float tesseract_size/* = 200.f*/)
{
    assert(state_);
    if(state_ == nullptr)
        return;

    // Remove all previous curves
    state_->curves.clear();
    // Reset size of the tesseract
    for(auto& s : state_->tesseract_size)
        s = tesseract_size;

    // The aggregative origin and size for all curves
    Scene_vertex_t total_origin(5), total_size(5);
    for(char i = 0; i < 5; ++i)
    {
        total_origin(i) = std::numeric_limits<float>::max();
        total_size(i)   = std::numeric_limits<float>::min();
    }

    std::vector<std::shared_ptr<Curve>> curves;
    
    // Load curves from files and calculate statistics
    for(auto& fn: fnames)
    {
        // Load curves
        auto curve = load_curve(fn);
        if(!curve)
            continue;

        Scene_vertex_t origin, size;
        curve->get_boundaries(origin, size);

        for(char i = 0; i < 5; ++i)
        {
            if(total_origin(i) > origin(i)) total_origin(i) = origin(i);
            if(total_size(i)   < size(i)  ) total_size(i)   = size(i);
        }

        curves.push_back(std::move(curve));
    }

    if(!state_->scale_tesseract)
    {
        float max_size = std::numeric_limits<float>::min();
        for(char i = 0; i < 4; ++i)
        {
            if(max_size < total_size(i)) max_size = total_size(i);
        }

        for(char i = 0; i < 4; ++i)
        {
            state_->tesseract_size[i] =
                tesseract_size * total_size[i] / max_size;
        }
    }

    Scene_vertex_t scale(5);
    scale[0] = state_->tesseract_size[0] / total_size[0];
    scale[1] = state_->tesseract_size[1] / total_size[1];
    scale[2] = state_->tesseract_size[2] / total_size[2];
    scale[3] = state_->tesseract_size[3] / total_size[3];
    scale[4] = 1;

    for(auto& c : curves)
    {
        c->translate_vertices(-0.5f * total_size - total_origin);
        c->scale_vertices(scale);

        auto curve = std::make_shared<Curve>(
            c->get_simpified_curve(cuve_min_rad));
        curve->update_stats(
            state_->stat_kernel_size,
            state_->stat_max_movement,
            state_->stat_max_value);
        state_->curves.push_back(std::move(curve));
    }

    // Save curve origin and size to the class members
    create_tesseract();

    if(state_->curves.size() > 0)
    {
        // Set selection (currently we take the range of the first curve, but
        // this behaviour can be reconsidered in the future)
        auto& first_curve = state_->curves.front();
        state_->curve_selection = std::make_unique<Curve_selection>();
        state_->curve_selection->t_start = first_curve->t_min();
        state_->curve_selection->t_end = first_curve->t_max();
    }
}

//******************************************************************************
// create ode
// generates trajectories during runtime
//******************************************************************************

std::shared_ptr<Curve> Scene::create_ode(float a, float b, float c, float x, float y, float z, float w, float cuve_min_rad, const char* system, float tesseract_size/* = 200.f*/) {

    //++
    /*
    assert(state_);
    if (state_ == nullptr)
    return;
    */

    // Remove all previous curves
    //state_->curves.clear();
    
    // Reset size of the tesseract
    for (auto& s : state_->tesseract_size)
    s = tesseract_size;

    // The aggregative origin and size for all curves
    Scene_vertex_t total_origin(5), total_size(5);
    for (char i = 0; i < 5; ++i)
    {
        total_origin(i) = std::numeric_limits<float>::max();
        total_size(i) = std::numeric_limits<float>::min();
    }
    //--

    auto curve = std::make_shared<Curve>();
    auto tg = std::make_unique<Trajectory_generator>();

    std::vector<float> vars{ a,b,c,x,y,z,w };

    std::vector<double> coordinates = tg->integrate(vars, system);
    Scene_vertex_t p(5);

    for (int i = 0; i < coordinates.size() - 5; i += 5) {

        p <<= coordinates.at(i + 1), coordinates.at(i + 2), coordinates.at(i + 3), coordinates.at(i + 4), 1;
        float t = coordinates.at(i);
        curve->add_point(p, t);
        /*
        std::cout << "point: ";
        for (auto p_ : p) {
            std::cout << p_ << ", ";
        }
        std::cout << "\n";
        */
    }

    Scene_vertex_t origin, size;
    curve->get_boundaries(origin, size);

    for (char i = 0; i < 5; ++i)
    {
        if (total_origin(i) > origin(i)) total_origin(i) = origin(i);
        if (total_size(i) < size(i)) total_size(i) = size(i);
    }

    if (!state_->scale_tesseract)
    {
        float max_size = std::numeric_limits<float>::min();
        for (char i = 0; i < 4; ++i)
        {
            if (max_size < total_size(i)) max_size = total_size(i);
        }

        for (char i = 0; i < 4; ++i)
        {
            state_->tesseract_size[i] =
                tesseract_size * total_size[i] / max_size;
        }
    }

    Scene_vertex_t scale(5);
    scale[0] = state_->tesseract_size[0] / total_size[0];
    scale[1] = state_->tesseract_size[1] / total_size[1];
    scale[2] = state_->tesseract_size[2] / total_size[2];
    scale[3] = state_->tesseract_size[3] / total_size[3];
    scale[4] = 1;
    
    curve->translate_vertices(-0.5f * total_size - total_origin);
    
    curve->scale_vertices(scale);

    //curve = std::make_shared<Curve>(curve->get_simpified_curve(cuve_min_rad));
    curve->update_stats(
        state_->stat_kernel_size,
        state_->stat_max_movement,
        state_->stat_max_value);

    state_->curves.push_back(std::move(curve));

    create_tesseract();
    auto& first_curve = state_->curves.front();
    state_->curve_selection = std::make_unique<Curve_selection>();
    state_->curve_selection->t_start = first_curve->t_min();
    state_->curve_selection->t_end = first_curve->t_max();

    return curve;

}

//******************************************************************************
// load_curve
//******************************************************************************

std::shared_ptr<Curve> Scene::load_curve(std::string fname)
{
    std::ifstream stream(fname);
    if(!stream.is_open())
        return nullptr;

    auto curve = std::make_shared<Curve>();

    std::string line;
    while(getline(stream, line))
    {
        // Remove some characters to make it easier to load files we get from
        // Wolfram Mathematica
        boost::erase_all(line, "\"{");
        boost::erase_all(line, "}\"");
        boost::replace_all(line, ", ", " ");
        boost::replace_all(line, "*^", "e");

        std::vector<std::string> vals;
        boost::split(vals, line, boost::is_any_of("\t, "));
        if(vals.size() == 5)
        {
            float t = std::stof(vals[0]);
            Scene_vertex_t p(5);
            p <<= std::stof(vals[1]), std::stof(vals[2]), std::stof(vals[3]),
                std::stof(vals[4]), 1;

            curve->add_point(p, t);
        }
    }

    return curve;
}

//******************************************************************************
// normalize_curve
//******************************************************************************

void Scene::normalize_curve(Curve& curve)
{
    Scene_vertex_t origin, size;
    curve.get_boundaries(origin, size);

    Scene_vertex_t scale(5);
    scale[0] = state_->tesseract_size[0] / size[0];
    scale[1] = state_->tesseract_size[0] / size[1];
    scale[2] = state_->tesseract_size[0] / size[2];
    scale[3] = state_->tesseract_size[0] / size[3];
    scale[4] = 1;
    curve.scale_vertices(scale);
}

//******************************************************************************
// create_tesseract
//******************************************************************************

void Scene::create_tesseract()
{
    assert(state_);
    if(state_ == nullptr)
        return;

    Scene_vertex_t origin(4), size(4);
    for(char i = 0; i < 4; ++i)
    {
        origin(i) = -0.5f * state_->tesseract_size[i];
        size(i) = state_->tesseract_size[i];
    }

    state_->tesseract = std::make_unique<Tesseract>(
        origin,
        size,
        state_->get_color(X_axis),
        state_->get_color(Y_axis),
        state_->get_color(Z_axis),
        state_->get_color(W_axis));
}

//******************************************************************************
// create_surface
//******************************************************************************

void Scene::create_surface(std::vector<float> &vars, std::vector<std::vector<double>> &initial, const char* system, glm::vec2 &integration_steps_size, float tesseract_size/* = 200.f*/)
{
    // Reset size of the tesseract
    for (auto& s : state_->tesseract_size)
        s = tesseract_size;

    auto tg = std::make_unique<Trajectory_generator>();
    auto trajectories = std::make_unique<std::vector<std::vector<double>>>();

    auto surface = std::make_shared<Drawable_Streamsurface>();
    surface->set_color(state_->get_curve_color(state_->surfaces.size()));

    if (state_->use_cpu) {
        surface->disable_compute_flag();
    }

    // The aggregative origin and size for all curves
    Scene_vertex_t total_origin(5), total_size(5);
    for (char i = 0; i < 5; ++i)
    {
        total_origin(i) = std::numeric_limits<float>::max();
        total_size(i) = std::numeric_limits<float>::min();
    }

    //calculate trajectories
    for (std::vector<double> p : initial) {

        //concatenate initial coordinates to the system parameters 
        // TODO currently only x,y,z
        std::vector<float> temp_vars(vars);
        temp_vars.push_back(p.at(0));
        temp_vars.push_back(p.at(1));
        temp_vars.push_back(p.at(2));
        temp_vars.push_back(p.at(3));

        std::vector<double> trajectory = tg->integrate(temp_vars, system, integration_steps_size[0], integration_steps_size[1]);
        //mean of trajectory
        //auto const count = static_cast<double>(trajectory.size());
        //auto mean = std::reduce(trajectory.begin(), trajectory.end()) / count;

        trajectories->push_back(trajectory);

    }
    //create Streamsurface object
    int minimum = INT_MAX;
    for (int i = 0; i < trajectories->size(); i++) {
        if (trajectories->at(i).size() < minimum) {
            minimum = trajectories->at(i).size();
        }
    }

    for (int i = 0; i < minimum; i+=5) {
        std::vector<Scene_vertex_t> points;
        for (int ii = 0; ii < trajectories->size(); ii++) {
            Scene_vertex_t p(5);
            p <<= trajectories->at(ii).at(i + 1), trajectories->at(ii).at(i + 2), trajectories->at(ii).at(i + 3), trajectories->at(ii).at(i + 4), 1;
            points.push_back(p);
            /*
            for (auto p_ : p) {
                std::cout << p_ << ", ";
            }
            std::cout << " \n";
            */
        }
        float t = trajectories->at(0).at(i);
        surface->add_point_strip(points, t);
    }

    //origin and size
    Scene_vertex_t origin(5), size(5);
    surface->get_boundaries(origin, size);
    for (char i = 0; i < 5; ++i)
    {
        if (total_origin(i) > origin(i)) total_origin(i) = origin(i);
        if (total_size(i) < size(i)) total_size(i) = size(i);
    }

    //surfaces.push_back(std::move(surface));

    //scale
    if (!state_->scale_tesseract)
    {
        float max_size = std::numeric_limits<float>::min();
        for (char i = 0; i < 4; ++i)
        {
            if (max_size < total_size(i)) max_size = total_size(i);
        }

        for (char i = 0; i < 4; ++i)
        {
            state_->tesseract_size[i] = tesseract_size * total_size[i] / max_size;
        }
    }

    Scene_vertex_t scale(5);
    scale[0] = state_->tesseract_size[0] / total_size[0];
    scale[1] = state_->tesseract_size[1] / total_size[1];
    scale[2] = state_->tesseract_size[2] / total_size[2];
    scale[3] = state_->tesseract_size[3] / total_size[3];
    scale[4] = 1;

    Scene_vertex_t translate = -0.5f * total_size - total_origin;

    surface->translate_vertices(translate);
    surface->scale_vertices(scale);
    surface->set_projection_camera(state_->projection_4D, state_->camera_4D);
    boost::numeric::ublas::matrix<float> identity = boost::numeric::ublas::identity_matrix<float>(5);
    surface->set_rotation(identity);
    surface->setup_mesh();

    state_->surfaces.push_back(std::move(surface));

    //state_->surfaces.push_back(std::move(surface));

    create_tesseract();

}

