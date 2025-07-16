package tree

import (
	"fmt"
	"reflect"
	"testing"

	"github.com/WinterAtne/GabrielEngine/core"
)

type mockNode struct {
	Object
	Prop0, Prop1 float32
	Prop2, Prop3 string
}

func init() {
	RegisterScriptName("mockNode", func() Script {return new(mockNode)})
}

func nodePrint(a *Node) {
	fmt.Println(a)
	for _, c := range a.children {
		nodePrint(c)
	}
}

// Validates that scene.Instantiate can properly instante a working node which
// is defined in resources/scenes/test_work.json & test_work_child.json
func TestSceneInstantiateWork(t *testing.T) {
	t.Chdir("../")
	LoadScenes()

	mtest_work := NewNode(new(Object), "test_work")
	mexample := NewNode(new(Object), "example")
	mexample.Transform = core.Transform{
		PositionX: 0.0,
		PositionY: 64.0,
		ScaleX: 1.0,
		ScaleY: 1.0,
		Rotation: 0.0,
		Layer: 1,
	}

	mtest_work.AddChild(mexample)

	mo0 := NewNode(new(Object), "o0")
	mo0.Transform = core.Transform{
		PositionX: -40.0,
		PositionY: 0.0,
		ScaleX: 1.0,
		ScaleY: 1.0,
		Rotation: 0.0,
		Layer: 5,
	}

	mo1 := NewNode(new(Object), "o1")
	mo1.Transform = core.Transform{
		PositionX: 32.0,
		PositionY: 0.0,
		ScaleX: 1.0,
		ScaleY: 1.0,
		Rotation: 0.0,
		Layer: 5,
	}
	
	mo5 := NewNode(&mockNode{
		Prop0: 5.0,
		Prop1: 6.66,
		Prop2: "hihi",
		Prop3: "byebye",
	}, "o5")
	mo5.Transform = core.Transform{
		PositionX: 0.0,
		PositionY: -40.0,
		ScaleX: 1.0,
		ScaleY: 1.0,
		Rotation: 0.0,
		Layer: 5,
	}

	mexample.AddChild(mo0)
	mexample.AddChild(mo1)
	mexample.AddChild(mo5)

	mo3 := NewNode(new(Object), "o3")
	mo3.Transform = core.Transform{
		PositionX: 0.0,
		PositionY: -40.0,
		ScaleX: 1.0,
		ScaleY: 1.0,
		Rotation: 0.0,
		Layer: 5,
	}
	mo0.AddChild(mo3)


	scene := GetScene("test_work.json")
	test_work, err := scene.Instantiate()
	if err != nil || test_work == nil {
		t.Error("test_work failled")
	}

	Process(0)

	if !reflect.DeepEqual(mtest_work, test_work) {
		fmt.Println()
		nodePrint(mtest_work)
		fmt.Println()
		nodePrint(test_work)
		fmt.Println()

		fmt.Printf("%v\n%v\n", test_work.children[0].children[2].Script, mo5.Script)
		t.Errorf("something")
	}
}
