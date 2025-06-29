package core

// #cgo LDFLAGS: -ldl -lglfw -lm -L${SRCDIR}/internal/c/bin/lib -lEngine
// #include "internal/c/src/transform.h"
import "C"

type Transform struct {
	backing C.Transform
}

func (trans *Transform) SetPosition(x float32, y float32) {
	trans.backing.position_x = C.float(x)
	trans.backing.position_y = C.float(y)
}

func (trans *Transform) GetPosition() (float32, float32) {
	return float32(trans.backing.position_x), float32(trans.backing.position_y)
}

func (trans *Transform) SetScale(x float32, y float32) {
	trans.backing.scale_x = C.float(x)
	trans.backing.scale_y = C.float(y)
}

func (trans *Transform) GetScale() (float32, float32) {
	return float32(trans.backing.scale_x), float32(trans.backing.scale_x)
}

func (trans *Transform) SetRotation(radians float32) {
	trans.backing.rotation = C.float(radians)
}

func (trans *Transform) GetRotation() (float32) {
	return float32(trans.backing.rotation)
}

func (trans *Transform) SetLayer(layer float32) {
	trans.backing.rotation = C.float(layer)
}

func (trans *Transform) GetLayer() (float32) {
	return float32(trans.backing.layer)
}
