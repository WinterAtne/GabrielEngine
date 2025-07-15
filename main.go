package main

import (

	"github.com/WinterAtne/GabrielEngine/core"
	"github.com/WinterAtne/GabrielEngine/tree"
)

func main() {
	core.Start()

	scene := tree.GetScene("main.json")
	node_0 := scene.Instantiate()

	tree.GetRoot().AddChild(node_0)

	tree.Process(0)

	finish := true
	for finish {

		tree.Process(0)

		core.FinishFrame()
		finish = core.BlockTillNextFrame()	
	}
}
