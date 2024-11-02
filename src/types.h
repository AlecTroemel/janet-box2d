#include <box2d/box2d.h>

// NOTE: this is stolen from jaylib source
static float idx_getfloat(JanetView idx, int index) {
    if (index >= idx.len) {
	janet_panicf("index %d outside of range [0, %d)", idx.len);
    }
    if (!janet_checktype(idx.items[index], JANET_NUMBER)) {
	janet_panicf("expected number, got %v", idx.items[index]);
    }
    return (float) janet_unwrap_number(idx.items[index]);
}


// b2Vec2
// are represented in janet as indexed (array or tuple)
static Janet box2d_wrap_vec2(b2Vec2 x) {
    Janet *tup = janet_tuple_begin(2);
    tup[0] = janet_wrap_number(x.x);
    tup[1] = janet_wrap_number(x.y);
    return janet_wrap_tuple(janet_tuple_end(tup));
}

static b2Vec2 box2d_getvec2(const Janet *argv, int32_t n) {
  JanetView idx = janet_getindexed(argv, n);

  return (b2Vec2) {
    idx_getfloat(idx, 0),
    idx_getfloat(idx, 1)
  };
}


static b2Vec2 box2d_unwrap_vec2(const Janet val) {
  JanetView view;
  if (!janet_indexed_view(val, &view.items, &view.len)) {
    janet_panic("expected vec to be an indexed type");
  }

  if (view.len != 2) {
    janet_panic("vec must have exactly 2 elements");
  }

  float x = idx_getfloat(view, 0);
  float y = idx_getfloat(view, 1);
  return (b2Vec2) { x, y };
}



// b2WorldId
// the main reference to the world
static int at_world_gc(void *p, size_t s) {
    (void) s;
    b2WorldId *worldId = (b2WorldId *)p;
    printf("destroy world %i\n", worldId->index1);
    b2DestroyWorld(*worldId);
    return 0;
}

int at_world_get(void *p, Janet key, Janet *out) {
  b2WorldId *worldId = (b2WorldId *)p;

  if (!janet_checktype(key, JANET_KEYWORD)) {
    janet_panic("expected keyword");
  }

  const uint8_t *kw = janet_unwrap_keyword(key);

  if (!janet_cstrcmp(kw, "body-count")) {
    b2Counters counters = b2World_GetCounters(*worldId);

    *out = janet_wrap_integer(counters.bodyCount);
    return 1;
  }

  return 0;
}

static void at_world_tostring(void *p, JanetBuffer *buffer) {
  char str[32];
  b2WorldId *worldId = (b2WorldId *)p;
  janet_buffer_push_cstring(buffer, str);
}

static const JanetAbstractType AT_World = {
  "b2/world",
  at_world_gc,
  NULL,
  at_world_get,
  NULL, NULL, NULL,
  at_world_tostring,
  JANET_ATEND_TOSTRING
};

static b2WorldId *box2d_getworld(const Janet *argv, int32_t n) {
  return ((b2WorldId *)janet_getabstract(argv, n, &AT_World));
};


// b2BodyId
// reference to a body
static int at_body_gc(void *p, size_t s) {
    (void) s;
    b2BodyId *bodyId = (b2BodyId *)p;
    printf("destroy body %i\n", bodyId->index1);
    b2DestroyBody(*bodyId);
    return 0;
}

int at_body_get(void *p, Janet key, Janet *out) {
  b2BodyId *bodyId = (b2BodyId *)p;

  if (!janet_checktype(key, JANET_KEYWORD)) {
    janet_panic("expected keyword");
  }

  const uint8_t *kw = janet_unwrap_keyword(key);

  if (!janet_cstrcmp(kw, "position")) {
    *out = box2d_wrap_vec2(b2Body_GetPosition(*bodyId));
    return 1;
  }

  if (!janet_cstrcmp(kw, "angle")) {
    *out = janet_wrap_number(b2Rot_GetAngle(b2Body_GetRotation(*bodyId)));
    return 1;
  }

  // TODO: get Rotation

  if (!janet_cstrcmp(kw, "linear-velocity")) {
    *out = box2d_wrap_vec2(b2Body_GetLinearVelocity(*bodyId));
    return 1;
  }

  if (!janet_cstrcmp(kw, "awake?")) {
    *out = janet_wrap_boolean(b2Body_IsAwake(*bodyId));
    return 1;
  }

  if (!janet_cstrcmp(kw, "enabled?")) {
    *out = janet_wrap_boolean(b2Body_IsEnabled(*bodyId));
    return 1;
  }

  return 0;
}

