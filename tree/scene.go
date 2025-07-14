package tree

import (
	"encoding/json"
	"os"

	"github.com/WinterAtne/Engine/core"
)


type Scene struct {
	definition []byte
}

var scenes map[string]*Scene = make(map[string]*Scene)

var typeToScript map[string](func() Script) = map[string]func() Script{
	"Sprite": func() Script {return new(Sprite)},
	"Object": func() Script {return new(Object)},
}

func init() {
	loadScenes()
}

func loadScenes() {
	const scenesDir string = "resources/scenes/"

	files, err := os.ReadDir(scenesDir)
	if err != nil {
		panic(err)
	}

	for _, file := range files {
		if file.IsDir() { continue }

		var scene *Scene = new(Scene)
		sceneDefByte, err := os.ReadFile(scenesDir+ file.Name())
		if err != nil {
			panic("Couldn't load scene " + file.Name())
		}
		scene.definition = sceneDefByte

		scenes[file.Name()] = scene
	}
}

func GetScene(name string) *Scene {
	return scenes[name]
}

func (scene *Scene) Instantiate() *Node {
	// We can return a node with an arbitrary script
	// Set of children,
	// Or any other node property

	t := struct{
		Typedef string `json:"Type"`
		Name string `json:"name"`
		Transform core.Transform `json:"transform"`
	}{}

	err := json.Unmarshal(scene.definition, &t)
	if err != nil {
		panic("Couldn't load scene")
	}

	sceneRoot := NewNode(typeToScript[t.Typedef](), t.Name)
	sceneRoot.Transform = t.Transform
	
	err = json.Unmarshal(scene.definition, &sceneRoot.script)
	if err != nil {
		panic(err)
	}

	return sceneRoot
}

