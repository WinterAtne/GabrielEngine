package core

// #cgo LDFLAGS: -ldl -lglfw -lm -L${SRCDIR}/CCore/bin/lib -lCCore
//
// #include "CCore/src/sprite.h"
// #include "CCore/options/windowopt.h"
import "C"

func QueueSprite(transform *Transform, texture *Texture) {
	CallOnRenderThead(func() {
		C.QueueSprite(
		C.float(transform.PositionX),
		C.float(transform.PositionY),
		C.float(transform.ScaleX * float32(C.DEFAULT_TILE)),
		C.float(transform.ScaleY * float32(C.DEFAULT_TILE)),
		C.float(transform.Rotation),
		C.int(transform.Layer),
		&texture.handle)
	})
}
