package tree

var root *Node

/* Init */
func init() {
	root = NewNode(new(rootObject), "root")
	root.queueAdd()
}

func GetRoot() *Node {
	return root
}

type rootObject struct {
	Object
}

