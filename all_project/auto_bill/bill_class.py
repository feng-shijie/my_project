#!/usr/bin/python3

import sqlite3
import sys
from enum import Enum

class DB:
    m_db            = None
    _db             = None

    table_user      = "email_user"
    table_admin     = "admin"
    table_help      = "help"
    table_url       = "url"
    table_now       = "now"

    type_id         = "id"
    type_url        = "url"
    type_email      = "email"
    type_name       = "name"
    type_server     = "server"
    type_password   = "password"
    def init():
        DB.m_db     = sqlite3.connect(f"{sys.path[0]}/db/user.db", check_same_thread=False)
        DB._db      = DB.m_db.cursor()

class Email:
    istype      = None      #是水费还是电费
    #admin
    smtp_server = ""
    smtp_port   = 25
    sender_email    = ""
    sender_password = ""

    #所有收件人
    receiver_email  = []
    now         = None  #当前要通知的缴费人员的ID

    #info
    now_email   = None
    balance     = None
    subject     = "!!!账单!!!"
    body        = "请及时缴费"
    water_text       = "水费不足 10元"
    electricity_text = "电费不足 10元"

    subject_success     = "!!!恭喜!!!"
    body_success        = "余额为：： "
    water_success_text       = " 水费充值成功"
    electricity_success_text = " 电费充值成功"

class Network:
    start = None

class Check_Time:
    m_morning       = 10
    m_night         = 22
    m_time_interval = 3

class Cmd:
    _ADD         = 0
    _ADMIN       = 1
    _SETNOW      = 2
    _SETURL      = 3
    _EDIT        = 4
    _EADMIN      = 5
    _REMOVE      = 6
    _SELECT      = 7
    _GETALL      = 8
    _GETADMIN    = 9
    _GETBILL     = 10
    _QUIT        = 11
    _HELP        = 12

class Index:
    _ID          = 0
    _EMAIL       = 1
    _NAME        = 2
    _SERVER      = 3
    _PASSWORD    = 4
