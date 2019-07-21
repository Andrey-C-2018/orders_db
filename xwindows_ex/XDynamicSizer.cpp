#include "XDynamicSizer.h"

XDynamicSizer::XDynamicSizer() : width_key(1), height_key(1), \
								margin_right(0), margin_bottom(0), \
								minus_x(0), minus_y(0), \
								multiplier_x(1.0F), multiplier_y(1.0F) { }

XDynamicSizer::XDynamicSizer(const int margin_right_, const int margin_bottom_) : \
			width_key(1), height_key(1), \
			margin_right(margin_right_), margin_bottom(margin_bottom_), \
			minus_x(0), minus_y(0), \
			multiplier_x(1.0F), multiplier_y(1.0F) { }

XDynamicSizer::XDynamicSizer(XPoint initial_coords_, XSize parent_size_, \
							const int margin_right_, const int margin_bottom_) : \
			initial_coords(initial_coords_), parent_size(parent_size_), \
			width_key(1), height_key(1), \
			margin_right(margin_right_), margin_bottom(margin_bottom_), \
			minus_x(0), minus_y(0), \
			multiplier_x(1.0F), multiplier_y(1.0F) { }

XDynamicSizer::~XDynamicSizer() { }