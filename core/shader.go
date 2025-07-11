package core

// #cgo LDFLAGS: -ldl -lglfw -lm -L${SRCDIR}/CCore/bin/lib -lCCore
//
// #include <stdlib.h>
//
// #include "CCore/src/shader.h"
import "C"

import (
	"unsafe"
)

type Shader struct {
	handle C.Shader
}

func NewShader(vertexSource, fragmentSource string) *Shader {
	vertex := C.CString(vertexSource)
	fragment := C.CString(fragmentSource)

	shader := Shader{handle: C.NewShader(vertex, fragment)}

	C.free(unsafe.Pointer(vertex))
	C.free(unsafe.Pointer(fragment))
	return &shader
}
