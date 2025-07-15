package tree

import (
	"encoding/json"
	"os"
)


type Scene struct {
	definition []byte
}

var scenes map[string]*Scene = make(map[string]*Scene)

var typeToScript map[string](func() Script) = map[string]func() Script {
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
	var sceneRoot *Node

	err := json.Unmarshal(scene.definition, &sceneRoot)
	if err != nil {
		panic(err)
	}

	return sceneRoot
}

func (node *Node) UnmarshalJSON(data []byte) error {
	var defs map[string]json.RawMessage = make(map[string]json.RawMessage)
	err := json.Unmarshal(data, &defs)
	if err != nil { panic(err) }

	if t, has := defs["type"]; has {
		script := typeToScript[string(t)[1:len(defs["type"])-1]]()
		name := string(defs["name"])[1:len(defs["name"])-1]
		newNode := NewNode(script, name)

		*node = *newNode
		if trans, has := defs["transform"]; has {
			err := json.Unmarshal(trans, &node.Transform)
			if err != nil { panic(err) }
		} else {
			node.Transform = newNode.Transform
		}
		
		err = json.Unmarshal(data, &node.Script)
		if err != nil { panic(err) }
		
		if c, has := defs["children"]; has {
			var rawChildren []json.RawMessage
			err = json.Unmarshal(c, &rawChildren)
			if err != nil { panic(err) }
			for _, childData := range rawChildren {
				var child *Node
				err := json.Unmarshal(childData, &child)
				if err != nil {panic(err)}
				node.AddChild(child)
			}
		}
	} else if sstr, has := defs["scene"]; has {
		if sdef, has := scenes[string(sstr)[1:len(sstr)-1]]; has {
			err := json.Unmarshal([]byte(sdef.definition), node)
			if err != nil {
				panic(err)
			}
		}
	}


	return nil
}

