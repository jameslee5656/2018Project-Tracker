from __future__ import print_function
from flask import Flask, render_template,redirect,url_for,request,jsonify
from flask_wtf import FlaskForm
from wtforms import SelectField, SubmitField
from wtforms.validators import DataRequired
from flask_pymongo import PyMongo
from forms import Selection,Register,Login, Log_out, Delete_friend, Supervise_choose_friend
from flask_login import LoginManager, UserMixin, login_user, login_required, logout_user, current_user
from SetFence import electricFence
import sys
from PIL import Image
import requests
from io import BytesIO
import matplotlib.pyplot as plt

app = Flask(__name__)
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

        '''產生好友列表'''
        Friends = mongo.db.Friend.find({'username': user})[0]['Friends']
        for friend in Friends:
            '''好友圖片處理'''
            temp = '../static/images/user/' + friend + '.jpg'
            friend_pic.append(temp)

        '''處理勾選好友選項'''
        if request.method == "POST":
            choose_friends = request.values.getlist("choose_friends")
            de = mongo.db.Supervise_Friend.delete_one({'username': user})
            insert = { 'username': user,
                       'Visable_Friends' : choose_friends}
            ins = mongo.db.Supervise_Friend.insert_one(insert)
        else:
            choose_friends = mongo.db.Supervise_Friend.find({'username': user})[0]['Friends']
            
        '''產生選擇的好友手環編號'''
        for choose in choose_friends:
            temp_number = mongo.db.User_Info.find({'username' : choose})[0]['bracelet_number']
            friends_number.append(temp_number)
    return render_template('supervise.html', user = user, friends_number = friends_number, Friends = Friends, friend_pic = friend_pic, pic = pic, choose_friends = choose_friends)

@app.route("/master_supervise")
def master_supervise():
    mongo = PyMongo(app)
    user_login = '請先登入'
    pic = '../static/images/user/blank.jpg'
    friends_number = []
    Friends = []
    if current_user.is_active:
        user = current_user.id
        user = str(user)
        pic = '../static/images/user/' + user + '.jpg'

        '''好友及時追蹤 實作'''
        friends = mongo.db.Friend.find({'username': user})[0]['Friends']
        for friend in friends:
            temp_number = mongo.db.User_Info.find({'username' : friend})[0]['bracelet_number']
            friends_number.append(temp_number)
    return render_template('master_supervise.html', user = user, friends_number = friends_number, friends = friends, pic = pic)

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

        print(FenceScale, file=sys.stderr)
        print(userlist, file=sys.stderr)
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


@app.route("/history/display", methods=['GET','POST'])
def history_display():
    global select
    mongo = PyMongo(app)
    select = Selection()
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

        '''顯示歷史軌跡：表單處理'''
        temp = select.data['date']
        s = temp.split('/')
        month = int(s[0])
        day = int(s[1])
        begin = int(select.data['begin_time'])
        duration = select.data['duration']
        hour = int(begin) + int(duration)

        if duration == 24:
            datas = mongo.db[user].find({'day': day})
        else:
            datas = mongo.db[user].find({'day': day,'month':month,'hour':{"$lt": hour},'hour':{"$gt": begin-1}})

        '''電子圍籬格子大小選擇'''
        if request.method == "POST":
            FenceScale = request.values.get("FenceScale")
            if FenceScale != None:
                FenceScale = int(FenceScale)
            else:
                FenceScale = 0

        '''使用者選擇'''
        if user == "manager":
            alluser = mongo.db.User_Info.find()
            for getuser in alluser:
                userlist.append(getuser['username'])
        else:
            userlist.append(user)

        print(FenceScale, file=sys.stderr)
        print(userlist, file=sys.stderr)
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
    '''for data in datas:
        print(data['latitude'], file=sys.stderr)'''

    return render_template('history_display.html', datas = datas, select = select, user = user, pic = pic, spacelist = spacelist, valuelist = valuelist)

