package tree

import (
	"github.com/WinterAtne/Engine/core"
)

type Sprite struct {
	Object
	tree []*Node
	Texture *core.Texture
	Shader *core.Shader
}

/* Script Methods */
func (sprite *Sprite) OnInit() {
	sprite.Object.OnInit()
}

func (sprite *Sprite) OnStart(node *Node) {
	sprite.Object.OnStart(node)
	sprite.tree = sprite.node.GetTree()

	if sprite.Shader == nil {
		*sprite.Shader = core.GetShader("default")
	}
}

func (sprite *Sprite) OnProcess(_delta float32) {
	sprite.Object.OnProcess(_delta)

	core.QueueSprite(GlobalTransform(sprite.tree), sprite.Texture, sprite.Shader)
}

func (sprite *Sprite) OnRemove() {
	sprite.Object.OnRemove()
}

func (sprite *Sprite) OnDestroy() {
	sprite.Object.OnDestroy()
}
