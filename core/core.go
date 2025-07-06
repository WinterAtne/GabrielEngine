/*
The package core provides functions and types for interfacing between the OpenGL and GLFW.
It provides a way to work with the Window, Sprites and Rendering, Input, and Audio.
All functions are asyncronus by default, do not call them as a goroutine. However,
all functions are safe to be called from goroutines.

Begin a context with Start(). In order to facilitate synchronization between threads
the function FinishFrame should be called when the program has made all nessesary modifications to the
current state of the program for rendering the next frame. The remaining queued functions will
be executed, the window processed, and the frame drawn.

If an external function needs to be called on the render thread use CallOnRenderThead(). See its documentation.
*/
package core

// #cgo LDFLAGS: -ldl -lglfw -lm -L${SRCDIR}/CCore/bin/lib -lCCore
//
// #include "CCore/src/window.h"
// #include "CCore/src/sprite.h"
import "C"
import (
	"runtime"
)

/* --- Constants --- */
const frameQueueWidth int = 32

/* --- Package Globals --- */

// A package global for sending functions to be run on the render thread.
var funcQueue chan func() = make(chan func(), frameQueueWidth)
/* --- Local --- */
// A signal that the no more functions will be put on to the 
var finishFrame chan bool = make(chan bool)
// A signal that the program should finish and close.
var finishProgram chan bool = make(chan bool)

var frameStart chan bool = make(chan bool)

var isProcessing bool = false

// Beings the asynchronus render goroutine loop.
func Start() {
	// This entire function must run asynchronusly to avoid blocking or locking the main goroutine.
	go func() {	
	isProcessing = true

	runtime.LockOSThread()
	c_err := C.InitWindow();
	if (c_err != 0) {
		panic("Failled to init window")
	}
	c_err = C.InitSprites();
	if (c_err != 0) {
		panic("Failled to init window")
	}
	
	var shouldBreak bool = false
	for (!bool(C.ShouldDestroyWindow()) && !shouldBreak) {

		finishFrameValue := false

		for !finishFrameValue || (len(funcQueue) != 0) && !shouldBreak {
			select {
			case f:=(<-funcQueue):
				f()
			case finishFrameValue=(<-finishFrame):
			default:
			}
		}

		C.DrawSpriteQueue()
		C.ProcessWindow()

		select {
		case shouldBreak=(<-finishProgram):
		default:	
		}
		if shouldBreak {
			break
		} else {
			frameStart <- true
		}
	}

	C.DestroyWindow()
	_=<-finishFrame
	frameStart <- false
	}()
}

/* 
Queues a function to be called on the render thread.
Other functions can be wrapped using anonymus functions.

	rtrn := make(chan int)
	CallOnRenderThead(func() {
		rtrn<-SomeOtherFunction(arg1, arg2, arg3)
	})
	value := <-return

*/
func CallOnRenderThead(call func()) {
	funcQueue <- call
}

// Signals to the Render Thread that no more functions will be put in the call queue.
// Blocks thread until next frame
func FinishFrame() {
	finishFrame <- true
}

// Blocks the current thread until the render thread has started the next frame.
// If a value of false is returned, the Render Thread has ended, and so calling
// this function again will block the thread permanently
func BlockTillNextFrame() bool {
	value:=<-frameStart

	return value
}

// Signals to the Render Thread to close the window after the current frame.
// Attempting further calls to this package is undefined behavior.
func Finish() {

	CallOnRenderThead(func() {
		C.DestroyWindow()
	})

}