@app.route("/master_history", methods=['GET','POST'])
def master_history():
    global select
    select = Selection()
    mongo = PyMongo(app)
    user = "請先登入"
    pic = '../static/images/user/blank.jpg'
    friend_pic = []
    Friends = []
    CheckedFriend = {}
    getuserlist = []
    if current_user.is_active:
        user = current_user.id

        '''電子圍籬格子大小、選人設定'''
        if request.method == "POST":
            getuserlist = request.values.getlist("people")
            if getuserlist[0] == "all":
                alluser = mongo.db.User_Info.find()
                for getuser in alluser:
                    getuserlist.append(getuser['username'])
            mongo.db.Select_People.update_one({'username':'manager'},{'$set':{'selet-people':getuserlist}})

        '''產生好友'''
        Friends = mongo.db.Friend.find({'username': user})[0]['Friends']
        SeletedPeople = mongo.db.Select_People.find({'username': 'manager'})[0]['selet-people']
        for friend in Friends:
            '''產生是否勾選'''
            if friend in SeletedPeople:
                CheckedFriend[friend] = 1
            else:
                CheckedFriend[friend] = 0
            
            '''產生好友圖片'''
            temp = '../static/images/user/' + friend + '.jpg'
            friend_pic.append(temp)
    return render_template('master_history.html', select = select, Friends = Friends, friend_pic = friend_pic, CheckedFriend = CheckedFriend)

@app.route("/history/master_history_display", methods=['GET','POST'])
def master_history_display():
    global select
    select = Selection()
    mongo = PyMongo(app)
    user = "請先登入"
    pic = '../static/images/user/blank.jpg'
    global chosen_person
    FenceScale = 0
    userlist = []
    boundlist = []
    if current_user.is_active:
        user = current_user.id
        '''動態產生選項'''
        dates = []
        checks = mongo.db[chosen_person].find({})
        for check in checks:
            temp_str = str(check['month']) + '/' + str(check['day'])
            if temp_str not in dates:
                dates.append(temp_str)
        select.date.choices = [(date,date) for date in dates]
        pic = '../static/images/user/' + user + '.jpg'


        '''顯示歷史軌跡：表單處理'''
        temp = select.data['date']
        s = temp.split('/')
        month = int(s[0])
        day = int(s[1])
        begin = int(select.data['begin_time'])
        duration = select.data['duration']
        hour = int(begin) + int(duration)

        if duration == 24:
            datas = mongo.db[chosen_person].find({'day': day})
        else:
            datas = mongo.db[chosen_person].find({'day': day,'month':month,'hour':{"$lt": hour},'hour':{"$gt": begin-1}})

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

        print(FenceScale, file=sys.stderr)
        print(userlist, file=sys.stderr)
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
            '''for data in datas:
        print(data['latitude'], file=sys.stderr)'''

    return render_template('master_history_display.html', datas = datas, select = select, user = user, pic = pic, spacelist = spacelist, valuelist = valuelist)


@app.route("/test", methods=['GET','POST'])
def test():
    select = Selection()
    if select.validate_on_submit():
        '''資料庫處理'''
    return render_template('test.html',select = select)

@app.route("/account", methods =['GET','POST'])
def account():
    mongo = PyMongo(app)
    reg = Register()
    log = Login()
    user = ''
    pic = '../static/images/user/blank.jpg'
    friend_pic = []

    if current_user.is_active:
        user = current_user.id
        pic = '../static/images/user/' + user + '.jpg'

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
                     'acount_mode' : 'OWNER'
                    }
        mongo.db.User_Info.insert_one(new_user)
        user = User()
        user.id = reg.data['username']
        login_user(user)
        return redirect(url_for('home'))

    '''刪除好友實作'''
    friend = mongo.db.Friend.find({'username': user})[0]['Friends']
    if request.method == "POST":
        delfriend = request.values.get("delfriend")
        friend.remove(delfriend)
        de = mongo.db.Friend.delete_one({'username': user})
        insert = { 'username': user,
                   'Friends' : friend}
        ins = mongo.db.Friend.insert_one(insert)

    '''動態產生好友列表'''
    Friends = mongo.db.Friend.find({'username': user})[0]['Friends']
    Friends.remove(user)
    for friend in Friends:
        temp = '../static/images/user/' + friend + '.jpg'
        friend_pic.append(temp)

    return render_template('account.html',pic = pic, reg = reg, log = log, user = user, Friends = Friends, friend_pic = friend_pic)

