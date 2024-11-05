#include "box2d/box2d.h"


static Janet janet_create_distance_joint(int32_t argc, Janet *argv) {
  janet_arity(argc, 3, 4);
  b2WorldId *worldId = box2d_getworld(argv, 0);
  b2DistanceJointDef jointDef = b2DefaultDistanceJointDef();

  jointDef.bodyIdA = *box2d_getbody(argv, 1);
  jointDef.bodyIdB = *box2d_getbody(argv, 2);

  if (argc >= 4) {
    JanetDictView tab = janet_getdictionary(argv, 3);

    Janet maybe_anchor_a = janet_dictionary_get(tab.kvs, tab.cap, janet_ckeywordv("anchor-a"));
    Janet maybe_anchor_b = janet_dictionary_get(tab.kvs, tab.cap, janet_ckeywordv("anchor-b"));
    Janet maybe_length = janet_dictionary_get(tab.kvs, tab.cap, janet_ckeywordv("length"));
    Janet maybe_hertz = janet_dictionary_get(tab.kvs, tab.cap, janet_ckeywordv("hertz"));
    Janet maybe_damping_ratio = janet_dictionary_get(tab.kvs, tab.cap, janet_ckeywordv("damping-ration"));
    Janet maybe_collide_connected = janet_dictionary_get(tab.kvs, tab.cap, janet_ckeywordv("collide-connected?"));

    if (!janet_checktype(maybe_anchor_a, JANET_NIL)) {
      jointDef.localAnchorA = box2d_getvec2(&maybe_anchor_a, 0);
    }

    if (!janet_checktype(maybe_anchor_b, JANET_NIL)) {
      jointDef.localAnchorB = box2d_getvec2(&maybe_anchor_b, 0);
    }

    if (!janet_checktype(maybe_length, JANET_NIL)) {
      jointDef.length = janet_getnumber(&maybe_length, 0);
    } else if (!janet_checktype(maybe_anchor_a, JANET_NIL) &&
	       !janet_checktype(maybe_anchor_b, JANET_NIL)) {
      b2Vec2 anchorA = b2Body_GetWorldPoint(jointDef.bodyIdA, jointDef.localAnchorA);
      b2Vec2 anchorB = b2Body_GetWorldPoint(jointDef.bodyIdB, jointDef.localAnchorB);
      jointDef.length = b2Distance(anchorA, anchorB);
    }

    if (!janet_checktype(maybe_hertz, JANET_NIL)) {
      jointDef.hertz = janet_getnumber(&maybe_hertz, 0);
    }

    if (!janet_checktype(maybe_damping_ratio, JANET_NIL)) {
      jointDef.dampingRatio = janet_getnumber(&maybe_damping_ratio, 0);
    }

    if (!janet_checktype(maybe_collide_connected, JANET_NIL)) {
      jointDef.collideConnected = janet_getboolean(&maybe_collide_connected, 0);
    }
  }

  b2JointId *jointId = janet_abstract(&AT_Joint, sizeof(b2JointId));
  *jointId = b2CreateDistanceJoint(*worldId, &jointDef);

  return janet_wrap_abstract(jointId);
}


static Janet janet_create_mouse_joint(int32_t argc, Janet *argv) {
  janet_arity(argc, 3, 4);
  b2WorldId *worldId = box2d_getworld(argv, 0);
  b2MouseJointDef jointDef = b2DefaultMouseJointDef();

  jointDef.bodyIdA = *box2d_getbody(argv, 1);
  jointDef.bodyIdB = *box2d_getbody(argv, 2);

  if (argc >= 4) {
    JanetDictView tab = janet_getdictionary(argv, 3);

    Janet maybe_target = janet_dictionary_get(tab.kvs, tab.cap, janet_ckeywordv("target"));
    Janet maybe_max_force = janet_dictionary_get(tab.kvs, tab.cap, janet_ckeywordv("max-force"));
    Janet maybe_hertz = janet_dictionary_get(tab.kvs, tab.cap, janet_ckeywordv("hertz"));
    Janet maybe_damping_ratio = janet_dictionary_get(tab.kvs, tab.cap, janet_ckeywordv("damping-ratio"));

    if (!janet_checktype(maybe_target, JANET_NIL)) {
      jointDef.target = box2d_getvec2(&maybe_target, 0);
    }

    if (!janet_checktype(maybe_max_force, JANET_NIL)) {
      jointDef.maxForce = janet_getnumber(&maybe_max_force, 0);
    }

    if (!janet_checktype(maybe_hertz, JANET_NIL)) {
      jointDef.hertz = janet_getnumber(&maybe_hertz, 0);
    }

    if (!janet_checktype(maybe_damping_ratio, JANET_NIL)) {
      jointDef.dampingRatio = janet_getnumber(&maybe_damping_ratio, 0);
    }
  }

  b2JointId *jointId = janet_abstract(&AT_Joint, sizeof(b2JointId));
  *jointId = b2CreateMouseJoint(*worldId, &jointDef);

  return janet_wrap_abstract(jointId);
}

static const JanetReg joint_cfuns[] = {
  {"create-distance-joint", janet_create_distance_joint, NULL},
  /* {"create-motor-joint", janet_create_distance_joint, NULL}, */
  {"create-mouse-joint", janet_create_mouse_joint, NULL},
  /* {"create-prismatic-joint", janet_create_distance_joint, NULL}, */
  /* {"create-revolute-joint", janet_create_distance_joint, NULL}, */
  /* {"create-weld-joint", janet_create_distance_joint, NULL}, */
  /* {"create-wheel-joint", janet_create_distance_joint, NULL}, */
  {NULL, NULL, NULL}
};
