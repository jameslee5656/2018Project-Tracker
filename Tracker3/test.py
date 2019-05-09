import socket
# 建立socket物件
ss = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
# 對方的IP和接收資料的埠組成的套接字 用來傳送資訊
address = ("120.126.136.17", 5687)
msg = input("輸入傳送的內容：")
# 傳送資訊
ss.sendto(msg.encode("gbk"), address)
# 接受對方傳送過來的資訊 限制傳輸的資料的大小 返回的是(msg, (ip, port))
# msg對方傳輸的資料 ip對方的ip port對方傳輸資料的埠
data = ss.recvfrom(1024)
print("接受來自於{} {}的資訊：{}".format(data[1][0], data[1][1], msg.decode("gbk")))
# 關閉連線
ss.close()