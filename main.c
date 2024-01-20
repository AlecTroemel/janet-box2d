#include <janet/janet.h>
#include "box2d/box2d.h"

static Janet janet_world(int32_t argc, Janet *argv) {
  janet_fixarity(argc, 0);

  // TODO pull this from args
  b2Vec2 gravity = {0.0f, -10.0f};

  b2WorldDef worldDef = b2_defaultWorldDef;

  // TODO use set these
  worldDef.workerCount = 1;
  worldDef.enableSleep = false;
  worldDef.bodyCapacity = 2;
  worldDef.contactCapacity = 2;
  worldDef.arenaAllocatorCapacity = 0;

  b2WorldId worldId = b2CreateWorld(&worldDef);

  return janet_wrap_false();
}

static const JanetReg cfuns[] = {
  {"world", janet_world, NULL},
};

JANET_MODULE_ENTRY(JanetTable *env) {
    janet_cfuns(env, "box2d", cfuns);
}
