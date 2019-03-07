#pragma once
#include <memory>

enum Layouts {
	LAYOUT_RECORD = 0, \
	LAYOUT_FIELD = 1
};

struct IGridCellsContainer;
struct ISharedComposerContainer;
struct IGCParamsList;
class CCellConfigurator;

typedef std::pair<IGridCellsContainer *, \
					ISharedComposerContainer *> LayoutObjects;

typedef std::pair<std::shared_ptr<CCellConfigurator>, \
					std::shared_ptr<CCellConfigurator> > GridConfigurators;

typedef std::pair<std::shared_ptr<IGCParamsList>, \
					std::shared_ptr<IGCParamsList> > GridGCParamsLists;