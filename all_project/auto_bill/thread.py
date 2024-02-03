#/*****************线程函数*****************/
#!/usr/bin/python3
# -*- coding: utf-8 -*-  

from datetime import datetime
from itertools import chain
import os
import sys
import time
import threading

import create
import water
import send_email
from bill_class import DB
from bill_class import Cmd
from bill_class import Index
from bill_class import Email
from bill_class import Check_Time


def edit_config():
    Email.m_bill_status     = False
    Email.m_old_water       = Email.m_now_water
    Email.m_old_electricity = Email.m_now_electricity

    if Email.m_now_electricity < Email.m_min_tip:    Email.m_bill_type   = True
    if Email.m_now_water       < Email.m_min_tip:    Email.m_bill_type   = False

# #/*****************检查费用余额*****************/
def check_balance():
    while True:
        _res = os.system("ping -c 1 www.baidu.com")
        if _res == 0:   break
        time.sleep(60)

# #获取账单低于10元发送email
# 修改一下，只有一个url也可以
    water.get_balanc()
    if Email.m_now_water == None or Email.m_now_electricity == None:    return

    Email.m_bill_status = False
    Email.m_bill_type   = False
    water.send_email()
    exit()

    if Email.m_now_electricity< Email.m_min_tip or Email.m_now_water < Email.m_min_tip:
        edit_config()

        while True:
            _res = os.system("ping -c 1 www.baidu.com")
            if _res == 0:
                water.send_email()
                return
            else:   time.sleep(60)

#/*****************时间线程*****************/
def time_thread(_temp):
    check_balance()

    while True:
        i_now_hour = datetime.now().hour
        #判断是否是白天
        if  i_now_hour > Check_Time.m_morning or i_now_hour < Check_Time.m_night:   time.sleep(7200)
        else:
            check_balance()
            time.sleep(60 * 30)

#/*****************交互线程*****************/
def check_email(_table, _email):
    _cmd = f"SELECT * FROM {_table} WHERE {DB.type_email} = '{_email}';"
    _res = DB._db.execute(_cmd)
    if len(list(_res)) == 0:
        return False
    return True

def result(_table, _email):
    DB.m_db.commit()
    _cmd = f"SELECT * FROM {_table} WHERE {DB.type_email} = '{_email}';"
    _res = DB._db.execute(_cmd)

    print(list(chain.from_iterable(list(_res))))

#false 为普通用户， true 为管理员用户
def add_user(_list, _is_special):
    if _is_special == False:
        if check_email(DB.table_user,_list[1]):
            print("用户已存在, 请使用edit命令来修改用户")
            return
        _cmd = f"INSERT INTO {DB.table_user} VALUES('{_list[1]}', '{_list[2]}');"
        DB._db.execute(_cmd)
        result(DB.table_user, _list[1])
    else:
        _result = DB._db.execute(f"SELECT (email) FROM {DB.table_admin};")
        _res    = list(chain.from_iterable(list(_result)))
        if bool(_res):
            print("admin用户已存在, 请使用eadmin命令来修改admin用户")
            return
        else:
            _cmd = f"INSERT INTO {DB.table_admin} VALUES({_list[1]}, {_list[2]}, {_list[3]}, {_list[4]});"
            DB._db.execute(_cmd)
            result(DB.table_admin, _list[1])
            if not check_email(DB.table_user,_list[1]):
                print("添加admin用户到用户列表")
                add_user(_list, False)

def setnow(_list):
    _result = DB._db.execute(f"SELECT * FROM {DB.table_user} WHERE {DB.type_email} = {_list[1]};")
    _res    = list(chain.from_iterable(list(_result)))

    if len(_res) == 0:
        print("请先在用户列表中添加用户")
        return

    DB._db.execute(f"INSERT INTO {DB.table_now} VALUES({_res[Index._EMAIL]});")
    DB.m_db.commit()
    print(f"now = {_res[Index._EMAIL]} 设置成功")

def seturl(_list):
    if not (_list[1] == "water" or _list[1] == "electricity"):
        print("name 错误 水费为:water 电费为:electricity")
        return
    DB._db.execute(f"UPDATE {DB.table_url} SET {DB.type_url} = '{_list[2]}' WHERE {DB.type_name} = '{_list[1]}';")
    DB.m_db.commit()
    print(f"name = {_list[1]} url = {_list[2]} 修改成功")


def remove_user(_list):
    if not check_email(DB.table_user,_list[1]):
        print("用户不存在, 请使用add命令来添加用户")
    else:
        result(DB.table_user, _list[1])
        _cmd = f"DELETE FROM {DB.table_user} WHERE {DB.type_email} = {_list[1]};"         
        DB._db.execute(_cmd)
        DB.m_db.commit()
        if check_email(DB.table_user,_list[1]):
            print("DELETE error")
        else:
            print("用户已删除")

