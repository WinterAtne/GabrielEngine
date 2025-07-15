package main

import (
	// "strconv"

	"fmt"

	"github.com/WinterAtne/GabrielEngine/core"
	"github.com/WinterAtne/GabrielEngine/tree"
)

func main() {
	core.Start()

	tex := core.GetTexture("test.png")
	core.FinishFrame()
	core.BlockTillNextFrame()	
	
	// for i := range 16 {
	// 	n := tree.NewNode(&tree.Sprite{Texture: &tex}, "sprite" + strconv.Itoa(i))
	// 	tree.GetRoot().AddChild(n)
	// 	n.Transform.PositionY = float32(i*16) + float32(- 128 + 8)
	// }
	//
	// for i := range 16 {
	// 	n := tree.NewNode(&tree.Sprite{Texture: &tex}, "sprite" + strconv.Itoa(i))
	// 	tree.GetRoot().AddChild(n)
	// 	n.Transform.PositionX = float32(i*16) + float32(- 128 + 8)
	// }

	character := tree.NewNode(&tree.Sprite{Texture: &tex, Shader: core.GetShader("circle")}, "character")
	tree.GetRoot().AddChild(character)

	finish := true
	for finish {

		mouseX, mouseY := core.GetMousePosition()

		fmt.Printf("%f, %f\n", mouseX, mouseY)

		character.Transform.PositionX = mouseX
		character.Transform.PositionY = mouseY

		// tree.GetRoot().Transform.Rotation += 0.016

		tree.Process(0)

		core.FinishFrame()
		finish = core.BlockTillNextFrame()	
	}
}
