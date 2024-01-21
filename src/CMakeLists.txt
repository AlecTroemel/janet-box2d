set(JANET_BOX2D_SOURCE_FILES
body.h
shapes.h
types.h
world.h
main.c
)

add_library(${TARGET_NAME} MODULE ${JANET_BOX2D_SOURCE_FILES})
target_link_libraries(${TARGET_NAME} box2d)
