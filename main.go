package main

import (

	"github.com/WinterAtne/Engine/core"
)

func main() {
	core.Start()

	tex := core.GetTexture("test_text.png")
	tex0 := core.GetTexture("test_text_1.jpg")

	var tra core.Transform = core.Transform{
		PositionX: 0,
		PositionY: 0,
		Layer: 1,
		ScaleX: 1,
		ScaleY: 1,
	}

	var tra1 core.Transform = core.Transform{
		PositionX: 0.5,
		PositionY: 0.5,
		Layer: 0,
		ScaleX: 1,
		ScaleY: 1,
	}
	var tra2 core.Transform = core.Transform{
		PositionX: 0.5,
		PositionY: -0.5,
		Layer: 2,
		ScaleX: 1,
		ScaleY: 1,
	}

	i := 0
	finish := true
	for finish {
		tra.Rotation = float32(i) * 0.01
		core.QueueSprite(&tra, &tex)
		core.QueueSprite(&tra1, &tex)
		core.QueueSprite(&tra2, &tex0)
		core.FinishFrame()
		finish = core.BlockTillNextFrame()
		if (core.IsKeyPressed('K')) {
			i++
		}
	}
}