void at_body_put(void *p, Janet key, Janet value) {
  b2BodyId *bodyId = (b2BodyId *)p;

  if (!janet_checktype(key, JANET_KEYWORD)) {
    janet_panic("expected keyword");
  }

  const uint8_t *kw = janet_unwrap_keyword(key);

  if (!janet_cstrcmp(kw, "linear-velocity")) {
      b2Body_SetLinearVelocity(*bodyId, box2d_unwrap_vec2(value));
  }

  if (!janet_cstrcmp(kw, "position")) {
    b2Rot oldRotation = b2Body_GetRotation(*bodyId);
    const Janet *idx = janet_unwrap_tuple(value);
    b2Vec2 position = (b2Vec2) {
      janet_getnumber(idx, 0),
      janet_getnumber(idx, 1)
    };
    b2Body_SetTransform(*bodyId, position, oldRotation);
  }

  if (!janet_cstrcmp(kw, "enabled?")) {
    if (janet_unwrap_boolean(value)) {
      b2Body_Enable(*bodyId);
    } else {
      b2Body_Disable(*bodyId);
    }
  }
}

static void at_body_tostring(void *p, JanetBuffer *buffer) {
  char str[32];
  b2BodyId *bodyId = (b2BodyId *)p;
  janet_buffer_push_cstring(buffer, str);
}

static const JanetAbstractType AT_Body = {
  "b2/body",
  at_body_gc,
  NULL,
  at_body_get,
  at_body_put,
  NULL, NULL,
  at_body_tostring,
  JANET_ATEND_TOSTRING
};

// same as normal body, but is not garbage collected
static const JanetAbstractType AT_Body_Weak_Ref = {
  "b2/body-weak-ref",
  NULL,
  NULL,
  at_body_get,
  at_body_put,
  NULL, NULL,
  at_body_tostring,
  JANET_ATEND_TOSTRING
};

static b2BodyId *box2d_getbody(const Janet *argv, int32_t n) {
  return ((b2BodyId *)janet_getabstract(argv, n, &AT_Body));
};

static Janet box2d_wrap_body_weak_ref(b2BodyId body) {
  b2BodyId *bodyId = janet_abstract(&AT_Body_Weak_Ref, sizeof(b2BodyId));
  *bodyId = body;
  return janet_wrap_abstract(bodyId);
}



// b2Polygon
static const JanetAbstractType AT_Polygon = {
  "b2/polygon",
  JANET_ATEND_NAME
};

static b2Polygon *box2d_getpolygon(const Janet *argv, int32_t n) {
  return ((b2Polygon *)janet_getabstract(argv, n, &AT_Polygon));
};


// b2ShapeId

static int at_shape_gc(void *p, size_t s) {
    (void) s;
    b2ShapeId *shapeId = (b2ShapeId *)p;
    printf("destroy shape %i\n", shapeId->index1);
    bool updateMass = true;
    b2DestroyShape(*shapeId, updateMass);
    return 0;
}

int at_shape_get(void *p, Janet key, Janet *out) {
  b2ShapeId *shapeId = (b2ShapeId *)p;

  if (!janet_checktype(key, JANET_KEYWORD)) {
    janet_panic("expected keyword");
  }

  const uint8_t *kw = janet_unwrap_keyword(key);
  b2Filter filter = b2Shape_GetFilter(*shapeId);

  if (!janet_cstrcmp(kw, "category-bits")) {
    *out = janet_wrap_integer(filter.categoryBits);
    return 1;
  }

  if (!janet_cstrcmp(kw, "mask-bits")) {
    *out = janet_wrap_integer(filter.maskBits);
    return 1;
  }

  if (!janet_cstrcmp(kw, "group-index")) {
    *out = janet_wrap_integer(filter.groupIndex);
    return 1;
  }

  return 0;
}

void at_shape_put(void *p, Janet key, Janet value) {
  b2ShapeId *shapeId = (b2ShapeId *)p;

  if (!janet_checktype(key, JANET_KEYWORD)) {
    janet_panic("expected keyword");
  }

  const uint8_t *kw = janet_unwrap_keyword(key);

  if (!janet_cstrcmp(kw, "filter")) {
    b2Filter filter = b2Shape_GetFilter(*shapeId);

    JanetDictView tab;
    if (!janet_dictionary_view(value, &tab.kvs, &tab.len, &tab.cap)) {
      janet_panic_type(value, 0, JANET_TFLAG_DICTIONARY);
    }

    Janet maybeCategoryBits = janet_dictionary_get(tab.kvs, tab.cap, janet_ckeywordv("category-bits"));
    Janet maybeMaskBits = janet_dictionary_get(tab.kvs, tab.cap, janet_ckeywordv("mask-bits"));
    Janet maybeGroupIndex = janet_dictionary_get(tab.kvs, tab.cap, janet_ckeywordv("group-index"));


    if (!janet_checktype(maybeCategoryBits, JANET_NIL)) {
      filter.categoryBits = janet_getinteger(&maybeCategoryBits, 0);
    }

    if (!janet_checktype(maybeMaskBits, JANET_NIL)) {
      filter.maskBits = janet_getinteger(&maybeMaskBits, 0);
    }

    if (!janet_checktype(maybeGroupIndex, JANET_NIL)) {
      filter.groupIndex = janet_getinteger(&maybeGroupIndex, 0);
    }

    b2Shape_SetFilter(*shapeId, filter);
  }
}

