package model

import (
	"fmt"

	"github.com/jinzhu/gorm"
	_ "github.com/jinzhu/gorm/dialects/mysql"
)

//表名会自动转成小写, 最后加s, 结果为users
type User struct {
	Id       int    `gorm:"column:id; type:int(11);primary_key;"` //实际上Id还自增
	Username string `gorm:"column:username; type:varchar(32);"`
	Password string `gorm:"column:password; type:varchar(512);"`
}

func Init() {
	db, err := gorm.Open("mysql", "root:19951020jxt@/goCMSdb?charset=utf8&parseTime=True&loc=Local")
	defer db.Close()
	if err != nil {
		panic(err)
	}
	if status := db.HasTable(&User{}); status == false {
		panic("Table users not existed!")
	} else {
		fmt.Println("Table users already existed")
	}
}

func Test() {
	db, err := gorm.Open("mysql", "root:19951020jxt@/goCMSdb?charset=utf8&parseTime=True&loc=Local")
	defer db.Close()
	if err != nil {
		panic(err)
	}
	var user = User{Username: "root2", Password: "root2"}
	var users []User
	db.Where("username = ?", "root2").Find(&users)

	if len(users) > 0 {
		fmt.Println("table users: root of username already existed")
		if targetUser := users[0]; targetUser.Password == "root" {
			fmt.Println("table users: root of username, password check ok!")
		} else {
			fmt.Println("table users: root of username, password check error!")
		}
	} else {
		if errc := db.Create(&user).Error; errc != nil {
			panic(errc)
		}
		fmt.Println("table users: root of username has been created")
	}
}

/*
 * 验证用户是否存在
 * 参数:username string用户名, password string密码
 * 应确保调用者已验证两者的格式
 * 返回code int:-1: 服务器内部错误, 0:成功, 1:用户已存在
 */
func CheckUserIsExist(username string, password string) int {
	db, err := gorm.Open("mysql", "root:19951020jxt@/goCMSdb?charset=utf8&parseTime=True&loc=Local")
	defer db.Close()
	if err != nil {
		panic(err)
		return -1
	}
	var users []User
	db.Where(&User{Username: username, Password: password}).Find(&users)
	if len(users) > 0 {
		return 0
	} else {
		return 1
	}
}

/*
 * 插入用户
 * 参数:username string用户名, password string密码
 * 应确保调用者已验证两者的格式
 * 返回code int:-1: 服务器内部错误, 0:成功, 1:用户已存在
 */
func InsertUser(username string, password string) int {
	db, err := gorm.Open("mysql", "root:19951020jxt@/goCMSdb?charset=utf8&parseTime=True&loc=Local")
	defer db.Close()
	if err != nil {
		panic(err)
		return -1
	}
	var users []User
	db.Where(&User{Username: username}).Find(&users)
	if len(users) > 0 {
		return 1
	} else {
		if errc := db.Create(&User{Username: username, Password: password}).Error; errc != nil {
			return -1
		} else {
			return 0
		}
	}
}
