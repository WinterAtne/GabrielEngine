package main

import (
	"strconv"

	"github.com/WinterAtne/Engine/core"
	"github.com/WinterAtne/Engine/tree"
)

func main() {
	core.Start()

	tex := core.GetTexture("test.png")
	for i := range 16 {
		n := tree.NewNode(&tree.Sprite{Texture: &tex}, "sprite" + strconv.Itoa(i))
		tree.GetRoot().AddChild(n)
		n.Transform.PositionY = float32(i*16) + float32(- 128 + 8)
	}

	for i := range 16 {
		n := tree.NewNode(&tree.Sprite{Texture: &tex}, "sprite" + strconv.Itoa(i))
		tree.GetRoot().AddChild(n)
		n.Transform.PositionX = float32(i*16) + float32(- 128 + 8)
	}

	finish := true
	for finish {

		tree.GetRoot().Transform.PositionX += 0.016

		tree.Process(0)

		core.FinishFrame()
		finish = core.BlockTillNextFrame()	
	}
}
