package core

// #cgo LDFLAGS: -ldl -lglfw -lm -L${SRCDIR}/CCore/bin/lib -lCCore
//
// #include <stdlib.h>
//
// #include "CCore/src/texture.h"
import "C"

import (
	"fmt"
	"os"
	"unsafe"
)

type Texture struct {
	id uint32	
}

var textures map[string]Texture = make(map[string]Texture)

func loadTextures() {
	const texturesDir string = "resources/textures/"

	files, err := os.ReadDir(texturesDir)
	if err != nil {
		panic(err)
	}

	for _, file := range files {
		if file.IsDir() { continue }

		fmt.Println(file.Name())

		var tex Texture
		location := C.CString(texturesDir + file.Name())
		defer C.free(unsafe.Pointer(location))
		tex.id = uint32(C.NewTexture(location))
		textures[file.Name()] = tex
	}
}

func GetTexture(name string) Texture {
	return textures[name]
}

func (texture *Texture) GetID() uint32 {
	return texture.id
}
