/*
This whole package has several massive TODOs:
TODO: replace .json scene format with .getscn format
TODO: precompute some of the work for instancing a scene
*/
package tree

import (
	"encoding/json"
	"errors"
	"log"
	"os"
)


type Scene struct {
	definition []byte
}

var scenes map[string]*Scene = make(map[string]*Scene)

var typeToScript map[string](func() Script) = make(map[string]func() Script)

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

		if json.Valid(sceneDefByte) {
			scene.definition = sceneDefByte
			scenes[file.Name()] = scene
		} else {
			log.Printf("Error: scene %s is not valid", file.Name())
		}

	}
}

func RegisterScriptName(name string, instanceFunc func() Script) error {
	if _, has := typeToScript[name]; has {
		return errors.New("Script " + name + " is already registered")
	} else {
		typeToScript[name] = instanceFunc
		return nil
	}
}

func GetScene(name string) *Scene {
	return scenes[name]
}

func (scene *Scene) Instantiate() *Node {
	const errorMessage string = "Failled to instance node: %s"
	
	if scene == nil {
		log.Printf(errorMessage, "scene does not exist")
		return nil
	}

	var sceneRoot *Node

	err := json.Unmarshal(scene.definition, &sceneRoot)
	if err != nil {log.Printf(errorMessage, err)}

	return sceneRoot
}

func (node *Node) UnmarshalJSON(data []byte) error {
	const errorMessage string = "Failled to instance node: %s"

	var defs map[string]json.RawMessage = make(map[string]json.RawMessage)
	err := json.Unmarshal(data, &defs)
	if err != nil { log.Printf(errorMessage, err) }

	if t, has := defs["type"]; has {
		scriptInstance, exists := typeToScript[string(t)[1:len(defs["type"])-1]]
		if !exists { log.Printf(errorMessage, "script doesn't exist"); return nil }
		script := scriptInstance()
		name := string(defs["name"])[1:len(defs["name"])-1]
		newNode := NewNode(script, name)

		*node = *newNode
		if trans, has := defs["transform"]; has {
			err := json.Unmarshal(trans, &node.Transform)
			if err != nil { log.Printf(errorMessage, err) }
		} else {
			node.Transform = newNode.Transform
		}
		
		err = json.Unmarshal(data, &node.Script)
		if err != nil { log.Printf(errorMessage, err) }
		
	} else if sstr, has := defs["scene"]; has {
		if sdef, has := scenes[string(sstr)[1:len(sstr)-1]]; has {
			err := json.Unmarshal([]byte(sdef.definition), node)
			if err != nil { log.Printf(errorMessage, err) }
		} else {
			log.Printf(errorMessage, "Unknown Scene")
		}
	}

	// Recurse through children
	if c, has := defs["children"]; has {
		var rawChildren []json.RawMessage
		err = json.Unmarshal(c, &rawChildren)
		if err != nil { log.Printf(errorMessage, err) }
		for _, childData := range rawChildren {
			var child *Node
			err := json.Unmarshal(childData, &child)
			if err != nil { log.Printf(errorMessage, err) }
			node.AddChild(child)
		}
	}

	return nil
}

