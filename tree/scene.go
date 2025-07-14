package tree

import (
	"encoding/json"
	"fmt"
	"os"
	// "github.com/WinterAtne/Engine/core"
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

	// Get type and transform
	// t := struct{
	// 	Typedef string `json:"Type"`
	// 	Name string `json:"name"`
	// 	Transform core.Transform `json:"transform"`
	// 	Children []struct {
	// 		Typedef string `json:"Type"`
	// 		Name string `json:"name"`
	// 		Transform core.Transform `json:"transform"`
	// 	}
	// }{}
	//
	// err := json.Unmarshal(scene.definition, &t)
	// if err != nil {
	// 	panic(err)
	// }
	//
	//
	// fmt.Println(t.Children)
	//
	// // Get the script fields
	// sceneRoot := NewNode(typeToScript[t.Typedef](), t.Name)
	// sceneRoot.Transform = t.Transform
	//
	// for _, cDef := range t.Children {
	// 	child := NewNode(typeToScript[cDef.Typedef](), cDef.Name)
	// 	child.Transform = cDef.Transform
	// 	sceneRoot.AddChild(child)
	// }
	//
	// err = json.Unmarshal(scene.definition, &sceneRoot.script)
	// if err != nil {
	// 	panic(err)
	// }

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

	for k, v := range defs {
		fmt.Println(k, string(v))
	}

	script := typeToScript[string(defs["type"])[1:len(defs["type"])-1]]()
	name := string(defs["name"])[1:len(defs["name"])-1]
	newNode := NewNode(script, name)

	*node = *newNode
	err = json.Unmarshal(defs["transform"], &node.Transform)
	if err != nil { panic(err) }
	err = json.Unmarshal(data, &node.Script)
	if err != nil { panic(err) }

	var rawChildren []json.RawMessage
	err = json.Unmarshal(defs["children"], &rawChildren)
	if err != nil { panic(err) }

	for _, childData := range rawChildren {
		var child *Node
		err := json.Unmarshal(childData, &child)
		if err != nil {panic(err)}
		node.AddChild(child)
	}

	// type base struct {
	// 	Script string `json:"type"`
	// 	Name string `json:"name"`
	// 	Transform core.Transform `json:"transform"`
	// 	Children []base
	// }
	//
	// var bootstrap base
	//
	// err := json.Unmarshal(data, &bootstrap)
	// if err != nil {
	// 	panic(err)
	// }
	//
	// *node = *NewNode(typeToScript[bootstrap.Script](), bootstrap.Name)
	// node.Transform = bootstrap.Transform
	// err = json.Unmarshal(data, &node.Script)
	// if err != nil {
	// 	panic(err)
	// }
	//
	// for _, c := range bootstrap.Children {
	// 	child := NewNode(typeToScript[c.Script](), c.Name)
	// 	child.Transform = c.Transform
	// 	err = json.Unmarshal(data, &c.Script)
	// 	if err != nil {
	// 		panic(err)
	// 	}
	//
	// 	node.AddChild(child)
	// }
	//
	// fmt.Println(bootstrap.Children)
	
	return nil
}

