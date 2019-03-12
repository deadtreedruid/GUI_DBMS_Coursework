#include "screen.h"
#include "elements\base.h"

namespace gui {
	screen::~screen() {
		for (auto elem : m_elems)
			safe_delete(elem);
		m_elems.clear();
		m_focus = nullptr;
	}

	// will call ~screen after calling this destructor so no need to delete elems again
	screen_subscreenable::~screen_subscreenable() {
		for (auto subscreen : m_subscreens)
			safe_delete(subscreen);
		m_subscreens.clear();
		m_active_subscreen = nullptr;
	}
}