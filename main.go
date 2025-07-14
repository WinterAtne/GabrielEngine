package main

import (
	// "strconv"

	// "fmt"

	// "github.com/WinterAtne/Engine/core"
	// "fmt"

	"github.com/WinterAtne/Engine/core"
	"github.com/WinterAtne/Engine/tree"
)

func main() {
	core.Start()

	scene := tree.GetScene("example.json")
	node_0 := scene.Instantiate()

	tex := core.GetTexture("test.png")
	sha := core.GetShader("default")
	node_1 := tree.NewNode(&tree.Sprite{Texture: &tex, Shader: &sha}, "node_1")
	node_1.Transform.PositionX += 64

	tree.GetRoot().AddChild(node_0)
	tree.GetRoot().AddChild(node_1)


	// tex := core.GetTexture("test.png")
	// core.FinishFrame()
	// core.BlockTillNextFrame()	
	// 
	// // for i := range 16 {
	// // 	n := tree.NewNode(&tree.Sprite{Texture: &tex}, "sprite" + strconv.Itoa(i))
	// // 	tree.GetRoot().AddChild(n)
	// // 	n.Transform.PositionY = float32(i*16) + float32(- 128 + 8)
	// // }
	// //
	// // for i := range 16 {
	// // 	n := tree.NewNode(&tree.Sprite{Texture: &tex}, "sprite" + strconv.Itoa(i))
	// // 	tree.GetRoot().AddChild(n)
	// // 	n.Transform.PositionX = float32(i*16) + float32(- 128 + 8)
	// // }
	//
	// character := tree.NewNode(&tree.Sprite{Texture: &tex, Shader: core.GetShader("circle")}, "character")
	// tree.GetRoot().AddChild(character)
	//
	

	finish := true
	for finish {

		// fmt.Println(*node_0)
		// fmt.Println(*node_1)
		// tree.GetRoot().Transform.Rotation += 0.016

		tree.Process(0)

		core.FinishFrame()
		finish = core.BlockTillNextFrame()	
	}
}
