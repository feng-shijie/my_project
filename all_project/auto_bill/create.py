#!/usr/bin/python3
# -*- coding: utf-8 -*-  

import sqlite3
import os
import sys
from bill_class import DB

def create_db(change = 0):
    _dir = sys.path[0] + "/db"
    if not os.path.isdir(_dir):
        _cmd = "mkdir " + _dir
        os.system(_cmd)

    _dir = _dir + "/user.db"
    if change == 0 and os.path.exists(_dir):
        # print("user.db Lib to exist!")
        return

    m_db = sqlite3.connect(_dir)
    _db = m_db.cursor()
    _db.execute("create table email_user(id INTEGER PRIMARY KEY,email,name);")
    _db.execute('''
                CREATE TRIGGER auto_sort AFTER DELETE ON email_user
                BEGIN
                    UPDATE email_user SET id = id-1 WHERE id > OLD.id;
                END;''')
    _db.execute("create table help(id INTEGER PRIMARY KEY,cmd);")
    _db.execute("create table admin(email,name,server,password);")
    _db.execute("create table now(id);")
    _db.execute("create table url(name,url);")
    _db.execute(f"INSERT INTO {DB.table_url}({DB.type_name}) VALUES ('warter');")
    _db.execute(f"INSERT INTO {DB.table_url}({DB.type_name}) VALUES ('electricity');")

    #重新导入帮助语句
    _result = _db.execute("SELECT *FROM help")
    if bool(_result):
        _db.execute("DELETE FROM help;")

    _db.execute("INSERT INTO help(cmd) VALUES('add|email+name|添加用户');")
    _db.execute("INSERT INTO help(cmd) VALUES('admin|email+name+smtp_server+password|添加管理员<管理员只能存在一位>');")
    _db.execute("INSERT INTO help(cmd) VALUES('setnow|ID|设置当前缴费者');")
    _db.execute("INSERT INTO help(cmd) VALUES('seturl|name+url|设置water/electricity url');")
    _db.execute("INSERT INTO help(cmd) VALUES('edit|email+name|修改用户');")
    _db.execute("INSERT INTO help(cmd) VALUES('eadmin|email+name|修改管理员');")
    _db.execute("INSERT INTO help(cmd) VALUES('remove|email+name|删除用户');")
    _db.execute("INSERT INTO help(cmd) VALUES('select|email|查找用户');")
    _db.execute("INSERT INTO help(cmd) VALUES('getall| |获取所有用户email和name');")
    _db.execute("INSERT INTO help(cmd) VALUES('getadmin| |获取管理员email和name');")
    _db.execute("INSERT INTO help(cmd) VALUES('getbalance| |获取当前的余额');")
    _db.execute("INSERT INTO help(cmd) VALUES('quit| |退出');")
    _db.execute("INSERT INTO help(cmd) VALUES('help| |打印帮助信息');")

    m_db.commit()
    m_db.close()
    print("user.db Lib create success")