static const JanetAbstractType AT_Shape = {
  "b2/shape",
  at_shape_gc,
  NULL,
  at_shape_get,
  at_shape_put,
  JANET_ATEND_PUT
};

static b2ShapeDef box2d_optshapedef(const Janet *argv, int32_t argc, int32_t n) {
  b2ShapeDef shapeDef = b2DefaultShapeDef();

  if (argc > n) {
    JanetDictView tab = janet_getdictionary(argv, n);
    Janet maybe_density = janet_dictionary_get(tab.kvs, tab.cap, janet_ckeywordv("density"));
    Janet maybe_friction = janet_dictionary_get(tab.kvs, tab.cap, janet_ckeywordv("friction"));

    if (!janet_checktype(maybe_density, JANET_NIL)) {
      shapeDef.density = (float) janet_getnumber(&maybe_density, 0);
    }

    if (!janet_checktype(maybe_friction, JANET_NIL)) {
      shapeDef.friction = (float) janet_getnumber(&maybe_friction, 0);
    }
  }

  return shapeDef;
};


// b2JointId
static int at_joint_gc(void *p, size_t s) {
    (void) s;
    b2JointId *jointId = (b2JointId *)p;
    printf("destroy joint %i\n", jointId->index1);
    b2DestroyJoint(*jointId);
    return 0;
}

int at_joint_get(void *p, Janet key, Janet *out) {
  b2JointId *jointId = (b2JointId *)p;

  if (!janet_checktype(key, JANET_KEYWORD)) {
    janet_panic("expected keyword");
  }

  const uint8_t *kw = janet_unwrap_keyword(key);

  if (!janet_cstrcmp(kw, "type")) {
    b2JointType jointType = b2Joint_GetType(*jointId);
    if (jointType == b2_distanceJoint) {
      *out = janet_wrap_keyword("distance");
    }
    if (jointType == b2_motorJoint) {
      *out = janet_wrap_keyword("motor");
    }
    if (jointType == b2_mouseJoint) {
      *out = janet_wrap_keyword("mouse");
    }
    if (jointType == b2_prismaticJoint) {
      *out = janet_wrap_keyword("prismatic");
    }
    if (jointType == b2_revoluteJoint) {
      *out = janet_wrap_keyword("revolute");
    }
    if (jointType == b2_weldJoint) {
      *out = janet_wrap_keyword("weld");
    }
    if (jointType == b2_wheelJoint) {
      *out = janet_wrap_keyword("wheel");
    }
    return 1;
  }

  if (!janet_cstrcmp(kw, "body-a")) {
    *out = box2d_wrap_body_weak_ref(b2Joint_GetBodyA(*jointId));
    return 1;
  }

  if (!janet_cstrcmp(kw, "body-b")) {
    *out = box2d_wrap_body_weak_ref(b2Joint_GetBodyB(*jointId));
    return 1;
  }

  return 0;
}

void at_joint_put(void *p, Janet key, Janet value) {
  b2JointId *jointId = (b2JointId *)p;

  if (!janet_checktype(key, JANET_KEYWORD)) {
    janet_panic("expected keyword");
  }

  const uint8_t *kw = janet_unwrap_keyword(key);

  /* if (!janet_cstrcmp(kw, "linear-velocity")) { */
  /*     b2Joint_SetLinearVelocity(*jointId, box2d_unwrap_vec2(value)); */
  /* }   */
}

static void at_joint_tostring(void *p, JanetBuffer *buffer) {
  char str[32];
  b2JointId *jointId = (b2JointId *)p;
  janet_buffer_push_cstring(buffer, str);
}

static const JanetAbstractType AT_Joint = {
  "b2/joint",
  at_joint_gc,
  NULL,
  at_joint_get,
  at_joint_put,
  NULL, NULL,
  at_joint_tostring,
  JANET_ATEND_TOSTRING
};

static b2JointId *box2d_getjoint(const Janet *argv, int32_t n) {
  return ((b2JointId *)janet_getabstract(argv, n, &AT_Joint));
};
