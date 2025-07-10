package tree

import (
	"fmt"
)

// Represents a game object with a position in 2d space.
type Object struct {
	node *Node
}

/* Public Methods */


/* Script Methods */
func (obj *Object) OnInit() {}

func (obj *Object) OnStart(node *Node) {
	obj.node = node
}

func (obj *Object) OnProcess(_delta float32) {
	fmt.Println("hi")
}

func (obj *Object) OnRemove() {}

func (obj *Object) OnDestroy() {}

