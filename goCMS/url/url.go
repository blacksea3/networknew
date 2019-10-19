package url

import (
	view "goCMS/view"

	"github.com/gin-gonic/gin"
)

func Url(e *gin.Engine) {
	e.GET("/login.html", view.LoginHTML)
	e.GET("/index.html", view.Index)
	e.POST("/register", view.Register)
	e.POST("/login", view.Login)
}
