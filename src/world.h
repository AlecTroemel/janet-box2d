#include <box2d/box2d.h>

static Janet janet_create_world(int32_t argc, Janet *argv) {
  janet_arity(argc, 0, 1);

  b2WorldDef worldDef = b2_defaultWorldDef;
  worldDef.workerCount = 1;
  worldDef.enableSleep = false;

  if (argc >= 1) {
    JanetDictView tab = janet_getdictionary(argv, 0);
    Janet maybe_gravity = janet_dictionary_get(tab.kvs, tab.cap, janet_ckeywordv("gravity"));

    if (!janet_checktype(maybe_gravity, JANET_NIL)) {
      worldDef.gravity = box2d_getvec2(&maybe_gravity, 0);
    }
  }

  b2WorldId *worldId = janet_abstract(&AT_World, sizeof(b2WorldId));
  *worldId = b2CreateWorld(&worldDef);
  return janet_wrap_abstract(worldId);
}

static Janet janet_world_step(int32_t argc, Janet *argv) {
  janet_fixarity(argc, 2);

  b2WorldId *worldId = box2d_getworld(argv, 0);
  float timeStep = (float) janet_getnumber(argv, 1);

  int velocityIterations = 10;
  int relaxIterations = 3;

  b2World_Step(*worldId, timeStep, velocityIterations, relaxIterations);
  return janet_wrap_nil();
}

static const JanetReg world_cfuns[] = {
  {
    "create-world",
    janet_create_world,
    "(create-world gravity-x gravity-y)\n\n"
    "Create a physics world."
  },
  {"world-step", janet_world_step, NULL},
  {NULL, NULL}
};
