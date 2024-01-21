#include <janet.h>
#include <box2d/box2d.h>

#include "types.h"
#include "body.h"
#include "world.h"
#include "shapes.h"

JANET_MODULE_ENTRY(JanetTable *env) {
  janet_cfuns(env, "janet-box2d", body_cfuns);
  janet_cfuns(env, "janet-box2d", world_cfuns);
  janet_cfuns(env, "janet-box2d", shapes_cfuns);
}
