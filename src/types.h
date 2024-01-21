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


// b2WorldId
// the main reference to the world
static int at_world_gc(void *p, size_t s) {
    (void) s;
    b2WorldId *worldId = (b2WorldId *)p;
    b2DestroyWorld(*worldId);
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
  NULL, NULL, NULL, NULL, NULL,
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
    *out = janet_wrap_number(b2Body_GetAngle(*bodyId));
    return 1;
  }

  return 0;
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
  NULL, NULL, NULL,
  at_body_tostring,
  JANET_ATEND_TOSTRING
};

static b2BodyId *box2d_getbody(const Janet *argv, int32_t n) {
  return ((b2BodyId *)janet_getabstract(argv, n, &AT_Body));
};


// b2Polygon
// a box2d shape
static const JanetAbstractType AT_Polygon = {
  "b2/polygon",
  JANET_ATEND_NAME
};

static b2Polygon *box2d_getpolygon(const Janet *argv, int32_t n) {
  return ((b2Polygon *)janet_getabstract(argv, n, &AT_Polygon));
};
