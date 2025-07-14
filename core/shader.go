package core

// #include <stdlib.h>
//
// #include "CCore/src/shader.h"
import "C"

import (
	"encoding/json"
	"os"
	"strings"
	"unsafe"
)

type Shader struct {
	handle C.Shader
	Name string
}

var shaders map[string]Shader = make(map[string]Shader)

func loadShaders() {
	const shadersDir string = "resources/shaders/"

	files, err := os.ReadDir(shadersDir)
	if err != nil {
		panic(err)
	}

	shaderJsonDefs := []*struct{
		Name string
		Vertex string `json:"vertex"`
		Fragment string `json:"fragment"`
	}{}

	for _, file := range files {
		if file.IsDir() { continue }

		name, found := strings.CutSuffix(file.Name(), ".json")
		if !found {continue}

		content, err := os.ReadFile(shadersDir+file.Name())
		if err != nil {
			panic("Failled to read shader")
		}

		shaderJsonDefs = append(shaderJsonDefs, nil)
		err = json.Unmarshal(content, &shaderJsonDefs[len(shaderJsonDefs)-1])
		shaderJsonDefs[len(shaderJsonDefs)-1].Name = name
	}

	for _, def := range shaderJsonDefs {
		vertexSource, err := os.ReadFile(shadersDir+def.Vertex)
		if err != nil {
			panic("Failled to read shader")
		}
		fragmentSource, err := os.ReadFile(shadersDir+def.Fragment)
		if err != nil {
			panic("Failled to read shader")
		}

		shader := NewShader(string(vertexSource), string(fragmentSource))
		shader.Name = def.Name
		shaders[def.Name] = shader
	}
}

func GetShader(name string) Shader {
	return (shaders[name])
}

func NewShader(vertexSource, fragmentSource string) Shader {
	vertex := C.CString(vertexSource)
	fragment := C.CString(fragmentSource)

	shader := Shader{handle: C.NewShader(vertex, fragment)}

	C.free(unsafe.Pointer(vertex))
	C.free(unsafe.Pointer(fragment))
	return shader
}


func (shader *Shader) UnmarshalJSON(data []byte) error {
	shader.handle = shaders[string(data[1:len(data)-1])].handle
	return nil
}
