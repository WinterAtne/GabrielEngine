package core

// #include <stdlib.h>
//
// #include "CCore/src/texture.h"
import "C"

import (
	"os"
	"unsafe"
)

type Texture struct {
	handle C.Texture
	Name string
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
		tex.Name = file.Name()
		textures[file.Name()] = tex
	}
}

func GetTexture(name string) Texture {
	return textures[name]
}

func (tex *Texture) UnmarshalJSON(data []byte) error {
	tex.handle = textures[string(data[1:len(data)-1])].handle
	tex.Name = textures[string(data[1:len(data)-1])].Name
	return nil
}
