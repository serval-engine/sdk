#ifndef SERVAL_SDK__COMPONENTS_CORE_HPP
#define SERVAL_SDK__COMPONENTS_CORE_HPP

#include "../types.hpp"

namespace components::core {

	// Provides a name by which to look this component up by in the scene
	struct Named {
		serval::Id name;
	};

	struct Position {
		serval::Scalar x;
		serval::Scalar y;
		serval::Scalar z;
	};

	struct Orientation {
		serval::Scalar x;
		serval::Scalar y;
		serval::Scalar z;
	};

	struct Scaled {
		serval::Scalar x;
		serval::Scalar y;
		serval::Scalar z;
	};

}

#endif