package core

// #cgo LDFLAGS: -ldl -lglfw -lm -L${SRCDIR}/CCore/bin/lib -lCCore
//
// #include "CCore/src/sprite.h"
import "C"

func QueueSprite(transform *Transform, texture *Texture) {
	CallOnRenderThead(func() {
		C.QueueSprite(C.float(transform.PositionX),
		C.float(transform.PositionY),
		C.float(transform.ScaleX),
		C.float(transform.ScaleY),
		C.float(transform.Rotation),
		C.int(transform.Layer),
		&texture.handle)
	})
}
