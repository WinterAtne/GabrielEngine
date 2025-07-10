package tree

import (
	"reflect"
	"testing"
)

func TestTree(t *testing.T) {
	getNames := func(want, got []*Node) (in, out string) {
		in = ""
		for _, n := range want {
			in += "/" + n.name
		}
		out = ""
		for _, n := range got {
			out += "/" + n.name
		}

		return in, out
	}

	root := GetRoot()

	main := NewNode(new(Object), "main")
	first := NewNode(new(Object), "first")
	second := NewNode(new(Object), "second")
	third := NewNode(new(Object), "third")
	fourth := NewNode(new(Object), "fourth")

	root.AddChild(main)

	main.AddChild(first)
	main.AddChild(second)

	first.AddChild(third)
	first.AddChild(fourth)

	Process(0) // Adding children is defered

	want := []*Node{fourth, first, main, root}
	got := fourth.GetTree()


	if !reflect.DeepEqual(want, got) {
		in, out := getNames(want, got)
		t.Fatalf("wanted: %s | got: %s", in, out)
	}

	want = []*Node{third, first, main, root}
	got = third.GetTree()

	if !reflect.DeepEqual(want, got) {
		in, out := getNames(want, got)
		t.Fatalf("wanted: %s | got: %s", in, out)
	}

	fourth.RemoveParent()

	Process(0) // Adding children is defered

	want = []*Node{fourth}
	got = fourth.GetTree()

	if !reflect.DeepEqual(want, got) {
		in, out := getNames(want, got)
		t.Fatalf("wanted: %s | got: %s", in, out)
	}

	main.RemoveParent()

	Process(0) // Adding children is defered
}

