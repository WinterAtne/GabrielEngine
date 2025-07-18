package tree

import (
	"github.com/WinterAtne/GabrielEngine/core"
)

type Sprite struct {
	Object
	tree []*Node
	Texture *core.Texture
	Shader *core.Shader
}

func init() {
	RegisterScriptName("Sprite", func() Script {return new(Sprite)})
}

/* Script Methods */
func (sprite *Sprite) Init(node *Node) {
	sprite.Object.Init(node)
}

func (sprite *Sprite) Start() {
	sprite.Object.Start()
	sprite.tree = sprite.node.GetTree()

	if sprite.Shader == nil {
		*sprite.Shader = core.GetShader("default")
	}
}

func (sprite *Sprite) Process(_delta float32) {
	sprite.Object.Process(_delta)

	core.QueueSprite(GlobalTransform(sprite.tree), sprite.Texture, sprite.Shader)
}

func (sprite *Sprite) Remove() {
	sprite.Object.Remove()
}
