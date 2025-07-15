package core

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
	handle C.Texture
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

		var tex Texture
		location := C.CString(texturesDir + file.Name())
		defer C.free(unsafe.Pointer(location))
		tex.handle = C.NewTexture(location)
		textures[file.Name()] = tex
	}
}

func GetTexture(name string) Texture {
	return textures[name]
}

func (tex *Texture) UnmarshalJSON(data []byte) error {
	name := string(data[1:len(data)-1])
	if newTexture, has := textures[name]; has {
		*tex = newTexture
		return nil
	} else {
		return fmt.Errorf("texture %s undefined", name)
	}
}
