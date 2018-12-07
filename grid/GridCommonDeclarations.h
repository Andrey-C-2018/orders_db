#pragma once
#include <memory>

enum Layouts {
	LAYOUT_RECORD = 0, \
	LAYOUT_FIELD = 1
};

struct IGridCellsContainer;
struct ISharedComposerContainer;

typedef std::pair<IGridCellsContainer *, \
					ISharedComposerContainer *> LayoutObjects;