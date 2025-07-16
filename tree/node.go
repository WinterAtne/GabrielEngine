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

	"github.com/WinterAtne/GabrielEngine/core"
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
	}

	CallDeffered(func() {node.Script.OnInit()})

	return node
}

// Places a node as the child of parent. If parent is queued, action is defered
// to the end of the frame, and the child is queued. Otherwise the action is
// imediate, and the child is not queued.
func (parent *Node) AddChild(child *Node) {
	if parent.index == -1 {
		addChild(parent, child)
	} else {
		CallDeffered(func() {
			addChild(parent, child)
			child.queueAdd()
		})
	}

}

func addChild(parent, child *Node) {
	if child == nil {
		log.Printf("Warning: attempted to give nil child parent")
		return
	} else if child.parent != nil || child.index != -1 {
		log.Printf("Warning: child %s already has parent", child.name)
		return
	}

	child.parent = parent
	parent.children = append(parent.children, child)
}

// Removes the parent of child. Also removes child from the queue if parent is queued.
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

func (node *Node) GetChildren() []*Node {
	return node.children
}

func GlobalTransform(tree []*Node) *core.Transform {
	global_transform := core.Transform{
		Layer: tree[0].Transform.Layer,
	}

	for _, node := range tree {
		global_transform = core.Transform{
			PositionX: node.Transform.PositionX + global_transform.PositionX,
			PositionY: node.Transform.PositionY + global_transform.PositionY,
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

