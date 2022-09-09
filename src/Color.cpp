#include "Color.h"

//******************************************************************************
// Color
//******************************************************************************

Color::Color()
    : r_{0}
    , g_{0}
    , b_{0}
    , a_{0}
{ }

Color::Color(int r, int g, int b, int a)
{
    r_ = r;
    g_ = g;
    b_ = b;
    a_ = a;
}

//******************************************************************************
// r
//******************************************************************************

int Color::r() const
{
    return r_;
}

//******************************************************************************
// g
//******************************************************************************

int Color::g() const
{
    return g_;
}

//******************************************************************************
// b
//******************************************************************************

int Color::b() const
{
    return b_;
}

//******************************************************************************
// a
//******************************************************************************

int Color::a() const
{
    return a_;
}

//******************************************************************************
// r_norm
//******************************************************************************

float Color::r_norm() const
{
    return static_cast<float>(r_) / 255.f;
}

//******************************************************************************
// g_norm
//******************************************************************************

float Color::g_norm() const
{
    return static_cast<float>(g_) / 255.f;
}

//******************************************************************************
// b_norm
//******************************************************************************

float Color::b_norm() const
{
    return static_cast<float>(b_) / 255.f;
}

//******************************************************************************
// a
//******************************************************************************

float Color::a_norm() const
{
    return static_cast<float>(a_) / 255.f;
}
