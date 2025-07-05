package core

// #cgo LDFLAGS: -ldl -lglfw -lm -L${SRCDIR}/CCore/bin/lib -lCCore
//
// #include "CCore/src/window.h"
// #include "CCore/src/sprite.h"
import "C"
import (
	"runtime"
)

// Should always be run from a goroutine
func Start(done chan bool) {
	runtime.LockOSThread()
	c_err := C.InitWindow();
	if (c_err != 0) {
		panic("Failled to init window")
	}
	c_err = C.InitSprites();
	if (c_err != 0) {
		panic("Failled to init window")
	}

	tex0 := C.NewTexture(C.CString("resources/textures/test_text.png"))
	
	i := 0.0
	for !C.ShouldDestroyWindow() {
		C.QueueSprite(0.0, 0.0, 1.0, 1.0, C.float(i), 1, tex0)
		C.DrawSpriteQueue()
		C.ProcessWindow()
		i+= 0.05
	}

	C.DestroyWindow()
	done <- true
}