#false为普通用户，true为admin用户
def edit_user(_list, _is_special):
    if not _is_special:
        if not check_email(DB.table_user,_list[1]):
            print("用户不存在, 请使用add命令来添加用户")
            return
        _cmd = "UPDATE %s SET %s = '%s' AND %s = '%s' WHERE %s = '%s' AND %s = '%s';" \
        %(DB.table_user, DB.type_email, _list[1], DB.type_name, _list[2], DB.type_email, _list[1], DB.type_name, _list[2])
        
        DB._db.execute(_cmd)
        result(DB.table_user, _list[1])
    else:
        if not check_email(DB.table_user,_list[1]):
            print("admin用户不存在, 请使用admin命令来添加admin用户")
            return
        _cmd = "UPDATE %s SET %s = '%s', %s = '%s', %s = '%s', %s = '%s' WHERE %s = '%s';" \
        %(DB.table_admin, DB.type_email, _list[1], DB.type_name, _list[2], DB.type_server, _list[3], DB.type_password, _list[4], DB.type_email, _list[1])
        
        DB._db.execute(_cmd)
        result(DB.table_admin, _list[1])
    print("用户已修改")


def select_user(_list):
    _cmd = f"SELECT * FROM {DB.table_user} WHERE {DB.type_email} = '{_list[1]}';" 
    _result = DB._db.execute(_cmd)
    _res = list(chain.from_iterable(list(_result)))     #把多维list解为一层list
    if bool(_res):  print(" email = ", _res[Index._EMAIL], " name = ", _res[Index._NAME])
    else:           print("用户不存在")

def getall_user():
    _cmd = f"SELECT * FROM {DB.table_user};"
    _result = DB._db.execute(_cmd)

    if len(list(_result)):
        for _val in _result:
            print(" email = ", _val[Index._EMAIL], " name = ", _val[Index._NAME])
    else:   print("没有用户, 请使用add命令添加用户")

def getadmin_user():
    _cmd = f"SELECT * FROM {DB.table_admin};"
    _result = DB._db.execute(_cmd)
    _res = list(chain.from_iterable(list(_result)))
    if bool(_res):
        pw_res = input("请输入admin用户的email密码：")
        if _res[Index._PASSWORD] == pw_res.strip():
            print("email = ", _res[0], " name = ", _res[1], " server = ", _res[2], " password = ", _res[3])
        else:   print("密码错误")
    else:       print("没有admin用户, 请使用add命令添加admin用户")

def get_balance():
    print("%s")
    print("%s")

def quit():
    DB._db.close()
    DB.m_db.close()
    exit()

def help(_dic):
    _result = DB._db.execute("SELECT * FROM help;")
    _mat = "{0:<10}\t{1:<35}\t{2:<10}"
    print(_mat.format("命令", "参数", "说明"))
    i = 0
    for _str in _result:
        _dic[_str[0]] = i   #ID key 字典 id与命令的映射关系
        i+=1
        print(_mat.format(_str[0], _str[1], _str[2]))
        # print("{0:<10}\t{1:<10}\t{2:<10}" .format(_str[0], _str[1], _str[2]))

def execute_cmd(_dic, _list):
    if len(_list[0]) == 0: return

    _val = _dic.get(_list[0])
    if _val == None:
        print("没有该命令, 请重试")
        return
    
    _cmd    = f"SELECT (param) FROM {DB.table_help} WHERE cmd = '{_list[0]}';"
    _result = DB._db.execute(_cmd)
    _res    = list(chain.from_iterable(list(_result)))
    if len(str(_res[0]).strip()) == 0:    param_len = 0
    else:                                 param_len = len(str(_res[0]).split('+'))

    if len(_list) - 1 != param_len:
        print("命令参数数量不正确")
        return

    if   _val == Cmd._ADD:      add_user(_list, False)
    elif _val == Cmd._ADMIN:    add_user(_list, True )
    elif _val == Cmd._SETNOW:   setnow(_list)
    elif _val == Cmd._SETURL:   seturl(_list)
    elif _val == Cmd._REMOVE:   remove_user(_list)
    elif _val == Cmd._EDIT:     edit_user(_list, False)
    elif _val == Cmd._EADMIN:   edit_user(_list, True )
    elif _val == Cmd._SELECT:   select_user(_list)
    elif _val == Cmd._GETALL:   getall_user()
    elif _val == Cmd._GETADMIN: getadmin_user()
    elif _val == Cmd._GETBILL:  get_balance()
    elif _val == Cmd._QUIT:     quit()
    elif _val == Cmd._HELP:     help()


def reply_thread(_temp):
    # _cmd        = []
    # _case_list  = []
    _dic        = {}
    help(_dic)
    
    while True:
        _cmd = input("请输入：")
        _cmd = _cmd.strip()
        # _case_list.append(_cmd.split(' '))    #append是把返回的列表添加到列表里面，所以它的type是list[list[]] 而不是 list["str"]
        _case_list = _cmd.split(' ')
        execute_cmd(_dic, _case_list)
        _case_list.clear()

if __name__ == '__main__':
    create.create_db()
    DB.init()

    if len(sys.argv) > 2:
        print("格式错误，进入交互线程 python3 thread.py back")
        sys.exit()
    elif len(sys.argv) == 2:
        if not sys.argv[1] == "back":
            print("格式错误，最后一个参数应为 back")
            sys.exit()
        t_thread = threading.Thread(target = reply_thread, args = (0,))
    else:   t_thread = threading.Thread(target = time_thread, args = (0,))
    t_thread.start()
    t_thread.join()