package core

// #include "CCore/src/input.h"
import "C"

func IsKeyPressed(key int) bool {
	return bool(C.IsKeyPressed(C.int(key)))
}

func GetMousePosition() (x, y float32) {
	mouseXY := C.MousePosition()

	return float32(mouseXY.x), float32(mouseXY.y)

}
