package core

// #include "CCore/src/input.h"
import "C"

func IsKeyPressed(key int) bool {

	return bool(C.IsKeyPressed(C.int(key)))
}
