from flask import Flask
from flask_wtf import FlaskForm
from wtforms import SelectField, SubmitField,StringField,PasswordField,RadioField,SelectMultipleField
from wtforms.validators import InputRequired, EqualTo,DataRequired,ValidationError
from flask_pymongo import PyMongo

app = Flask(__name__)
app.config["MONGO_URI"] = "mongodb://120.126.136.17:27017/Tracker"

class Selection(FlaskForm):
	user = SelectField('選擇人員:',choices=[])
	date = SelectField('選擇日期:',choices=[])
	begin_time = SelectField('起始時間:',choices=[
		(0,'00:00'),
		(1,'01:00'),
		(2,'02:00'),
		(3,'03:00'),
		(4,'04:00'),
		(5,'05:00'),
		(6,'06:00'),
		(7,'07:00'),
		(8,'08:00'),
		(9,'09:00'),
		(10,'10:00'),
		(11,'11:00'),
		(12,'12:00'),
		(13,'13:00'),
		(14,'14:00'),
		(15,'15:00'),
		(16,'16:00'),
		(17,'17:00'),
		(18,'18:00'),
		(19,'19:00'),
		(20,'20:00'),
		(21,'21:00'),
		(22,'22:00'),
		(23,'23:00'),
		(24,'24:00'),
		])
	duration = SelectField('經過時間:',choices=[
		(1,'1小時'),
		(3,'3小時'),
		(6,'6小時'),
		(12,'12小時'),
		(24,'整天')
		])

class Supervise_choose_friend(FlaskForm):
	choose_friend = SelectMultipleField('好友', choices=[
        ('leo', 'leo'),
        ('dn2', 'dn2'),
        ('james', 'james')
    ])

class Register(FlaskForm):
    username = StringField('帳號', validators=[InputRequired()])
    password = PasswordField('密碼', validators=[InputRequired()])
    confirm = PasswordField('再次確認密碼', validators=[DataRequired(message = ' '),EqualTo('password',message = '與密碼不符')])
    bracelet_number = StringField('手環識別碼')
    organization = SelectField('所屬機構',choices=[
		('ntpu','國立台北大學'),
		('hospital','恩主公醫院'),
		('none','無')
		])

    def validate_username(self, username):
    	mongo = PyMongo(app)
    	user = mongo.db.User_Info.find({'username' : username.data}).count()
    	if user == 1:
    		raise ValidationError('帳號已經存在！')

class Login(FlaskForm):
	username = StringField('帳號', validators=[InputRequired()])
	password = PasswordField('密碼', validators=[InputRequired()])

	def validate_username(self, username):
		mongo = PyMongo(app)
		user = mongo.db.User_Info.find({'username' : username.data}).count()
		if user != 1:
			raise ValidationError('此帳號不存在！')

	def validate_password(self, password):
		mongo = PyMongo(app)
		if mongo.db.User_Info.find({'username' : self.username.data}).count() == 1:
			p = mongo.db.User_Info.find({'username' : self.username.data})[0]['password']
			if password.data != p:
				raise ValidationError('密碼錯誤')

class Log_out(FlaskForm):
	logout = SubmitField('登出')

class Delete_friend(FlaskForm):
	delete_friend = SubmitField('')
