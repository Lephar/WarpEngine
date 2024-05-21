#pragma once

#include <vector>

namespace Graphics {
	void initialize(const char *title, std::vector<const char *> extensions);

	void destroy(void);
}
