package tree

import (
)

// Represents a game object with a position in 2d space.
type Object struct {
	node *Node
}

/* Public Methods */


/* Script Methods */
func init() {
	RegisterScriptName("Object", func() Script {return new(Object)})
}

func (obj *Object) Init(node *Node) {
	obj.node = node
}

func (obj *Object) Ready() {}

func (obj *Object) Start() {}

func (obj *Object) Process(_delta float32) {}

func (obj *Object) Remove() {}

