/*
This whole package has several massive TODOs:
TODO: replace .json scene format with .getscn format
TODO: precompute some of the work for instancing a scene
*/
package tree

import (
	"encoding/json"
	"errors"
	"fmt"
	"log"
	"os"
)


type Scene struct {
	definition []byte
}

var scenes map[string]*Scene = make(map[string]*Scene)

var typeToScript map[string](func() Script) = make(map[string]func() Script)

func LoadScenes() {
	const scenesDir string = "resources/scenes/"

	files, err := os.ReadDir(scenesDir)
	if err != nil { 
		// Failling to load one scene is fine
		// Failling to load every scene is unrecoverable
		panic("Failled to load scenes")
	}

	for _, file := range files {
		if file.IsDir() { continue }

		var scene *Scene = new(Scene)
		sceneDefByte, err := os.ReadFile(scenesDir + file.Name())
		if err != nil {
			log.Printf("Failled to read scene file: %s", file.Name())
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

// Instantiates a scene and returns the scene root.
// if the scene failles to instance, it will log an error and return nil.
func (scene *Scene) Instantiate() (*Node, error) {
	const errorMessage string = "Failled to instance node: %s"
	
	if scene == nil {
		log.Printf(errorMessage, "scene does not exist or is not valid")
		return nil, nil
	}

	var sceneRoot *Node

	err := json.Unmarshal(scene.definition, &sceneRoot)
	if err != nil {
		log.Println(err)
		sceneRoot = nil
	}

	return sceneRoot, err
}

var childError = errors.New("Child Error") // Some child of sceneRoot encountered an error
var instanceError = errors.New("Instance Error") // SceneRoot encounted an error and could not be instanced
// Returns one of two errors:  & childError
// Both will wrap a json error of some kind.
func (node *Node) UnmarshalJSON(data []byte) error {
	var defs map[string]json.RawMessage = make(map[string]json.RawMessage)
	err := json.Unmarshal(data, &defs)
	if err != nil {
		return fmt.Errorf("%w: failled to get scene definition: %w", instanceError, err)
	}

	if t, has := defs["type"]; has {
		scriptInstance, exists := typeToScript[string(t)[1:len(defs["type"])-1]]
		if !exists { 
			return fmt.Errorf("%w: script does not exist", instanceError)
		}
		script := scriptInstance()

		name := string(defs["name"])[1:len(defs["name"])-1]
		*node = Node{
			index: -1,
			name: name,
			Script: script,
		}

		
		err = json.Unmarshal(data, &node.Script)
		if err != nil {
			return fmt.Errorf("%w: failled to get script properties: %w", instanceError, err)
		}

		node.Init(node)
		
	} else if sceneName, has := defs["scene"]; has {

		if sceneDef, has := scenes[string(sceneName)[1:len(sceneName)-1]]; has {

			err := json.Unmarshal([]byte(sceneDef.definition), node)
			if err != nil {
				return (err) // Recursion, use the previous error
			}

		} else {
			return fmt.Errorf("%w: unknown scene %s", instanceError, sceneName)
		}

	} else {
		return fmt.Errorf("%w: unknown instance type", instanceError)
	}

	if trans, has := defs["transform"]; has {
		err := json.Unmarshal(trans, &node.Transform)
		if err != nil { 
			return fmt.Errorf("%w: invalid transform: %w", instanceError, err)
		}

	}

	// Recurse through children
	if c, has := defs["children"]; has {
		var rawChildren []json.RawMessage
		err = json.Unmarshal(c, &rawChildren)
		if err != nil {
			return (fmt.Errorf("%w: failled to get child definition %w", childError, err))
		}

		for _, childData := range rawChildren {
			var child *Node
			err := json.Unmarshal(childData, &child)
			if err != nil {
				if errors.Is(err, childError) {
					return err
				} else {
					return fmt.Errorf("%w: %w", childError, err)
				}
			}

			node.AddChild(child)
		}
	}

	return nil
}

