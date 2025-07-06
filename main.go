package main

import (
	"fmt"
	"time"

	"github.com/WinterAtne/Engine/core"
)

func main() {
	core.Start()
	
	var texChan chan core.Texture = make(chan core.Texture)
	core.NewTexture("resources/textures/test_text.png", texChan)
	tex:=<-texChan
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
	var keepGoing bool = true
	for keepGoing {
		t := time.Now()
		tra.Rotation+= 0.01
		tra.ScaleX+= 0.001
		tra1.Rotation-= 0.02
		tra2.PositionX+= 0.001
		tra2.Rotation+= 0.001
		core.QueueSprite(&tra, &tex)
		core.QueueSprite(&tra1, &tex)
		core.QueueSprite(&tra2, &tex)
		core.FinishFrame()
		keepGoing = core.BlockTillNextFrame()
		i++
		if i % 60 == 0 {
			dif := time.Since(t)
			fmt.Println(dif)
		}
	}
}
