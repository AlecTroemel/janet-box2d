#include <janet.h>
#include <box2d/box2d.h>


// Box2d World
static int at_world_gc(void *p, size_t s) {
    (void) s;
    b2WorldId *worldId = (b2WorldId *)p;
    b2DestroyWorld(*worldId);
    return 0;
}

static void at_world_tostring(void *p, JanetBuffer *buffer) {
  char str[32];
  b2WorldId *worldId = (b2WorldId *)p;
  sprintf(str, "%i", worldId->index);
  janet_buffer_push_cstring(buffer, str);
}

static const JanetAbstractType AT_World = {
  "b2/world",
  at_world_gc,
  NULL, NULL, NULL, NULL, NULL,
  at_world_tostring,
  JANET_ATEND_TOSTRING
};

// Box2d Body
static int at_body_gc(void *p, size_t s) {
    (void) s;
    b2BodyId *bodyId = (b2BodyId *)p;
    b2DestroyBody(*bodyId);
    return 0;
}

static void at_body_tostring(void *p, JanetBuffer *buffer) {
  char str[32];
  b2BodyId *bodyId = (b2BodyId *)p;
  sprintf(str, "%i", bodyId->index);
  janet_buffer_push_cstring(buffer, str);
}

// helper getter functions
static b2WorldId *box2d_getworld(const Janet *argv, int32_t n) {
  return ((b2WorldId *)janet_getabstract(argv, n, &AT_World));
};

static float idx_getfloat(JanetView idx, int index) {
    if (index >= idx.len) {
	janet_panicf("index %d outside of range [0, %d)", idx.len);
    }
    if (!janet_checktype(idx.items[index], JANET_NUMBER)) {
	janet_panicf("expected number, got %v", idx.items[index]);
    }
    return (float) janet_unwrap_number(idx.items[index]);
}

static b2Vec2 box2d_getvec2(const Janet *argv, int32_t n) {
    JanetView idx = janet_getindexed(argv, n);
    return (b2Vec2) {
	idx_getfloat(idx, 0),
	idx_getfloat(idx, 1)
    };
}

static const JanetAbstractType AT_Body = {
  "b2/body",
  at_body_gc,
  NULL, NULL, NULL, NULL, NULL,
  at_body_tostring,
  JANET_ATEND_TOSTRING
};

static Janet janet_create_body(int32_t argc, Janet *argv) {
  janet_arity(argc, 1, 4);

  b2WorldId *worldId = box2d_getworld(argv, 0);
  b2BodyId *bodyId = janet_abstract(&AT_Body, sizeof(b2BodyId));
  b2BodyDef bodyDef = b2_defaultBodyDef;

  if (argc >= 2) bodyDef.position = box2d_getvec2(argv, 1);
  if (argc >= 3) bodyDef.angle = (float)janet_getnumber(argv, 2);

  *bodyId = b2CreateBody(*worldId, &bodyDef);
  return janet_wrap_abstract(bodyId);
}

static Janet janet_create_world(int32_t argc, Janet *argv) {
  janet_arity(argc, 0, 1);

  b2WorldDef worldDef = b2_defaultWorldDef;

  if (argc >= 2) worldDef.gravity = box2d_getvec2(argv, 1);
  worldDef.workerCount = 1;
  worldDef.enableSleep = false;
  worldDef.bodyCapacity = 2;
  worldDef.contactCapacity = 2;
  worldDef.arenaAllocatorCapacity = 0;

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


static const JanetReg cfuns[] = {
  {
    "create-world",
    janet_create_world,
    "(create-world gravity-x gravity-y)\n\n"
    "Create a physics world."
  },
  {"create-body", janet_create_body, NULL},
  {"world-step", janet_world_step, NULL},
  {NULL, NULL}
};

JANET_MODULE_ENTRY(JanetTable *env) {
  janet_cfuns(env, "janetbox2d", cfuns);
  janet_register_abstract_type(&AT_World);
  janet_register_abstract_type(&AT_Body);
}
