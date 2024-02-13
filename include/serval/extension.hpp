#ifndef SERVAL_SDK__SERVAL_EXTENSION_HPP
#define SERVAL_SDK__SERVAL_EXTENSION_HPP

#include "sdk/events/extension.hpp"
#include "sdk/api.hpp"
#include "sdk/macros.hpp"

void serval_extension_load(serval::Init&);
void serval_extension_unload(serval::Init&);
void serval_extension_reload(serval::Init&);

#endif