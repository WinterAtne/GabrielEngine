package core

// #cgo LDFLAGS: -ldl -lglfw -lm -L${SRCDIR}/internal/c/bin/lib -lEngine
// #cgo nocallback rendering_initialize
//
// #include "internal/c/src/rendering.h"
import "C"

import _ "fmt" // C could be yelling at us and noone would hear

type Sprite struct {
	transform Transform
	cReference C.Sprite
}

func init() {
	clear_color := [4]float32{0.1, 0.1, 0.1, 1.0}
	c_err := C.rendering_initialize(800, 600, 1, C.CString("Hello, World!"), (*C.float)(&clear_color[0]))
	if c_err != 0 {
		panic(c_err)
	}
}

func Run() {
	s0 := MakeSprite()

	tex0 := C.texture_make(C.CString("resources/textures/test_text.png"))
	C.sprite_transform_get(s0.cReference, &s0.transform.backing)
	s0.transform.SetPosition(0.5, 0.4)
	// s0.transform.SetScale(1.0, 1.0)
	C.sprite_texture_set(s0.cReference, tex0)
	C.sprite_transform_set(s0.cReference, &s0.transform.backing)


	for (!C.window_should_close()) {
		C.sprite_transform_get(s0.cReference, &s0.transform.backing)
		s0.transform.SetRotation(s0.transform.GetRotation()+0.01)
		C.sprite_transform_set(s0.cReference, &s0.transform.backing)
		C.rendering_process();
	}
}

func MakeSprite() *Sprite {
	var sprite *Sprite = new(Sprite)
	sprite.cReference = C.sprite_make()
	return sprite
}

