package main

// #cgo LDFLAGS: -ldl -lglfw -lm -L${SRCDIR}/core/internal/c/bin/lib -lEngine
// #include "core/internal/c/src/rendering.h"
import "C"

func main() {
	clear_color := [4]float32{0.1, 0.1, 0.1, 1.0}
	C.rendering_initialize(800, 600, 1, C.CString("Hello, World!"), (*C.float)(&clear_color[0]))

	s0 := C.sprite_make()
	C.sprite_rotate(s0, C.float(0.1))
	tex0 := C.texture_make(C.CString("resources/textures/test_text.png"))
	C.sprite_texture_set(s0, tex0)

	for (!C.window_should_close()) {
		C.sprite_rotate(s0, C.float(0.001))
		C.rendering_process();
	}
}
