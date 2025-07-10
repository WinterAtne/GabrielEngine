package main

import (

	"github.com/WinterAtne/Engine/core"
	"github.com/WinterAtne/Engine/tree"
)

func main() {
	core.Start()

	tex := core.GetTexture("test_text.png")
	// tex0 := core.GetTexture("test_text_1.jpg")

	sprite := tree.NewNode(&tree.Sprite{
		Texture: &tex,
	}, "sprite_0")

	finish := true
	for finish {
		sprite.Transform.Rotation += 0.01

		tree.Process(0)

		core.FinishFrame()
		finish = core.BlockTillNextFrame()	
	}
}
