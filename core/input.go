package core

// #cgo LDFLAGS: -ldl -lglfw -lm -L${SRCDIR}/CCore/bin/lib -lCCore
//
// #include "CCore/src/input.h"
import "C"

func IsKeyPressed(key int) bool {

	return bool(C.IsKeyPressed(C.int(key)))
}
