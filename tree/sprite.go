package tree

import (

	"github.com/WinterAtne/Engine/core"
)

type Sprite struct {
	Object
	Texture *core.Texture
}

/* Script Methods */
func (sprite *Sprite) OnInit() {
	sprite.Object.OnInit()
}

func (sprite *Sprite) OnStart(node *Node) {
	sprite.Object.OnStart(node)
}

func (sprite *Sprite) OnProcess(_delta float32) {
	sprite.Object.OnProcess(_delta)

	core.QueueSprite(&sprite.node.Transform, sprite.Texture)
}

func (sprite *Sprite) OnRemove() {
	sprite.Object.OnRemove()
}

func (sprite *Sprite) OnDestroy() {
	sprite.Object.OnDestroy()
}
