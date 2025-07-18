package tree

var root *Node

/* Init */
func init() {
	root = &Node{
		index: -1,
		name: "root",
		Script: new(rootObject),
	}
	root.queueAdd()
}

func GetRoot() *Node {
	return root
}

type rootObject struct {
	Object
}

