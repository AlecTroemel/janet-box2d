#include <box2d/box2d.h>

static Janet janet_make_box(int32_t argc, Janet *argv) {
  janet_fixarity(argc, 2);
  float halfWidth = (float) janet_getnumber(argv, 0);
  float halfHeight = (float) janet_getnumber(argv, 0);

  b2Polygon *box = janet_abstract(&AT_Polygon, sizeof(b2Polygon));
  *box = b2MakeBox(halfWidth, halfHeight);

  return janet_wrap_abstract(box);
}

static Janet janet_create_polygon_shape(int32_t argc, Janet *argv) {
  janet_arity(argc, 2, 3);

  b2BodyId *bodyId = box2d_getbody(argv, 0);
  b2Polygon *polygon = box2d_getpolygon(argv, 1);

  // NOTE: I've swapped the order here!
  b2ShapeDef shapeDef = b2_defaultShapeDef;
  if (argc >= 3) {
    JanetDictView tab = janet_getdictionary(argv, 2);
    Janet maybe_density = janet_dictionary_get(tab.kvs, tab.cap, janet_ckeywordv("density"));
    Janet maybe_friction = janet_dictionary_get(tab.kvs, tab.cap, janet_ckeywordv("friction"));

    if (!janet_checktype(maybe_density, JANET_NIL)) {
      shapeDef.density = (float) janet_getnumber(&maybe_density, 0);
    }

    if (!janet_checktype(maybe_friction, JANET_NIL)) {
      shapeDef.friction = (float) janet_getnumber(&maybe_friction, 0);
    }
  }

  b2CreatePolygonShape(*bodyId, &shapeDef, polygon);
  return janet_wrap_nil();
}



static const JanetReg shapes_cfuns[] = {
  {"make-box", janet_make_box, NULL},
  {"create-polygon-shape", janet_create_polygon_shape, NULL},
  {NULL, NULL}
};
