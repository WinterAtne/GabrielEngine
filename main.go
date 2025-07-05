package main

import "github.com/WinterAtne/Engine/core"

func main() {
	done := make(chan bool)
	go core.Start(done)

	<- done
}
