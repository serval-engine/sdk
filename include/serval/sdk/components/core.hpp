#ifndef SERVAL_SDK__COMPONENTS_CORE_HPP
#define SERVAL_SDK__COMPONENTS_CORE_HPP

#include "../types.hpp"

namespace components::core {

	// Provides a name by which to look this component up by in the scene
	struct Named {
		entt::hashed_string name;
	};

	struct Position {
		Scalar x;
		Scalar y;
		Scalar z;
	};

	struct Orientation {
		Scalar x;
		Scalar y;
		Scalar z;
	};

	struct Scaled {
		Scalar x;
		Scalar y;
		Scalar z;
	};

}

#endif