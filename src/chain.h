#include "box2d/box2d.h"

static b2ChainDef *box2d_getchaindef(const Janet *argv, int32_t n) {
    JanetView view = janet_getindexed(argv, n);
    b2Vec2 *points = janet_smalloc(sizeof(b2Vec2) * view.len);
    for (int32_t i = 0; i < view.len; i++) {
        points[i] = box2d_getvec2(view.items, i);
    }

    b2ChainDef chainDef = b2DefaultChainDef();
    chainDef.points = points;
    chainDef.count = view.len;

    return chainDef;
}

// b2ChainId: reference to a chain Shape
static int at_chain_gc(void *p, size_t s) {
    (void) s;
    b2ChainId *chainId = (b2ChainId *)p;
    printf("destroy chain shape %i\n", chainId->index1);
    b2DestroyChain(*chainId);
    return 0;
}

int at_chain_get(void *p, Janet key, Janet *out) {
  b2ChainId *chainId = (b2ChainId *)p;

  if (!janet_checktype(key, JANET_KEYWORD)) {
    janet_panic("expected keyword");
  }

  const uint8_t *kw = janet_unwrap_keyword(key);

  if (!janet_cstrcmp(kw, "world")) {
    b2WorldId *worldId = janet_abstract(&AT_World, sizeof(b2WorldId));
    *worldId = b2Chain_GetWorld(*chainId);
    *out = janet_wrap_abstract(worldId);
    return 1;
  }

  if (!janet_cstrcmp(kw, "segment-count")) {
    *out = janet_wrap_integer(b2Chain_GetSegmentCount(*chainId));
    return 1;
  }

  // TODO
  /* if (!janet_cstrcmp(kw, "segments")) {        */
  /*   *out = janet_wrap_nil(); */
  /*   return 1; */
  /* } */

  if (!janet_cstrcmp(kw, "valid?")) {
    *out = janet_wrap_boolean(b2Chain_IsValid(*chainId));
    return 1;
  }

  return 0;
}

void at_chain_put(void *p, Janet key, Janet value) {
  b2ChainId *chainId = (b2ChainId *)p;

  if (!janet_checktype(key, JANET_KEYWORD)) {
    janet_panic("expected keyword");
  }

  const uint8_t *kw = janet_unwrap_keyword(key);

  if (!janet_cstrcmp(kw, "friction")) {
    b2Chain_SetFriction(*chainId, janet_unwrap_number(value));
  }

  if (!janet_cstrcmp(kw, "restitution")) {
    b2Chain_SetRestitution(*chainId, janet_unwrap_number(value));
  }
}

static void at_chain_tostring(void *p, JanetBuffer *buffer) {
  char str[32];
  b2ChainId *chainId = (b2ChainId *)p;
  janet_buffer_push_cstring(buffer, str);
}

static const JanetAbstractType AT_Chain = {
  "b2/chainShape",
  at_chain_gc,
  NULL,
  at_chain_get,
  at_chain_put,
  NULL, NULL,
  at_chain_tostring,
  JANET_ATEND_TOSTRING
};


static Janet janet_create_chain(int32_t argc, Janet *argv) {
  janet_fixarity(argc, 2);
  b2BodyId *bodyId = box2d_getbody(argv, 0);
  b2ChainDef chainDef = box2d_getchaindef(argv, 1);

  b2ChainId *chainId = janet_abstract(&AT_Chain, sizeof(b2ChainId));
  *chainId = b2CreateChain(*myBodyId, &chainDef);

  return janet_wrap_abstract(chainId);
}

static const JanetReg joint_cfuns[] = {
  {"create-chain", janet_create_chain, NULL},
  {NULL, NULL, NULL}
};
