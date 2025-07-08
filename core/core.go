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
const frameQueueWidth int = 1024

/* --- Package Globals --- */

// A package global for sending functions to be run on the render thread.
var funcQueue chan func() = make(chan func(), frameQueueWidth)
/* --- Local --- */
// A signal that the no more functions will be put in the queue.
var finishFrame chan bool = make(chan bool)

var frameStart chan bool = make(chan bool)

// Beings the asynchronus render goroutine loop.
func Start() {
	// This entire function must run asynchronusly to avoid blocking or locking the main goroutine.
	go func() {	
	runtime.LockOSThread() // OpenGL throws a fit when you change the thread
	c_err := C.InitWindow();
	if (c_err != 0) {
		panic("Failled to init window")
	}
	c_err = C.InitSprites();
	if (c_err != 0) {
		panic("Failled to init window")
	}
	
	renderLoop()

	C.DestroyWindow()
	_=<-finishFrame
	frameStart <- false
	}()
}

func renderLoop() {
	for (!bool(C.ShouldDestroyWindow())) {
		finishFrameValue := false

		for ((len(funcQueue) != 0) || !finishFrameValue) {
			select {
			case f:=(<-funcQueue):
				f()
			case finishFrameValue=(<-finishFrame):
			default:
			}
		}

		C.DrawSpriteQueue()
		C.ProcessWindow()

		frameStart<-true
	}
}

/* 
Queues a function to be called on the render thread. Guarentees order of execution.

Other functions can be wrapped using anonymus functions:

	rtrn := make(chan int)
	CallOnRenderThead(func() {
		rtrn<-SomeOtherFunction(arg1, arg2, arg3)
	})
	value := <-return
	close(rtrn)

*/
func CallOnRenderThead(call func()) {
	funcQueue <- call
}

// Signals to the Render Thread that no more functions will be put in the call queue until the next frame.
// It is recomended to block the thread using BlockTillNextFrame soon after calling this.
// Should not be called more than once per frame.
func FinishFrame() {
	finishFrame <- true
}

// TODO can only be used by one thread at a time
// Blocks the current thread until the render thread has started the next frame.
// If a value of false is returned, the Render Thread has ended, and so calling
// this function again will block the thread permanently
func BlockTillNextFrame() bool {
	return <-frameStart
}

// Signals to the Render Thread to close the window after the current frame.
// Attempting further calls to this package is undefined behavior.
func Finish() {
	CallOnRenderThead(func() {
		C.DestroyWindow()
	})
}

