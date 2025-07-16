package core

// #include "CCore/src/sprite.h"
// #include "CCore/options/windowopt.h"
import "C"

func QueueSprite(transform *Transform, texture *Texture, shader *Shader) {
	CallOnRenderThead(func() {
		C.QueueSprite(
		C.float(transform.PositionX),
		C.float(transform.PositionY),
		C.float(float32(texture.handle.width)),
		C.float(float32(texture.handle.height)),
		C.float(0.0),
		C.int(transform.Layer),
		&texture.handle,
		&shader.handle)
	})
}
