#include <box2d/box2d.h>

static Janet janet_create_body(int32_t argc, Janet *argv) {
  janet_arity(argc, 1, 2);

  b2WorldId *worldId = box2d_getworld(argv, 0);
  b2BodyId *bodyId = janet_abstract(&AT_Body, sizeof(b2BodyId));
  b2BodyDef bodyDef = b2_defaultBodyDef;

  if (argc >= 2) {
    JanetDictView tab = janet_getdictionary(argv, 1);
    Janet maybe_type = janet_dictionary_get(tab.kvs, tab.cap, janet_ckeywordv("type"));
    Janet maybe_position = janet_dictionary_get(tab.kvs, tab.cap, janet_ckeywordv("position"));
    Janet maybe_angle = janet_dictionary_get(tab.kvs, tab.cap, janet_ckeywordv("angle"));
    Janet maybe_fixed_rotation = janet_dictionary_get(tab.kvs, tab.cap, janet_ckeywordv("fixed-rotation?"));

    if (!janet_checktype(maybe_type, JANET_NIL)) {
      const uint8_t *type = janet_getkeyword(&maybe_type, 0);
      if (!janet_cstrcmp(type, "static")) {
	bodyDef.type = b2_staticBody;
      } else if (!janet_cstrcmp(type, "kinematic")) {
	bodyDef.type = b2_kinematicBody;
      } else if (!janet_cstrcmp(type, "dynamic")) {
	bodyDef.type = b2_dynamicBody;
      } else {
	janet_panicf("unknown body type ':%s'", type);
      }
    }

    if (!janet_checktype(maybe_position, JANET_NIL)) {
      bodyDef.position = box2d_getvec2(&maybe_position, 0);
    }

    if (!janet_checktype(maybe_angle, JANET_NIL)) {
      bodyDef.angle = (float)janet_getnumber(&maybe_angle, 0);
    }

    if (!janet_checktype(maybe_fixed_rotation, JANET_NIL)) {
      bodyDef.fixedRotation = janet_getboolean(&maybe_fixed_rotation, 0);
    }
  }

  *bodyId = b2CreateBody(*worldId, &bodyDef);
  return janet_wrap_abstract(bodyId);
}

static Janet janet_body_apply_force_to_center(int32_t argc, Janet *argv) {
  janet_arity(argc, 2, 4);
  b2BodyId *bodyId = box2d_getbody(argv, 0);
  b2Vec2 force = box2d_getvec2(argv, 1);
  bool wake = janet_optboolean(argv, argc, 2, false);

  b2Body_ApplyForceToCenter(*bodyId, force, wake);
  return janet_wrap_nil();
}

static const JanetReg body_cfuns[] = {
  {"create-body", janet_create_body, NULL},
  {"body-apply-force-to-center", janet_body_apply_force_to_center, NULL},
  {NULL, NULL}
};
