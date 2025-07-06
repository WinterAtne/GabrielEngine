package main

import (
	"fmt"

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


	i := 0
	var keepGoing bool = true
	for keepGoing {
		tra.Rotation+= 0.01
		core.QueueSprite(&tra, &tex)
		core.FinishFrame()
		keepGoing = core.BlockTillNextFrame()
		fmt.Println(i)
		if (i >= 32) { core.Finish(); break }
		i++
	}
}
