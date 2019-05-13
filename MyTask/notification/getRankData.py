from pymongo import MongoClient
import time
import paho.mqtt.client as mqtt

def getRankData():

    t = time.localtime(time.time())
    year = t.tm_year
    month = t.tm_mon
    day = t.tm_mday
    hour = t.tm_hour
    rlist = []

    client=mqtt.Client()
    client.connect("120.126.136.17",1883,60)
    topic = ""
    mongo = MongoClient('120.126.136.17', 27017)
    db = mongo['Tracker']


    collection = db['User_Info']
    friendCol = collection.find()

    for x in friendCol:
        name = x['username']
        if name!= 'pitest' and name!= 'jack' and name!= 'manager':
            rlist.append(name)
    rsize=len(rlist)
    
    collection = db['rank']
    
    data = collection.find({'year':year,'month':month,'day':day,'hour':hour})

    #排名
    for x in data:
        topic = x['user']
        print(topic)
        rank =1
        pre=99999
        pro=-99999
        for name in rlist:
            if topic != name:
                if x[name]>0 and x[name]<pre:
                    rank=rank+1
                    pre=x[name]                 
                if x[name]<0 and x[name]>pro:
                    pro=x[name]
        if rank==1 and pre==99999:
            rank_mes_top = str(rank)+"/"+str(rsize)
            rank_mes_top = '%-7s' % rank_mes_top
            rank_mes_top = "排名 "+ rank_mes_top
            rank_mes_bot = "第"+str(rank)+"名,好棒棒"
            rank_mes=rank_mes_top+rank_mes_bot
            print(rank_mes)
            client.publish(topic,rank_mes) 
        elif pro==-99999:
            rank_mes_top = str(rank)+"/"+str(rsize)
            rank_mes_top = '%-7s' % rank_mes_top
            rank_mes_top = "排名 "+ rank_mes_top
            rank_mes_bot = "第"+str(rank)+"名,太混囉"
            rank_mes=rank_mes_top+rank_mes_bot
            print(rank_mes)
            client.publish(topic,rank_mes)
        else:
            rank_mes_top = str(rank)+"/"+str(rsize)
            rank_mes_top = '%-7s' % rank_mes_top
            rank_mes_top = "排名 "+ rank_mes_top
            rank_mes_bot = "第"+str(rank)+"名,再加油"
            rank_mes=rank_mes_top+rank_mes_bot
            print(rank_mes)
            client.publish(topic,rank_mes)
        time.sleep(0.01)
            
    time.sleep(5)
    print("======================")
    
    #前一名
    data = collection.find({'year':year,'month':month,'day':day,'hour':hour})
    
    for x in data:
        topic = x['user']
        print(topic)
        rank =1
        pre=99999
        pro=-99999
        for name in rlist:
            if topic != name:
                if x[name]>0 and x[name]<pre:
                    rank=rank+1
                    pre=x[name]                 
                if x[name]<0 and x[name]>pro:
                    pro=x[name]
        if rank==1 and pre==99999:
            rank_mes = "保持冠軍喔!"
            print(rank_mes)
            client.publish(topic,rank_mes) 
        else:
            rank_mes_top = "與前一名落差"
            rank_mes_bot = str(int(pre))+"步"
            rank_mes=rank_mes_top+rank_mes_bot
            print(rank_mes)
            client.publish(topic,rank_mes)
            
    time.sleep(5)
    print("======================")
    
    #後一名
    data = collection.find({'year':year,'month':month,'day':day,'hour':hour})
    
    for x in data:
        topic = x['user']
        print(topic)
        rank =1
        pre=99999
        pro=-99999
        for name in rlist:
            if topic != name:
                if x[name]>0 and x[name]<pre:
                    rank=rank+1
                    pre=x[name]                 
                if x[name]<0 and x[name]>pro:
                    pro=x[name]
        if pro==-99999:
            rank_mes = "吊車尾加把勁"
            print(rank_mes)
            client.publish(topic,rank_mes) 
        else:
            rank_mes_top = "與後一名落差"
            rank_mes_bot = str(abs(int(pro)))+"步"
            rank_mes=rank_mes_top+rank_mes_bot
            print(rank_mes)
            client.publish(topic,rank_mes)
            
    mongo.close()
    client.disconnect()


#呼叫   
getRankData()
