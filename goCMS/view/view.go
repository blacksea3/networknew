package view

import (
	"bytes"
	model "goCMS/model"
	"io/ioutil"
	"net/http"

	"fmt"
	"regexp"

	"github.com/gin-gonic/gin"
)

func LoginHTML(c *gin.Context) {
	c.HTML(200, "login.html", gin.H{})
}

func Index(c *gin.Context) {
	c.HTML(200, "index.html", gin.H{})
}

/*
 * 登录Controller:
 * 与前端的交互: 除内部错误500外其他情况为200
 * 向前端返回String: code#content,  code is int, content is string
 * 登录成功返回"0#", 登录失败返回"1#失败原因"
 */
func Login(c *gin.Context) {
	bodyByts, err := ioutil.ReadAll(c.Request.Body)
	if err != nil {
		c.String(http.StatusBadRequest,
			err.Error())
		c.Abort()
	}
	//ioutil.ReadAll之后, c.PostForm拿不到数据, 故
	// 添加c.Request.Body = ioutil.NopCloser...
	c.Request.Body = ioutil.NopCloser(
		bytes.NewBuffer(bodyByts))
	userName := c.PostForm("username")
	passWord := c.PostForm("password")

	//判断合法性, 正则表达式
	username_re := regexp.MustCompile("[a-zA-Z0-9_]{8,16}")         //用户名包括大小写字母/数字/下划线,共8-16位
	password_re := regexp.MustCompile("[a-zA-Z][a-zA-Z0-9_]{7,15}") //密码以字母开头,包括大小字母/数字/下划线,共8-16位
	usernameIsok := fullymatch(userName, username_re)
	passwordIsok := fullymatch(passWord, password_re)
	if (usernameIsok == true) && (passwordIsok == true) {
		code := model.CheckUserIsExist(userName, passWord)
		switch code {
		case -1:
			{
				c.String(500, "1#内部错误")
				break
			}
		case 0:
			{
				c.String(200, "0#验证成功")
				break
			}
		case 1:
			{
				c.String(200, "1#用户不存在或密码错误")
				break
			}
		default:
			{
				c.String(500, "1#内部错误")
				break
			}
		}
	} else {
		c.String(200, "1#用户名或密码格式错误")
	}
}

/*
 * 注册Controller:
 * 与前端的交互: 除内部错误500外其他情况为200
 * 向前端返回String: code#content,  code is int, content is string
 * 注册成功返回"0#", 注册失败返回"1#失败原因"
 */
func Register(c *gin.Context) {
	bodyByts, err := ioutil.ReadAll(c.Request.Body)
	if err != nil {
		c.String(http.StatusBadRequest,
			err.Error())
		c.Abort()
	}
	//ioutil.ReadAll之后, c.PostForm拿不到数据, 故
	// 添加c.Request.Body = ioutil.NopCloser...
	c.Request.Body = ioutil.NopCloser(
		bytes.NewBuffer(bodyByts))
	userName := c.PostForm("username")
	passWord := c.PostForm("password")

	//判断合法性, 正则表达式
	username_re := regexp.MustCompile("[a-zA-Z0-9_]{8,16}")         //用户名包括大小写字母/数字/下划线,共8-16位
	password_re := regexp.MustCompile("[a-zA-Z][a-zA-Z0-9_]{7,15}") //密码以字母开头,包括大小字母/数字/下划线,共8-16位
	usernameIsok := fullymatch(userName, username_re)
	passwordIsok := fullymatch(passWord, password_re)
	if (usernameIsok == true) && (passwordIsok == true) {
		code := model.InsertUser(userName, passWord)
		switch code {
		case -1:
			{
				c.String(500, "1#内部错误")
				break
			}
		case 0:
			{
				c.String(200, "0#注册成功")
				break
			}
		case 1:
			{
				c.String(200, "1#用户已存在")
				break
			}
		default:
			{
				c.String(500, "1#内部错误")
				break
			}
		}
	} else {
		c.String(200, "1#用户名或密码格式错误")
	}
}

func fullymatch(targets string, r *regexp.Regexp) bool {
	s := r.FindStringSubmatch(targets)
	if (len(s) == 0) || (len(s[0]) != len(targets)) {
		return false
	} else {
		return true
	}
}

func TestUserFormatCheck() {
	username_re := regexp.MustCompile("[a-zA-Z0-9_]{8,16}")         //用户名可包括大写小写字母数字和下划线,8-16位
	password_re := regexp.MustCompile("[a-zA-Z][a-zA-Z0-9_]{7,15}") //密码以字母开头,可包括大写小写字母数字和下划线,一共8-16位

	fmt.Println(fullymatch("username", username_re))
	fmt.Println(fullymatch("useAna1e", username_re))
	fmt.Println(fullymatch("user", username_re))
	fmt.Println(fullymatch("TOOBIGTOOBIGTOOBIGddsdsds", username_re))
	fmt.Println(fullymatch("us-11dsadADds", username_re))
	fmt.Println(fullymatch("us-1!1!ds!adA!Dds", username_re))

	fmt.Println(fullymatch("1sername", password_re))
	fmt.Println(fullymatch("useAna1e", password_re))
	fmt.Println(fullymatch("user", password_re))
	fmt.Println(fullymatch("TOOBIGTOOBIGTOOBIGddsdsds", password_re))
	fmt.Println(fullymatch("us-11dsadADds", password_re))
	fmt.Println(fullymatch("us-1!1!ds!adA!Dds", password_re))
}
