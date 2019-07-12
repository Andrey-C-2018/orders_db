#include "XDynamicSizer.h"

XDynamicSizer::XDynamicSizer() : margin_right(0), margin_bottom(0), \
									multiplier(1.0F), minus(0) { }

XDynamicSizer::XDynamicSizer(const int margin_right_, const int margin_bottom_) : \
			margin_right(margin_right_), margin_bottom(margin_bottom_), \
			multiplier(1.0F), minus(0) { }

XDynamicSizer::XDynamicSizer(XPoint initial_coords_, XSize parent_size_, \
							const int margin_right_, const int margin_bottom_) : \
			initial_coords(initial_coords_), parent_size(parent_size_), \
			margin_right(margin_right_), margin_bottom(margin_bottom_), \
			multiplier(1.0F), minus(0) { }

XDynamicSizer::~XDynamicSizer() { }