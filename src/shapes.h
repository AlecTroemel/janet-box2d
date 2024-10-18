#include "box2d/box2d.h"

// TODO: box2d/geometry.h
// TODO: box2d.h

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

  // NOTE: I've swapped the order here to put the optional shape def at end
  b2BodyId *bodyId = box2d_getbody(argv, 0);
  b2Polygon *polygon = box2d_getpolygon(argv, 1);
  b2ShapeDef shapeDef = box2d_optshapedef(argv, argc, 2);

  b2ShapeId *shapeId = janet_abstract(&AT_Shape, sizeof(b2ShapeId));
  *shapeId = b2CreatePolygonShape(*bodyId, &shapeDef, polygon);

  return janet_wrap_abstract(shapeId);
}

static Janet janet_create_circle_shape(int32_t argc, Janet *argv) {
  janet_arity(argc, 3, 4);

  // NOTE: I've swapped the order here to put the optional shape def at end
  b2BodyId *bodyId = box2d_getbody(argv, 0);
  b2Vec2 point = box2d_getvec2(argv, 1);
  float radius = janet_getnumber(argv, 2);

  b2ShapeDef shapeDef = box2d_optshapedef(argv, argc, 3);
  b2Circle circle = (b2Circle) { point, radius };

  b2ShapeId *shapeId = janet_abstract(&AT_Shape, sizeof(b2ShapeId));
  *shapeId = b2CreateCircleShape(*bodyId, &shapeDef, &circle);

  return janet_wrap_abstract(shapeId);
}

static const JanetReg shapes_cfuns[] = {
  {"make-box", janet_make_box, NULL},
  {"create-polygon-shape", janet_create_polygon_shape, NULL},
  {"create-circle-shape", janet_create_circle_shape, NULL},
  {NULL, NULL, NULL}
};
