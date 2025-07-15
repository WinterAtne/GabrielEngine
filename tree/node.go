/*
Provides functions for modifying the state of the tree.
Should be noted that many functions will be defered till the end of the frame.
These include all operations to add or remove nodes from the tree. This prevents
certain bugs.

Children should only access their parents in ways which are guarentteed to work,
primarily those defined here.
*/
package tree

import (
	"log"
	"slices"

	"github.com/WinterAtne/Engine/core"
)

// Represents a single node in the tree. Can have at most one script and one parent.
// Can have many children. None of these are accessed directly, and are instead
// modified through the functions defined below.
type Node struct {
	name string
	index int
	parent *Node
	children []*Node
	 Script

	Transform core.Transform
}

// Represents a script, attached to a node. Node represents the attached node.
type Script interface {
	OnInit()
	OnStart(node *Node)
	OnProcess(_delta float32)
	OnRemove()
	OnDestroy()
}

// The node queue. Nodes in the queue are processed. Those which are not... are not.
var queue []*Node
// Function calls which have been defered to the end of the frame.
var defferedCalls []func()

/* --- Queue / Tree Operations --- */

// Creates a new unqueued node.
func NewNode(script Script, name string) *Node {
	node := &Node{
		name: name,
		index: -1,
		Script: script,
		Transform: core.Transform{
			ScaleX: 1,
			ScaleY: 1,
		},
	}

	node.Script.OnInit()

	return node
}

// Places a node as the child of parent. Places child in the queue.
func (parent *Node) AddChild(child *Node) {

	CallDeffered(func() {
	if child == nil {
		log.Printf("Warning: attempted to give nil child parent")
		return
	} else if child.parent != nil || child.index != -1 {
		log.Printf("Warning: child %s already has parent", child.name)
		return
	}

	child.parent = parent
	parent.children = append(parent.children, child)
	if parent.index != -1 {
		child.queueAdd()
	}
	})
}

// Removes the parent of child. Also removes child from the queue.
func (child *Node) RemoveParent() {
	CallDeffered(func() {
	// Second condition should never happen if the first condition is true
	if child.parent == nil || child.index == -1 {
		log.Printf("Warning: child %s already has parent", child.name)
		return
	}

	parent := child.parent
	parent.children[slices.Index(parent.children, child)] = parent.children[len(parent.children) - 1]
	parent.children = parent.children[:len(parent.children) - 1]

	if child.parent.index != -1 {
		child.queueRemove()
	}
	child.parent = nil
	})
}

// TODO: This can be moved to a function of runtime.AddCleanup (probably)
// TODO: or remove this entirely.
// Cannot guarenttee a node is destroyed. If a node A has a reference
// in node B, node A will persist until node B is no longer referneced.
// Attempting to access a node after this function is called is undefined
// behavior.
func (node *Node) Destroy() {
	CallDeffered(func() {
	node.destroy()
	node.RemoveParent()
	})
}

// Recursively adds a node and its children from the queue.
func (node *Node) queueAdd() {
	node.index = len(queue)
	queue = append(queue, node)
	for _, n := range node.children {
		n.queueAdd()
	}
	node.Script.OnStart(node)
}

// Recursively removes a node and its children from the queue.
func (node *Node) queueRemove() {
	for _, n := range node.children {
		n.queueRemove()
	}
	queue[node.index] = queue[len(queue) - 1]
	queue[node.index].index = node.index
	queue = queue[:len(queue) - 1]
	node.index = -1
	node.Script.OnRemove()
}

// Recursively calls the destructor on a node and its children.
func (node *Node) destroy() {
	for _, n := range node.children {
		n.destroy()
	}
	node.Script.OnDestroy()
}

/* --- Processes --- */

// Calls OnProcess on all node.Scripts in the queue.
func Process(_delta float32) {
	for _, node := range queue {
		node.Script.OnProcess(_delta)
	}

	for _, call := range defferedCalls {
		call()
	}

	defferedCalls = defferedCalls[0:0]
}

// Defers a function to be called at at the end of the process loop.
func CallDeffered(call func()) {
	defferedCalls = append(defferedCalls, call)
}

/* General Object Functions */

func (node *Node) GetName() string {
	return node.name
}

func (node *Node) GetParent() *Node {
	return node.parent
}

// TODO: Make rotations and scales from parents apply correctly.
// This will probably require changing how transforms are represented
// under the hood.
//
// Gets the combined transform of a node, accounting for parents.
// Takes a tree to avoid needing to rebuild the tree every frame.
// Layer is not modified. Scale is multiplied. Rotation is added.
func GlobalTransform(tree []*Node) *core.Transform {
	global_transform := core.Transform{
		Layer: tree[0].Transform.Layer,
		ScaleX: 1,
		ScaleY: 1,
	}

	for _, node := range tree {
		global_transform = core.Transform{
			PositionX: node.Transform.PositionX + global_transform.PositionX,
			PositionY: node.Transform.PositionY + global_transform.PositionY,
			ScaleX: node.Transform.ScaleX * global_transform.ScaleX,
			ScaleY: node.Transform.ScaleY * global_transform.ScaleY,
			Rotation: node.Transform.Rotation + global_transform.Rotation,
			Layer: global_transform.Layer,
		}
	}

	return &global_transform
}

// Returns the all nodes from node to root.
func (node *Node) GetTree() []*Node {
	var tree []*Node

	for next := node; next != nil; next = next.parent {
		tree = append(tree, next)
	}

	return tree
}

func (node *Node) GetChildren() []*Node {
	return node.children
}

