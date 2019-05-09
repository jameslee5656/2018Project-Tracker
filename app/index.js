// Import packages
var mongodb = require('mongodb');
var ObjectID = mongodb.ObjectID;
var crypto = require('crypto');
var express = require('express');
var bodyParser = require('body-parser');

//Create Express Service
var app = express();
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({extended: true}));

//Create MongoDB Client
var MongoClient = mongodb.MongoClient;

//Connection URL
var url = 'mongodb://120.126.136.17:27017';
MongoClient.connect(url,{useNewUrlParser: true},function(err,client){
	if(err){
		console.log('Unable to connect to the MongoDB server.', err);
	}
	else{

		//Register
		app.post('/register',(request,response,next)=>{
			var post_data = request.body;

			var password = post_data.password;

			 var name = post_data.name;

			 var insertJson = {
			 	'username': name,
			 	'password': password
			 };
			 var db = client.db('Tracker');

			 //Check exist name
			 db.collection('User_Info')
			 	.find({'username':name}).count(function(err,number){
			 		if(number != 0){
			 			response.json('UserName already exists');
			 			console.log('UserName already exists');
			 		}
			 		else{
			 			//Insert data
			 			db.collection('User_Info')
			 				.insertOne(insertJson,function(error,res){
			 					response.json('Registration success');
			 					console.log('Registration success');
			 				})
			 			//Create User Collection
			 			db.createCollection(name,function(err,res){
			 				console.log(name + "'s Collection is created.")
			 			})
			 		}
			 	})
		});

		app.post('/login',(request,response,next)=>{
			var post_data = request.body;


			var name = post_data.name;
			var userPassword = post_data.password;


			
			var db = client.db('Tracker');

			 //Check exist name
			 db.collection('User_Info')
			 	.find({'username':name}).count(function(err,number){
			 	if(number == 0){
			 		response.json('UserName not exists');
			 		console.log('UserName not exists');
			 	}
			 	else{
			 			//Insert data
			 			db.collection('User_Info')
			 			.findOne({'username':name},function(err,user){
			 				var database_password = userPassword;
			 				var input_password = user.password; // Get password from user
			 				if(database_password == input_password){
			 					response.json('Login success');
			 					console.log('Login success');
			 				}
			 				else{
			 					response.json('Wrong password');
			 					console.log('Wrong password');
			 				}
			 			})
			 		}
			 	})
		});
		//debug get		
 		app.post('/get',(request,response,next)=>{
                        var post_data = request.body;
  

                        var name = post_data.name;                    
                        var db = client.db('Tracker');

                         //Check exist name
                         db.collection('User_Info')
                                .find({'username':name}).count(function(err,number){
                                if(number == 0){
                                        response.json('UserName not exists');
                                        console.log('UserName not exists');
                                }
                                else{
                                                //Insert data
                                                db.collection('User_Info')
                                                .findOne({'username':name},function(err,user){                                                        
                                                	response.json(user.password);
                                                        console.log('get success');
                                                        
                                                })
                                        }
                                })
                });
                //getDayData
                app.post('/getDayData',(request,response,next)=>{
                        var post_data = request.body;


                        var name = post_data.name;
						var year = parseInt(post_data.year);
						var month =parseInt(post_data.month);
						var day = parseInt(post_data.day);
                        var db = client.db('Tracker');

                         //Check exist prediction
                        db.collection('prediction')
                                .findOne({'user':name,'year':year,'month':month,'day':day,'type':'daily'},function(err,p){
                                if(p == null){
                                        response.json("No this pediction");
                                        console.log("No this prediction");
                                }else{
                                	//post data
                                        response.json(p.prediction);
                                        console.log('getDayData success');
                                	}
                                })
                });
		//getHourData
		app.post('/getHourData',(request,response,next)=>{
                        var post_data = request.body;


                        var name = post_data.name;
                        var year = parseInt(post_data.year);
                        var month = parseInt(post_data.month);
                        var day = parseInt(post_data.day);
                        var hour = parseInt(post_data.hour);
						var db = client.db('Tracker');

                         //Check exist prediction
                         db.collection('prediction')
                                .findOne({'user':name,'year':year,'month':month,'day':day,'hour':hour,'type':'hourly'},function(err,p){
                                if(p == null){
                                        response.json("No this pediction");
                                        console.log("No this pediction");
                                }else{
                                	//post data
                                        response.json(p.prediction+"_"+p.totalsteps);
                                        console.log('getHourData success');
                                	}
                                })
                });

		//getHourData
		app.post('/getRank',(request,response,next)=>{
                        var post_data = request.body;
                        var name = post_data.name;
                        var year = parseInt(post_data.year);
                        var month = parseInt(post_data.month);
                        var day = parseInt(post_data.day);
                        var hour = parseInt(post_data.hour);
						var db = client.db('Tracker');

                         //Check exist rank
                         db.collection('rank')
                                .findOne({'user':name,'year':year,'month':month,'day':day,'hour':hour},function(err,p){
                                if(p == null){
                                        response.json("No this Rank");
                                        console.log("No this Rank");
                                }else{
                                	//post data
                                		var compare = {};
                                		for(var pkey in p){
                                			//check if the property/key is defined in theobject itself, not in parent
                                			// if (p.hasOwnProperty(key)){
                                			// 	console.log(key, p[key]);
                                			// }
                                			if ((pkey != 'day') & (pkey != '_id') & (pkey != 'hour')
                                				 & (pkey != 'month') & (pkey != 'year') & (pkey != 'user')){
                                				// console.log(key, p[key]);
                                				compare[pkey] = p[pkey];
                                			}
                                		}
                                		// create items array
                                		var items = Object.keys(compare).map(function(key) {
  											return [key, compare[key]];
										});
                                		// sort the array based on the second element
                                		items.sort(function(first, second){
                                			return second[1] - first[1];
                                		});
                                		var responseMss = "";
                                		for(index = 0; index < items.length; index++){
                                			if (items[index][0] == p.user){
                                				responseMss += String(index + 1);
                                				responseMss += '/'+ String(items.length + 1)
                                				if (index == 0){
                                					responseMss += ' None';
                                				}
                                				else{
                                					responseMss += ' ' + items[index - 1][0] + ':' + String(items[index - 1][1] * -1)
                                				}
                                				if (index == items.length-1){
                                					responseMss += ' None';
                                				}
                                				else{
                                					responseMss += ' ' + items[index + 1][0] + ':+' + String(items[index + 1][1] * -1)
                                				}
                                			}

                                		}
                                		console.log(responseMss);
                                		

                                        response.json(responseMss);
                                        console.log('getRank success');
                                	}
                                })
                });

		//Start Web Server
		app.listen(3000,()=>{
			console.log('Connected to MongoDB server , WebService running on port 3000');
		})
	}
});




