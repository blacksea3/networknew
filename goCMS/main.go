package main

import (
	model "goCMS/model"
	url "goCMS/url"

	"github.com/gin-gonic/gin"
)

func main() {
	model.Init()
	r := gin.Default()
	r.Static("/static", "./static")
	r.LoadHTMLGlob("templates/*")
	url.Url(r)
	r.Run()
}

/*
import (
	"fmt"
	model "goCMS/model"
)

func main() {
	fmt.Println(model.InsertUser("root3", "root3"))
}*/
