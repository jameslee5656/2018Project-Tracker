from __future__ import print_function
from flask import Flask, render_template,redirect,url_for,request,jsonify,flash
from flask_wtf import FlaskForm
from wtforms import SelectField, SubmitField
from wtforms.validators import DataRequired
from flask_pymongo import PyMongo
from flask_login import LoginManager, UserMixin, login_user, login_required, logout_user, current_user
from forms import Selection,Register,Login, Log_out, Delete_friend, Supervise_choose_friend
from SetFence import electricFence
import datetime
import matplotlib.pyplot as plt
from PIL import Image
from io import BytesIO
from health import Health
from Ranking import Rank
import requests
import socket
import time
import sys
import os

app = Flask(__name__)
app.config['SEND_FILE_MAX_AGE_DEFAULT'] = datetime.timedelta(seconds=0)
app.config["MONGO_URI"] = "mongodb://120.126.136.17:27017/Tracker"
app.config['SECRET_KEY'] = "025300a65059e046175068af08abe39d"
login_manager = LoginManager()
login_manager.login_view = 'login'
login_manager.init_app(app)

class User(UserMixin):
    pass

@login_manager.user_loader
def user_loader(username):
    mongo = PyMongo(app)
    if mongo.db.User_Info.find({'username': username}) is not None:
        user = User()
        user.id = username
        return user

@app.route("/", methods=['GET','POST'])
@app.route("/home", methods=['GET','POST'])
def home():
    mongo = PyMongo(app)
    reg = Register()
    log = Login()
    user = ''

    if current_user.is_active:
        user = current_user.id

    '''登入實作'''
    if log.validate_on_submit():
        user = User()
        user.id = log.data['username']
        login_user(user)
        return redirect(url_for('home'))
        
    '''註冊實作'''
    if reg.validate_on_submit():
        '''加入Friend'''
        friends = mongo.db.User_Info.find()
        temp = []
        for f in friends:
            temp.append(f['username'])
        friend = { 'username' : reg.data['username'],
                   'Friends' : temp}
        mongo.db.Friend.insert_one(friend)

        '''加入Select'''
        sel = { 'username' : reg.data['username'],
                'fencescale' : 10,
                'person' : reg.data['username']}
        mongo.db.Select_People.insert_one(sel)

        '''加入User_Info'''
        new_user = { 'username' : reg.data['username'],
                     'password' : reg.data['password'],
                     'bracelet_number' : reg.data['bracelet_number'],
                     'acount_mode' : 'OWNER',
                     'organization' : reg.data['organization']
                    }
        mongo.db.User_Info.insert_one(new_user)

        user = User()
        user.id = reg.data['username']
        login_user(user)
        return redirect(url_for('home'))
    return render_template('index.html',reg = reg, log = log, user = user)

@app.route("/user_information",methods=['GET','POST'])
def user_information():
    mongo = PyMongo(app)
    friend_pic = []
    if current_user.is_active:
        user = current_user.id
        friends = mongo.db.Friend.find({'username': user})[0]['Friends']

        '''好友圖片處理'''
        for friend in friends:
            temp = '../static/images/user/' + friend + '.jpg'
            friend_pic.append(temp)
        return render_template('userInfo.html', user = user, friends = friends, friend_pic = friend_pic)
    else:
        return '請先登入！'

@app.route("/delete_friend_api",methods=['GET','POST'])
def delete_friend():
    mongo = PyMongo(app)
    user = current_user.id
    friend = mongo.db.Friend.find({'username': user})[0]['Friends']
    if request.method == "GET":
        del_friend = request.values.get("del")
        friend.remove(del_friend)
        de = mongo.db.Friend.delete_one({'username': user})
        insert = { 'username': user,
                   'Friends' : friend}
        ins = mongo.db.Friend.insert_one(insert)
    return redirect(url_for('user_information'))

@app.route("/add_friend_api",methods=['GET','POST'])
def add_friend():
    mongo = PyMongo(app)
    user = current_user.id
    allusers = []

    #取得好友資料
    friend = mongo.db.Friend.find({'username': user})[0]['Friends']
    if request.method == "GET":
        add = request.values.get("add")
        try:
            get_users = mongo.db.User_Info.find({'username': add})[0]
            if add in friend:
                flash('已經為好友')
            else:
                if add==user:
                    flash('不要玩自己')
                else:
                    friend.append(add)
                    de = mongo.db.Friend.delete_one({'username': user})
                    insert = { 'username': user,
                            'Friends' : friend}
                    ins = mongo.db.Friend.insert_one(insert)
                    flash('新增好友成功')
        except Exception as e:
            flash('查無使用者')
    return redirect(url_for('user_information'))

@app.route("/supervise", methods=['GET','POST'])
def supervise():
    mongo = PyMongo(app)
    user_login = '請先登入'
    pic = '../static/images/user/blank.jpg'
    Friends = []
    friends_number = []
    friend_pic = []
    choose_friends = []
    if current_user.is_active:
        user = current_user.id
        user = str(user)
        pic = '../static/images/user/' + user + '.jpg'
    else:
        return '請先登入！'

    return render_template('supervise.html', user = user, pic = pic)


