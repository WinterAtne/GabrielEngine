package core

// #cgo LDFLAGS: -ldl -lglfw -lm -L${SRCDIR}/CCore/bin/lib -lCCore
//
// #include <stdlib.h>
//
// #include "CCore/src/texture.h"
import "C"

import (
	"unsafe"
)

type Texture struct {
	id uint32	
}

func NewTexture(location string, rtrn chan<- Texture) {
	CallOnRenderThead(func() {
		var tex Texture
		loc := C.CString(location)
		tex.id = uint32(C.NewTexture(loc))
		defer C.free(unsafe.Pointer(loc))

		rtrn <- tex
	})
}

func (texture *Texture) GetID() uint32 {
	return texture.id
}