@app.route("/logout/")
def logout():
    logout_user()
    return redirect(url_for('home'))

@app.route("/delete_friend/")
def delete_friend():
    return redirect(url_for('home'))

@app.route('/Move_Fence')
def Move_Fence():
    mongo = PyMongo(app)
    fenceScale = request.args.get('fenceScale', 0, type=float)
    lat = request.args.get('lat', 0, type=float)
    lng = request.args.get('lng', 0, type=float)
    baseLocation = [lat-0.005,lng-0.005]

    userlist = mongo.db.Select_People.find({'username': 'manager'})[0]['selet-people']
    #FenceScale = mongo.db.Select_People.find({'username': 'manager'})[0]['FenceScale']
    FenceScale = int(fenceScale)
    elFence = electricFence()
    elFence.pullData(userlist)
    elFence.onlySanxia()
    elFence.removeOutlier()
    spacelist,valuelist = elFence.squareBounds(FenceScale,baseLocation)
    '''key = 'AIzaSyD_xrySG3MlQuGCwglYYeXztFQehgNGDbw'#api key
    name = 0;
    for i in valuelist[:10]:
        base = "https://maps.googleapis.com/maps/api/streetview?size=1200x800&location="
        MyUrl = base + str(round(float(i[0][0])+ 0.00005,5))+',' + str(round(float(i[0][1])+ 0.00005,5)) + '&key=' + key  #added url encoding
        response = requests.get(MyUrl)
        img = Image.open(BytesIO(response.content))
        img.save( 'static/images/streetview/' + str(name) + ".jpg", "JPEG", quality=80, 
            optimize=True, progressive=True)
        name += 1
    print("return", file=sys.stderr)'''
    return jsonify(spacelist,valuelist)


@app.route('/change_Date')
def change_Date():
    mongo = PyMongo(app)
    startDate = request.args.get('startDate', 0, type=float)
    endDate = request.args.get('endDate', 0, type=float)
    fenceScale = request.args.get('fenceScale', 0, type=float)
    lat = request.args.get('lat', 0, type=float)
    lng = request.args.get('lng', 0, type=float)
    baseLocation = [lat-0.005,lng-0.005]

    userlist = mongo.db.Select_People.find({'username': 'manager'})[0]['selet-people']
    FenceScale = mongo.db.Select_People.find({'username': 'manager'})[0]['FenceScale']
    FenceScale = int(fenceScale)
    elFence = electricFence()
    elFence.pullData(userlist,startDate,endDate)
    elFence.onlySanxia()
    elFence.removeOutlier()
    spacelist,valuelist = elFence.squareBounds(FenceScale,baseLocation)
    # '''key = 'AIzaSyD_xrySG3MlQuGCwglYYeXztFQehgNGDbw'#api key
    # name = 0;
    # for i in valuelist[:10]:
    #     base = "https://maps.googleapis.com/maps/api/streetview?size=1200x800&location="
    #     MyUrl = base + str(round(float(i[0][0])+ 0.00005,5))+',' + str(round(float(i[0][1])+ 0.00005,5)) + '&key=' + key  #added url encoding
    #     response = requests.get(MyUrl)
    #     img = Image.open(BytesIO(response.content))
    #     img.save( 'static/images/streetview/' + str(name) + ".jpg", "JPEG", quality=80, 
    #         optimize=True, progressive=True)
    #     name += 1
    # print("return", file=sys.stderr)'''
    return jsonify(spacelist,valuelist)

if __name__ == '__main__':
    app.run(host = "localhost",debug=True)