@app.route("/health")
def health():
    mongo = PyMongo(app)
    if current_user.is_active:
        user = current_user.id

        #抓選項
        scale = mongo.db.Select_People.find({'username': user})[0]['fencescale']
        person = mongo.db.Select_People.find({'username': user})[0]['person']

        Friends = mongo.db.Friend.find({'username': user})[0]['Friends']
        Friends.append(user)
    return render_template('health.html', Friends = Friends, scale = scale, person = person)

@app.route("/change_person",methods=['GET','POST'])
def change_person():
    mongo = PyMongo(app)
    user = current_user.id
    if request.method == "POST":
        change = request.values.get("change")
        scale = mongo.db.Select_People.find({'username': user})[0]['fencescale']
        de = mongo.db.Select_People.delete_one({'username': user})
        insert = { 'username': user,
                   'fencescale' : scale, 
                   'person' : change}
        ins = mongo.db.Select_People.insert_one(insert)
    return redirect(url_for('health'))


@app.route("/bracelet")
def bracelet():
    ss = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
    address = ("120.126.136.17", 5687)
    msg = "Get message from Web!"
    ss.sendto(msg.encode("gbk"), address)
    ss.close()
    return jsonify([])

@app.route("/logout/")
def logout():
    logout_user()
    return redirect(url_for('home'))

@app.route("/Change_pic", methods =['GET','POST'])
def Change_pic():
    if current_user.is_active:
        user = current_user.id
        if request.method == "POST":
            #刪除現有圖片
            del_img = r"C:/Users/user/Desktop/Tracker3/static/images/user/" + user + ".jpg"
            try:
                os.remove(del_img)
            except OSError as e:
                print(e)
            else:
                print("File is deleted successfully", file=sys.stderr)

            img = request.files['uploadpic']
            new_img = img.read()
            print(img, file=sys.stderr)
            with open(del_img, 'wb') as f:
                f.write(new_img)
    return redirect(url_for('supervise'))

@app.route('/change_Date')
def change_Date():
    mongo = PyMongo(app)
    startDate = request.args.get('startDate', 0, type=float)
    endDate = request.args.get('endDate', 0, type=float)
    fenceScale = request.args.get('fenceScale', 0, type=float)

    if current_user.is_active:
        user = current_user.id
        person = mongo.db.Select_People.find({'username': user})[0]['person']
        userlist = [person]
        FenceScale = int(fenceScale)
        elFence = electricFence()
        elFence.pullDataWithTime(user, userlist,startDate,endDate)
        spacelist,valuelist,base = elFence.squareBounds(FenceScale)
    return jsonify(spacelist,valuelist)

@app.route("/GetRanking_api", methods =['GET','POST'])
def GetRanking_api():
    if current_user.is_active:
        user = current_user.id
        rank = Rank(user)
        ranking = rank.GetRanking()
    return jsonify(ranking)

@app.route("/GetHealth_api", methods =['GET','POST'])
def GetHealth_api():
    mongo = PyMongo(app)
    datatype = request.args.get('type', 0)
    duration = request.args.get('duration')
    time = request.args.get('time', 0)
    day = int(time.split('/',2)[1])
    month = int(time.split('/',2)[0])

    if current_user.is_active:
        user = current_user.id
        person = mongo.db.Select_People.find({'username': user})[0]['person']
        health = Health(person)
        #處理天
        if duration != 'false':
            x_time,y_data = health.Day(datatype,day,month)
        #處理週
        else:
            x_time,y_data = health.Week(datatype)
    return jsonify(x_time,y_data)

@app.route('/heatpoint_api')
def heatpoint_api():
    mongo = PyMongo(app)
    userlist = []
    if current_user.is_active:
        user = current_user.id
        users = mongo.db.User_Info.find()
        for user in users:
            userlist.append(user['username'])
        elFence = electricFence() 
        elFence.pullData(user,userlist,days=14)
        spacelist,valuelist,base = elFence.squareBounds()
    return jsonify(valuelist,base)

@app.route('/prediction_api')
def prediction_api():
    mongo = PyMongo(app)
    time = request.args.get('time', 0)
    day = int(time.split('/',2)[1])
    month = int(time.split('/',2)[0])
    x = []
    predict_y = []
    real_y = []

    if current_user.is_active:
        user = current_user.id
        select_user = mongo.db.Select_People.find({'username': user})[0]['person']
        predict_data = mongo.db.prediction.find({'user': select_user , 'day': day , 'month': month , 'type': 'hourly'})
        for data in predict_data:
            x.append(data['hour'])
            predict_y.append(data['prediction'])

        for sub_x in x:
            hour_step = 0
            real_data = mongo.db[select_user].find({'day': day , 'month': month , 'hour': sub_x})
            for data in real_data:
                if data['step_value']  != '':
                    hour_step = hour_step + int(data['step_value'])
            real_y.append(hour_step)

        print(x, file=sys.stderr)
        print(predict_y, file=sys.stderr)
        print(real_y, file=sys.stderr)

    return jsonify(x ,predict_y ,real_y)



if __name__ == '__main__':
    app.run(host = "0.0.0.0")