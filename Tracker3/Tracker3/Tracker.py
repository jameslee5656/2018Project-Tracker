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
    if current_user.is_active:
        user = current_user.id
        friends = mongo.db.Friend.find({'username': user})[0]['Friends']
        return render_template('userInfo.html', user = user, friends = friends)
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

        # '''產生好友列表'''
        # Friends = mongo.db.Friend.find({'username': user})[0]['Friends']
        # for friend in Friends:
        #     '''好友圖片處理'''
        #     temp = '../static/images/user/' + friend + '.jpg'
        #     friend_pic.append(temp)

        # '''處理勾選好友選項'''
        # if request.method == "POST":
        #     choose_friends = request.values.getlist("choose_friends")
        #     de = mongo.db.Supervise_Friend.delete_one({'username': user})
        #     insert = { 'username': user,
        #                'Visable_Friends' : choose_friends}
        #     ins = mongo.db.Supervise_Friend.insert_one(insert)
        # else:
        #     choose_friends = mongo.db.Supervise_Friend.find({'username': user})[0]['Friends']
            
        # '''產生選擇的好友手環編號'''
        # for choose in choose_friends:
        #     temp_number = mongo.db.User_Info.find({'username' : choose})[0]['bracelet_number']
        #     friends_number.append(temp_number)

    return render_template('supervise.html', user = user, pic = pic)

@app.route("/history", methods=['GET','POST'])
def history():
    global select
    mongo = PyMongo(app)
    select = Selection()
    mongo = PyMongo(app)
    user = "請先登入"
    pic = '../static/images/user/blank.jpg'
    FenceScale = 0
    userlist = []
    boundlist = []
    if current_user.is_active:
        user = current_user.id
        '''動態產生選項'''
        dates = []
        checks = mongo.db[user].find({})
        for check in checks:
            temp_str = str(check['month']) + '/' + str(check['day'])
            if temp_str not in dates:
                dates.append(temp_str)
        select.date.choices = [(date,date) for date in dates]
        select.user.choices = [(user,user)]
        pic = '../static/images/user/' + user + '.jpg'

        '''電子圍籬格子大小選擇'''
        if request.method == "POST":
            FenceScale = request.values.get("FenceScale")
            FenceScale = int(FenceScale)

        '''使用者選擇'''
        if user == "manager":
            alluser = mongo.db.User_Info.find()
            for getuser in alluser:
                userlist.append(getuser['username'])
        else:
            userlist.append(user)

        # print(FenceScale, file=sys.stderr)
        # print(userlist, file=sys.stderr)
        '''產生電子圍籬'''
        elFence = electricFence()
        elFence.pullData(userlist)
        elFence.onlySanxia()
        elFence.removeOutlier()
        if FenceScale == 0:
            spacelist = []
            valuelist = []
        else:
            spacelist,valuelist = elFence.squareBounds(FenceScale)
    return render_template('history.html', select = select, user = user, pic = pic, spacelist = spacelist, valuelist = valuelist)

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
    lat = request.args.get('lat', 0, type=float)
    lng = request.args.get('lng', 0, type=float)
    baseLocation = [lat-0.005,lng-0.005]

    if current_user.is_active:
        user = current_user.id
        person = mongo.db.Select_People.find({'username': user})[0]['person']
        userlist = [person]
        FenceScale = int(fenceScale)
        elFence = electricFence()
        elFence.pullData(user, userlist,startDate,endDate)
        elFence.onlySanxia()
        elFence.removeOutlier()
        spacelist,valuelist = elFence.squareBounds(FenceScale,baseLocation)
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
    if current_user.is_active:
        user = current_user.id
        userlist = ['james','leo'] 
        elFence = electricFence() 
        elFence.pullData(user,userlist,days=14)
        spacelist,valuelist,base = elFence.squareBounds()
        print(spacelist)
        print(valuelist)
        print(base)
    return jsonify(valuelist,base)


if __name__ == '__main__':
    app.run(host = "localhost",debug=True)