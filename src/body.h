#include "box2d/box2d.h"

static Janet janet_create_body(int32_t argc, Janet *argv) {
  janet_arity(argc, 1, 2);

  b2WorldId *worldId = box2d_getworld(argv, 0);
  b2BodyId *bodyId = janet_abstract(&AT_Body, sizeof(b2BodyId));
  b2BodyDef bodyDef = b2DefaultBodyDef();

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

    // TODO: should we just always pass in the full sin/cos roation?
    if (!janet_checktype(maybe_angle, JANET_NIL)) {
      bodyDef.rotation = b2MakeRot((float)janet_getnumber(&maybe_angle, 0));
    }

    if (!janet_checktype(maybe_fixed_rotation, JANET_NIL)) {
      bodyDef.fixedRotation = janet_getboolean(&maybe_fixed_rotation, 0);
    }
  }

  *bodyId = b2CreateBody(*worldId, &bodyDef);
  return janet_wrap_abstract(bodyId);
}

static Janet janet_body_apply_force(int32_t argc, Janet *argv) {
  janet_arity(argc, 3, 4);

  b2BodyId *bodyId = box2d_getbody(argv, 0);
  b2Vec2 force = box2d_getvec2(argv, 1);
  b2Vec2 point = box2d_getvec2(argv, 2);
  bool wake = janet_optboolean(argv, argc, 3, false);

  b2Body_ApplyForce(*bodyId, force, point, wake);
  return janet_wrap_nil();
}

static Janet janet_body_apply_force_to_center(int32_t argc, Janet *argv) {
  janet_arity(argc, 2, 4);

  b2BodyId *bodyId = box2d_getbody(argv, 0);
  b2Vec2 force = box2d_getvec2(argv, 1);
  bool wake = janet_optboolean(argv, argc, 2, false);

  b2Body_ApplyForceToCenter(*bodyId, force, wake);
  return janet_wrap_nil();
}

static Janet janet_body_apply_torque(int32_t argc, Janet *argv) {
  janet_arity(argc, 2, 3);

  b2BodyId *bodyId = box2d_getbody(argv, 0);
  float torque = (float)janet_getnumber(argv, 1);
  bool wake = janet_optboolean(argv, argc, 2, false);

  b2Body_ApplyTorque(*bodyId, torque, wake);
  return janet_wrap_nil();
}

static Janet janet_body_apply_linear_impulse(int32_t argc, Janet *argv) {
  janet_arity(argc, 3, 4);

  b2BodyId *bodyId = box2d_getbody(argv, 0);
  b2Vec2 impulse = box2d_getvec2(argv, 1);
  b2Vec2 point = box2d_getvec2(argv, 2);
  bool wake = janet_optboolean(argv, argc, 3, false);

  b2Body_ApplyLinearImpulse(*bodyId, impulse, point, wake);
  return janet_wrap_nil();
}

static Janet janet_body_apply_linear_impulse_to_center(int32_t argc, Janet *argv) {
  janet_arity(argc, 2, 4);

  b2BodyId *bodyId = box2d_getbody(argv, 0);
  b2Vec2 impulse = box2d_getvec2(argv, 1);
  bool wake = janet_optboolean(argv, argc, 2, false);

  b2Body_ApplyLinearImpulseToCenter(*bodyId, impulse, wake);
  return janet_wrap_nil();
}

static Janet janet_body_apply_angular_impulse(int32_t argc, Janet *argv) {
  janet_arity(argc, 2, 3);

  b2BodyId *bodyId = box2d_getbody(argv, 0);
  float impulse = (float)janet_getnumber(argv, 1);
  bool wake = janet_optboolean(argv, argc, 2, false);

  b2Body_ApplyAngularImpulse(*bodyId, impulse, wake);
  return janet_wrap_nil();
}

static Janet janet_body_set_awake(int32_t argc, Janet *argv) {
  janet_fixarity(argc, 2);
  b2BodyId *bodyId = box2d_getbody(argv, 0);
  bool wake = janet_getboolean(argv, 1);
  b2Body_SetAwake(*bodyId, wake);
  return janet_wrap_nil();
}

static const JanetReg body_cfuns[] = {
  {"create-body", janet_create_body, NULL},
  {"body-apply-force", janet_body_apply_force, NULL},
  {"body-apply-force-to-center", janet_body_apply_force_to_center, NULL},
  {"body-apply-torque", janet_body_apply_torque, NULL},
  {"body-apply-linear-impulse", janet_body_apply_linear_impulse, NULL},
  {"body-apply-linear-impulse-to-center", janet_body_apply_linear_impulse_to_center, NULL},
  {"body-apply-angular-impulse", janet_body_apply_angular_impulse, NULL},
  {"body-set-awake", janet_body_set_awake, NULL},
  {NULL, NULL, NULL}
};
