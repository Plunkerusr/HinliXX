#pragma once

#include "View.hpp"

#include <wisterialib/Vector.hpp>

namespace UI {

class ViewGroup : public View {
public:
    inline void add_view(View* view, LayoutParams* layout_params)
    {
        view->set_layout_params(layout_params);
        m_children.push_back(view);
    }

protected:
    // Validates layout params of its children
    bool check_layout_params(LayoutParams* params);
    // Returns a safe set of layout parameters based on the supplied layout params
    bool generate_layout_params(LayoutParams* params);
    // These parameters are requested when the View has no layout already set
    LayoutParams* generate_default_layout_params();

protected:
    Vector<View*> m_children {};
    LayoutParams* layout_params {};
};

}
